#include <Rcpp.h>
#include <htslib/hts.h>
#include <htslib/sam.h>
#include <htslib/thread_pool.h>

// [[Rcpp::depends(Rhtslib)]]

// An optimised attempt to read and preprocess BAM in place. To do:
// [ ] OpenMP SIMD?
// [+] HTSlib threads
// [+] rec_seq_rs and rec_xm_rs as char*
// [?] reverse QNAME
// [ ] free resources on interrupt

// PAIRED-END BAM

// [[Rcpp::export]]
Rcpp::DataFrame rcpp_read_bam_paired (std::string fn,                           // file name
                                      int min_mapq,                             // min read mapping quality
                                      int min_baseq,                            // min base quality
                                      bool skip_duplicates,                     // skip marked duplicates
                                      int nthreads)                             // HTSlib threads, >0 for multiple
{
  // constants
  int max_qname_width = 1024;                                                   // max QNAME length, not expanded yet, ever error-prone?
  int max_templ_width = 8192;                                                   // max insert size, expanded if necessary
  min_baseq = min_baseq - (min_baseq>0);                                        // decrease base quality by one to include bases with QUAL==min_baseq
  
  // file IO
  htsFile *bam_fp = hts_open(fn.c_str(), "r");                                  // try open file
  if (bam_fp==NULL) Rcpp::stop("Unable to open BAM file for reading");          // fall back if error
  htsThreadPool thread_pool = {NULL, 0};                                        // thread pool cuts time by 30%
  if (nthreads>0) {
    thread_pool.pool = hts_tpool_init(nthreads);                                // when initiated for >0 threads
    hts_set_opt(bam_fp, HTS_OPT_THREAD_POOL, &thread_pool);                     // and bound to the file pointer
  }
  bam_hdr_t *bam_hdr = sam_hdr_read(bam_fp);                                    // try read file header
  if (bam_hdr==NULL) Rcpp::stop("Unable to read BAM header");                   // fall back if error  
  bam1_t *bam_rec = bam_init1();                                                // create BAM alignment structure
  
  // main containers
  std::vector<std::string>* seq = new std::vector<std::string>;                 // SEQ
  std::vector<std::string>* xm = new std::vector<std::string>;                  // XM
  std::vector<int> rname, strand, start;                                        // id for RNAME, id for CT==1/GA==2, POS
  int nrecs = 0, ntempls = 0;                                                   // counters: BAM records, templates (consecutive proper read pairs)
  
  // reserve some memory
  rname.reserve(0xFFFFF); strand.reserve(0xFFFFF); start.reserve(0xFFFFF); 
  seq->reserve(0xFFFFF); xm->reserve(0xFFFFF);
  
  // template holders
  char *templ_qname = (char*) malloc(max_qname_width * sizeof(char));           // template QNAME
  uint8_t *templ_qual_rs = (uint8_t*) malloc(max_templ_width * sizeof(uint8_t));// template QUAL array
  uint8_t *templ_seq_rs  = (uint8_t*) malloc(max_templ_width * sizeof(uint8_t));// template SEQ array
  uint8_t *templ_xm_rs   = (uint8_t*) malloc(max_templ_width * sizeof(uint8_t));// template XM array
  int templ_rname = 0, templ_start = 0, templ_strand = 0, templ_width = 0;      // template RNAME, POS, STRAND, ISIZE
  
  #define push_template {               /* pushing template data to vectors */ \
    rname.push_back(templ_rname + 1);                            /* RNAME+1 */ \
    strand.push_back(templ_strand);                               /* STRAND */ \
    start.push_back(templ_start + 1);                              /* POS+1 */ \
    seq->emplace_back((const char*) templ_seq_rs, templ_width);      /* SEQ */ \
    xm->emplace_back( (const char*) templ_xm_rs,  templ_width);       /* XM */ \
    ntempls++;                                                        /* +1 */ \
  }
  
  // process alignments
  while( sam_read1(bam_fp, bam_hdr, bam_rec) > 0 ) {                            // rec by rec
    nrecs++;                                                                    // BAM alignment records ++
    if ((nrecs & 0xFFFFF) == 0) Rcpp::checkUserInterrupt();                     // every ~1M reads check for the interrupt
    
    if ((bam_rec->core.qual < min_mapq) ||                                      // skip if mapping quality < min.mapq
        (!(bam_rec->core.flag & BAM_FPROPER_PAIR)) ||                           // or if not a proper pair
        (skip_duplicates && (bam_rec->core.flag & BAM_FDUP))) continue;         // or if record is an optical/PCR duplicate
    
    char *rec_strand = (char*) bam_aux_get(bam_rec, "XG");                      // genome strand
    char *rec_xm = (char*) bam_aux_get(bam_rec, "XM");                          // methylation string
    if ((rec_strand==NULL) || (rec_xm==NULL)) continue;                         // skip if no XM/XG tags (no methylation info available)
    
    // check if not the same template (QNAME)
    if ((strcmp(templ_qname, bam_get_qname(bam_rec)) != 0)) {                
      // store previous template if it's a valid record
      if (templ_strand!=0) push_template;                                       // templ_strand is 0 for empty records (very start of BAM and/or when invalid records are in front of BAM)
      
      // initialize new template
      strcpy(templ_qname, bam_get_qname(bam_rec));                              // store template QNAME
      templ_rname = bam_rec->core.tid;                                          // store template RNAME
      templ_start = bam_rec->core.pos < bam_rec->core.mpos ?                    // smallest of POS,MPOS is a start
        bam_rec->core.pos : bam_rec->core.mpos;
      templ_width = abs(bam_rec->core.isize);                                   // template ISIZE
      templ_strand = ( rec_strand[1] == 'C' ) ? 1 : 2 ;                         // STRAND is 1 if "ZCT"/"+", 2 if "ZGA"/"-"
      
      // resize containers if necessary
      if (templ_width > max_templ_width) {
        max_templ_width = templ_width;                                          // expand template holders
        templ_qual_rs = (uint8_t *) realloc(templ_qual_rs, max_templ_width);
        templ_seq_rs  = (uint8_t *) realloc(templ_seq_rs,  max_templ_width);
        templ_xm_rs   = (uint8_t *) realloc(templ_xm_rs,   max_templ_width);
        if (templ_qual_rs==NULL || templ_seq_rs==NULL || templ_xm_rs==NULL)     // check memory allocation
          Rcpp::stop("Unable to allocate memory for BAM record #%i", nrecs);
      }
      std::memset(templ_qual_rs, (uint8_t) min_baseq, templ_width);             // clean template holders
      std::memset(templ_seq_rs, 'N', templ_width);
      std::memset(templ_xm_rs,  '-', templ_width);
      // std::fill_n(templ_qual_rs, templ_width, (uint8_t) min_baseq);             // clean template holders
      // std::fill_n(templ_seq_rs,  templ_width, 'N');
      // std::fill_n(templ_xm_rs,   templ_width, '-');
     }
    
    // add another read to the template
    // source containers
    uint8_t *rec_qual = bam_get_qual(bam_rec);                                  // quality string (Phred scale with no +33 offset)
    rec_xm++;                                                                   // remove leading 'Z' from XM string
    uint8_t *rec_pseq = bam_get_seq(bam_rec);                                   // packed sequence string (4 bit per base)
    
    // apply CIGAR
    uint32_t n_cigar = bam_rec->core.n_cigar;                                   // number of CIGAR operations
    uint32_t *rec_cigar = bam_get_cigar(bam_rec);                               // CIGAR array
    uint32_t query_pos = 0;                                                     // starting position in query array
    uint32_t dest_pos = bam_rec->core.pos - templ_start;                        // starting position in destination array
    for (size_t i=0; i<n_cigar; i++) {                                          // op by op
      uint32_t cigar_op = bam_cigar_op(rec_cigar[i]);                           // CIGAR operation
      uint32_t cigar_oplen = bam_cigar_oplen(rec_cigar[i]);                     // CIGAR operation length
      switch(cigar_op) {
        case BAM_CMATCH :                                                       // 'M', 0
        case BAM_CEQUAL :                                                       // '=', 7
        case BAM_CDIFF :                                                        // 'X', 8
          for (size_t j=0; j<cigar_oplen; j++) {
            if (rec_qual[query_pos+j] > templ_qual_rs[dest_pos+j]) {
              templ_qual_rs[dest_pos+j] = rec_qual[query_pos+j];
              templ_seq_rs[dest_pos+j] = seq_nt16_str[bam_seqi(rec_pseq,query_pos+j)];
              templ_xm_rs[dest_pos+j] = rec_xm[query_pos+j];
            }
          }
          query_pos += cigar_oplen;
          dest_pos += cigar_oplen;
          break;
        case BAM_CINS :                                                         // 'I', 1
        case BAM_CSOFT_CLIP :                                                   // 'S', 4
          query_pos += cigar_oplen;
          break;
        case BAM_CDEL :                                                         // 'D', 2
        case BAM_CREF_SKIP :                                                    // 'N', 3
          dest_pos += cigar_oplen;
          break;
        case BAM_CHARD_CLIP :                                                   // 'H', 5
        case BAM_CPAD :                                                         // 'P', 6
        case BAM_CBACK :
          break;
        default :
          Rcpp::stop("Unknown CIGAR operation for BAM entry %s",                // unknown CIGAR operation
                     bam_get_qname(bam_rec));
      }
    }
  }
  
  // push last, yet unsaved template (no empty files enter this function)
  push_template;
  
  // cleaning
  bam_destroy1(bam_rec);                                                        // clean BAM alignment structure 
  hts_close(bam_fp);                                                            // close BAM file
  if (thread_pool.pool) hts_tpool_destroy(thread_pool.pool);                    // free thread pool
  free(templ_qname);                                                            // and free manually allocated memory
  free(templ_qual_rs);
  free(templ_seq_rs);
  free(templ_xm_rs);
  
  // wrap and return the results
  Rcpp::DataFrame res = Rcpp::DataFrame::create(                                // final DF
    Rcpp::Named("rname") = rname,                                               // numeric ids (factor) for reference names
    Rcpp::Named("strand") = strand,                                             // numeric ids (factor) for reference strands
    Rcpp::Named("start") = start                                                // start positions of reads
  );
  
  // factor levels
  std::vector<std::string> chromosomes (                                        // vector of reference names
      bam_hdr->target_name, bam_hdr->target_name + bam_hdr->n_targets);
  std::vector<std::string> strands = {"+", "-"};
  
  Rcpp::IntegerVector col_rname = res["rname"];                                 // make rname a factor
  col_rname.attr("class") = "factor";
  col_rname.attr("levels") = chromosomes;
  
  Rcpp::IntegerVector col_strand = res["strand"];                               // make strand a factor
  col_strand.attr("class") = "factor";
  col_strand.attr("levels") = strands;
  
  Rcpp::XPtr<std::vector<std::string>> seq_xptr(seq, true);
  res.attr("seq_xptr") = seq_xptr;                                              // external pointer to sequences
  Rcpp::XPtr<std::vector<std::string>> xm_xptr(xm, true);
  res.attr("xm_xptr") = xm_xptr;                                                // external pointer to methylation strings
  
  res.attr("nrecs") = nrecs;                                                    // number of records in BAM file
  res.attr("npushed") = ntempls;                                                // number of templates pushed to data.frame
  
  return(res);
}

// #############################################################################

// SINGLE-END BAM

// [[Rcpp::export]]
Rcpp::DataFrame rcpp_read_bam_single (std::string fn,                           // file name
                                      int min_mapq,                             // min read mapping quality
                                      int min_baseq,                            // min base quality
                                      bool skip_duplicates,                     // skip marked duplicates
                                      int nthreads)                             // HTSlib threads, >0 for multiple
{
  // constants
  int max_record_width  = 1024;                                                 // max record width, expanded if necessary
  
  // file IO
  htsFile *bam_fp = hts_open(fn.c_str(), "r");                                  // try open file
  if (bam_fp==NULL) Rcpp::stop("Unable to open BAM file for reading");          // fall back if error
  htsThreadPool thread_pool = {NULL, 0};                                        // thread pool cuts time by 30%
  if (nthreads>0) {
    thread_pool.pool = hts_tpool_init(nthreads);                                // when initiated for >0 threads
    hts_set_opt(bam_fp, HTS_OPT_THREAD_POOL, &thread_pool);                     // and bound to the file pointer
  }
  bam_hdr_t *bam_hdr = sam_hdr_read(bam_fp);                                    // try read file header
  if (bam_hdr==NULL) Rcpp::stop("Unable to read BAM header");                   // fall back if error  
  bam1_t *bam_rec = bam_init1();                                                // create BAM alignment structure
  
  // main containers
  std::vector<std::string>* seq = new std::vector<std::string>;                 // SEQ
  std::vector<std::string>* xm = new std::vector<std::string>;                  // XM
  std::vector<int> rname, strand, start;                                        // id for RNAME, id for CT==1/GA==2, POS
  int nrecs = 0, npushed = 0;                                                   // counters: BAM records read, BAM records pushed to data.table
  
  // reserve some memory
  rname.reserve(0xFFFFF); strand.reserve(0xFFFFF); start.reserve(0xFFFFF); 
  seq->reserve(0xFFFFF); xm->reserve(0xFFFFF);
  
  // read holders
  int record_width = max_record_width;                                          // record ISIZE/TLEN
  uint8_t *record_seq_rs  = (uint8_t*) malloc(record_width * sizeof(uint8_t));  // record SEQ array
  uint8_t *record_xm_rs   = (uint8_t*) malloc(record_width * sizeof(uint8_t));  // record XM array
  
  // process alignments
  while( sam_read1(bam_fp, bam_hdr, bam_rec) > 0 ) {                            // rec by rec
    nrecs++;                                                                    // BAM alignment records ++
    if ((nrecs & 0xFFFFF) == 0) Rcpp::checkUserInterrupt();                     // every ~1M reads check for the interrupt

    if ((bam_rec->core.qual < min_mapq) ||                                      // skip if mapping quality < min.mapq
        (skip_duplicates && (bam_rec->core.flag & BAM_FDUP))) continue;         // or if record is an optical/PCR duplicate
    
    char *record_strand = (char*) bam_aux_get(bam_rec, "XG");                   // genome strand
    char *record_xm = (char*) bam_aux_get(bam_rec, "XM");                       // methylation string
    if ((record_strand==NULL) || (record_xm==NULL)) continue;                   // skip if no XM/XG tags (no methylation info available)
    
    // clean after old record
    std::memset(record_seq_rs, 'N', record_width);
    std::memset(record_xm_rs,  '-', record_width);
    
    // start with new record
    record_width = abs(bam_rec->core.isize);                                    // template ISIZE, but can be 0 for single-ended!
    
    // // Proper resize should be based on sum of cigar_oplen if bam_rec->core.isize is 0
    // // I'll disable resize for now, see if it ever causes troubles
    // // resize containers if necessary
    // if (record_width > max_record_width) {
    //   max_record_width = record_width;                                          // expand template holders
    //   record_seq_rs  = (uint8_t *) realloc(record_seq_rs, max_record_width);
    //   record_xm_rs   = (uint8_t *) realloc(record_xm_rs,  max_record_width);
    //   if (record_seq_rs==NULL || record_xm_rs==NULL)                            // check memory allocation
    //     Rcpp::stop("Unable to allocate memory for BAM record #%i", nrecs);
    // }
    
    // get record sequence, XM, quality string
    uint8_t *record_qual = bam_get_qual(bam_rec);                               // quality string (Phred scale with no +33 offset)
    record_xm++;                                                                // remove leading 'Z' from XM string
    uint8_t *record_pseq = bam_get_seq(bam_rec);                                // packed sequence string (4 bit per base)
    
    // apply CIGAR
    uint32_t n_cigar = bam_rec->core.n_cigar;                                   // number of CIGAR operations
    uint32_t *record_cigar = bam_get_cigar(bam_rec);                            // CIGAR array
    uint32_t query_pos = 0;                                                     // starting position in query array
    uint32_t dest_pos = 0;                                                      // starting position in destination array
    for (size_t i=0; i<n_cigar; i++) {                                          // op by op
      uint32_t cigar_op = bam_cigar_op(record_cigar[i]);                        // CIGAR operation
      uint32_t cigar_oplen = bam_cigar_oplen(record_cigar[i]);                  // CIGAR operation length
      switch(cigar_op) {
      case BAM_CMATCH :                                                         // 'M', 0
      case BAM_CEQUAL :                                                         // '=', 7
      case BAM_CDIFF :                                                          // 'X', 8
        for (size_t j=0; j<cigar_oplen; j++) {
          if (record_qual[query_pos+j] >= min_baseq) {
            record_seq_rs[dest_pos+j] = seq_nt16_str[bam_seqi(record_pseq,query_pos+j)];
            record_xm_rs[dest_pos+j] = record_xm[query_pos+j];
          }
        }
        query_pos += cigar_oplen;
        dest_pos += cigar_oplen;
        break;
      case BAM_CINS :                                                           // 'I', 1
      case BAM_CSOFT_CLIP :                                                     // 'S', 4
        query_pos += cigar_oplen;
        break;
      case BAM_CDEL :                                                           // 'D', 2
      case BAM_CREF_SKIP :                                                      // 'N', 3
        dest_pos += cigar_oplen;
        break;
      case BAM_CHARD_CLIP :                                                     // 'H', 5
      case BAM_CPAD :                                                           // 'P', 6
      case BAM_CBACK :
        break;
      default :
        Rcpp::stop("Unknown CIGAR operation for BAM entry %s",                  // unknown CIGAR operation
                   bam_get_qname(bam_rec));
      }
    }
    
    // pushing record data to vectors
    rname.push_back(bam_rec->core.tid + 1);                                     // RNAME+1 
    strand.push_back(( record_strand[1] == 'C' ) ? 1 : 2);                      // STRAND is 1 if "ZCT"/"+", 2 if "ZGA"/"-"
    start.push_back(bam_rec->core.pos + 1);                                     // POS+1 
    seq->emplace_back((const char*) record_seq_rs, dest_pos);                   // SEQ 
    xm->emplace_back( (const char*) record_xm_rs,  dest_pos);                   // XM 
    npushed++;                                                                  // +1 
    
    record_width = dest_pos;                                                    // because bam_rec->core.isize can be 0 for single-end
  }
  
  // cleaning
  bam_destroy1(bam_rec);                                                        // clean BAM alignment structure 
  hts_close(bam_fp);                                                            // close BAM file
  if (thread_pool.pool) hts_tpool_destroy(thread_pool.pool);                    // free thread pool
  free(record_seq_rs);                                                          // and free manually allocated memory
  free(record_xm_rs);
  
  // wrap and return the results
  Rcpp::DataFrame res = Rcpp::DataFrame::create(                                // final DF
    Rcpp::Named("rname") = rname,                                               // numeric ids (factor) for reference names
    Rcpp::Named("strand") = strand,                                             // numeric ids (factor) for reference strands
    Rcpp::Named("start") = start                                                // start positions of reads
  );
  
  // factor levels
  std::vector<std::string> chromosomes (                                        // vector of reference names
      bam_hdr->target_name, bam_hdr->target_name + bam_hdr->n_targets);
  std::vector<std::string> strands = {"+", "-"};
  
  Rcpp::IntegerVector col_rname = res["rname"];                                 // make rname a factor
  col_rname.attr("class") = "factor";
  col_rname.attr("levels") = chromosomes;
  
  Rcpp::IntegerVector col_strand = res["strand"];                               // make strand a factor
  col_strand.attr("class") = "factor";
  col_strand.attr("levels") = strands;
  
  Rcpp::XPtr<std::vector<std::string>> seq_xptr(seq, true);
  res.attr("seq_xptr") = seq_xptr;                                              // external pointer to sequences
  Rcpp::XPtr<std::vector<std::string>> xm_xptr(xm, true);
  res.attr("xm_xptr") = xm_xptr;                                                // external pointer to methylation strings
  
  res.attr("nrecs") = nrecs;                                                    // number of records in BAM file
  res.attr("npushed") = npushed;                                                // number of records pushed to data.frame
  
  return(res);
}




// #############################################################################
// test code and sourcing don't work on OS X
/*** R
*/
// #############################################################################
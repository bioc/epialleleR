#' extractPatterns
#'
#' @description
#' This function extracts methylation patterns (epialleles) for a given genomic
#' region of interest.
#'
#' @details
#' The function matches reads (for paired-end sequencing alignment files - read
#' pairs as a single entity) to the genomic
#' region provided in a BED file/\code{\linkS4class{GRanges}} object, extracts
#' methylation statuses of bases within those reads, and returns a data frame
#' which can be used for plotting of DNA methylation patterns.
#' 
#' @param bam BAM file location string OR preprocessed output of
#' \code{\link[epialleleR]{preprocessBam}} function. Read more about BAM file
#' requirements and BAM preprocessing at \code{\link{preprocessBam}}.
#' @param bed Browser Extensible Data (BED) file location string OR object of
#' class \code{\linkS4class{GRanges}} holding genomic coordinates for
#' regions of interest. It is used to match sequencing reads to the genomic
#' regions prior to eCDF computation. The style of seqlevels of BED file/object
#' must match the style of seqlevels of the BAM file/object used. The 
#' BED/\code{\link[GenomicRanges]{GRanges}} rows are \strong{not} sorted
#' internally.
#' @param bed.row single non-negative integer specifying what `bed` region
#' should be included in the output (default: 1).
#' @param zero.based.bed boolean defining if BED coordinates are zero based
#' (default: FALSE).
#' @param match.min.overlap integer for the smallest overlap between read's and
#' BED/\code{\link[GenomicRanges]{GRanges}} start or end positions during
#' matching of capture-based NGS reads (default: 1).
#' @param extract.context string defining cytosine methylation context used
#' to report:
#' \itemize{
#'   \item "CG" (the default) -- CpG cytosines (called as zZ)
#'   \item "CHG" -- CHG cytosines (xX)
#'   \item "CHH" -- CHH cytosines (hH)
#'   \item "CxG" -- CG and CHG cytosines (zZxX)
#'   \item "CX" -- all cytosines
#' }
#' @param min.context.freq real number in the range [0;1] (default: 0.01).
#' Genomic positions that are covered by smaller fraction of patterns (e.g.,
#' with erroneous context) won't be included in the report.
#' @param clip.patterns boolean if patterns should not extend over the edge of
#' `bed` region (default: FALSE).
#' @param strand.offset single non-negative integer specifying the offset of
#' bases at the reverse (-) strand compared to the forward (+) strand. Allows
#' to "merge" genomic positions when methylation is symmetric (in CG and CHG
#' contexts). By default, equals 1 for `extract.context`=="CG", 2 for "CHG", or
#' 0 otherwise.
#' @param highlight.positions integer vector with genomic positions of bases
#' to include in every overlapping pattern. Allows to visualize the
#' distribution of single-nucleotide variations (SNVs) among methylation
#' patterns. `highlight.positions` takes precedence if any of these positions
#' overlap with within-the-context positions of methylation pattern.
#' @param ... other parameters to pass to the
#' \code{\link[epialleleR]{preprocessBam}} function.
#' Options have no effect if preprocessed BAM data was supplied as an input.
#' @param verbose boolean to report progress and timings (default: TRUE).
#' @return \code{\link[data.table]{data.table}} object containing
#' per-read (pair) base methylation information for the genomic region of
#' interest. The report columns are:
#' \itemize{
#'   \item seqnames -- read (pair) reference sequence name
#'   \item strand -- read (pair) strand
#'   \item start -- start of the read (pair)
#'   \item end -- end of the read (pair)
#'   \item nbase -- number of within-the-context bases for this read (pair)
#'   \item beta -- beta value of this read (pair), calculated as a ratio of the
#'   number of methylated within-the-context bases to the total number of
#'   within-the-context bases
#'   \item pattern -- hex representation of 64-bit FNV-1a hash calculated for
#'   all reported base positions and bases in this read (pair). This
#'   hash value depends only on included genomic positions and their methylation
#'   call string chars (hHxXzZ) or nucleotides (ACGT, for highlighted bases
#'   only), thus it is expected to be unique for every
#'   methylation pattern, although equal for identical methylation patterns
#'   independently on read (pair) start, end, or strand (when correct
#'   `strand.offset` is given)
#'   \item ... -- columns for each genomic position that hold corresponding
#'   methylation call string char, or NA if position is not present in the read
#'   (pair)
#' }
#' @seealso \code{\link{preprocessBam}} for preloading BAM data,
#' \code{\link{generateCytosineReport}} for methylation statistics at the level
#' of individual cytosines, \code{\link{generateBedReport}} for genomic
#' region-based statistics, \code{\link{generateVcfReport}} for evaluating
#' epiallele-SNV associations, \code{\link{generateBedEcdf}} for analysing the
#' distribution of per-read beta values, and `epialleleR` vignettes for the
#' description of usage and sample data.
#' @examples
#'   # amplicon data
#'   amplicon.bam <- system.file("extdata", "amplicon010meth.bam",
#'                               package="epialleleR")
#'   amplicon.bed <- system.file("extdata", "amplicon.bed",
#'                               package="epialleleR")
#'   
#'   # let's get our patterns
#'   patterns <- extractPatterns(bam=amplicon.bam, bed=amplicon.bed, bed.row=3)
#'   nrow(patterns)  # read pairs overlap genomic region of interest
#'   
#'   # these are positions of bases
#'   base.positions <- grep("^[0-9]+$", colnames(patterns), value=TRUE)
#'   
#'   # let's make a summary table with counts of every pattern
#'   patterns.summary <- patterns[, c(lapply(.SD, unique), .N),
#'                                by=.(pattern, beta), .SDcols=base.positions]
#'   nrow(patterns.summary)  # unique methylation patterns
#'   
#'   # let's melt and plot them
#'   plot.data <- data.table::melt.data.table(patterns.summary,
#'     measure.vars=base.positions, variable.name="pos", value.name="base")
#'   
#'   # upset-like plot of all patterns, categorical positions, sorted by counts
#'   if (require("ggplot2", quietly=TRUE) & require("gridExtra", quietly=TRUE)){
#'     grid.arrange(
#'       ggplot(na.omit(plot.data),
#'              aes(x=pos, y=reorder(pattern,N),
#'                  color=factor(base, levels=c("z","Z")))) +
#'         geom_line(color="grey") +
#'         geom_point() +
#'         scale_colour_grey(start=0.8, end=0) +
#'         theme_light() +
#'         scale_x_discrete(breaks=function(x){x[c(rep(FALSE,5), TRUE)]}) +
#'         theme(axis.text.y=element_blank(), legend.position="none") +
#'         labs(x="position", y=NULL, title="epialleles", color="base"),
#'       
#'       ggplot(unique(na.omit(plot.data)[, .(pattern, N, beta)]),
#'              aes(x=N+0.5, y=reorder(pattern,N), alpha=beta, label=N)) +
#'         geom_col() +
#'         geom_text(alpha=0.5, nudge_x=0.2, size=3) +
#'         scale_x_log10() +
#'         theme_minimal() +
#'         theme(axis.text.y=element_blank(), legend.position="none") +
#'         labs(x="count", y=NULL, title=""),
#'       ncol=2, widths=c(0.75, 0.25)
#'     )
#'   }
#'   
#' @export
extractPatterns <- function (bam,
                             bed,
                             bed.row=1,
                             zero.based.bed=FALSE,
                             match.min.overlap=1,
                             extract.context=c("CG", "CHG", "CHH", "CxG", "CX"),
                             min.context.freq=0.01,
                             clip.patterns=FALSE,
                             strand.offset=c("CG"=1, "CHG"=2, "CHH"=0,
                                             "CxG"=0, "CX"=0)[extract.context],
                             highlight.positions=c(),
                             ...,
                             verbose=TRUE)
{
  bed.row             <- as.integer(bed.row[1])
  extract.context     <- match.arg(extract.context, extract.context)
  strand.offset       <- as.integer(strand.offset[1])
  highlight.positions <- as.integer(highlight.positions)
  
  if (!methods::is(bed, "GRanges"))
    bed <- .readBed(bed.file=bed, zero.based.bed=zero.based.bed,
                    verbose=verbose)
  
  bam <- preprocessBam(bam.file=bam, ..., verbose=verbose)
  
  patterns <- .getPatterns(
    bam.processed=bam, bed=bed, bed.row=bed.row,
    match.min.overlap=match.min.overlap,
    extract.context=paste0(.context.to.bases[[extract.context]]
                           [c("ctx.meth","ctx.unmeth")], collapse=""),
    min.context.freq=min.context.freq, clip.patterns=clip.patterns,
    strand.offset=strand.offset, highlight.positions=highlight.positions,
    verbose=verbose
  )
  
  return(patterns)
}

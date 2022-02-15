// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// rcpp_cx_report
Rcpp::DataFrame rcpp_cx_report(Rcpp::DataFrame& df, Rcpp::LogicalVector& pass, std::string ctx);
RcppExport SEXP _epialleleR_rcpp_cx_report(SEXP dfSEXP, SEXP passSEXP, SEXP ctxSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type df(dfSEXP);
    Rcpp::traits::input_parameter< Rcpp::LogicalVector& >::type pass(passSEXP);
    Rcpp::traits::input_parameter< std::string >::type ctx(ctxSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_cx_report(df, pass, ctx));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_extract_patterns
Rcpp::DataFrame rcpp_extract_patterns(Rcpp::DataFrame& df, unsigned int target_rname, unsigned int target_start, unsigned int target_end, signed int min_overlap, std::string& ctx, double min_ctx_freq, bool clip, unsigned int reverse_offset);
RcppExport SEXP _epialleleR_rcpp_extract_patterns(SEXP dfSEXP, SEXP target_rnameSEXP, SEXP target_startSEXP, SEXP target_endSEXP, SEXP min_overlapSEXP, SEXP ctxSEXP, SEXP min_ctx_freqSEXP, SEXP clipSEXP, SEXP reverse_offsetSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type df(dfSEXP);
    Rcpp::traits::input_parameter< unsigned int >::type target_rname(target_rnameSEXP);
    Rcpp::traits::input_parameter< unsigned int >::type target_start(target_startSEXP);
    Rcpp::traits::input_parameter< unsigned int >::type target_end(target_endSEXP);
    Rcpp::traits::input_parameter< signed int >::type min_overlap(min_overlapSEXP);
    Rcpp::traits::input_parameter< std::string& >::type ctx(ctxSEXP);
    Rcpp::traits::input_parameter< double >::type min_ctx_freq(min_ctx_freqSEXP);
    Rcpp::traits::input_parameter< bool >::type clip(clipSEXP);
    Rcpp::traits::input_parameter< unsigned int >::type reverse_offset(reverse_offsetSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_extract_patterns(df, target_rname, target_start, target_end, min_overlap, ctx, min_ctx_freq, clip, reverse_offset));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_fep
std::vector<double> rcpp_fep(Rcpp::DataFrame& df, std::vector<std::string> colnames);
RcppExport SEXP _epialleleR_rcpp_fep(SEXP dfSEXP, SEXP colnamesSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type df(dfSEXP);
    Rcpp::traits::input_parameter< std::vector<std::string> >::type colnames(colnamesSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_fep(df, colnames));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_get_base_freqs
Rcpp::NumericMatrix rcpp_get_base_freqs(Rcpp::DataFrame& df, std::vector<bool> pass, Rcpp::DataFrame& vcf);
RcppExport SEXP _epialleleR_rcpp_get_base_freqs(SEXP dfSEXP, SEXP passSEXP, SEXP vcfSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type df(dfSEXP);
    Rcpp::traits::input_parameter< std::vector<bool> >::type pass(passSEXP);
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type vcf(vcfSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_get_base_freqs(df, pass, vcf));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_get_xm_beta
std::vector<double> rcpp_get_xm_beta(Rcpp::DataFrame& df, std::string ctx_meth, std::string ctx_unmeth);
RcppExport SEXP _epialleleR_rcpp_get_xm_beta(SEXP dfSEXP, SEXP ctx_methSEXP, SEXP ctx_unmethSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type df(dfSEXP);
    Rcpp::traits::input_parameter< std::string >::type ctx_meth(ctx_methSEXP);
    Rcpp::traits::input_parameter< std::string >::type ctx_unmeth(ctx_unmethSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_get_xm_beta(df, ctx_meth, ctx_unmeth));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_match_amplicon
std::vector<int> rcpp_match_amplicon(Rcpp::DataFrame& df, Rcpp::DataFrame& bed, int tolerance);
RcppExport SEXP _epialleleR_rcpp_match_amplicon(SEXP dfSEXP, SEXP bedSEXP, SEXP toleranceSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type df(dfSEXP);
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type bed(bedSEXP);
    Rcpp::traits::input_parameter< int >::type tolerance(toleranceSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_match_amplicon(df, bed, tolerance));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_match_capture
std::vector<int> rcpp_match_capture(Rcpp::DataFrame& df, Rcpp::DataFrame& bed, signed int min_overlap);
RcppExport SEXP _epialleleR_rcpp_match_capture(SEXP dfSEXP, SEXP bedSEXP, SEXP min_overlapSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type df(dfSEXP);
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type bed(bedSEXP);
    Rcpp::traits::input_parameter< signed int >::type min_overlap(min_overlapSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_match_capture(df, bed, min_overlap));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_read_bam_paired
Rcpp::DataFrame rcpp_read_bam_paired(std::string fn, int min_mapq, int min_baseq, bool skip_duplicates, int nthreads);
RcppExport SEXP _epialleleR_rcpp_read_bam_paired(SEXP fnSEXP, SEXP min_mapqSEXP, SEXP min_baseqSEXP, SEXP skip_duplicatesSEXP, SEXP nthreadsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type fn(fnSEXP);
    Rcpp::traits::input_parameter< int >::type min_mapq(min_mapqSEXP);
    Rcpp::traits::input_parameter< int >::type min_baseq(min_baseqSEXP);
    Rcpp::traits::input_parameter< bool >::type skip_duplicates(skip_duplicatesSEXP);
    Rcpp::traits::input_parameter< int >::type nthreads(nthreadsSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_read_bam_paired(fn, min_mapq, min_baseq, skip_duplicates, nthreads));
    return rcpp_result_gen;
END_RCPP
}
// rcpp_threshold_reads
std::vector<bool> rcpp_threshold_reads(Rcpp::DataFrame& df, std::string ctx_meth, std::string ctx_unmeth, std::string ooctx_meth, std::string ooctx_unmeth, unsigned int min_n_ctx, double min_ctx_meth_frac, double max_ooctx_meth_frac);
RcppExport SEXP _epialleleR_rcpp_threshold_reads(SEXP dfSEXP, SEXP ctx_methSEXP, SEXP ctx_unmethSEXP, SEXP ooctx_methSEXP, SEXP ooctx_unmethSEXP, SEXP min_n_ctxSEXP, SEXP min_ctx_meth_fracSEXP, SEXP max_ooctx_meth_fracSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::DataFrame& >::type df(dfSEXP);
    Rcpp::traits::input_parameter< std::string >::type ctx_meth(ctx_methSEXP);
    Rcpp::traits::input_parameter< std::string >::type ctx_unmeth(ctx_unmethSEXP);
    Rcpp::traits::input_parameter< std::string >::type ooctx_meth(ooctx_methSEXP);
    Rcpp::traits::input_parameter< std::string >::type ooctx_unmeth(ooctx_unmethSEXP);
    Rcpp::traits::input_parameter< unsigned int >::type min_n_ctx(min_n_ctxSEXP);
    Rcpp::traits::input_parameter< double >::type min_ctx_meth_frac(min_ctx_meth_fracSEXP);
    Rcpp::traits::input_parameter< double >::type max_ooctx_meth_frac(max_ooctx_meth_fracSEXP);
    rcpp_result_gen = Rcpp::wrap(rcpp_threshold_reads(df, ctx_meth, ctx_unmeth, ooctx_meth, ooctx_unmeth, min_n_ctx, min_ctx_meth_frac, max_ooctx_meth_frac));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_epialleleR_rcpp_cx_report", (DL_FUNC) &_epialleleR_rcpp_cx_report, 3},
    {"_epialleleR_rcpp_extract_patterns", (DL_FUNC) &_epialleleR_rcpp_extract_patterns, 9},
    {"_epialleleR_rcpp_fep", (DL_FUNC) &_epialleleR_rcpp_fep, 2},
    {"_epialleleR_rcpp_get_base_freqs", (DL_FUNC) &_epialleleR_rcpp_get_base_freqs, 3},
    {"_epialleleR_rcpp_get_xm_beta", (DL_FUNC) &_epialleleR_rcpp_get_xm_beta, 3},
    {"_epialleleR_rcpp_match_amplicon", (DL_FUNC) &_epialleleR_rcpp_match_amplicon, 3},
    {"_epialleleR_rcpp_match_capture", (DL_FUNC) &_epialleleR_rcpp_match_capture, 3},
    {"_epialleleR_rcpp_read_bam_paired", (DL_FUNC) &_epialleleR_rcpp_read_bam_paired, 5},
    {"_epialleleR_rcpp_threshold_reads", (DL_FUNC) &_epialleleR_rcpp_threshold_reads, 8},
    {NULL, NULL, 0}
};

RcppExport void R_init_epialleleR(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}

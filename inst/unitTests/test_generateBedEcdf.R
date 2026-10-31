test_generateBedEcdf <- function () {
  amplicon.ecdfs <- generateBedEcdf(
    bam=system.file("extdata", "amplicon010meth.bam", package="epialleleR"),
    bed=system.file("extdata", "amplicon.bed", package="epialleleR"),
    bed.rows=c(1,2), verbose=TRUE
  )
  
  RUnit::checkEquals(
    sapply(unlist(amplicon.ecdfs, use.names=FALSE), function (x) {x(0.5)}),
    c(0.914473684211, 1, 0.883333333333, 1),
    tolerance=1e-08
  )
  
  amplicon.ecdfs <- generateBedEcdf(
    bam=system.file("extdata", "amplicon010meth.bam", package="epialleleR"),
    bed=system.file("extdata", "amplicon.bed", package="epialleleR"),
    min.mapq=30, min.baseq=20,
    bed.rows=NULL, verbose=TRUE
  )
  
  RUnit::checkEquals(
    sapply(unlist(amplicon.ecdfs, use.names=FALSE), function (x) {x(0.5)}),
    c(0.914473684211, 1, 0.883333333333, 1, 0.945652173913, 1,
      0.898734177215, 1, 0.872549019608, 1),
    tolerance=1e-08
  )
}

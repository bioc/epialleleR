test_generateMhlReport <- function () {
  capture.bam <- system.file("extdata", "capture.bam", package="epialleleR")
  
  generateMhlReport(capture.bam, report.file=tempfile())
  generateMhlReport(preprocessBam(capture.bam), report.file=tempfile())
  
  RUnit::checkTrue(
    identical(
      generateMhlReport(capture.bam, max.haplotype.window=1)[, lmhl],
      generateCytosineReport(capture.bam, threshold.reads=FALSE)[, meth/(meth+unmeth)]
    )
  )
  
  # amplicon 10%
  amplicon.bam <- system.file("extdata", "amplicon010meth.bam", package="epialleleR")
  
  mhl.report <- generateMhlReport(amplicon.bam)
  RUnit::checkEquals(
    sum(mhl.report$coverage),
    7081
  )
  RUnit::checkEquals(
    c(sum(mhl.report$length), sum(mhl.report$lmhl)),
    c(6060.46765, 45.78637)
  )
  
  # amplicon 100%
  amplicon.bam <- system.file("extdata", "amplicon100meth.bam", package="epialleleR")
  RUnit::checkEquals(
    generateMhlReport(amplicon.bam, max.haplotype.window=1,
                      min.mapq=30, min.baseq=20)[, lmhl],
    generateCytosineReport(amplicon.bam, threshold.reads=FALSE,
                           min.mapq=30, min.baseq=20)[, meth/(meth+unmeth)],
    tolerance=0.022992 # because sequencing errors affect lMHL but not beta
  )
  
  # simulated
  out.bam <- tempfile(pattern="simulated", fileext=".bam")
  simulateBam(
    output.bam.file=out.bam,
    cigar=c("10000M1H"),
    XM=c(
      paste(sample(c("Z",rep("z", 9)), 10000, replace=TRUE), collapse=""),
      paste(sample(c("Z",rep("z", 9)), 10000, replace=TRUE), collapse="")
    ),
    XG=c("CT")
  )
  cg.beta <- generateCytosineReport(out.bam, threshold.reads=FALSE)
  mhl.report <- generateMhlReport(out.bam, max.haplotype.window=1)
  RUnit::checkEquals(
    c(sum(mhl.report$coverage), sum(mhl.report$length)),
    c(20000, 100000000)
  )
  RUnit::checkIdentical(
    mhl.report[, lmhl],
    cg.beta[, meth/(meth+unmeth)]
  )
}
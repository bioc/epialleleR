# Fast, epiallele-aware methylation<br> caller and reporter <a href="https://github.com/BBCG/epialleleR"><img src="vignettes/epialleleR_logo.svg" alt="logo" align="right" height="50%" class="pull-right" /></a>

[![](https://github.com/BBCG/epialleleR/workflows/R-CMD-check-bioc/badge.svg)](https://github.com/BBCG/epialleleR/actions)
[![](https://codecov.io/gh/BBCG/epialleleR/branch/devel/graph/badge.svg)](https://app.codecov.io/gh/BBCG/epialleleR/tree/devel)
[![](https://bioconductor.org/shields/years-in-bioc/epialleleR.svg)](https://bioconductor.org/packages/release/bioc/html/epialleleR.html)
[![install with bioconda](https://img.shields.io/badge/install%20with-bioconda-brightgreen.svg?style=flat)](http://bioconda.github.io/recipes/bioconductor-epialleler/README.html)

## Introduction

*`epialleleR`* is an R package for calling and reporting cytosine methylation
and hypermethylated variant epiallele frequencies (VEF) at the level of
genomic regions or individual cytosines
in next-generation sequencing data using binary alignment map (BAM) files as
an input. See below for additional functionality.

![](./vignettes/epialleles.png)

### Current Features

 * calling cytosine methylation and saving calls in BAM file
 (*`callMethylation`*)
 * creating sample BAM files given mandatory and optional BAM fields
 (*`simulateBam`*)
 * conventional reporting of cytosine methylation (*`generateCytosineReport`*)
 * reporting the hypermethylated variant epiallele frequency (VEF) at the
 level of genomic regions (*`generate[Bed|Amplicon|Capture]Report`*) or
 individual cytosines (*`generateCytosineReport`*)
 * reporting linearised Methylated Haplotype Load (lMHL, 
 *`generateMhlReport`*)
 * extracting methylation patterns for genomic region of interest
 (*`extractPatterns`*)
 * visualising methylation patterns (*`plotPatterns`*)
 * testing for the association between epiallele methylation
 status and sequence variations (*`generateVcfReport`*)
 * assessing the distribution of per-read beta values for genomic regions of
 interest (*`generateBedEcdf`*)
 
### Recent improvements

##### v1.14 [BioC 3.20]

 * creates pretty plots of methylation patterns

##### v1.12 [BioC 3.19]

 * inputs long-read sequencing alignments
 * full support for short-read sequencing alignments by Illumina DRAGEN,
 Bismark, bwa-meth, BSMAP
 * RRBS-specific options
 * lower memory usage

##### v1.10 [BioC 3.18]

 * inputs both single-end and paired-end sequencing alignments
 * makes and stores methylation calls
 * creates sample BAM files
 * reports linearised MHL

##### v1.4 [BioC 3.15]

 * significant speed-up
 * method to extract and visualize methylation patterns

##### v1.2 [BioC 3.14]

 * even faster and more memory-efficient BAM loading (by means of HTSlib)
 * min.baseq parameter to reduce the effect of low quality bases on 
 methylation or SNV calling (in v1.0 the output of *`generateVcfReport`* was
 equivalent to the one of `samtools mpileup -Q 0 ...`)

check out NEWS for more!
 
-------

## Installation

### install via Bioconductor
```r
if (!requireNamespace("BiocManager", quietly = TRUE))
    install.packages("BiocManager")

BiocManager::install("epialleleR")
```

### Install the latest version via install_github
```r
library(devtools)
install_github("BBCG/epialleleR", build_vignettes=FALSE,
  repos=BiocManager::repositories(),
  dependencies=TRUE, type="source")
```

-------

## Using the package

Please read *`epialleleR`* vignette
at [GitHub pages](https://bbcg.github.io/epialleleR/articles/epialleleR.html)
or within the R environment: `vignette("epialleleR", package="epialleleR")`, or
consult the function's help pages for the extensive information on usage,
parameters and output values.

Comparison of beta, VEF and lMHL values for various use cases is given by the
[values](https://bbcg.github.io/epialleleR/articles/values.html)
vignette (`vignette("values", package="epialleleR")`)

Very brief synopsis:

```r
library(epialleleR)

# make methylation calls if necessary
callMethylation(
  input.bam.file=system.file("extdata", "test", "dragen-se-unsort-xg.bam", package="epialleleR"),
  output.bam.file=tempfile(pattern="output-", fileext=".bam"),
  genome=system.file("extdata", "test", "reference.fasta.gz", package="epialleleR")
)

# make a sample BAM file from scratch
simulateBam(output.bam.file=tempfile(pattern="simulated-", fileext=".bam"),
            pos=c(1, 2), XM=c("ZZZzzZZZ", "ZZzzzzZZ"), XG=c("CT", "AG"))

# or use external files
amplicon.bam <- system.file("extdata", "amplicon010meth.bam",
                            package="epialleleR")
amplicon.bed <- system.file("extdata", "amplicon.bed", package="epialleleR")
amplicon.vcf <- system.file("extdata", "amplicon.vcf.gz", package="epialleleR")

# preload the data
bam.data <- preprocessBam(amplicon.bam)

# methylation patterns and their plot
patterns <- extractPatterns(bam=amplicon.bam, bed=amplicon.bed, bed.row=3)
plotPatterns(patterns)

# CpG VEF report for individual bases
cg.vef.report <- generateCytosineReport(bam.data)

# BED-guided VEF report for genomic ranges
bed.report <- generateBedReport(bam=amplicon.bam, bed=amplicon.bed,
                                bed.type="capture")

# VCF report
vcf.report <- generateVcfReport(bam=amplicon.bam, bed=amplicon.bed,
                                vcf=amplicon.vcf, vcf.style="NCBI")

# lMHL report
mhl.report <- generateMhlReport(bam=amplicon.bam)
```

-------

### Citing the *`epialleleR`* package
Oleksii Nikolaienko, Per Eystein Lønning, Stian Knappskog, *epialleleR*: an R/Bioconductor package for sensitive allele-specific methylation analysis in NGS data. *GigaScience*, Volume 12, 2023, giad087, [https://doi.org/10.1093/gigascience/giad087](https://doi.org/10.1093/gigascience/giad087).
Data: [GSE201690](https://www.ncbi.nlm.nih.gov/geo/query/acc.cgi?acc=GSE201690)

### Our experimental studies that use the package
Per Eystein Lonning, Oleksii Nikolaienko, Kathy Pan, Allison W. Kurian, Hans Petter Petter Eikesdal, Mary Pettinger, Garnet L Anderson, Ross L Prentice, Rowan T. Chlebowski, and Stian Knappskog. Constitutional *BRCA1* methylation and risk of incident triple-negative breast cancer and high-grade serous ovarian cancer. *JAMA Oncology* 2022. [https://doi.org/10.1001/jamaoncol.2022.3846](https://doi.org/10.1001/jamaoncol.2022.3846)

Oleksii Nikolaienko, Hans P. Eikesdal, Elisabet Ognedal, Bjørnar Gilje, Steinar Lundgren, Egil S. Blix, Helge Espelid, Jürgen Geisler, Stephanie Geisler, Emiel A.M. Janssen, Synnøve Yndestad, Laura Minsaas, Beryl Leirvaag, Reidun Lillestøl, Stian Knappskog, Per E. Lønning. Prenatal *BRCA1* epimutations contribute significantly to triple-negative breast cancer development. *Genome Medicine* 2023. [https://doi.org/10.1186/s13073-023-01262-8](https://doi.org/10.1186/s13073-023-01262-8).
Data: [GSE243966](https://www.ncbi.nlm.nih.gov/geo/query/acc.cgi?acc=GSE243966)

### *`epialleleR`* at Bioconductor
[release](https://bioconductor.org/packages/release/bioc/html/epialleleR.html), 
[development version](https://bioconductor.org/packages/devel/bioc/html/epialleleR.html)

-------

License
---------
Artistic License 2.0

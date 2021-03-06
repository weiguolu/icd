% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/parse-comorbid.R
\name{icd9_parse_ahrq_sas}
\alias{icd9_parse_ahrq_sas}
\title{parse AHRQ SAS code to get mapping}
\usage{
icd9_parse_ahrq_sas(save_data = FALSE, offline = TRUE)
}
\arguments{
\item{save_data}{logical whether to save the result in the source tree.
Defaults to \code{FALSE}.}

\item{condense}{Deprecated. The map is not condensed by default. If required,
it can reduced to a smaller set of codes using \code{icd_condense}.}
}
\value{
invisibly returns the list of vectors, where a co-morbidity name is
  associated with a character vector of ICD-9 codes.
}
\description{
Raw data taken directly from the AHRQ web site and parsed. It is then saved
in the development tree data directory, so this is an internal function, used
in generating the package itself.
}
\keyword{internal}
\keyword{manip}

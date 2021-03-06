% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/score.R
\name{icd_count_codes}
\alias{icd_count_codes}
\title{Count ICD codes or comorbidities for each patient}
\usage{
icd_count_codes(x, visit_name = get_visit_name(x), return_df = FALSE)
}
\arguments{
\item{x}{data frame with one row per patient, and a true/false or 1/0 flag
for each column. By default, the first column is the patient identifier and
is not counted. If \code{visit_name} is not specified, the first column is
used.}

\item{visit_name}{The name of the column in the data frame which contains the
patient or visit identifier. Typically this is the visit identifier, since
patients come leave and enter hospital with different ICD-9 codes. It is a
character vector of length one. If left empty, or \code{NULL}, then an
attempt is made to guess which field has the ID for the patient encounter
(not a patient ID, although this can of course be specified directly). The
guesses proceed until a single match is made. Data frames may be wide with
many matching fields, so to avoid false positives, anything but a single
match is rejected. If there are no successful guesses, and \code{visit_id}
was not specified, then the first column of the data frame is used.}

\item{return_df}{single logical, if \code{TRUE}, return the result as a data
frame with the first column being the \code{visit_name}, and the second
being the count. If \code{visit_name} was a factor or named differently in
the input, this is preserved.}
}
\value{
vector of the count of comorbidities for each patient. This is
  sometimes used as a metric of comorbidity load, instead of, or in addition
  to metrics like the Charlson Comorbidity Index (aka Charlson Score)
}
\description{
\code{icd_count_codes} takes a data frame with a column for \code{visit_name}
and another for ICD-9 code, and returns the number of distinct codes for each
patient.
}
\details{
The \code{visit_name} field is typically the first column. If there is no
column called \code{visit_name} and \code{visit_name} is not specified, the
first column is used.
}
\examples{
  mydf <- data.frame(visit_name = c("r", "r", "s"),
                   icd9 = c("441", "412.93", "044.9"))
  icd_count_codes(mydf, return_df = TRUE)
  icd_count_codes(mydf)

  cmb <- icd9_comorbid_quan_deyo(mydf, isShort = FALSE, return_df = TRUE)
  icd_count_comorbid(cmb)

  wide <- data.frame(visit_name = c("r", "s", "t"),
                   icd9_1 = c("0011", "441", "456"),
                   icd9_2 = c(NA, "442", NA),
                   icd9_3 = c(NA, NA, "510"))
  icd_count_codes_wide(wide)
  # or:
  library(magrittr)
  wide \%>\% icd_wide_to_long \%>\% icd_count_codes
}

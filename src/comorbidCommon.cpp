// Copyright (C) 2014 - 2017  Jack O. Wasey
//
// This file is part of icd.
//
// icd is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// icd is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with icd. If not, see <http://www.gnu.org/licenses/>.

// [[Rcpp::interfaces(r, cpp)]]
// [[Rcpp::plugins(openmp)]]
#include "local.h"
#include "util.h"
#include <algorithm>

//' core search for ICD code in a map
//' @keywords internal
// [[Rcpp::export]]
void lookupComorbidByChunkFor(const VecVecInt& vcdb,
                              const VecVecInt& map,
                              const VecVecIntSz chunkSize,
                              const VecVecIntSz ompChunkSize,
                              ComorbidOut& out) {
  const VecVecIntSz num_comorbid = map.size();
  const VecVecIntSz last_i = vcdb.size() - 1;
  VecVecIntSz chunk_end_i;
  VecVecIntSz vis_i;
  const VecVecIntSz vsz = vcdb.size();
#ifdef ICD_DEBUG_TRACE
  Rcpp::Rcout << "vcdb.size() = " << vcdb.size() << "\n";
  Rcpp::Rcout << "map.size() = " << map.size() << "\n";
#endif
#ifdef ICD_DEBUG_PARALLEL
  debug_parallel();
#endif

#ifdef ICD_OPENMP
#pragma omp parallel for schedule(static) default(none) shared(out, Rcpp::Rcout, vcdb, map) private(chunk_end_i, vis_i)
  // SOMEDAY: need to consider other processes using multiple cores, see Writing R Extensions.
  //	omp_set_schedule(omp_sched_static, ompChunkSize);
#endif
  // loop through chunks at a time, by integer size:
  // https://stackoverflow.com/questions/2513988/iteration-through-std-containers-in-openmp
  for (vis_i = 0; vis_i < vsz; vis_i += chunkSize) {
#ifdef ICD_DEBUG_TRACE
    Rcpp::Rcout << "vis_i = " << vis_i << "\n";
#endif
#ifdef ICD_DEBUG_PARALLEL
    debug_parallel();
#endif
    // chunk end is an index, so for zero-based vis_i and chunk_end should be
    // the last index in the chunk
    chunk_end_i = vis_i + chunkSize - 1;
    if (chunk_end_i > last_i)
      chunk_end_i = last_i;
    ComorbidOut chunk;
#ifdef ICD_DEBUG_TRACE
    Rcpp::Rcout << "OMP vcdb.size() = " << vcdb.size() << "\n";
    Rcpp::Rcout << "OMP map.size() = " << map.size() << "\n";
#endif
    const VecVecIntSz& begin = vis_i;
    const VecVecIntSz& end = chunk_end_i;

#ifdef ICD_DEBUG_TRACE
    Rcpp::Rcout << "lookupComorbidChunk begin = " << begin << ", end = " << end << "\n";
#endif
    const ComorbidOut falseComorbidChunk(num_comorbid * (1 + end - begin), false);
    chunk = falseComorbidChunk;
    for (VecVecIntSz urow = begin; urow <= end; ++urow) { //end is index of end of chunk, so we include it in the loop.
#ifdef ICD_DEBUG_TRACE
      Rcpp::Rcout << "row: " << 1 + urow - begin << " of " << 1 + end - begin << "\n";
#endif
      for (VecVecIntSz cmb = 0; cmb < num_comorbid; ++cmb) { // loop through icd codes for this visitId
#ifdef ICD_DEBUG_TRACE
        Rcpp::Rcout << "cmb = " << cmb << "\n";
        Rcpp::Rcout << "vcdb length in lookupOneChunk = " << vcdb.size() << "\n";
        Rcpp::Rcout << "map length in lookupOneChunk = " << map.size() << "\n";
#endif

        const VecInt& codes = vcdb[urow]; // these are the ICD-9 codes for the current visitid
        const VecInt& mapCodes = map[cmb]; // may be zero length

        const VecInt::const_iterator cbegin = codes.begin();
        const VecInt::const_iterator cend = codes.end();
        for (VecInt::const_iterator code_it = cbegin; code_it != cend;
        ++code_it) {
          bool found_it = std::binary_search(mapCodes.begin(),
                                             mapCodes.end(), *code_it);
          if (found_it) {
            const ComorbidOut::size_type chunk_idx = num_comorbid
            * (urow - begin) + cmb;
#ifdef ICD_DEBUG
            chunk.at(chunk_idx) = true;
#else
            chunk[chunk_idx] = true;
#endif
            break;
          } // end if found_it
        } // end loop through codes in one comorbidity
      } // end loop through all comorbidities
    } // end loop through visits
#ifdef ICD_DEBUG_TRACE
    Rcpp::Rcout << "finished with one chunk\n";
#endif

    // next block doesn't need to be single threaded(?), but doing so improves
    // cache contention
#ifdef ICD_OPENMP
#pragma omp critical
#endif
{
#ifdef ICD_DEBUG_TRACE
  Rcpp::Rcout << "writing a chunk beginning at: " << vis_i << "\n";
#endif
  // write calculated data to the output matrix (must sync threads before this)
  std::copy(chunk.begin(), chunk.end(),
            out.begin() + (num_comorbid * vis_i));
}
  } // end parallel for

#ifdef ICD_DEBUG
  Rcpp::Rcout << "finished looking up all chunks in for loop\n";
#endif
}

ComorbidOut lookupComorbidByChunkFor(const VecVecInt& vcdb, const VecVecInt& map,
                                     const int chunkSize, const int ompChunkSize) {
  // initialize output matrix with all false for all comorbidities
  ComorbidOut out(vcdb.size() * map.size(), false);
#ifdef ICD_DEBUG_TRACE
  Rcpp::Rcout << "top level vcdb.size() = " << vcdb.size() << "\n";
  Rcpp::Rcout << "top level map.size() = " << map.size() << "\n";
#endif
  //TODO: pass the output by reference, instead?
  lookupComorbidByChunkFor(vcdb, map, chunkSize, ompChunkSize, out);
  return out;
}


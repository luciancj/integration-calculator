#ifndef ENGINE_CSV_PROCESSOR_H
#define ENGINE_CSV_PROCESSOR_H

#include "../integration/integrator.h"
#include <iostream>
#include <string>
#include <vector>

namespace engine {

// One successfully processed CSV row.
struct CsvRow {
  std::string function;
  double a = 0.0;
  double b = 0.0;
  int segments = 0;
  double area = 0.0;
};

// One row that could not be processed.
struct CsvRowError {
  int lineNum = 0;
  std::string line;
  std::string reason; // "malformed", "bounds", "parse", or "eval"
};

// Summary returned by processCsv: how many rows were processed, the per-row
// results (for callers that want the data, e.g. the GUI history), and any
// per-row errors.
struct CSVResult {
  int processed = 0;
  std::vector<CsvRow> rows;
  std::vector<CsvRowError> errors;
};

// Process a CSV file: read rows, integrate each function, write result rows to
// `out`, and return a summary (processed count + rows + errors).
CSVResult processCsv(std::istream &in, std::ostream &out, IntegrationMethod method);

} // namespace engine

#endif

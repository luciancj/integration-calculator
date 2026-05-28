#include "processor.h"
#include "../expression/exception.h"
#include "../expression/parser.h"
#include "../integration/stats.h"
#include <iomanip>

namespace engine {

// Find the position of a comma in the line, starting from 'start'.
// Returns -1 if not found.
static int findComma(const std::string &line, int start) {
  for (int i = start; i < (int)line.size(); i++) {
    if (line[i] == ',')
      return i;
  }
  return -1;
}

// Tells the caller what happened when we tried to read one CSV row.
//   OK      - all three fields were read and the bounds are numbers
//   SKIP    - the bounds are not numbers (e.g. a title row), so skip it
//   INVALID - the line does not have two commas, so it cannot be a row
enum class RowKind { OK, SKIP, INVALID };

// Split one CSV line into function, a and b. See RowKind for the meaning
// of each return value.
static RowKind parseRow(const std::string &line, std::string &function, double &a, double &b) {
  int pos1 = findComma(line, 0);
  if (pos1 < 0)
    return RowKind::INVALID;
  int pos2 = findComma(line, pos1 + 1);
  if (pos2 < 0)
    return RowKind::INVALID;

  function = line.substr(0, pos1);
  std::string aStr = line.substr(pos1 + 1, pos2 - pos1 - 1);
  std::string bStr = line.substr(pos2 + 1);

  // If the bounds are not numbers, treat the line as a header row.
  auto aVal = evaluateConstant(aStr);
  auto bVal = evaluateConstant(bStr);
  if (!aVal || !bVal)
    return RowKind::SKIP;

  a = *aVal;
  b = *bVal;
  return RowKind::OK;
}

CSVResult processCsv(std::istream &in, std::ostream &out, IntegrationMethod method) {
  out << "Function,Interval Start,Interval End,Segments,Area,"
         "Min Value,Max Value,Mean Value,RMS Value,Delta\n";

  CSVResult result;
  std::string line;
  int lineNum = 0;

  while (std::getline(in, line)) {
    lineNum++;
    if (line.empty())
      continue;

    std::string function;
    double a = 0.0, b = 0.0;
    RowKind kind = parseRow(line, function, a, b);

    // Header rows are skipped without writing an error marker.
    if (kind == RowKind::SKIP)
      continue;

    // A line without two commas cannot be read at all.
    if (kind == RowKind::INVALID) {
      out << "INVALID FORMAT - " << line << ",,,,,,,,,\n";
      result.errors.push_back({lineNum, line, "malformed"});
      continue;
    }

    // The interval must go from a smaller value to a larger one.
    if (a >= b) {
      out << function << ",INVALID BOUNDS,,,,,,,,\n";
      result.errors.push_back({lineNum, line, "bounds"});
      continue;
    }

    // The function text must parse into an expression tree.
    Node *root = nullptr;
    try {
      root = parseExpression(function);
    } catch (const ParseException &) {
      out << function << ",PARSE ERROR,,,,,,,,\n";
      result.errors.push_back({lineNum, line, "parse"});
      continue;
    }

    // Integrate and calculate statistics. Evaluation can still fail here if the
    // function calls something the engine does not know (e.g. a typo).
    try {
      int segments = 0;
      double area = integrate(method, root, a, b, 0.0001, segments);
      IntegrationStats stats = calculateStats(root, a, b, segments, area);

      out << std::fixed << std::setprecision(4) << function << "," << std::setprecision(2) << a
          << "," << b << "," << segments << "," << std::setprecision(4) << stats.getArea() << ","
          << stats.getMinVal() << "," << stats.getMaxVal() << "," << stats.getMeanVal() << ","
          << stats.getRmsVal() << "," << stats.getDelta() << "\n";

      result.rows.push_back({function, a, b, segments, stats.getArea()});
      result.processed++;
    } catch (const EvaluationException &) {
      out << function << ",EVAL ERROR,,,,,,,,\n";
      result.errors.push_back({lineNum, line, "eval"});
    }
    delete root;
  }

  return result;
}

} // namespace engine

#include "utils.h"
#include "engine/csv/processor.h"
#include "engine/integration/integrator.h"
#include <sstream>
#include <string>

static bool testValidRows() {
  std::istringstream in("x,0,2\nx^2,0,1\n");
  std::ostringstream out;
  engine::CSVResult result = engine::processCsv(in, out, engine::IntegrationMethod::Trapezoidal);

  bool ok = true;
  ok &= checkBool(result.processed == 2, "csv: 2 rows processed");
  ok &= checkBool(result.errors.empty(), "csv: no errors");
  if (result.rows.size() >= 2) {
    ok &= check(approxEqual(result.rows[0].area, 2.0,       1e-2), "csv row 0: x over [0,2] = 2",      result.rows[0].area, 2.0);
    ok &= check(approxEqual(result.rows[1].area, 1.0 / 3.0, 1e-2), "csv row 1: x^2 over [0,1] = 1/3", result.rows[1].area, 1.0 / 3.0);
  }
  return ok;
}

static bool testMalformedRow() {
  std::istringstream in("not_a_valid_row\n");
  std::ostringstream out;
  engine::CSVResult result = engine::processCsv(in, out, engine::IntegrationMethod::Simpsons);

  bool ok = true;
  ok &= checkBool(result.processed == 0,  "csv malformed: 0 rows processed");
  ok &= checkBool(!result.errors.empty(), "csv malformed: error recorded");
  return ok;
}

static bool testEmptyInput() {
  std::istringstream in("");
  std::ostringstream out;
  engine::CSVResult result = engine::processCsv(in, out, engine::IntegrationMethod::Simpsons);

  bool ok = true;
  ok &= checkBool(result.processed == 0, "csv empty: 0 rows processed");
  ok &= checkBool(result.errors.empty(), "csv empty: no errors");
  return ok;
}

static bool testInvalidBounds() {
  // a >= b should be rejected
  std::istringstream in("x,5,1\n");
  std::ostringstream out;
  engine::CSVResult result = engine::processCsv(in, out, engine::IntegrationMethod::Simpsons);

  bool ok = true;
  ok &= checkBool(result.processed == 0,  "csv bad bounds: 0 rows processed");
  ok &= checkBool(!result.errors.empty(), "csv bad bounds: error recorded");
  return ok;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "usage: csv_tests <test-name>\n";
    return 2;
  }

  std::string name = argv[1];

  // clang-format off
  if (name == "valid_rows")     return testValidRows()     ? 0 : 1;
  if (name == "malformed_row")  return testMalformedRow()  ? 0 : 1;
  if (name == "empty_input")    return testEmptyInput()    ? 0 : 1;
  if (name == "invalid_bounds") return testInvalidBounds() ? 0 : 1;
  // clang-format on

  std::cerr << "unknown test: " << name << "\n";
  return 2;
}

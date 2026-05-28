#include "stats.h"
#include <cmath>
#include <iomanip>

namespace engine {

// Computes statistics (Min, Max, Mean, RMS, Delta)
IntegrationStats calculateStats(const Node *root, double lowerLimit, double upperLimit,
                                int segmentCount, double area) {
  IntegrationStats stats;
  stats.setSegmentCount(segmentCount);
  stats.setArea(area);
  stats.setMinVal(root->evaluate(lowerLimit));
  stats.setMaxVal(stats.getMinVal());
  stats.setMeanVal(0.0);
  stats.setRmsVal(0.0);

  double stepSize = (upperLimit - lowerLimit) / segmentCount;

  for (int i = 0; i <= segmentCount; i++) {
    double x = lowerLimit + i * stepSize;
    double y = root->evaluate(x);

    if (y < stats.getMinVal())
      stats.setMinVal(y);
    if (y > stats.getMaxVal())
      stats.setMaxVal(y);

    stats.setMeanVal(stats.getMeanVal() + y);
    stats.setRmsVal(stats.getRmsVal() + y * y);
  }

  stats.setMeanVal(stats.getMeanVal() / (segmentCount + 1));
  stats.setRmsVal(std::sqrt(stats.getRmsVal() / (segmentCount + 1)));
  stats.setDelta(root->evaluate(upperLimit) - root->evaluate(lowerLimit));

  return stats;
}

bool IntegrationStats::isFinite() const {
  return std::isfinite(area_) && std::isfinite(minVal_) && std::isfinite(maxVal_) &&
         std::isfinite(meanVal_) && std::isfinite(rmsVal_) && std::isfinite(delta_);
}

// OPERATOR OVERLOADING (global): ostream << IntegrationStats.
// Used for clean serialized output (e.g. writing to files).
std::ostream &operator<<(std::ostream &os, const IntegrationStats &stats) {
  os << std::fixed << std::setprecision(6);
  os << "Result Area:        " << stats.area_ << std::endl;
  os << "Min Value:          " << stats.minVal_ << std::endl;
  os << "Max Value:          " << stats.maxVal_ << std::endl;
  os << "Mean Value:         " << stats.meanVal_ << std::endl;
  os << "Root Mean Square:   " << stats.rmsVal_ << std::endl;
  os << "Delta (f(b)-f(a)):  " << stats.delta_ << std::endl;
  os << "Segments:           " << stats.segmentCount_ << std::endl;
  return os;
}

} // namespace engine

#ifndef ENGINE_INTEGRATION_STATS_H
#define ENGINE_INTEGRATION_STATS_H

#include "../expression/tree.h"
#include <iostream>

namespace engine {

// Holds the summary numbers produced for one integration: the area plus a few
// statistics sampled across the interval (min, max, mean, RMS, delta).
class IntegrationStats {
public:
  double getResult() const {
    return result_;
  }
  double getArea() const {
    return area_;
  }
  double getMinVal() const {
    return minVal_;
  }
  double getMaxVal() const {
    return maxVal_;
  }
  double getMeanVal() const {
    return meanVal_;
  }
  double getRmsVal() const {
    return rmsVal_;
  }
  double getDelta() const {
    return delta_;
  }
  int getSegmentCount() const {
    return segmentCount_;
  }

  void setResult(double value) {
    result_ = value;
  }
  void setArea(double value) {
    area_ = value;
  }
  void setMinVal(double value) {
    minVal_ = value;
  }
  void setMaxVal(double value) {
    maxVal_ = value;
  }
  void setMeanVal(double value) {
    meanVal_ = value;
  }
  void setRmsVal(double value) {
    rmsVal_ = value;
  }
  void setDelta(double value) {
    delta_ = value;
  }
  void setSegmentCount(int value) {
    segmentCount_ = value;
  }

  // True only if every reported value is a finite number. A non-finite value
  // means the function blew up somewhere in the interval (e.g. a singularity
  // such as division by zero), so the result is not meaningful.
  bool isFinite() const;

  // Grants the stream operator access to private fields so it can format
  // the value directly without going through every getter.
  friend std::ostream &operator<<(std::ostream &os, const IntegrationStats &stats);

private:
  double result_ = 0.0;
  double area_ = 0.0;
  double minVal_ = 0.0;
  double maxVal_ = 0.0;
  double meanVal_ = 0.0;
  double rmsVal_ = 0.0;
  double delta_ = 0.0;
  int segmentCount_ = 0;
};

// Samples the function across the interval to fill in the statistics for a
// result whose area and segment count are already known.
IntegrationStats calculateStats(const Node *root, double lowerLimit, double upperLimit,
                                int segmentCount, double area);

// OPERATOR OVERLOADING (global): ostream << IntegrationStats.
std::ostream &operator<<(std::ostream &os, const IntegrationStats &stats);

} // namespace engine

#endif // ENGINE_INTEGRATION_STATS_H

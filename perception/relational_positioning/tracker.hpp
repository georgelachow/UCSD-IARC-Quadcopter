#ifndef TRACKER_H
#define TRACKER_H
#include "roomba.hpp"
#include "helpers.hpp"
#include <opencv2/opencv.hpp>

class RoombaTracker{
public:
  virtual void track(const cv::Mat src) = 0;
  virtual void draw(cv::Mat& dst) = 0;
};

class ThresholdTracker: public RoombaTracker{
public:
  ThresholdTracker();
  ~ThresholdTracker();
  void track(cv::Mat src);
  void draw(cv::Mat& dst);
  void setLower(int v1, int v2, int v3);
  void setUpper(int v1, int v2, int v3);

private:
  // Threshold values
  std::vector<int> lower_threshold;
  std::vector<int> upper_threshold;

  // Tracked Roombas and settings
  std::vector<Roomba*> trackedRoombas;
  int areaThreshold;
  double distThreshold;
};
#endif /* TRACKER_H */

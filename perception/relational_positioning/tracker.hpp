#ifndef TRACKER_H
#define TRACKER_H
#include "roomba.hpp"
#include "helpers.hpp"
#include <opencv2/opencv.hpp>

class Distribution{
public:
  Distribution(int x, int y);
  ~Distribution();
  void show(const char* winName);
  void decay();

  cv::Mat distribution;
  unsigned int decayVal;
};

class RoombaTracker{
public:
  virtual void track(const cv::Mat src) = 0;
  virtual void draw(cv::Mat dst) = 0;
};

class ThresholdTracker: public RoombaTracker{
public:
  ThresholdTracker();
  ~ThresholdTracker();

  // Track (Update function on tracker)
  void track(cv::Mat src);

  // Draw (More of a debug feature)
  void draw(cv::Mat dst);

  // Clean up roombas
  void removeDead();
  void decayTrackedRoombas();
  void setLower(int v1, int v2, int v3);
  void setUpper(int v1, int v2, int v3);

  std::vector<Roomba*> trackedRoombas;
  cv::Mat threshFrame;
private:
  // Threshold values
  std::vector<int> lower_threshold;
  std::vector<int> upper_threshold;

  // Hyperparameters
  int areaThreshold;
  double distThreshold;
};

class KFTracker: public RoombaTracker{
public:
  KFTracker();
  ~KFTracker();

  void track(cv::Mat src);
  void draw(cv::Mat dst);

  std::vector<Roomba*> trackedRoombas;
private:

};
#endif /* TRACKER_H */

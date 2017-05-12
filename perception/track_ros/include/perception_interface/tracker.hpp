#ifndef TRACKER_H
#define TRACKER_H
#include <perception_interface/roomba.hpp>
#include <perception_interface/helpers.hpp>
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

class Tracker{
public:
  virtual void track(const cv::Mat src) = 0;
  virtual void draw(cv::Mat dst) = 0;

  int frame_count;
};

class ThresholdTracker: public Tracker{
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
  cv::Mat ROI;
private:
  // Threshold values
  std::vector<int> lower_threshold;
  std::vector<int> upper_threshold;

  // Hyperparameters
  int areaThreshold;
  double distThreshold;
};

class CircleTracker : public Tracker{
public:
  CircleTracker();
  ~CircleTracker();
  void track(cv::Mat src);
  void draw(cv::Mat dst);

  std::vector<Roomba*> trackedRoombas;
  std::vector<cv::Vec3f> circles;

};

class GridTracker : public Tracker{
public:
  GridTracker();
  ~GridTracker();
  void track(cv::Mat src);
  void draw(cv::Mat dst);

  cv::Mat grid;
  std::vector<cv::Vec2f> grid_lines;
  std::vector<cv::Vec2f> merged_lines;
  std::vector<cv::Vec2f> intersections;

  int max_lines;
  int thetaThreshold;
  int distThreshold;
private:

};

class KFTracker: public Tracker{
public:
  KFTracker();
  ~KFTracker();

  void track(cv::Mat src);
  void draw(cv::Mat dst);

  std::vector<Roomba*> trackedRoombas;
private:

};
#endif /* TRACKER_H */

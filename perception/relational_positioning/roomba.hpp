#ifndef ROOMBA_H
#define ROOMBA_H
#include <opencv2/opencv.hpp>
#include <vector>
#include <utility>
#include <stdlib.h>
#include "helpers.hpp"

class Roomba{
public:
  Roomba();
  ~Roomba();

  cv::Point2f getScreenLoc();
  void updateScreenLoc(int x, int y);
  void updateTrajectory();
  void drawBound(cv::Mat& dst);
  void drawTrajectory(cv::Mat& dst,int scale);

  cv::Scalar color;
  cv::Rect boundRect;
  int screenLoc_x, screenLoc_y;
  unsigned int life;
  unsigned int locationTrackLength;

  std::vector<cv::Point2f> previousLocations;
  cv::Vec2f trajectory;
};

#endif /* ROOMBA_H */

#ifndef ROOMBA_H
#define ROOMBA_H
#include <opencv2/opencv.hpp>
#include <vector>
#include <utility>
#include <stdlib.h>

class Roomba{
public:
  Roomba();
  ~Roomba();

  void setScreenLoc(int x, int y);
  void drawBound(cv::Mat& dst);

  cv::Scalar color;
  cv::Rect boundRect;

  int screenLoc_x, screenLoc_y;
  std::vector<std::pair<std::vector<int>,std::vector<int> > > trajectory;
};

#endif /* ROOMBA_H */

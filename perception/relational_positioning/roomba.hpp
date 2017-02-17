#include <opencv2/opencv.hpp>
#include <vector>
#include <utility>

class Roomba{
public:
  Roomba();

  cv::Scalar color;
  cv::Rect boundRect;
  std::vector<int> screenLocation;
  std::vector<std::pair<std::vector<int>,std::vector<int> > > trajectory;
};

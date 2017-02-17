#include "tracker.hpp"

using namespace std;
using namespace cv;

ThresholdTracker::ThresholdTracker(){
  // Default Treshold
  lower_threshold = {160,100,0};
  upper_threshold = {179,255,255};
  areaThreshold = 500;
  distThreshold = 50;
}

void ThresholdTracker::track(const cv::Mat src){
  double largestCont;
  int largest_i = 0;
  int center_x, center_y;
  std::vector<std::vector<cv::Point>> contours; // Contours of thresholded
  std::vector<cv::Vec4i> heirarchy;
  vector<Point> contour_poly;                   // Polygon of contour
  Rect boundRect;

  // Find largest contour
  findContours(src, contours, heirarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1,-1));
  largestCont = contourArea(contours[0]);
  for(unsigned int i =0; i < contours.size(); i++){
    double tmp = contourArea(contours[i]);
    if(tmp > largestCont){
      largestCont = tmp;
      largest_i = i;
    }
  }
  approxPolyDP(Mat(contours[largest_i]), contour_poly, 3, true);
  boundRect = boundingRect(Mat(contour_poly));

  // Calculate screen location and see if its a new roomba that was found
	center_x = (boundRect.br().x - boundRect.tl().x) / 2;
	center_y = (boundRect.br().y - boundRect.tl().y) / 2;

  for(auto roomba = trackedRoombas.begin(); roomba != trackedRoombas.end(); roomba++){

  }
}

void ThresholdTracker::draw(cv::Mat& dst){
	// Draw bounding boxes for each tracked Roomba
  for(auto roomba = trackedRoombas.begin(); roomba != trackedRoombas.end(); roomba++)
    rectangle(dst, roomba->boundRect.tl(), roomba->boundRect.br(), roomba->color,2, 8, 0);
}

void ThresholdTracker::setLower(int v1, int v2, int v3){
  lower_threshold.clear();
  lower_threshold.push_back(v1);
  lower_threshold.push_back(v2);
  lower_threshold.push_back(v3);
}

void ThresholdTracker::setUpper(int v1, int v2, int v3){
  upper_threshold.clear();
  upper_threshold.push_back(v1);
  upper_threshold.push_back(v2);
  upper_threshold.push_back(v3);
}

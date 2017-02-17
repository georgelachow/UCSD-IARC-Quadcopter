#include "tracker.hpp"

using namespace std;
using namespace cv;

ThresholdTracker::ThresholdTracker(){
  // Default Treshold
  lower_threshold = {160,100,0};
  upper_threshold = {179,255,255};
  areaThreshold = 500;
  distThreshold = 25;
}

ThresholdTracker::~ThresholdTracker(){
  // Remove all tracked roombas
  for(auto roomba = trackedRoombas.begin(); roomba != trackedRoombas.end(); roomba++){
    delete (*roomba);
    (*roomba) = NULL;
  }

  trackedRoombas.clear();
}

void ThresholdTracker::track(cv::Mat src){
  double largestCont;
  int largest_i = 0;
  int center_x, center_y;
  std::vector<std::vector<cv::Point>> contours; // Contours of thresholded
  std::vector<cv::Vec4i> heirarchy;
  vector<Point> contour_poly;                   // Polygon of contour
  Rect boundRect;

  // Convert to HSV and perform threshold
  cvtColor(src, src, COLOR_BGR2HSV);
	inRange(src, lower_threshold, upper_threshold, src);

  // Find largest contour from thresholded color
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
  center_x = boundRect.br().x - (boundRect.br().x - boundRect.tl().x) / 2;
  center_y = boundRect.tl().y + (boundRect.br().y - boundRect.tl().y) / 2;

  // Temp variable for now
  bool found = false;

  // Update
  for(auto roomba = trackedRoombas.begin(); roomba != trackedRoombas.end(); roomba++){
    double d = dist((*roomba)->screenLoc_x, (*roomba)->screenLoc_y, center_x, center_y);

    if(d < distThreshold){
      found = true;
      (*roomba)->boundRect = boundRect;
      (*roomba)->updateScreenLoc(center_x,center_y);
    }
  }

  if(!found){
    std::cout << "NEW ROOMBA!" << std::endl;
    Roomba* new_Roomba = new Roomba();
    new_Roomba->updateScreenLoc(center_x, center_y);
    new_Roomba->boundRect = boundRect;
    trackedRoombas.push_back(new_Roomba);
  }

}

void ThresholdTracker::draw(cv::Mat dst){
	// Draw bounding boxes for each tracked Roomba
  for(auto roomba = trackedRoombas.begin(); roomba != trackedRoombas.end(); roomba++){
    (*roomba)->drawBound(dst);
    (*roomba)->drawTrajectory(dst,200);
  }
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

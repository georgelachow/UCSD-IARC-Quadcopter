#include "tracker.hpp"

using namespace std;
using namespace cv;

Distribution::Distribution(int x, int y){
  distribution = Mat(Size(x,y), CV_8U, Scalar(0));
  decayVal = 15;
}

Distribution::~Distribution(){
}

void Distribution::decay(){
  uchar* p;

  for(int i = 0; i < distribution.rows; i++){
    p = distribution.ptr<uchar>(i);

    for(int j = 0; j < distribution.cols; j++){
      uchar element = p[j];
      if(element > 0){
        p[j] = element > decayVal ? element - decayVal: 0;
      }
    }
  }

}

void Distribution::show(const char* winName){
  decay();
  imshow(winName, distribution);
}

ThresholdTracker::ThresholdTracker(){
  // Default Treshold
  lower_threshold = {28,92,0};
  upper_threshold = {255,255,255};
  areaThreshold = 1000;
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
  int center_x, center_y;                       // Center of contour boundRect
  double ca;                                    // Contour area
  std::vector<std::vector<cv::Point>> contours; // Contours of thresholded
  std::vector<cv::Vec4i> heirarchy;
  vector<Point> contour_poly;                   // Polygon of contour
  Rect boundRect;

  // Convert to HSV and perform threshold
  cvtColor(src, threshFrame, COLOR_BGR2HSV);
	inRange(threshFrame, lower_threshold, upper_threshold, threshFrame);

  // Find contours that satify our area Threshold
  findContours(threshFrame, contours, heirarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1,-1));
  for(unsigned int i =0; i < contours.size(); i++){
    ca = contourArea(contours[i]);
    if(ca > areaThreshold){
      approxPolyDP(Mat(contours[i]), contour_poly, 3, true);
      boundRect = boundingRect(Mat(contour_poly));

      // Calculate screen location and see if its a new roomba that was found
      center_x = boundRect.br().x - (boundRect.br().x - boundRect.tl().x) / 2;
      center_y = boundRect.tl().y + (boundRect.br().y - boundRect.tl().y) / 2;

      // Search to see if this center point is close to another center point, if so
      // then chances are it is the same roomba.
      bool found = false;
      for(auto roomba = trackedRoombas.begin(); roomba != trackedRoombas.end(); roomba++){
        double d = dist((*roomba)->screenLoc_x, (*roomba)->screenLoc_y, center_x, center_y);

        // Check to see if the center of the roomba is close to a previous location
        if(d < distThreshold){
          found = true;
          (*roomba)->boundRect = boundRect;
          (*roomba)->updateScreenLoc(center_x,center_y);
        }
      }

      // If there is no roomba close to this point, then chances are its a new roomba
      if(!found){
        std::cout << "NEW ROOMBA!" << std::endl;
        Roomba* new_Roomba = new Roomba();
        new_Roomba->updateScreenLoc(center_x, center_y);
        new_Roomba->boundRect = boundRect;
        trackedRoombas.push_back(new_Roomba);
      }
    }
  }


}

void ThresholdTracker::draw(cv::Mat dst){
	// Draw bounding boxes for each tracked Roomba
  for(auto roomba = trackedRoombas.begin(); roomba != trackedRoombas.end(); roomba++){
    (*roomba)->drawBound(dst);
    (*roomba)->drawTrajectory(dst,200);
  }
}

void ThresholdTracker::removeDead(){
  // If any roombas are dead remove them
  std::vector<int> deadIndices;
  std::vector<Roomba*>::iterator it;

  // Find indices of dead roombas and delete them
  it = trackedRoombas.begin();
  for(unsigned int i = 0; i < trackedRoombas.size(); i++){
    Roomba *roomba = *(it+i);
    if(roomba->life == 0){
      delete roomba;
      roomba = NULL;
      deadIndices.push_back(i);
    }
  }

  // Now remove null pointers in trackedRoombas
  for(auto jt = deadIndices.begin(); jt != deadIndices.end(); jt++){
    trackedRoombas.erase(it + (*jt));
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

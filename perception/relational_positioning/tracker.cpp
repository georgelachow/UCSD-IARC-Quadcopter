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
  imshow(winName, distribution);
}

ThresholdTracker::ThresholdTracker(){
  // Default Treshold
  lower_threshold = {25,92,0};      // Default on green threshold
  upper_threshold = {80,255,255};
  areaThreshold = 30;
  distThreshold = 20; // If roomba contour is < disThreshold
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
  bool found;
  double d, mean_x, mean_y;
  std::vector<std::vector<cv::Point>> contours; // Contours of thresholded
  std::vector<cv::Vec4i> heirarchy;
  vector<Point> contour_poly;                   // Polygon of contour
  Rect boundRect;


  // Since track acts as an update function, we need to decay our roomba life per update
  decayTrackedRoombas();

  // Remove the dead.
  removeDead();

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
      found = false;
      for(auto roomba = trackedRoombas.begin(); roomba != trackedRoombas.end(); roomba++){
        // Check the mean of the previousLocations of our roomba with distance
        mean_x = 0; mean_y = 0;
        for(int i = 0; i < (*roomba)->previousStates.size(); i++){
          mean_x += (*roomba)->previousStates[i].screenLoc_x / (*roomba)->previousStates.size();
          mean_y += (*roomba)->previousStates[i].screenLoc_y / (*roomba)->previousStates.size();
        }
        d = dist(mean_x, mean_y, center_x, center_y);

        // Check to see if the center of the roomba is close to a previous location
        // Used for maintaining already instantiated tracked roombas
        if(d < distThreshold){
          found = true;
          (*roomba)->boundRect = boundRect;
          (*roomba)->updateScreenLoc(center_x,center_y);
        }
      }
      // If there is no roomba close to this point, then chances are its a new roomba
      if(!found){
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
  std::vector<Roomba*>::iterator it;
  std::vector<Roomba*> updatedTracked;

  // Find indices of dead roombas and delete them
  for(unsigned int i = 0; i < trackedRoombas.size(); i++){
    Roomba *roomba= trackedRoombas[i];
    if(roomba->life > 0)
      updatedTracked.push_back(roomba);
    else{
      delete trackedRoombas[i];
      roomba = 0;
    }
  }
  //trackedRoombas.clear();
  trackedRoombas = updatedTracked;

}

void ThresholdTracker::decayTrackedRoombas(){
  for(auto it = trackedRoombas.begin(); it != trackedRoombas.end(); it++){
    (*it)->life = (*it)->life > 0 ? (*it)->life - 1 : 0;
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

GridTracker::GridTracker(){
}

GridTracker::~GridTracker(){
}

void GridTracker::track(cv::Mat src){
  Mat dilate_kernel = Mat(3,3, CV_8UC1, 1);
  vector<Vec2f> lines;
  int dilate_num = 1;

  // Copy matrix
  grid = src.clone();

  /////////////////////////////////////////
  // GRID DETECTION
  /////////////////////////////////////////

  // Scale the grid down (Reduces computation cost, removes some noise)
  resize(grid, grid, Size(0,0), 0.5,0.5);

  // Simple gauss blur for noise removal
  GaussianBlur(grid, grid, Size(11,11), 0);

  // Threshold
  cvtColor(grid,grid, COLOR_BGR2GRAY);
  //threshold(grid, grid, 170, 255, 0); // Global Threshold
  adaptiveThreshold(grid, grid, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 2);// Adaptive Threshold BETTER
  bitwise_not(grid,grid);

  for(int i = 0; i < dilate_num; i++){
    dilate(grid,grid,dilate_kernel);
  }

  // Perform Hough line transform
  HoughLines(grid, lines, 1, CV_PI/180, 200);

  // Draw lines onto grid
  grid.setTo(Scalar::all(0));
  for(int i = 0; i < lines.size(); i++){
    drawLine(lines[i], grid);
  }
  // Scale back up
}

void GridTracker::draw(cv::Mat dst){
  // I guess for this function we can just over lay the grid on the dst
}


KFTracker::KFTracker(){

}

KFTracker::~KFTracker(){

}

void KFTracker::track(cv::Mat src){

}

void KFTracker::draw(cv::Mat dst){

}

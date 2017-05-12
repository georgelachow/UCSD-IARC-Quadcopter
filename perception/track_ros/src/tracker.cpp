#include <perception_interface/tracker.hpp>

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

CircleTracker::CircleTracker(){

}

CircleTracker::~CircleTracker(){

}

void CircleTracker::track(cv::Mat src){
  Mat frame = src.clone();
  cvtColor(frame, frame, CV_BGR2GRAY);

  adaptiveThreshold(frame, frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 2);// Adaptive Threshold BETTER
  bitwise_not(frame,frame);

  // Blur for noise removal
  GaussianBlur(frame, frame, Size(11,11),2,2);

  // Apparently Hough Circle has an internal Canny Edge Detector
  circles.clear();
  HoughCircles(frame, circles, CV_HOUGH_GRADIENT, 1, 10,300,100,0,0);

}

void CircleTracker::draw(cv::Mat dst){
  int radius = 0;
  for(size_t i = 0; i < circles.size(); i++){
    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    radius = cvRound(circles[i][2]);
    circle(dst, center, 3, Scalar(0,255,0), -1, 8,0);
    circle(dst,center, radius, Scalar(0,0,255),3,8,0);
  }
}

GridTracker::GridTracker(){
  frame_count = 0;
  max_lines = 120;
  thetaThreshold = 12;
  distThreshold = 80;
}

GridTracker::~GridTracker(){
}

void GridTracker::track(cv::Mat src){
  Mat dilate_kernel = Mat(3,3, CV_8UC1, 1);
  vector<Vec2f> lines;
  vector<Vec2f> merge;
  vector<int> merge_memo;
  Point p1current, p2current, p1next, p2next;
  float rho = 0.0, theta = 0.0, m, c;

  // Copy matrix
  grid = src.clone();

  /////////////////////////////////////////
  // GRID DETECTION
  /////////////////////////////////////////
  // Simple gauss blur for noise removal
  // NOTE: Kernel size should be dependent on altitude
  GaussianBlur(grid, grid, Size(11,11), 0);

  // Threshold
  cvtColor(grid,grid, COLOR_BGR2GRAY);
  //threshold(grid, grid, 170, 255, 0); // Global Threshold
  adaptiveThreshold(grid, grid, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 2);// Adaptive Threshold BETTER
  bitwise_not(grid,grid);

  // Canny Edge detection
  Canny(grid, grid, 1,300,3);

  // Perform Hough line transform
  // Use queue like structure
  HoughLines(grid, lines, 1, CV_PI/180, 200);
  for(auto it = lines.begin(); it != lines.end(); it++){
    if(grid_lines.size() >= max_lines){
      grid_lines.erase(grid_lines.begin());
      grid_lines.push_back(*it);
    }
    else
      grid_lines.push_back(*it);
  }

  for(int i = 0; i < grid_lines.size(); i++)
    merge_memo.push_back(0);

  // Perform line merge
  // Find the mean of neighboring lines to generate a new single line
  for(int i = 0; i < grid_lines.size(); i++){
    if(merge_memo[i] == 1)
      continue;

    // Horizontal
    if(grid_lines[i][1]!=0){
      m = -1/tan(grid_lines[i][1]);
      c = grid_lines[i][0]/sin(grid_lines[i][1]);
      p1current = Point(0, c);    // Left
      p2current = Point(src.size().width, m*src.size().width+c); // Right
    }
    // Vertical
    else{
      p1current = Point(grid_lines[i][0], 0); // Top
      p2current = Point(grid_lines[i][0], src.size().height); // Bottom
    }
    merge.push_back(grid_lines[i]);

    // Compare with possible neighbor
    for(int j = 0; j < grid_lines.size(); j++){
      if(i == j || merge_memo[j] == 1)
        continue;

      // Horizontal
      if(grid_lines[j][1]!=0){
        m = -1/tan(grid_lines[j][1]);
        c = grid_lines[j][0]/sin(grid_lines[j][1]);
        p1next = Point(0, c);    // Left
        p2next = Point(src.size().width, m*src.size().width+c); // Right
      }
      // Vertical
      else{
        p1next = Point(grid_lines[j][0], 0); // Top
        p2next = Point(grid_lines[j][0], src.size().height); // Bottom
      }

      // Compare distance
      // This is seriously terrible. Need a better solution.
      if((dist(p1current.x,p1current.y, p1next.x, p1next.y) < distThreshold &&
          dist(p2current.x,p2current.y, p2next.x, p2next.y) < distThreshold) ||
          (dist(p1current.x,p1current.y, p2next.x, p2next.y) < distThreshold &&
          dist(p2current.x,p2current.y, p1next.x, p1next.y) < distThreshold)){
         merge.push_back(grid_lines[j]);
         merge_memo[j] = 1;
       }
    }
    // Calculate mean line
    for(auto it = merge.begin(); it != merge.end(); it++){
      rho += (*it)[0];
      theta += (*it)[1];
    }
    if(merge.size() > 0){
      rho /= merge.size();
      theta /= merge.size();
      merged_lines.push_back(Vec2f(rho,theta));
    }

    merge.clear();
    rho = 0.0;
    theta = 0.0;
    merge_memo[i] = 1;
  }
  merge_memo.clear();

  grid.setTo(Scalar(0));
  for(int i = 0; i < merged_lines.size(); i++){
    drawLine(merged_lines[i], grid);
  }
  merged_lines.clear();

  // Calculate intersections
  // Unfortunately another n^2

  // Keep track of frames gone by
  frame_count++;
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

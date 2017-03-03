#include "roomba.hpp"

using namespace cv;
Roomba::Roomba(){
  color = Scalar(rand() % 256, rand() % 256, rand() % 256);
  screenLoc_x = 0;
  screenLoc_y = 0;
  life = 15;
  locationTrackLength = 8;
}

Roomba::~Roomba(){

}

Point2f Roomba::getScreenLoc(){
    return Point2f(screenLoc_x, screenLoc_y);
}

void Roomba::updateScreenLoc(int x, int y){
  screenLoc_x = x;
  screenLoc_y = y;

  if(previousLocations.size() < locationTrackLength)
    previousLocations.push_back(Point2f(x,y));
  else{
    previousLocations.erase(previousLocations.begin());
    previousLocations.push_back(Point2f(x,y));
  }
}
void Roomba::updateTrajectory(){
  float d;
  int next;

  // RIGHT NOW ASSUME previousLocations will always be size 2
  /*
  for(auto loc = previousLocations.begin(); loc != previousLocations.end(); loc++){

    trajectory[0] = (post.x - pre.x)/d;
    trajectory[1] = (post.y - pre.y)/d;
  }
  */

  next = locationTrackLength - 1;
  if(previousLocations.size() == locationTrackLength){
    d = dist(previousLocations[0].x, previousLocations[0].y,
             previousLocations[next].x, previousLocations[next].y);
    trajectory[0] = (previousLocations[0].x - previousLocations[next].x) / d;
    trajectory[1] = (previousLocations[0].y - previousLocations[next].y) / d;
  }
}

void Roomba::drawBound(cv::Mat& dst){
  rectangle(dst, boundRect.tl(), boundRect.br(), color,2, 8, 0);
}

void Roomba::drawTrajectory(cv::Mat& dst, int scale){
			arrowedLine(dst,getScreenLoc(),
            Point2f(screenLoc_x - trajectory[0]*scale,
                    screenLoc_y - trajectory[1]*scale), color, 5);
}

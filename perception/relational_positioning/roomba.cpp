#include "roomba.hpp"

using namespace cv;
Roomba::Roomba(){
  color = Scalar(rand() % 256, rand() % 256, rand() % 256);
  screenLoc_x = 0;
  screenLoc_y = 0;
  life = 5;
  locationTrackLength = 8;
}

Roomba::Roomba(const Roomba& copy){
  color = copy.color;
  screenLoc_x = copy.screenLoc_x;
  screenLoc_y = copy.screenLoc_y;
  life = copy.life;
  locationTrackLength = copy.locationTrackLength;
  boundRect = copy.boundRect;
  trajectory = copy.trajectory;
  //previousStates = copy.previousStates; // Not sure if reference or full copy
}

Roomba::~Roomba(){

}

Point2f Roomba::getScreenLoc(){
    return Point2f(screenLoc_x, screenLoc_y);
}

void Roomba::updateScreenLoc(int x, int y){
  Roomba state;

  screenLoc_x = x;
  screenLoc_y = y;
  life = 5;

  state = Roomba(*this);

  // AFTER I CHANGED THIS TO KEEP TRACK OF STATEs, THERE IS A HANG ISSUE

  if(previousStates.size() < locationTrackLength)
     previousStates.insert(previousStates.begin(),state);
  else{
    previousStates.pop_back();
    previousStates.insert(previousStates.begin(),state);
  }
}
void Roomba::updateTrajectory(){
  float d;
  int next;

  next = locationTrackLength - 1;
  if(previousStates.size() == locationTrackLength){
    d = dist(previousStates[0].screenLoc_x, previousStates[0].screenLoc_y,
             previousStates[next].screenLoc_x, previousStates[next].screenLoc_y);
    trajectory[0] = (previousStates[0].screenLoc_x - previousStates[next].screenLoc_x) / d;
    trajectory[1] = (previousStates[0].screenLoc_y - previousStates[next].screenLoc_y) / d;
  }
}

void Roomba::drawBound(cv::Mat& dst){
  rectangle(dst, boundRect.tl(), boundRect.br(), color,2, 8, 0);
}

void Roomba::drawTrajectory(cv::Mat& dst, int scale){
    /*
			arrowedLine(dst,getScreenLoc(),
            Point2f(screenLoc_x - trajectory[0]*scale,
                    screenLoc_y - trajectory[1]*scale), color, 5);
            */
}

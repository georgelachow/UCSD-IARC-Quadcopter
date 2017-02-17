#include "roomba.hpp"

using namespace cv;
Roomba::Roomba(){
  color = Scalar(rand() % 256, rand() % 256, rand() % 256);
  screenLoc_x = 0;
  screenLoc_y = 0;
}

Roomba::~Roomba(){

}

void Roomba::setScreenLoc(int x, int y){
  screenLoc_x = x;
  screenLoc_y = y;
}

void Roomba::drawBound(cv::Mat& dst){
  rectangle(dst, boundRect.tl(), boundRect.br(), color,2, 8, 0);
}

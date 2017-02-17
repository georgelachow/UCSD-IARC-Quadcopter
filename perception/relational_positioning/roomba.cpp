#include "roomba.hpp"

using namespace cv;
Roomba::Roomba(){
  color = Scalar(rand() % 256, rand() % 256, rand() % 256);
}

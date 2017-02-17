#include "helpers.hpp"

double dist(double x1, double y1, double x2, double y2){
  double dx, dy;

  dx = x1 - x2;
  dy = y1 - y2;

  return sqrt(dx*dx + dy*dy);
}

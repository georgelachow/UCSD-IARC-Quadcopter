#include <math.h>

/* Helper functions to use when calculating relational positioning */
double distance(double x1, double y1, double x2, double y2){
  double dx, dy;

  dx = x1 - x2;
  dy = y1 - y2;

  return sqrt(dx*dx + dy*dy);
}

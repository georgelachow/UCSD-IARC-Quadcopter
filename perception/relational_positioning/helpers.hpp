#ifndef HELPERS_H
#define HELPERS_H
#include <math.h>
#include <vector>
#include <opencv2/opencv.hpp>

// Helper functions to use when calculating relational positioning
double dist(double x1, double y1, double x2, double y2);

// Stiches together matrices
cv::Mat stitch(std::vector<cv::Mat> sources);

#endif /* HELPERS_H */

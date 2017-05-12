#ifndef HELPERS_H
#define HELPERS_H
#include <math.h>
#include <vector>
#include <opencv2/opencv.hpp>

// Helper functions to use when calculating relational positioning
double dist(double x1, double y1, double x2, double y2);

// Stiches together matrices
cv::Mat stitch(std::vector<cv::Mat> sources, int row_len);

// Draw hough lines
void drawLine(cv::Vec2f line, cv::Mat& src, cv::Scalar rgb = CV_RGB(0,0,255));

#endif /* HELPERS_H */

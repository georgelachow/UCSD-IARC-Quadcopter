#ifndef SETTINGS_H
#define SETTINGS_H
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

// Directories
const std::string video_dir = std::string("/home/jason/IARC/data/hexagon/3m_5shexroomba.avi");

// Scales
const double scale_x = 0.75;
const double scale_y = 0.75;

// Set HSV threshold
std::vector<int> red_lower = {160,100,0};
std::vector<int> red_upper = {179,255,255};

// UI Parameters
int frame_count = 0;
char sift_ransac[100];
char hsv_thresh[100];

#endif /* SETTINGS_H */

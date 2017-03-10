#include "helpers.hpp"

using namespace std;
using namespace cv;

double dist(double x1, double y1, double x2, double y2){
  double dx, dy;

  dx = x1 - x2;
  dy = y1 - y2;

  return sqrt(dx*dx + dy*dy);
}

Mat stitch(vector<Mat> sources, int row_len){
  Mat stitched, src;
  int acc_col = 0, max_row = 0, tmp_row = 0, tmp_col = 0, row_offset = 0, col_offset = 0;
  uchar *p, *p_src;
  int ch = 3;
  int mat_count = 0;

  // Get dimension of stitched matrix
  acc_col = sources[0].cols*row_len;
  for(auto it = sources.begin(); it != sources.end(); it++){
    max_row = it->rows > max_row ? it->rows : max_row;
  }
  max_row = max_row * ceil((double)sources.size() / (double)row_len);
  stitched = Mat(max_row, acc_col, CV_8UC3, Scalar(0,0,0));

  // Copy matrices over to stitched
  for(auto it = sources.begin(); it != sources.end(); it++){
    // Make sure that we have 3 channels
    if(it->channels() < 3)
      cvtColor(*it, src, COLOR_GRAY2BGR);
    else
      src = *it;

    tmp_row = it->rows;
    tmp_col = it->cols;

    // Copy src to stitched
    for(int i = 0; i < tmp_row; i++){
      // ith row in stitched/src
      p = stitched.ptr<uchar>(row_offset + i);
      p_src = src.ptr<uchar>(i);

      // now iterate through cols in the ith row and copy over
      for(int j = col_offset*ch; j < (col_offset + tmp_col)*ch; j++)
        p[j] = p_src[j - col_offset*ch];
    }
    col_offset = (col_offset + tmp_col);
    mat_count++;

    // Set new row for every 4 matrices
    if((mat_count % row_len) == 0){
      row_offset += it->rows;
      col_offset = 0;
    }
  }

  return stitched;
}

// USED FUNCTION FROM:
// http://aishack.in/tutorials/sudoku-grabber-opencv-detection/
void drawLine(cv::Vec2f line, cv::Mat& src, cv::Scalar rgb){
  // Horizontal
  if(line[1]!=0){
    float m = -1/tan(line[1]);
    float c = line[0]/sin(line[1]);
    cv::line(src, Point(0, c), Point(src.size().width, m*src.size().width+c), rgb);
  }
  // Vertical
  else
    cv::line(src, Point(line[0], 0), Point(line[0], src.size().height), rgb);

}

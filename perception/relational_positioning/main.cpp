#include "settings.hpp"
#include "tracker.hpp"
using namespace std;
using namespace cv;

void hsv_threshold(Mat src, Mat& dst, InputArray lower, InputArray upper);

int main(){
	cout << CV_VERSION << endl;

	// Setup video feed
	VideoCapture cap(video_dir);
	if(!cap.isOpened()){
		printf("Failed to capture video!\n");
		return -1;
	}
  double w = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  double h = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	cout << "Width: " << w*scale_x<< endl;
	cout << "Height: " << h*scale_y<< endl;
  Size frameSize(static_cast<int>(w), static_cast<int>(h));

	/*
  VideoWriter vw ("/home/jason/IARC/relational_positioning/BoundingBoxOnColor_GLOSS_EFFECT.avi", CV_FOURCC('P','I','M','1'),20, frameSize, true);
  if(!vw.isOpened()){
    cout << "FAiLED!\n";
    return -1;
  }
	*/

	// Windows
	Mat normal, threshold_red, threshold_green, kmeans;

  // Bounding Box / Contours
  vector<vector<Point>> contours;
  vector<Vec4i> heirarchy;

	// Sift / Ransac Tracking
	/*
	SiftFeatureDetector detector;
	SiftDescriptorExtractor extractor;
	vector<KeyPoint> keypoints_roomba, keypoints_video;
	Mat sift_roomba = imread("./sift_images/roomba_red.jpg");
	resize(sift_roomba, sift_roomba, Size(0,0),scale_x,scale_y);

	Mat descriptors1, descriptors2;

	FlannBasedMatcher matcher;
	vector<DMatch> matches;
	Mat img_matches;
	namedWindow("matches",1);

	detector.detect(sift_roomba, keypoints_roomba);
	extractor.compute(sift_roomba, keypoints_roomba, descriptors1);
	*/

	///////////////////////////////////////////////////
	// START VIDEO CAPTURE
	///////////////////////////////////////////////////
	while(true){
		Mat frame;
		cap >> frame;

		//////////////////////////////////////////////////
		// Preprocessing
		//////////////////////////////////////////////////
		resize(frame, frame, Size(0,0),scale_x,scale_y);
		normal = frame;

		// Empty Descriptor matches
		/*
		if(!matches.empty())
			matches.clear();
		*/

		// Extracct descrptors and match with image
		/*
		keypoints_video.clear();
		vector<DMatch> good_matches;
		if((frame_count % 1) == 0){
			detector.detect(frame, keypoints_video);
			extractor.compute(frame, keypoints_video, descriptors2);
			matcher.match(descriptors1, descriptors2, matches);

			// Min and max of our matches in terms of their distances
			double max_dist = 0; double min_dist = 100;
			for(int i =0; i < descriptors1.rows; i++){
				double dist = matches[i].distance;
				if(dist < min_dist){
					min_dist = dist;
				}
				if(dist > max_dist){
					max_dist = dist;
				}
			}

			for(int i =0; i < descriptors1.rows; i++){
					if(matches[i].distance <= max(3*min_dist, 0.02)){
						good_matches.push_back(matches[i]);
					}
			}
		}

		// RANSAC
		vector<Point2f> roomba;
		vector<Point2f> arena;
		for(size_t i = 0; i < good_matches.size(); i++){
			roomba.push_back(keypoints_roomba[good_matches[i].queryIdx].pt);
			arena.push_back(keypoints_video[good_matches[i].trainIdx].pt);
		}

		vector<Point2f> roomba_corners(4);
		vector<Point2f> arena_corners(4);
		if(roomba.size() >= 4){
			Mat H = findHomography(roomba, arena, CV_RANSAC);

			roomba_corners[0] = cvPoint(0,0);																	// TL
			roomba_corners[1] = cvPoint(sift_roomba.cols, 0);									// TR
			roomba_corners[2] = cvPoint(sift_roomba.cols, sift_roomba.rows);	// BR
			roomba_corners[3] = cvPoint(0, sift_roomba.rows);									// BL

			perspectiveTransform(roomba_corners, arena_corners, H);
			//cout << "ROOMBA LOCATION: " << arena_corners[0] << " " << arena_corners[1] << " " << arena_corners[2] << " " << arena_corners[3] << endl;
		}

		drawMatches(sift_roomba, keypoints_roomba, normal, keypoints_video, good_matches, img_matches,
								Scalar::all(-1), Scalar::all(-1),vector<char>(),DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

		cout << arena_corners[0] << " " << arena_corners[1] << " " << arena_corners[2] << " " << arena_corners[3] << endl;
		line( img_matches, arena_corners[0] + Point2f( sift_roomba.cols, 0), arena_corners[1] + Point2f( sift_roomba.cols, 0), Scalar(0, 0, 255), 2 );
	  line( img_matches, arena_corners[1] + Point2f( sift_roomba.cols, 0), arena_corners[2] + Point2f( sift_roomba.cols, 0), Scalar( 0, 0, 255), 2 );
	  line( img_matches, arena_corners[2] + Point2f( sift_roomba.cols, 0), arena_corners[3] + Point2f( sift_roomba.cols, 0), Scalar( 0, 0, 255), 2 );
	  line( img_matches, arena_corners[3] + Point2f( sift_roomba.cols, 0), arena_corners[0] + Point2f( sift_roomba.cols, 0), Scalar( 0, 0, 255), 2 );
		*/

		// Center point of camera
		/*
		double offset_x = sift_roomba.cols;
		Point2f camera_center((frame.rows/2), (frame.cols/2) + offset_x);
		*/

		// Center of sift box
		/*
		double midpoint_x = (arena_corners[0].x + arena_corners[2].x) / 2;
		double midpoint_y = (arena_corners[0].y + arena_corners[2].y) / 2;
		Point2f sift_center(midpoint_x + offset_x,midpoint_y);
		line(img_matches, camera_center, sift_center, Scalar(255,0,0), 4);
		*/

		/*
		Point2f boundrect_center( (boundRect.br().x - diff_x) + offset_x,
															(boundRect.br().y - diff_y));
		line(img_matches, camera_center, boundrect_center, Scalar(255,0,0), 4);
		*/

		/////////////////////////////////////////////////
		// DEBUG ON-SCREEN TEXT
		/////////////////////////////////////////////////
		/*
		snprintf(sift_ransac, 100, "SIFT_RANSAC Center:       (%d, %d)", (int)sift_center.x, (int)sift_center.y);
		snprintf(hsv_thresh, 100, "HSV_THRESH_CONTOUR Center: (%d, %d)", (int)boundrect_center.x, (int)boundrect_center.y);
		putText(img_matches, sift_ransac, cvPoint(5,110), FONT_HERSHEY_COMPLEX_SMALL, 0.65, cvScalar(255,255,255),1, CV_AA);
		putText(img_matches, hsv_thresh, cvPoint(5,130), FONT_HERSHEY_COMPLEX_SMALL, 0.65, cvScalar(255,255,255),1, CV_AA);
		*/

		////////////////////////////////////////////////
    // WINDOW DISPLAY
		////////////////////////////////////////////////
		if(!frame.empty()){
			imshow("Normal", normal);
      //imshow("Threshold", threshold_red);
			//imshow("Matches", img_matches);
      //imshow("Threshold2", threshold_green);
      //trackbar.show();
      //vw.write(normal);
			frame_count++;
    }

    // Need this for some reason.
		if((waitKey(1)&(0xff)) == 27) break;
	}
	return 0;
}

void hsv_threshold(Mat src, Mat& dst, InputArray lower, InputArray upper){
	// Convert to HSV if not already
	cvtColor(src, dst, COLOR_BGR2HSV);
	// Threshold
	inRange(dst, lower, upper, dst);
}
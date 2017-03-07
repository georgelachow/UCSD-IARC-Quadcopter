#include "settings.hpp"
#include "tracker.hpp"
#include "helpers.hpp"

using namespace std;
using namespace cv;

int main(){
	double w,h;								// Resolution
	int frame_count;
	Mat normal;
	Mat output, stitched;
	Mat pre_frame, post_frame;
	Mat debug, debug2;
	Point2f camera_center;
	vector<cv::Mat> toStitch;

	GridTracker gridTracker;
	ThresholdTracker threshTracker;
	ThresholdTracker threshTracker2;
	Distribution* threshDistribution;
	Distribution* threshDistribution2;

	char str_buffer[255] = {0};

	cout << CV_VERSION << endl;

	// Setup video feed
	VideoCapture cap(video_dir);
	if(!cap.isOpened()){
		printf("Failed to capture video!\n");
		return -1;
	}
  w = cap.get(CV_CAP_PROP_FRAME_WIDTH)*scale_x;
  h = cap.get(CV_CAP_PROP_FRAME_HEIGHT)*scale_y;

	threshDistribution = new Distribution(w,h);
	threshDistribution->decayVal = 5;
	threshDistribution2 = new Distribution(w,h);
	threshDistribution2->decayVal = 5;

	threshTracker2.setLower(0,92,0);
	threshTracker2.setUpper(5,255,255);

	cout << "Width: " << w<< endl;
	cout << "Height: " << h<< endl;

	///////////////////////////////////////////////////
	// START VIDEO CAPTURE
	///////////////////////////////////////////////////
	frame_count = 0;
	while(true){
		cap >> post_frame;

		//////////////////////////////////////////////////
		// PREPROCESSING / OTHERS
		//////////////////////////////////////////////////
		resize(post_frame, post_frame, Size(0,0),scale_x,scale_y);
		camera_center = Point2f((post_frame.cols/2), (post_frame.rows/2));
		output = post_frame.clone();
		normal = post_frame.clone();
		debug = Mat(h,w, CV_8UC3, Scalar(0,0,0));
		debug2 = Mat(h,w, CV_8UC3, Scalar(0,0,0));
		threshDistribution->decay();
		threshDistribution2->decay();

		//////////////////////////////////////////////////
		// TRESHOLD TRACKING
		//////////////////////////////////////////////////
		threshTracker.track(post_frame);

		// Get Tracked Roombas and perform calculations
		for(auto roomba = threshTracker.trackedRoombas.begin(); roomba != threshTracker.trackedRoombas.end(); roomba++){
			// Distance line from center
			// Just draw the trajectory
			line(output,(*roomba)->getScreenLoc(), camera_center, (*roomba)->color, 2);

			// Trajectory
			//(*roomba)->updateTrajectory();

			// Distribution Update
			threshDistribution->distribution.at<uchar>((*roomba)->getScreenLoc()) = 255;
		}

		// Draw
		threshTracker.draw(output);
		/*
		threshTracker2.track(post_frame);

		// Get Tracked Roombas and perform calculations
		for(auto roomba = threshTracker2.trackedRoombas.begin(); roomba != threshTracker2.trackedRoombas.end(); roomba++){
			// Distance line from center
			// Just draw the trajectory
			line(normal,(*roomba)->getScreenLoc(), camera_center, (*roomba)->color, 2);

			// Trajectory
			//(*roomba)->updateTrajectory();

			// Distribution Update
			threshDistribution2->distribution.at<uchar>((*roomba)->getScreenLoc()) = 255;
		}

		// Draw
		threshTracker2.draw(output);
		*/

		//////////////////////////////////////////////////
		// GRID TRACKING
		//////////////////////////////////////////////////
		gridTracker.track(normal);
		gridTracker.draw(output);

		/////////////////////////////////////////////////
		// SCREEN DEBUG
		/////////////////////////////////////////////////
		snprintf(str_buffer, 255, "Roomba Count, ThresholdTracker: %d", (int)threshTracker.trackedRoombas.size());
		putText(debug, str_buffer, cvPoint(5,30), FONT_HERSHEY_COMPLEX_SMALL, 1.00, cvScalar(255,255,255),1, CV_AA);
		for(int i = 0; i < threshTracker.trackedRoombas.size(); i++){
			snprintf(str_buffer, 255, "Roomba Pixel Location %d: (%d,%d), Distance from Quadcopter: %f", i,
				threshTracker.trackedRoombas[i]->screenLoc_x, threshTracker.trackedRoombas[i]->screenLoc_y,
				dist(threshTracker.trackedRoombas[i]->screenLoc_x, threshTracker.trackedRoombas[i]->screenLoc_y, camera_center.x, camera_center.y));
			putText(debug, str_buffer, cvPoint(5,50 + i*20), FONT_HERSHEY_COMPLEX_SMALL, 1.00, cvScalar(255,255,255),1, CV_AA);
		}
		snprintf(str_buffer, 255, "Roomba Count, ThresholdTracker2: %d", (int)threshTracker2.trackedRoombas.size());
		putText(debug2, str_buffer, cvPoint(5,30), FONT_HERSHEY_COMPLEX_SMALL, 1.00, cvScalar(255,255,255),1, CV_AA);
		for(int i = 0; i < threshTracker2.trackedRoombas.size(); i++){
			snprintf(str_buffer, 255, "Roomba Pixel Location %d: (%d,%d), Distance from Quadcopter: %f", i,
				threshTracker2.trackedRoombas[i]->screenLoc_x, threshTracker2.trackedRoombas[i]->screenLoc_y,
				dist(threshTracker2.trackedRoombas[i]->screenLoc_x, threshTracker2.trackedRoombas[i]->screenLoc_y, camera_center.x, camera_center.y));
			putText(debug2, str_buffer, cvPoint(5,50 + i*20), FONT_HERSHEY_COMPLEX_SMALL, 1.00, cvScalar(255,255,255),1, CV_AA);
		}

		////////////////////////////////////////////////
    // WINDOW DISPLAY
		////////////////////////////////////////////////
		if(!post_frame.empty()){
			toStitch.clear();
			toStitch.push_back(output);
			toStitch.push_back(threshDistribution->distribution);
			toStitch.push_back(threshTracker.threshFrame);
			toStitch.push_back(debug);
			/*
			toStitch.push_back(threshDistribution2->distribution);
			toStitch.push_back(threshTracker2.threshFrame);
			toStitch.push_back(debug2);
			*/
			toStitch.push_back(threshTracker.ROI);
			toStitch.push_back(gridTracker.grid);
			stitched = stitch(toStitch, 4);
			imshow("Display", stitched);

			pre_frame = normal.clone();
			frame_count++;
    }

    // Need this for some reason.
		if((waitKey(1)&(0xff)) == 27) break;
	}
	return 0;
}

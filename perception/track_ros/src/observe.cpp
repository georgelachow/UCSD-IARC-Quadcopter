#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <perception_interface/settings.hpp>
#include <perception_interface/tracker.hpp>
#include <perception_interface/helpers.hpp>

using namespace std;
using namespace cv;

static const std::string OPENCV_WINDOW = "Observer's Gaze";

class Observer
{
public:
  Observer(int argc, char **argv, const char* cam_topic): it_(nh_){
    /////////////////////////////////
    // Setup Observer processing
    /////////////////////////////////

    // Indicates sizes of each mini-window
    w = 960*scale_x;
    h = 720*scale_y;

  	threshDistribution = new Distribution(w,h);
  	threshDistribution->decayVal = 5;
  	threshDistribution2 = new Distribution(w,h);
  	threshDistribution2->decayVal = 5;

  	threshTracker2.setLower(0,92,0);
  	threshTracker2.setUpper(5,255,255);

    frame_count = 0;

    ///////////////////////////////////////////////////////////////
    // Subscribe to input video feed and publish output video feed
    ///////////////////////////////////////////////////////////////
    image_sub_ = it_.subscribe(cam_topic,1, &Observer::process, this);

    //////////////////////////////
    // Window Settings
    //////////////////////////////
    if(argc == 3){
      window_width = atoi(argv[1]);
      window_height = atoi(argv[2]);
    }
    else{
      window_width = 1920;
      window_height = 1080;
    }
    cv::namedWindow(OPENCV_WINDOW);
  }
  ~Observer(){  cv::destroyWindow(OPENCV_WINDOW);}

  void process(const sensor_msgs::ImageConstPtr& msg){
    //////////////////////////////////////////////////////
    // Grab the image pointer from the erlecopter camera
    //////////////////////////////////////////////////////
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }
    post_frame = cv_ptr->image;

		//////////////////////////////////////////////////
		// PREPROCESSING / INITIALIZATION
		//////////////////////////////////////////////////

    // Resize the post_frame to some scale
		resize(post_frame, post_frame, Size(0,0),scale_x,scale_y);
    cout << post_frame.rows << " " << post_frame.cols << endl;

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
		// CIRCLE TRACKING
		//////////////////////////////////////////////////
		//circleTracker.track(normal);
		//circleTracker.draw(output);

		//////////////////////////////////////////////////
		// GRID TRACKING
		//////////////////////////////////////////////////
        /*
		gridTracker.track(normal);
		gridTracker.draw(output);
		grid = gridTracker.grid.clone();
		threshTracker.draw(grid);
        */

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
    /*
		snprintf(str_buffer, 255, "Roomba Count, ThresholdTracker2: %d", (int)threshTracker2.trackedRoombas.size());
		putText(debug2, str_buffer, cvPoint(5,30), FONT_HERSHEY_COMPLEX_SMALL, 1.00, cvScalar(255,255,255),1, CV_AA);
		for(int i = 0; i < threshTracker2.trackedRoombas.size(); i++){
			snprintf(str_buffer, 255, "Roomba Pixel Location %d: (%d,%d), Distance from Quadcopter: %f", i,
				threshTracker2.trackedRoombas[i]->screenLoc_x, threshTracker2.trackedRoombas[i]->screenLoc_y,
				dist(threshTracker2.trackedRoombas[i]->screenLoc_x, threshTracker2.trackedRoombas[i]->screenLoc_y, camera_center.x, camera_center.y));
			putText(debug2, str_buffer, cvPoint(5,50 + i*20), FONT_HERSHEY_COMPLEX_SMALL, 1.00, cvScalar(255,255,255),1, CV_AA);
		}
    */

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
			toStitch.push_back(threshTracker.ROI);
			toStitch.push_back(grid);
			*/

			stitched = stitch(toStitch, 4);
  		display_frame = Mat(window_width,window_height, CV_8UC3, Scalar(0,0,0));
      cv::resize(stitched, display_frame, cv::Size(window_width, window_height), 0, 0, cv::INTER_CUBIC);
			imshow(OPENCV_WINDOW, display_frame);
      cv::waitKey(3);

			pre_frame = normal.clone();
			frame_count++;
    }
  }

private:
  // ROS Interface
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;

  // OpenCV Settings
	double w,h;								        // Resolution per window
  int window_width, window_height;  // Total resolution
	int frame_count;
	Mat normal;
  Mat display_frame;
	Mat output, stitched;
	Mat pre_frame, post_frame;
	Mat grid;
	Mat debug, debug2;
	Point2f camera_center;
	vector<cv::Mat> toStitch;

	//GridTracker gridTracker;
	ThresholdTracker threshTracker;
	ThresholdTracker threshTracker2;
	CircleTracker circleTracker;
	Distribution* threshDistribution;
	Distribution* threshDistribution2;

	char str_buffer[255] = {0};
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "observer");
  Observer obs(argc, argv, "/erlecopter/sensors/camera/image_raw");

  // Spin the thread
  ros::spin();
  return 0;
}

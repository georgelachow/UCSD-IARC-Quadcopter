# Roomba tracking and relational positioning

### __Dependencies:__
1. opencv 2.4.8

###__Possible Future Dependencies:__
1. cuDNN
2. cuda
3. Caffe

### __How to run ROS Integrated:__
* Put the track_ros package into your catkin workspace src folder and do a catkin_make.
* Next, after setting up simulation, run the command rosrun perception_interface observe

### __Todo:__
1. Implement multi-object Kalman Filter based tracking
2. Grid Detection & Tracking (Using Hough right now)
3. Use a neural network somewhere for fun.
	* Possible nets, YOLO v2, SSD, FlowNet, Optical Flow & Semantic Segmentation
4. Implement more possible tracking algorithms, Sparse Tracker, SiftFLOW
5. Improve line projection
	* __I think we need grid detection for this__
6. Trajectory calculation
	* __I Think we need some sort of external data from the camera in order to account for the quadcopter motion which causes all points to shift__

### __Comments:__
* Naive trajectory calculation currently implemented, looks at pre and post images
	* Will also need to project this trajectory onto the flat surface
	* Possible improvement: Trajectory prediction, predict trajectory based upon incoming collision of another roomba.
	* __Need to figure out way to track roombas with a moving camera__
* Naive line projection for distance measurement, draws line from center of camera to center of roomba.

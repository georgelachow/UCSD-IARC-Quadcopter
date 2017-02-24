# Roomba tracking and relational positioning

### __Dependencies:__
1. opencv 3.20

###__Possible Future Dependencies:__
1. cuDNN
2. cuda
3. Caffe

### __How to run:__
* If you have the dependencies installed, simply go to settings.hpp and change absolute directory of the video to the video you want to run the tracker on.

### __Todo:__
1. Implement multi-object Kalman Filter based tracking
2. Use a neural network somewhere for fun.
	* Possible nets, YOLO v2, SSD, FlowNet, Optical Flow & Semantic Segmentation
3. Modularize SIFT Tracking, possible use of canny edge detection for edge keypoint matching and noise removal.
	* __Do not think SIFT is a feasible solution for multi object tracking__
4. Implement more possible tracking algorithms, Sparse Tracker, SiftFLOW
5. Gather more data, in various situations
	* __Simulator data in the works__
6. Improve line projection
	* __I think we need grid detection for this__
7. Trajectory calculation
	* __I Think we need some sort of external data from the camera in order to account for the quadcopter motion which causes all points to shift__

### __Comments:__
* Naive trajectory calculation currently implemented, looks at pre and post images
	* Will also need to project this trajectory onto the flat surface
	* Possible improvement: Trajectory prediction, predict trajectory based upon incoming collision of another roomba.
	* __Need to figure out way to track roombas with a moving camera__
* Naive line projection for distance measurement, draws line from center of camera to center of roomba.

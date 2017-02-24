# Roomba tracking and relational positioning 

### __Dependencies:__
1. opencv 3.20
2. cuda

### __Comments:__
* Naive trajectory calculation currently implemented, looks at pre and post images
	* Will also need to project this trajectory onto the flat surface
	* Possible improvement: Trajectory prediction, predict trajectory based upon incoming collision of another roomba.
* Naive line projection for distance measurement, draws line from center of camera to center of roomba. 

### __Todo:__
1. Implement Trajectory calculations
2. Modularize SIFT Tracking, possible use of canny edge detection for edge keypoint matching and noise removal.
3. Cleanup code
4. Implement more possible tracking algorithms, FlowNet, Sparse Tracker, SiftFLOW, custom neural network
5. Gather more data, in various situations
6. Improve line projection


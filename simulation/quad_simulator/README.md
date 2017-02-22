# quad_simulator
Simulator for the Quadrotor Project, IIT Kharagpur
Edited by Alexander Khoury, UCSD IARC Quadcopter Team

For setting it up:

Install ros-indigo with gazebo2

Now edit ~/.bashrc and add the following lines at the end of the file. 

Edit catkin_ws with your catkin_ws name 



source ~/catkin_ws/devel/setup.bash

export ROS_WORKSPACE=~/catkin_ws/

export ROS_PACKAGE_PATH+=~/catkin_ws/src

export GAZEBO_MODEL_PATH=~/gazebo_models/

export GAZEBO_RESOURCE_PATH=/usr/share/gazebo-2.2/worlds/



to launch sim

roslaunch quad_simulator ucsd_iarc.launch

to make roombas move

rosrun quad_simulator groundbot_node

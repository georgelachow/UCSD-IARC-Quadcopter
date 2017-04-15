#!/bin/bash
# Run the simulation on terminal 1
source ~/simulation/ros_catkin_ws/devel/setup.bash
cd ~/simulation/ardupilot/ArduCopter/
../Tools/autotest/sim_vehicle.sh -j 4 -f Gazebo

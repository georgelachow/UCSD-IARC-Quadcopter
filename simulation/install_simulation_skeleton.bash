#!/bin/bash
#install ros indigo
sudo apt-get install git &&
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'	
sudo apt-key adv --keyserver hkp://ha.pool.sks-keyservers.net:80 --recv-key 421C365BD9FF1F717815A3895523BAEEB01FA116 &&
sudo apt-get update &&
sudo apt-get -y install ros-indigo-desktop-full &&
sudo rosdep init &&
rosdep update &&
echo "source /opt/ros/indigo/setup.bash" >> ~/.bashrc &&
source ~/.bashrc &&


#erle sim
sudo apt-get update &&
sudo apt-get -y install gawk make git curl cmake &&
sudo apt-get -y install g++ python-pip python-matplotlib python-serial python-wxgtk2.8 python-scipy python-opencv python-numpy python-pyparsing ccache realpath libopencv-dev &&

sudo pip install future &&
sudo apt-get -y install libxml2-dev libxslt1-dev &&
sudo pip2 install pymavlink catkin_pkg --upgrade &&
sudo pip install MAVProxy==1.5.2 &&

sudo apt-get install -y qtbase5-dev libqt5core5a libqt5gui5 libqt5concurrent5 libqt5widgets5 libqt5test5 libqt5opengl5-dev libqt5opengl5

cd ~ &&
git clone https://github.com/ackhoury/simulation.git &&
cd ~/ucsd_iarc_simulation &&
sudo apt-get -y install libtool automake autoconf libexpat1-dev &&
cd jsbsim &&
./autogen.sh --enable-libraries &&
make -j2 &&
sudo make install &&

sudo apt-get -y    install python-rosinstall       \
                        ros-indigo-octomap-msgs    \
                        ros-indigo-joy             \
                        ros-indigo-geodesy         \
                        ros-indigo-octomap-ros     \
                        ros-indigo-mavlink         \
                        ros-indigo-control-toolbox \
                        unzip &&

#install gazebo 7

sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list' &&
wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add - &&
sudo apt-get update &&
sudo apt-get -y remove .*gazebo.* '.*sdformat.*' '.*ignition-math.*' && 
sudo apt-get update && 
sudo apt-get -y install gazebo7 libgazebo7-dev drcsim7 &&

sudo apt-get -y install ros-indigo-transmission-interface ros-indigo-joint-limits-interface &&

cd ~/simulation/ros_catkin_ws &&
source /opt/ros/indigo/setup.bash &&
catkin_make clean &&
catkin_make --pkg mav_msgs mavros_msgs gazebo_msgs &&
source devel/setup.bash &&
source /opt/ros/indigo/setup.bash &&
catkin_make -j16 &&
echo "source ~/simulation/ros_catkin_ws/devel/setup.bash" >> ~/.bashrc &&
source ~/.bashrc &&

mkdir -p ~/.gazebo/models &&
git clone https://github.com/erlerobot/erle_gazebo_models &&
mv erle_gazebo_models/* ~/.gazebo/models

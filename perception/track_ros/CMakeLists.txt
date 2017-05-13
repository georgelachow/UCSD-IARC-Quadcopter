cmake_minimum_required(VERSION 2.8.3)
project(perception_interface)
## Add support for C++11, supported in ROS Kinetic and newer
# add_definitions(-std=c++11)
## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  sensor_msgs
  std_msgs
)
add_definitions(-std=c++11)
include_directories(include ${catkin_INCLUDE_DIRS})
catkin_package(
 INCLUDE_DIRS include
 LIBRARIES perception_interface
 CATKIN_DEPENDS cv_bridge image_transport roscpp sensor_msgs std_msgs
 DEPENDS system_lib
)
include_directories(
  ${catkin_INCLUDE_DIRS}
)
add_executable(observe src/observe.cpp src/helpers.cpp src/roomba.cpp src/tracker.cpp)
target_link_libraries(observe ${catkin_LIBRARIES})
add_dependencies(observe perception_interface_generate_messages_cpp)

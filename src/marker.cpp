#include "ros/ros.h"
#include <ros/console.h>
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include <geometry_msgs/Vector3.h>
#include <ardrone_autonomy/Navdata.h>
#include <ar_track_alvar_msgs/AlvarMarker.h>
#include <ar_track_alvar_msgs/AlvarMarkers.h>
#include <thread>
#include <sstream> 
#include <string>

void Sleepfor(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void markerCallBack(const ar_track_alvar_msgs::AlvarMarkers::ConstPtr& msg)
{
	ROS_DEBUG("A marker has been detected");
}


int main(int argc, char **argv)
{
	ros::init(argc, argv, "readmarker");
	ros::NodeHandle n;
	
	ros::Subscriber sub = n.subscribe("/ar_pose_marker", 1000, markerCallback);

	ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
	ros::Publisher land_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);
	std_msgs::Empty emp_msg;
	
	ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	geometry_msgs::Twist twist_msg;

	ros::spin();
	
	return 0;
}

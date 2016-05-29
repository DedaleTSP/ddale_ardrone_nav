#include "ros/ros.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include <geometry_msgs/Vector3.h>
#include <ardrone_autonomy/Navdata.h>
#include <aruco_msgs/MarkerArray.h>
#include <geometry_msgs/PoseWithCovariance.h>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream> 
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

int markerid = -1;
geometry_msgs::PoseWithCovariance markerpose;

std::string FloatToString(float n)
{
	//std::string s = boost::lexical_cast<std::string>(n);	
	std::ostringstream ss;
	ss << n;
	return (ss.str());	
}

void Sleepfor(int ms)
{
	 boost::this_thread::sleep_for(boost::chrono::milliseconds(ms));
}
 void chatterCallback(const aruco_msgs::MarkerArray::ConstPtr msg)
{
     	
     aruco_msgs::MarkerArray lastMsg = *msg;
     aruco_msgs::Marker lastMarker = lastMsg.markers[0];
      
     markerid = lastMarker.id;
     markerpose = lastMarker.pose;
     
     ROS_INFO("I heard: %i]",markerid);
     //ROS_INFO("I heard: [%s]", FloatToString(msg->rotZ).c_str());
     //std::string X, Y, Z; 
     //X = FloatToString(x); 
     //Y = FloatToString(y); 
     //Z = FloatToString(z); 
     //ROS_INFO("I heard: [%s,%s,%s] , [%f,%f,%f]", X.c_str(), Y.c_str(), Z.c_str(),x,y,z);
     

}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "readmarker");
	ros::NodeHandle n;
	
	ros::Subscriber mark_sub = n.subscribe("/aruco_marker_publisher/markers", 1,chatterCallback);
	
	ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
	ros::Publisher land_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);
	std_msgs::Empty emp_msg;

	ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	geometry_msgs::Twist twist_msg;	

	start:

	ROS_INFO("Looking for a marker");

	while(markerid == -1)
	{
		ros::spinOnce();
	}
	
	ROS_INFO("Marker found");

	switch(markerid)
	{
		case 0:
			ROS_INFO("Taking off");
			takeoff_pub.publish(emp_msg);
			goto start;

		case 1:
			ROS_INFO("Going Forward"); 
			twist_msg.linear.x = 0.1;
			twist_pub.publish(twist_msg);
			ros::Duration(1).sleep();;
			twist_msg.linear.x = 0;
			twist_pub.publish(twist_msg);
			goto start;

		case 2:
			ROS_INFO("Turning clockwise");
			twist_msg.angular.z = -0.1;
			twist_pub.publish(twist_msg);
			ros::Duration(2).sleep();;
			twist_msg.angular.z = 0;
			twist_pub.publish(twist_msg);
			goto start;
		case 3:
			ROS_INFO("Turning counter clockwise");
			twist_msg.angular.z = 0.1;
			twist_pub.publish(twist_msg);
			ros::Duration(2).sleep();
			twist_msg.angular.z = 0;
			twist_pub.publish(twist_msg);
			goto start;

		case 4:
			ROS_INFO("Landing");
			land_pub.publish(emp_msg);
			goto end;
	}
	
	ROS_INFO("Out of switch");

	end:

	ROS_INFO("Exiting program");

	return 0;
}

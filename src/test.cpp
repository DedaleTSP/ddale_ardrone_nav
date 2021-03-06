#include "ros/ros.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include <geometry_msgs/Vector3.h>
#include <ardrone_autonomy/Navdata.h>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream> 
#include <string>
#include <iostream>

using namespace std;
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
 void chatterCallback(const ardrone_autonomy::Navdata::ConstPtr msg)
{
     	
     ardrone_autonomy::Navdata lastMsg = *msg;
     float x, y, z; 
     x = lastMsg.rotX; 
     y = lastMsg.rotY; 
     z = lastMsg.rotZ; 
     ROS_INFO("I heard: [%f,%f,%f]",x,y,z);
     //ROS_INFO("I heard: [%s]", FloatToString(msg->rotZ).c_str());
     //std::string X, Y, Z; 
     //X = FloatToString(x); 
     //Y = FloatToString(y); 
     //Z = FloatToString(z); 
     //ROS_INFO("I heard: [%s,%s,%s] , [%f,%f,%f]", X.c_str(), Y.c_str(), Z.c_str(),x,y,z);
     

}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "testdrone");
	ros::NodeHandle n;
	
	ros::Subscriber nav_sub = n.subscribe("/ardrone/navdata", 1,chatterCallback);
	
	ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
	ros::Publisher land_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);
	std_msgs::Empty emp_msg;

	ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	geometry_msgs::Twist twist_msg;	
	
	takeoff_pub.publish(emp_msg);
	ros::Duration(3).sleep();
	
	ros::spinOnce();
	ROS_INFO("##mycontroller:z=1");
	twist_msg.angular.z=1;
	twist_pub.publish(twist_msg);
	ros::Duration(2).sleep();

	ros::spinOnce();
	ROS_INFO("##mycontroller:z=0");
	twist_msg.angular.z=0;
	twist_pub.publish(twist_msg);
	ros::Duration(2).sleep();

	ros::spinOnce();
	ROS_INFO("##mycontroller:z=-1");
	twist_msg.angular.z=-1;
	twist_pub.publish(twist_msg);
	ros::Duration(2).sleep();

	ros::spinOnce();
	ROS_INFO("##mycontroller:z=0");
	twist_msg.angular.z=0;
	twist_pub.publish(twist_msg);
	ros::Duration(2).sleep();
	
	ros::Duration(2).sleep();
	land_pub.publish(emp_msg);
	ros::Duration(2).sleep();
	return 0;
}

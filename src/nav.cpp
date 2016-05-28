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

int main(int argc, char **argv)
{
	ros::init(argc, argv, "takeoffAndLand");
	ros::NodeHandle n;
	
	ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
	ros::Publisher land_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);
	std_msgs::Empty emp_msg;
	
	ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	geometry_msgs::Twist twist_msg;

	while (ros::ok())
  	{
    		takeoff_pub.publish(emp_msg);

		Sleepfor(5000);

		twist_msg.angular.z=0;
		twist_msg.linear.x=0.1;    
                twist_pub.publish(twist_msg);
                Sleepfor(1000);

		twist_msg.linear.x=0;
		twist_msg.angular.z=0.5;
		twist_pub.publish(twist_msg);
		Sleepfor(2000);

		twist_msg.angular.z=0;
		twist_msg.linear.x=0.1;
		twist_pub.publish(twist_msg);
		Sleepfor(1000);

		twist_msg.angular.x=0;
		twist_msg.angular.z=0.5;
                twist_pub.publish(twist_msg);
                Sleepfor(2000); 

		twist_msg.angular.z=0;
		twist_msg.linear.x=0.1;    
                twist_pub.publish(twist_msg);
                Sleepfor(1000);

		twist_msg.linear.x=0;
		twist_msg.angular.z=0.5; 
                twist_pub.publish(twist_msg);
                Sleepfor(2000); 

		twist_msg.angular.z=0;
		twist_msg.linear.x=0.1;    
                twist_pub.publish(twist_msg);
                Sleepfor(1000);

		land_pub.publish(emp_msg);

		ros::Duration(2).sleep();

		ros::spinOnce();
	}
	return 0;
}

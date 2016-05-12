#include "ros/ros.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include <geometry_msgs/Vector3.h>

int main(int argc, char **argv)
{
	ros::init(argc, argv, "takeoffAndLand");
	ros::NodeHandle n;
	
	ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
	ros::Publisher land_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);
	std_msgs::Empty emp_msg;
	
	ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	geometry_msgs::Twist twist_msg;
	twist_msg.angular.z=1;

	while (ros::ok())
  	{
    		takeoff_pub.publish(emp_msg);
		ros::Duration(10).sleep();
		twist_pub.publish(twist_msg);
		ros::Duration(2).sleep();
		land_pub.publish(emp_msg);
		ros::spinOnce();
		ros::Duration(5).sleep();
	}
	return 0;
}

#include "ros/ros.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include <geometry_msgs/Vector3.h>
#include <ardrone_autonomy/Navdata.h>

int main(int argc, char **argv)
{
	ros::init(argc, argv, "takeoffAndLand");
	ros::NodeHandle n;
	
	ros::Subscriber nav_sub = n.subscribe("/ardrone/navdata", 1);
	
	ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
	ros::Publisher land_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);
	std_msgs::Empty emp_msg;
	
	ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	geometry_msgs::Twist twist_msg;

	while (ros::ok())
  	{
    		takeoff_pub.publish(emp_msg);

		ros::Duration(0.5).sleep();

		twist_msg.angular.z=1;
		twist_pub.publish(twist_msg);

		ros::Duration(0.5).sleep();

		twist_msg.angular.z=-0.5;
		twist_pub.publish(twist_msg);

		ros::Duration(0.5).sleep();

		twist_msg.angular.z=0;
		twist_pub.publish(twist_msg);

		ros::Duration(0.5).sleep();

		land_pub.publish(emp_msg);

		ros::Duration(2).sleep();

		ros::spinOnce();
	}
	return 0;
}

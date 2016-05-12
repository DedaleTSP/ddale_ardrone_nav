#include "ros/ros.h"
#include "std_msgs/Empty.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "takeoffAndLand");
  ros::NodeHandle n;
  ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
  ros::Publisher land_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);

  while (ros::ok())
  {
    	takeoff_pub.publish(std_msgs::Empty());

    	ros::Duration(10).sleep();

    	land_pub.publish(std_msgs::Empty());

    	ros::spinOnce();

	ros::Duration(5).sleep();
  }


  return 0;
}

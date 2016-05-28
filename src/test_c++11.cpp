#include "ros/ros.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include <geometry_msgs/Vector3.h>
#include <ardrone_autonomy/Navdata.h>
#include <thread>
#include <sstream> 
#include <string>
#include <iostream>

using namespace std;
std::string FloatToString(float n)
{
	std::ostringstream ss;
	ss << n;
	return (ss.str());	
}


void Sleepfor(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
 void chatterCallback(const ardrone_autonomy::NavdataConstPtr &msg)
{
     ROS_INFO("I heard: [%s]", FloatToString(msg->rotZ).c_str());	
}





void MyController(bool * continu,ros::NodeHandle *n)
{
	ros::Publisher twist_pub = n->advertise<geometry_msgs::Twist>("cmd_vel", 3);
	geometry_msgs::Twist twist_msg;
	if(! *(continu))
	{
		ROS_INFO("Arret mycontroller");
		return;
	}	
	else
	{
		ROS_INFO("##mycontroller:z=1");
		twist_msg.angular.z=1;
		twist_pub.publish(twist_msg);
		//Sleepfor(2000);
		ros::Duration(2).sleep();
		ROS_INFO("##mycontroller:z=0");
		twist_msg.angular.z=0;
		twist_pub.publish(twist_msg);
		//Sleepfor(2000);
		ros::Duration(2).sleep();
		ROS_INFO("##mycontroller:z=-1");
		twist_msg.angular.z=-1;
		twist_pub.publish(twist_msg);
		//Sleepfor(1000);
		ros::Duration(2).sleep();
		ROS_INFO("##mycontroller:z=0");
		twist_msg.angular.z=0;
		twist_pub.publish(twist_msg);
		ros::Duration(2).sleep();
		//Sleepfor(2000);
	}
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "takeoffAndLand");
	ros::NodeHandle n;
	
	ros::Subscriber nav_sub = n.subscribe("/ardrone/navdata", 1,chatterCallback);
	
	ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
	ros::Publisher land_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);
	std_msgs::Empty emp_msg;
	
	
	takeoff_pub.publish(emp_msg);
	//Sleepfor(5000);
	ros::Duration(5).sleep();	
	bool continu=true;
	std::thread( [&continu,&n]() {		MyController(&continu,&n); 	}	).detach();	
	while (ros::ok())
  	{
		//cout<<"#main:spin"<<endl;
    		continu=ros::ok();
		ros::spinOnce();
	}
	continu=false;
	//Sleepfor(2000);
	ros::Duration(2).sleep();
	land_pub.publish(emp_msg);
	ros::Duration(2).sleep();
	//Sleepfor(5000);
	return 0;
}

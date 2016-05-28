#include "ros/ros.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include <geometry_msgs/Vector3.h>
#include <ardrone_autonomy/Navdata.h>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
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
	 boost::this_thread::sleep_for(boost::chrono::milliseconds(ms));
}
 void chatterCallback(const ardrone_autonomy::NavdataConstPtr &msg)
{
     ROS_INFO("I heard: [%s]", FloatToString(msg->rotZ).c_str());	
}





void MyController(bool * continu,ros::NodeHandle *n)
{
	ros::Publisher twist_pub = n->advertise<geometry_msgs::Twist>("cmd_vel", 1);
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
		Sleepfor(1000);
		ROS_INFO("##mycontroller:z=0");
		twist_msg.angular.z=0;
		twist_pub.publish(twist_msg);
		Sleepfor(1000);
		ROS_INFO("##mycontroller:z=-1");
		twist_msg.angular.z=-1;
		twist_pub.publish(twist_msg);
		Sleepfor(1000);
		ROS_INFO("##mycontroller:z=0");
		twist_msg.angular.z=0;
		twist_pub.publish(twist_msg);
		Sleepfor(1000);
	}
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "testdrone");
	ros::NodeHandle n;
	
	ros::Subscriber nav_sub = n.subscribe("/ardrone/navdata", 1,chatterCallback);
	
	ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
	ros::Publisher land_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);
	std_msgs::Empty emp_msg;
	
	
	takeoff_pub.publish(emp_msg);
	Sleepfor(2000);
	
	bool continu=true;
	//std::thread( [&continu,&n]() {		MyController(&continu,&n); 	}	).detach();
	boost::thread t (&MyController,&continu,&n);	

	while (ros::ok())
  	{
		//cout<<"#main:spin"<<endl;
    		continu=ros::ok();
		ros::spinOnce();
	}
	continu=false;
	t.join();
	Sleepfor(1000);
	land_pub.publish(emp_msg);
	Sleepfor(1000);
	return 0;
}

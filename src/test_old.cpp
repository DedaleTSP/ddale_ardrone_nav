#include "ros/ros.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include <geometry_msgs/Vector3.h>
#include <ardrone_autonomy/Navdata.h>
#include <thread>
#include <sstream> 
#include <string>
#include <boost/bind.hpp>
#include <iostream>

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





void MyController(ros::NodeHandle &n, bool * continu)//,int &argc, char &**argv )
{
	//ros::init(argc, argv, "takeoffAndLand");
	ROS_INFO("Thread");
	ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	geometry_msgs::Twist twist_msg;
	if(! *(continu))
	{
		ROS_INFO("Continu == True");
		return;
	}
	else
	{
		ROS_INFO("Continu == False");
		twist_msg.angular.z=1;
		twist_pub.publish(twist_msg);
		Sleepfor(2000);
		twist_msg.angular.z=0;
		twist_pub.publish(twist_msg);
		Sleepfor(2000);
		twist_msg.angular.z=-1;
		twist_pub.publish(twist_msg);
		Sleepfor(1000);
		twist_msg.angular.z=0;
		twist_pub.publish(twist_msg);
		Sleepfor(2000);
	}
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "testdrone");
	
//	if( ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Debug) )
  //	ros::console::notifyLoggerLevelsChanged();

	ros::NodeHandle n;

	ros::Subscriber nav_sub = n.subscribe("/ardrone/navdata", 1000,chatterCallback);
	
	ros::Publisher takeoff_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1000,true);
	ros::Publisher land_pub = n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1000,true);
	std_msgs::Empty emp_msg;
	
	
	takeoff_pub.publish(emp_msg);
	Sleepfor(5000);
	
	bool continu=true;
	std::thread( [&n, &continu]() 
	{		
		boost::bind(&MyController, boost::ref(n), &continu);
		//MyController(&n, &continu);
 	}
	).detach();	
	//while (ros::ok())
  	//{
    	//	continu=ros::ok();
		ros::spin();
	//}
	land_pub.publish(emp_msg);
	Sleepfor(5000);
	return 0;
}

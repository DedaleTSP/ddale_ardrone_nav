#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <chrono>

using namespace std;


void SleepFor(int ms)
{
	this_thread::sleep_for(chrono::milliseconds(ms));
}
void debug(string mess,bool s=false)
{
	cout << mess << endl;
	if (s)
		system("pause");
}
void debugBool(bool b,string name="" )
{
	string la = "";
	if (b)
		la = " -->OK";
	else
		la = " -->NOK";
	debug(name + la);
}

//############################CLass Declarations#######################
//-------------------------------------------------------NextAction--------------------------------
class NextActionList
{
protected:
	list <bool> move_or_turn; //true = move, false =turn
	list <float> val;//between -1 1


protected:
	void debug(const string mess, const  bool s = false);
public:
	bool addAction(const bool M_O_T, const float value);
	bool  removeNextAction();
	void  deleteAllActions();
	bool isStable();
	const bool getNextType();
	const float getNextVal();
	const int  getSize();

};
//-----------------------------------------DroneSend----------------------------
class DroneSend
{
protected:
	static int number_of_DroneSend;
	float buff_turn;
	float buff_mov;
	//ros::NodeHandle n;
	//ros::Publisher com_pub;
	//ros::Publisher takeoff_pub;
	//ros::Publisher landing_pub;
	//std_msgs::Empty emp_msg;
	//geometry_msgs::Twist twist_msg;
public:
	DroneSend();
	~DroneSend();
	void bufferTurn(float angle);// We can't just send them directly in case we want to send at the same time a turn and move (the drone would receive 2 packets too quickly=>ignore one)
	void bufferMove(float speed);//in mm/s
	void send();
	void land();
	void takeoff();
};
//-----------------------------------------DroneReadAngleThread----------------------------
class DroneController;
class DroneReadAngleThread
{
protected:
	static int number_of_DroneReadAngleThread;
	DroneController *target;
	bool continu;
	//ros::NodeHandle nh_;
	//ros::Subscriber navdata_sub; // drone navdata
protected:
	//void receiveMessage(const ardrone_autonomy::NavdataConstPtr navdataPtr);
	//Probably wont work :'( might need static membership (but someway to reach the object nervertheless) 
	void updateSpeed(const float speedX = 0, const float speedY = 0, const float speedZ = 0);
	void updateAngle(const float angle = 0);
	float convAngleToDegree(const float angle);
public:
	DroneReadAngleThread(DroneController *dad);
	~DroneReadAngleThread();
	void detachUpdate();
	void stop();
	void rosLoop();	//before the loop use navdata_sub = nh_.subscribe(navdata_channel, 10, &DroneReadAngleThread::receiveMessage, this);
};
//-----------------------------------------DroneController----------------------------------
class DroneController
{
	friend class DroneReadAngleThread;
protected:
	static int number_of_DroneController;
	NextActionList to_do;
	DroneReadAngleThread reading_thread_obj;
	DroneSend send_to_drone;
	bool continu;
	float current_angle;
	float current_speedX;//speeds are supposed to be in mm/s
	float current_speedY;
	float current_speedZ;
	float precision_angle;//once we are between  desired-precision;desired+precision we stop
	float precision_distance;//once we are between  desired-precision;desired+precision we stop IN millimeters
	int refresh_time;
protected:
	void debug(const string mess, const  bool s = false);
	void turn(const float desired_angle);
	void move(const float desired_dist);

	float angularTreatment(float dif);// gives how much we try to change the angle when we have a difference of dif (btw wanted and actual) 
	//RIGHT NOW  we don't know the unit nor the interval in which it is supposed to be...
	float linearTreatment(float dif);//gives IN mm/s the speed we need to add and dif is IN mm
public:
	void launch(NextActionList orders);
	DroneController(int time_btw_refresh = 50, float Prec_angle = 5, float Prec_dist = 1000);// 5° angle precision, 1m distance precision
	~DroneController();
	void land();
	void takeOff();
	void setParam(int time_btw_refresh = 50, float Prec_angle = 5, float Prec_dist = 1000);// 5° angle precision, 1m distance precision
};
//-----------------------------------------DroneScheduler----------------------------------
class DroneScheduler
{
protected:
	NextActionList to_do;
	DroneController scheduled;
protected:
	void debug(const string mess, const  bool s = false);
public:
	void addTurn(const float angle);//angle in degrees € [-360,360]
	void addMove(const float distance);//distance in METERS € [-100,100]
	void launchSequence();
	void stop();
};


//############################Function definition################################"
//-------------------------------------------------------NextAction--------------------------------
bool NextActionList::isStable()
{
	int mleng = move_or_turn.size();
	int vleng = val.size();
	return (vleng == mleng);
}
void NextActionList::debug(const string mess, const bool s)
{
	cout << mess << endl;
	if (s)
		system("pause");
}
bool NextActionList::addAction(const bool M_O_T, const float value)
{
	if ((value > 1) || (value < -1))
	{
		debug("Value must be btw -1,1");
		return false;
	}
	move_or_turn.push_back(M_O_T);
	val.push_back(value);
	return isStable();
}
bool NextActionList::removeNextAction()
{
	if (!isStable())
	{
		debug("The NextAction object is unstable, removing not permitted");
		return false;
	}
	move_or_turn.pop_front();
	val.pop_front();
	return true;
}
void  NextActionList::deleteAllActions()
{
	for (int i = 0; i < move_or_turn.size(); i++)//separate both in case the objects is unstable
	{
		move_or_turn.pop_front();
	}
	for (int i = 0; i < val.size(); i++)
	{
		val.pop_front();
	}
}
const bool NextActionList::getNextType()
{
	if (move_or_turn.size() <= 0)
		return false;
	return move_or_turn.front();
}
const float NextActionList::getNextVal()
{
	if (move_or_turn.size() <= 0)
		return -1;
	return val.front();
}
const int  NextActionList::getSize()
{
	if (!isStable())
	{
		debug("The NextActionList is unstable, cannot retrieve size");
		return -1;
	}
	return val.size();
}

//-----------------------------------------DroneScheduler----------------------------------
void DroneScheduler::debug(const string mess, const bool s)
{
	cout << mess << endl;
	if (s)
		system("pause");
}
void DroneScheduler::addMove(const float distance)//distance in METERS € [-100,100]
{
	if ((distance > 100) || (distance < -100))
	{
		debug("distance must be btw -100 and 100");
		return;
	}
	to_do.addAction(true, distance / 100.0);
}
void DroneScheduler::addTurn(const float angle)//positive angle = to the right , angle in degrees € [-360,360]
{
	if ((angle > 360) || (angle < -360))
	{
		debug("Speed must be btw -360 and 360");
		return;
	}
	to_do.addAction(false, angle / 360.0);
}
void DroneScheduler::launchSequence()
{
	scheduled.setParam(800,5.0,100.0);
	scheduled.launch(to_do);
}
void DroneScheduler::stop()
{
	scheduled.land();
}
//-----------------------------------------DroneSend----------------------------
int DroneSend::number_of_DroneSend = 0;
DroneSend::DroneSend()
{
	number_of_DroneSend++;
	if (number_of_DroneSend > 1)
	{
		debug("Only one DroneController can exist at a given time, closing now");
		this->~DroneSend();//not sure this will work we'll see
		return;
	}
	buff_turn = 0;
	buff_mov = 0;
	//com_pub= n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	//takeoff_pub=n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
	//landing_pub=n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);
}
DroneSend::~DroneSend()
{
	number_of_DroneSend--;
}
void DroneSend::bufferTurn(float angles)
{
	buff_turn = angles;
	debug("wanna turn of " + to_string(buff_turn));
}
void DroneSend::bufferMove(float speed)
{
	buff_mov = speed;
	debug("wanna change speed to " + to_string(buff_mov));
}
void DroneSend::send()
{
	debug("Sent:(S,A)= (" + to_string(buff_mov) + "," + to_string(buff_turn) + ")");
}
void DroneSend::takeoff()
{
	debug("Takeoff !!");
}
void DroneSend::land()
{
	debug("Landing !!");
}

//-----------------------------------------DroneController----------------------------------
int DroneController::number_of_DroneController = 0;
void DroneController::debug(const string mess, const bool s)
{
	cout << mess << endl;
	if (s)
		system("pause");
}
DroneController::DroneController( int time_btw_refresh,  float Prec_angle , float Prec_dist ) :reading_thread_obj(this)
{
	number_of_DroneController++;
	if (number_of_DroneController > 1)
	{
		debug("Only one DroneController can exist at a given time, closing now");
		this->~DroneController();//not sure this will work we'll see
		return;
	}
	continu = false;
	current_angle = 0;	
	current_speedX = 0;
	current_speedY = 0;
	current_speedZ = 0;
	setParam(time_btw_refresh, Prec_angle, Prec_dist);

	//------------------Penser a lui rajouter un ross::init()

}
void DroneController::setParam(int time_btw_refresh, float Prec_angle, float Prec_dist)
{
	precision_angle = Prec_angle;
	precision_distance = Prec_dist;
	refresh_time = time_btw_refresh;
}
DroneController::~DroneController()
{
	number_of_DroneController--;
}
void DroneController::launch(NextActionList orders)
{
	to_do = orders;
	continu = true;
	if (!to_do.isStable())
	{
		debug("The todo list of the drone is unstable, a problem happend in the scheduling process");
		return;
	}
	reading_thread_obj.detachUpdate();
	takeOff();

	bool m_t = false;
	float value = 0;
	while (to_do.getSize() > 0)
	{
		 m_t = to_do.getNextType();
		 value = to_do.getNextVal();
		 if (m_t)
		 {
			 move(value*100*1000);// value is meters/100 , convert those meters in mm
		 }
		 else
		 {
			 turn((value*360)+current_angle);//work in degrees 
		 }
		 to_do.removeNextAction();
	}	
	land();

}
void DroneController::turn(const float desired_angle)
{
	send_to_drone.bufferTurn(0);//we stop the drone from moving while we turn
	send_to_drone.bufferMove(0);
	send_to_drone.send();
	SleepFor(5 * refresh_time);//just in case
	float dif = desired_angle - current_angle;
	while ((abs(dif) > precision_angle) && continu)
	{
		send_to_drone.bufferTurn(angularTreatment(dif));
		send_to_drone.bufferMove(0);


		send_to_drone.send();
		dif = desired_angle - current_angle;
		SleepFor(refresh_time);
	}
}
void DroneController::move(const float desired_dist)
{
	send_to_drone.bufferTurn(0);//we stop the drone from turning while we move
	send_to_drone.bufferMove(0);
	send_to_drone.send();
	SleepFor(5 * refresh_time);//just in case
	float desired_angle = current_angle;//we want to move in a straight line
	float approximate_distance_flown=0;//really rough estimation...
	float dif_a = desired_angle - current_angle;
	float dif_d = desired_dist - approximate_distance_flown;
	while ((abs(dif_d) > precision_distance) && continu)
	{		
		if (abs(dif_a) > precision_angle)
		{
			send_to_drone.bufferTurn(angularTreatment(dif_a));
		}
		else
		{
			send_to_drone.bufferTurn(0);//in case we were sending things before
		}
		send_to_drone.bufferMove(linearTreatment(dif_d));


		send_to_drone.send();
		approximate_distance_flown += refresh_time  * (current_speedX + current_speedY) / 1000;//speeds are supposed to be in mm/s and we work in mm and ms
					//really rough estimation... and normally current_speedZ is supposed to be 0 while we are operating
		dif_d = desired_dist - approximate_distance_flown;
		dif_a = desired_angle - current_angle;
		SleepFor(refresh_time);
	}

}
void DroneController::land()
{
	continu = false;
	SleepFor(3*refresh_time);
	send_to_drone.land();
	reading_thread_obj.stop();
}
void DroneController::takeOff()
{
	continu = true;
	send_to_drone.takeoff();
	SleepFor(3*refresh_time);
}
float DroneController::angularTreatment(float dif)
{
	//right now just a linear thing, will probably need to optimized
	return dif;
}
float DroneController::linearTreatment(float dif)
{
	//right now just a linear thing, will probably need to be optimized
	return dif;//we have a difference in mm, we return a speed in mm/s equal to it 
}

//-----------------------------------------DroneReadAngleThread----------------------------
int DroneReadAngleThread::number_of_DroneReadAngleThread = 0;
DroneReadAngleThread::DroneReadAngleThread(DroneController *dad)
{
	number_of_DroneReadAngleThread++;
	if (number_of_DroneReadAngleThread > 1)
	{
		debug("Only one DroneController can exist at a given time, closing now");
		this->~DroneReadAngleThread();//not sure this will work we'll see
		return;
	}
	target = dad;
	continu = false;
	//com_pub= n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
	//takeoff_pub=n.advertise<std_msgs::Empty>(n.resolveName("ardrone/takeoff"),1,true);
	//landing_pub=n.advertise<std_msgs::Empty>(n.resolveName("ardrone/land"),1,true);
}
DroneReadAngleThread::~DroneReadAngleThread()
{
	stop();
	SleepFor(700);
	number_of_DroneReadAngleThread--;
}
void DroneReadAngleThread::detachUpdate()
{
	DroneReadAngleThread *moi=this;
	continu = true;
	std::thread(
		[moi]() //le pointeur
	{
		moi->rosLoop();
	}
	).detach();
}
void DroneReadAngleThread::stop()
{
	continu = false;
}
void DroneReadAngleThread::rosLoop()
{
	while (continu)
	{
		updateAngle(7);
		updateSpeed(0,0);
		SleepFor(15000);
		debug("cont", true);

		updateAngle(0);
		SleepFor(5000);
		debug("cont", true);

		updateSpeed(200, 0);
		SleepFor(1000);
		debug("cont", true);

		updateSpeed(100, 0);
		SleepFor(1000);
		debug("cont", true);

		updateSpeed(20, 0);
		SleepFor(1000);
		debug("cont", true);

		updateSpeed(0, 0);
		SleepFor(1000);
		debug("finBoucle", true);

	}

}
void DroneReadAngleThread::updateSpeed(const float speedX, const float speedY, const float speedZ)
{
	if (continu)
	{
		target->current_speedX = speedX;
		target->current_speedY = speedY;
		target->current_speedZ = speedZ;
	}
}
void DroneReadAngleThread::updateAngle(const float angle)
{
	if (continu)
	{
		target->current_angle = convAngleToDegree(angle);
	}

}
float DroneReadAngleThread::convAngleToDegree(const float angle)
{
	int rest= angle / 360;
	float res = angle;//maybe something more complicated will be needed here
	return(res - (float)rest*360.0);//euclidian division my way since % doesn't work with float
}
//------------------------------------------Main---------------------------------------------
int main()
{
	debug("hello");
	DroneScheduler lala;
	lala.addTurn(-7);
	lala.addMove(5);
	lala.addTurn(-7);
	lala.addTurn(14);
	lala.launchSequence();
	debug("fin", true);
	return 0;
}

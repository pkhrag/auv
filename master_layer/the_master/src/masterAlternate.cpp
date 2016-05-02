#include <ros/ros.h>
#include <task_commons/alignAction.h>
#include <task_commons/alignActionFeedback.h>
#include <task_commons/alignActionResult.h>

#include <motion_actions/ForwardAction.h>
#include <motion_actions/ForwardActionFeedback.h>
#include <motion_actions/ForwardActionResult.h>

#include <task_commons/orangeAction.h>
#include <task_commons/orangeActionFeedback.h>
#include <task_commons/orangeActionResult.h>

#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>

using namespace std;

typedef actionlib::SimpleActionClient<task_commons::orangeAction> orange;
typedef actionlib::SimpleActionClient<task_commons::alignAction> align;
typedef actionlib::SimpleActionClient<motion_actions::ForwardAction> forward;


task_commons::orangeGoal orangegoal;
task_commons::alignGoal aligngoal;
motion_actions::ForwardGoal forwardgoal;

bool orangeSuccess= false;
bool alignSuccess = false;
bool forwardSuccess = false;

void spinThread(){
	ros::spin();
}

void forwardCb(task_commons::orangeActionFeedback msg){
	ROS_INFO("feedback recieved %d",msg.feedback.nosignificance);
}
void alignCb(task_commons::alignActionFeedback msg){
	ROS_INFO("feedback recieved %fsec remaining ",msg.feedback.AngleRemaining);
}

int main(int argc, char** argv){
	if(argc<2){
		cout<<"please specify the time for forward motion" << endl;
		return 0;
	}
	float forwardTime;
	forwardTime = atof(argv[1]);

	ros::init(argc, argv,"TheMasterNode" );
	ros::NodeHandle nh;
	//here task_commonsserver is the name of the node of the actionserver.
	ros::Subscriber subOrange = nh.subscribe<task_commons::orangeActionFeedback>("/task_commonsserver/feedback",1000,&forwardCb);
	ros::Subscriber subalign = nh.subscribe<task_commons::alignActionFeedback>("/align/feedback",1000,&alignCb);

	orange orangeClient("task_commonsserver");

	align alignClient("align");

	forward forwardClient("forward");

	ROS_INFO("Waiting for task_commons server to start.");
	orangeClient.waitForServer();
	ROS_INFO("task_commons server started");

	ROS_INFO("Waiting for align server to start.");
	alignClient.waitForServer();
	ROS_INFO("align server started.");

	boost::thread spin_thread(&spinThread);

	while(ros::ok()){
		orangegoal.order = true;
		orangeClient.sendGoal(orangegoal);
		ROS_INFO("Orange detection started");
		orangeClient.waitForResult();
		orangeSuccess = (*(orangeClient.getResult())).MotionCompleted;
		if(orangeSuccess){
			ROS_INFO("orange colour detected");
		}
		else
			ROS_INFO("orange not detected");

		aligngoal.StartDetection = true;
		alignClient.sendGoal(aligngoal);
		ROS_INFO("alignment started");
		alignClient.waitForResult();
		alignSuccess = (*(orangeClient.getResult())).MotionCompleted;
		if(alignSuccess){
			ROS_INFO("alignment successful");
		}
		else
			ROS_INFO("alignment failed");
		forwardgoal.Goal = forwardTime;
		forwardClient.sendGoal(forwardgoal);
		ROS_INFO("Moving forward");
		forwardClient.waitForResult();
		forwardSuccess = (*(forwardClient.getResult())).Result;
		if(forwardSuccess){
			ROS_INFO("forward motion successful");
		}
		else
			ROS_INFO("forward motion unsuccessful");			

	}
	return 0;
}
#include <ros/ros.h> //Ros package
#include <tf/tf.h>
#include <move_base.msg/MoveBaseAction.h>//Set nagivation goal for turtlebot
#include <actionlib/client/simple_action_client.h> //
#include <tf/transform_listener.h> //Make task of receiving transforms easier
#include <actionlib/server/simple_action_server.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/PointStamped.h"

//Function declarations
void update_endpoints (const geometry_msgs::PointStamped::ConstPtr& msg);
void moving_turtlebot (double x, double y, double yaw);
void callback (const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& msg);

//Global variables for two endpoints
double first_x;
double first_y;
double second_x;
double second_y;
int counter;

int main (int argc, char **argv){
  ros::init(argc, argv, " Moving Turtlebot ")
  ros::NodeHandle n;

  // Subscribes to two topics: the points given by the user and the position of the robot
  ros::Subscriber sub = n.subscribe("/clicked_point",1000, update_endpoints);
  ros::Subscriber sub2 = n.subscribe("/amcl_pose",1000, callback);

  ros::spin();
}


// Name: update_endpoints
// Parameters: geometry_msgs/PoseStamped target_position
// Description: Updates the global points between which the robot will patrol
//              and once they are declared, sends the robot to first coordinates
void update_endpoints (const geometry_msgs::PointStamped::ConstPtr& msg){
        if(counter == 0) {
		first_x = msg->point.x;
		first_y = msg->point.y;
		counter++;
	}
	else if(counter == 1) {
		second_x = msg->point.x;
		second_y = msg->point.y;
		counter++;

		ROS_INFO("Thank you! Beginning patrol now...\n");
		moving_turtlebot(first_x, first_y, 0.0);

	}
}


// Name: moving_turtlebot
// Parameters: x and y position, angle
// Description: sends a message to move the robot to the given x,y spot and
// 	        face in the given direction
void moving_turtlebot (double x, double y, double yaw) {
        actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> ac("move_base", true);

                ac.waitForServer();
                move_base_msgs::MoveBaseGoal goal;

                goal.target_pose.header.stamp = ros::Time::now();
                goal.target_pose.header.frame_id = "/map";

                goal.target_pose.pose.position.x = x;
                goal.target_pose.pose.position.y = y;
                goal.target_pose.pose.position.z = 0.0;
                goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(yaw);
                ac.sendGoal(goal);
                ac.waitForResult();
}


// Name: callback
// Parameters: pointer to geometry_msgs::PoseWithCovarianceStamped
// Description: checks which point the robot is closest to and then goes to the opposite point
void callback (const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& msg)
{
		if((first_x == -1) || (second_x == -1)) {
			 ROS_INFO("Please use R-Viz to 'Publish' two points\n");
			return;
		}

		if((msg->pose.pose.position.x < (first_x + 0.5)) &&
			(msg->pose.pose.position.x > (first_x - 0.5)) &&
			(msg->pose.pose.position.y < (first_y + 0.5)) &&
                        (msg->pose.pose.position.y > (first_y - 0.5)))
		{
			moving_turtlebot(second_x, second_y, 0.0);
		}
		else {
			moving_turtlebot(first_x, first_y, 0.0);
		}

}

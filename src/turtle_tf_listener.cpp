#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Spawn.h>
#include <math.h>
#include <iostream>
#include <sstream> // for ostringstream
#include <string>
#define RtoD(x) x*180/M_PI

int main(int argc, char** argv){
  ros::init(argc, argv, "my_tf_listener");

  ros::NodeHandle node;

  ros::service::waitForService("spawn");
  ros::ServiceClient add_turtle =
    node.serviceClient<turtlesim::Spawn>("spawn");
  turtlesim::Spawn srv;
  srv.request.x = 10.0f;
  srv.request.y = 10.0f;
  srv.request.theta = M_PI/4.0f;
  if (add_turtle.call(srv))
  {
    ROS_INFO("turtle2 x:%f, y:%f", (float)srv.request.x, (float)srv.request.y);
  } else {

    ROS_INFO("error!");

  }
  ros::Publisher turtle_vel =
    node.advertise<geometry_msgs::Twist>("turtle2/cmd_vel", 10);

  tf::TransformListener listener;

  ros::Rate rate(10.0);
  while (node.ok()){
    tf::StampedTransform transform;
    try{
      //transform is from turtle 2 to turtle 1
      listener.lookupTransform("/turtle2", "/turtle1",
                               ros::Time(0), transform);
    }
    catch (tf::TransformException &ex) {
      ROS_ERROR("%s",ex.what());
      ros::Duration(1.0).sleep();
    }
    tf::Matrix3x3 m = transform.getBasis();
    tfScalar rpy[3];
    std::stringstream ss;

    ss<<std::endl;
    for (int i =0 ;i<3 ;i++) {
        for(int j=0 ; j<3 ; j++) {

          ss<<m[i][j]<<" ";

        }
        ss<<std::endl;
    }
    ROS_INFO_STREAM(ss.str());
    m.getRPY(rpy[0], rpy[1], rpy[2], 1);
    ROS_INFO("Rotate from turtle2 to turtle1: roll: %f, pitch: %f, yaw: %f\n", RtoD(rpy[0]), RtoD(rpy[1]), RtoD(rpy[2]) );
    geometry_msgs::Twist vel_msg;
    // vel_msg.angular.z = 4.0 * atan2(transform.getOrigin().y(),
    //                                 transform.getOrigin().x());
    //vel_msg.angular.z =0.5* rpy[2];
    // //displayment = sqrt((x1-x2)^2+(y1-y2)^2)
    // vel_msg.linear.x = 2.0 * sqrt(pow(transform.getOrigin().x(), 2) +
    //                               pow(transform.getOrigin().y(), 2));
    turtle_vel.publish(vel_msg);

    rate.sleep();
  }
  return 0;
};

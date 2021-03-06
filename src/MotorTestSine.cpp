#include "ros/ros.h"
#include "medlab_motor_control_board/McbEncoders.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "nodeCommandSender");

  ros::NodeHandle n;

  ros::Publisher pubEncoderCommand1 = n.advertise<medlab_motor_control_board::McbEncoders>("MCB1/encoder_command", 1);
  ros::Publisher pubEncoderCommand2 = n.advertise<medlab_motor_control_board::McbEncoders>("MCB2/encoder_command", 1);
  ros::Publisher pubEncoderCommand3 = n.advertise<medlab_motor_control_board::McbEncoders>("MCB3/encoder_command", 1);
  ros::Publisher pubEncoderCommand4 = n.advertise<medlab_motor_control_board::McbEncoders>("MCB4/encoder_command", 1);

  double amplitude = 10000.0;
  double signal_hertz = 0.5;
  double loop_hertz = 250.0;
  ros::Rate loop_rate(loop_hertz);
  double current_time = 0.0;
  double count = 0;
  int offset = 0;
  while (ros::ok())
  {
    medlab_motor_control_board::McbEncoders enc;
    enc.count[0] = (int)(amplitude*sin((2.0*M_PI*signal_hertz/loop_hertz)*count)        +amplitude + offset);
    enc.count[1] = (int)(amplitude*sin((2.0*M_PI*signal_hertz/loop_hertz)*count + 0.25) +amplitude + offset);
    enc.count[2] = (int)(amplitude*sin((2.0*M_PI*signal_hertz/loop_hertz)*count + 0.5)  +amplitude + offset);
    enc.count[3] = (int)(amplitude*sin((2.0*M_PI*signal_hertz/loop_hertz)*count + 0.75) +amplitude + offset);
    enc.count[4] = (int)(amplitude*sin((2.0*M_PI*signal_hertz/loop_hertz)*count + 1.0)  +amplitude + offset);
    enc.count[5] = (int)(amplitude*sin((2.0*M_PI*signal_hertz/loop_hertz)*count + 1.25) +amplitude + offset);

    //ROS_INFO("message sent");

    pubEncoderCommand1.publish(enc);
    pubEncoderCommand2.publish(enc);
    pubEncoderCommand3.publish(enc);    
    pubEncoderCommand4.publish(enc);

    ros::spinOnce();

    loop_rate.sleep();

    count += 1.0;
  }

  return 0;
}
// %EndTag(FULLTEXT)%


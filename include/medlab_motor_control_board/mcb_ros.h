#ifndef MCBROS_H
#define MCBROS_H

#include <QObject>
#include <string>
#include <QString>
#include <stdint.h>
#include <QVector>
#include <utility>
#include <array>
#include <medlab_motor_control_board/McbEncoders.h>
#include <medlab_motor_control_board/McbEncoderCurrent.h>
#include <medlab_motor_control_board/McbStatus.h>
#include <medlab_motor_control_board/EnableMotor.h>
#include <medlab_motor_control_board/McbGains.h>
#include "ros/ros.h"

Q_DECLARE_METATYPE(std::vector<int32_t>)
Q_DECLARE_METATYPE(medlab_motor_control_board::McbEncoderCurrent)

namespace mcb {
// custom type aliases for mapping actuators to individual motors
using Motor = std::pair<uint8_t,uint8_t>; // pair of < Motherboard_index, Daughterboard_index >
using MotorMap = std::array< Motor, 7 >;


class McbRos
    : public QObject
{
  Q_OBJECT

public:
  McbRos();
  ~McbRos();
  std::string nodeName(void) {return nodeName_;}
  void init(std::string nodeName);
  bool isConnected(void);
  void queueDesiredPosition(uint8_t motor, int desiredPosition); // puts desired positions into a queue, which is not sent until calling setDesiredPosition(void)
  void flushDesiredPositionQueue();
  bool setDesiredPosition(void); // publishes all queued desired positions
  bool setDesiredPosition(medlab_motor_control_board::McbEncoders desiredPositions);
  bool setDesiredPosition(int motor, qint32 desiredPosition);
  medlab_motor_control_board::McbEncoderCurrent currentPositions(void) {return encoderCurrent_;}
  int currentPosition(uint8_t motor) {return encoderCurrent_.measured[motor];} // NOTE: does not check that motor is between 0-5!
  std::array<bool,6> getCurrentLimitStates(void) {return currentLimitStates_;}
  double getP(uint8_t motor); // returns Kp gain value from the latest status
  double getI(uint8_t motor); // returns Ki gain value from the latest status
  double getD(uint8_t motor); // returns Kd gain value from the latest status
  int    getNumMotors(void) {return (connected_ ? currentStatus_.number_modules : 0);} // returns number of detected motor modules
  QString getIp(void);   // returns IP address via uint8_t[4]
  QString getMac(void); // returns MAC address via uint8_t[6]
  QVector<float> getEfforts(void);
  bool isRosControlEnabled(void) {return currentControlState_;} // NOTE: may not be accurate if state has changed since last status query
  bool isMotorEnabled(uint8_t motor);

public slots:
  void enableRosControl(bool cmd);
  bool enableMotor(uint8_t motor, bool cmd);
  void enableAllMotors(bool cmd);
  void requestStatus(void); // for manual use if desired; automatically called by callbackStatusTimer
  bool setGains(quint8 motor, double p, double i, double d);
  bool zeroCurrentPosition(uint8_t motor); // zeroes encoder count of a single motor
  bool zeroCurrentPositions(void); // zeroes encoder count for all motors
  bool resetDacs(void); // re-initializes all DACs (useful if you notice a motor not moving despite being enabled and with a large control effort)

private:
  std::string     nodeName_;
  ros::NodeHandle nh_;
  ros::Publisher  pubEnableRos_;
  ros::Publisher  pubEnableAllMotors_;
  ros::Publisher  pubEnableMotor_;
  ros::Publisher  pubGetStatus_;
  ros::Publisher  pubEncoderCommand_;
  ros::Publisher  pubZeroSingle_;
  ros::Publisher  pubZeroAll_;
  ros::Publisher  pubResetDacs_;
  ros::Publisher  pubSetGains_;
  ros::Subscriber subStatus_;
  ros::Subscriber subEncoderCurrent_;
  ros::Subscriber subLimitSwitchEvent_;

  bool       connected_; // true after first status message received
  ros::Timer statusTimer_;
  double     statusTimerInterval_;
  void       callbackStatusTimer(const ros::TimerEvent &e);
  uint       statusRequestsWithoutReply_; // number of requestStatus() calls since last response
  const uint statusRequestsWithoutReplyLimit_; // limit before connected_ is set false and connectionLost() is signaled

  bool currentControlState_; // 0 => ROS Idle; 1 => ROS Control
  medlab_motor_control_board::McbStatus currentStatus_; // most recently received status
  medlab_motor_control_board::McbEncoderCurrent encoderCurrent_; // measured and desired positions
  std::array<bool,6> currentLimitStates_;
  std::array<int,6>  desiredPositionQueue_;
  std::array<bool,6> desiredPositionQueueState_; // true after a desired position has been queued

  void callbackSubEncoderCurrent(const medlab_motor_control_board::McbEncoderCurrent::ConstPtr& msg);
  void callbackSubStatus(const medlab_motor_control_board::McbStatus::ConstPtr& msg);
  void callbackSubLimitSwitchEvent(const medlab_motor_control_board::EnableMotor::ConstPtr& msg);

signals:
  void connectionEstablished(void);
  void connectionLost(void);
  void newPositions(medlab_motor_control_board::McbEncoderCurrent);
  void controlStateChanged(bool); // 0 => ROS Idle; 1 => ROS Control
  void motorStateChanged(int); // index of motor that changed
  void newStatus(void); // emitted after every new status is received
  void lastMotorStates(QVector<bool>);
  void limitSwitchEvent(int motor, bool state);
};
} // medlab_motor_control_board namespace
#endif // MCBROS_H

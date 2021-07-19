#ifndef SERVO_MOTOR_H_INCLUDED
#define SERVO_MOTOR_H_INCLUDED

#include "servo_angulo_interface.h"

class ServoMotor : public ServoAnguloInterface
{
public:
  ServoMotor(int);
  void servoAngulo();
  void servoFecha();
  void servoPulse(int);
private:
  int angle_max;
  int angle;
  int servo_pin;
  int pwm;
};

#endif // SERVO_MOTOR_H_INCLUDED

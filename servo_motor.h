#ifndef SERVO_MOTOR_H_INCLUDED
#define SERVO_MOTOR_H_INCLUDED

#include <Servo.h>
#include "servo_angulo_interface.h"

class ServoMotor : public ServoAnguloInterface
{
public:
  ServoMotor(int pino);
  //void servoAngulo(int tempo);
  void servoAngulo();
  void servoFecha();

private:
  Servo servo;
  int servo_pin;
  int servo_angulo;
};

#endif // SERVO_MOTOR_H_INCLUDED

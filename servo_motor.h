#ifndef SERVO_MOTOR_H_INCLUDED
#define SERVO_MOTOR_H_INCLUDED

#include "servo_angulo.h"

class ServoMotor
{
public:
  /************************
   ServoAngulo
  Requisita tempo de abertura do angulo
  entradas
    tempo de abertura
  saidas
    nenhuma
  *************************/
  ServoMotor(int pino);
  void servoAngulo(int tempo);

private:
    int servo_pin;
    int servo_angulo;
};

#endif // SERVO_MOTOR_H_INCLUDED
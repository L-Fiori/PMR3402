#include <Arduino.h>

#include "definicoes_sistema.h"
#include "servo_motor.h"

ServoMotor::ServoMotor(int pino, int angulo)
{
    this->servo_pin = pino;
    this->servo_angulo = angulo;
    this->servo.attach(pino);
    this->servo.write(0);
}

void ServoMotor::servoAngulo()
{
    servo.write(this->servo_angulo);
}

void ServoMotor::servoFecha()
{
    servo.write(0);
}
#include <Arduino.h>

#include "definicoes_sistema.h"
#include "servo_motor.h"

ServoMotor::ServoMotor(int pino)
{
    this->servo_pin = pino;
    this->servo_angulo = angulo;
    this->servo.attach(pino);
    this->servo.write(0);
}

void ServoMotor::servoAngulo(int tempo)
{
    servo.write(angulo);
    delay(tempo);
    servo.write(0);
}
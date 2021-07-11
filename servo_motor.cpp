#include <Arduino.h>
#include <Servo.h>
Servo servo;

#include "definicoes_sistema.h"
#include "servo_motor.h"

ServoMotor::ServoMotor(int pino)
{
    this->servo_pin = pino;
    this->servo_angulo = angulo;
    servo.attach(pino);
    servo.write(0);
}

void servoAngulo(int tempo)
{
    servo.write(angulo);
    delay(tempo);
    servo.write(0);
}
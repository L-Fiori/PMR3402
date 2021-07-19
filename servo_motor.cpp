#include <Arduino.h>

#include "definicoes_sistema.h"
#include "servo_motor.h"

ServoMotor::ServoMotor(int pino)
{
    //Serial.println("SERVOOO");
    this->servo_pin = pino;
    this->servo_angulo = 30;
    this->servo.attach(pino);
    this->servo.write(5);
}

void ServoMotor::servoAngulo()
{
    Serial.println("SERVOOO");
    servo.write(30);
}

void ServoMotor::servoFecha()
{
    servo.write(5);
}

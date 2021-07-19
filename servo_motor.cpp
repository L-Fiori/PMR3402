#include <Arduino.h>

#include "definicoes_sistema.h"
#include "servo_motor.h"

ServoMotor::ServoMotor(int pino)
{
    this->servo_pin = pino; // Pino do servo, com PWM
    pinMode(servo_pin,OUTPUT);
    this->angle_max = 95; // Angulo máximo de abertura
    this->angle = 0;
    this->pwm = 0;
}

void ServoMotor::servoAngulo()
{
  for (angle = 5; angle <= 95; angle += 5)  {
    this->servoPulse(angle);  }    
}

void ServoMotor::servoFecha()
{
  for (angle = 95; angle >= 5; angle -= 5)  {
    this->servoPulse(angle);  }
}

void ServoMotor::servoPulse(int angle)
{
  pwm = (angle*11) + 500;      // Convert angle to microseconds
  digitalWrite(servo_pin, HIGH);
  delayMicroseconds(pwm);
  digitalWrite(servo_pin, LOW);
  delay(50);   
}

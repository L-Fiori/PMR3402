#include <Servo.h>
Servo servo;

int pino = 6;
int tempo = 2000;
int angulo = 30;

void setup(){
  servo.attach(pino);
  servo.write(0);
}

void servoAngulo(int tempo)
{
    servo.write(angulo);
    delay(tempo);
    servo.write(0);
}

void loop(){
  servoAngulo(tempo);
  delay(3000);
}

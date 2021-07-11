#ifndef SERVO_ANGULO_INTERFACE_H_INCLUDED
#define SERVO_ANGULO_INTERFACE_H_INCLUDED

class ServoAnguloInterface
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
	virtual void servoAngulo(int tempo) = 0;
};

#endif // SERVO_ANGULO_INTERFACE_H_INCLUDED
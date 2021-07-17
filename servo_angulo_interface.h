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
	virtual void servoAngulo() = 0;
  virtual void servoFecha() = 0;
};

#endif // SERVO_ANGULO_INTERFACE_H_INCLUDED

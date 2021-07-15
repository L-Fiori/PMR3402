#ifndef RECEBE_PARAMETROS_INTERFACE_H_INCLUDED
#define RECEBE_PARAMETROS_INTERFACE_H_INCLUDED

class RecebeParametrosInterface
{
    /************************
	  recebeParametros
	Entrega os parâmetros enviados pelo celular via  Bluetooth
	entradas
	  nenhuma
	saidas
	  parâmetros
	*************************/
	virtual String recebeParametros() = 0;
	/************************
	  escreveTela
	Manda mensagem da máquina de estados para a tela do usuário
	entradas
	  String da mensagem
	saidas
	  nenhuma
	*************************/
	virtual void escreveTela(String tela) = 0;
};

#endif // RECEBE_PARAMETROS_INTERFACE_H_INCLUDED
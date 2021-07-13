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
	virtual int recebeParametros() = 0;
};

#endif // RECEBE_PARAMETROS_INTERFACE_H_INCLUDED
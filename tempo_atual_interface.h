#ifndef TEMPO_ATUAL_INTERFACE_H_INCLUDED
#define TEMPO_ATUAL_INTERFACE_H_INCLUDED

class TempoAtualInterface
{
	/************************
	  tempoAtual
	Entrega o horário atual
	entradas
	  nenhuma
	saidas
	  tempo atual
	*************************/
	virtual DateTime tempoAtual() = 0;
};

#endif // TEMPO_ATUAL_INTERFACE_H_INCLUDED

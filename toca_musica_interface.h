#ifndef TOCA_MUSICA_INTERFACE_H_INCLUDED
#define TOCA_MUSICA_INTERFACE_H_INCLUDED

class TocaMusicaInterface
{
public:
	/************************
	 tocaMusica
	requisita o sinal sonoro
	entradas
	 nenhuma
	saidas
	 nenhuma
	*************************/
	virtual void tocaMusica() = 0;
};

#endif // TOCA_MUSICA_INTERFACE_H_INCLUDED
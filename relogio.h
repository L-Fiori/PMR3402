#ifndef RELOGIO_H_INCLUDED
#define RELOGIO_H_INCLUDED

#include <RTC.h>
#include "tempo_atual.h"

class Relogio
{
public:
  /************************
   tempoAtual
  Entrega o horário atual
  entradas
    nenhuma
  saidas
    tempo atual
  *************************/
  Relogio(int pino);
  void tempoAtual();

private:
    int rtc_pin;
};

#endif // RELOGIO_H_INCLUDED
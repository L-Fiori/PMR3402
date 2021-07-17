#ifndef RELOGIO_H_INCLUDED
#define RELOGIO_H_INCLUDED

#include <Wire.h>
#include <RTClib.h>
#include "tempo_atual_interface.h"

class Relogio : public TempoAtualInterface
{
public:
  //Relogio(int pino);
  //Relogio;
  DateTime tempoAtual();

private:
  RTC_DS1307 rtc;
  //int rtc_pin;
};

#endif // RELOGIO_H_INCLUDED

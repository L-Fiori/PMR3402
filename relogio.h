#ifndef RELOGIO_H_INCLUDED
#define RELOGIO_H_INCLUDED

#include <Wire.h>
#include <RTClib.h>
#include "tempo_atual_interface.h"

class Relogio : public TempoAtualInterface
{
public:
  Relogio();
  DateTime tempoAtual();
  RTC_DS1307 rtc;
  
private:
  //int rtc_pin;
};

#endif // RELOGIO_H_INCLUDED

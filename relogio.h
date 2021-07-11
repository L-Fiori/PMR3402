#ifndef RELOGIO_H_INCLUDED
#define RELOGIO_H_INCLUDED

#include <RTC.h>
#include <RTClib.h>
#include "tempo_atual_interface.h"

class Relogio : public TempoAtualInterface
{
public:
  Relogio(int pino);
  void tempoAtual();

private:
  RTC_DS3231 rtc;
  int rtc_pin;
};

#endif // RELOGIO_H_INCLUDED
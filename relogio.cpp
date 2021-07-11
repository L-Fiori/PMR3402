#include <Arduino.h>
#include <RTClib.h>
RTC_DS3231 rtc;

#include "definicoes_sistema.h"
#include "relogio.h"

Relogio::Relogio(int pino)
{
    this->rtc_pin = pino;
    pinMode(rtc_pin,INPUT);
}

int tempoAtual()
{
    DateTime now = rtc.now();
    return {now.hour(), now.minute()};
}
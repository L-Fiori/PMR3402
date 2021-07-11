#include <Arduino.h>

#include "definicoes_sistema.h"
#include "relogio.h"

Relogio::Relogio(int pino)
{
    this->rtc_pin = pino;
    pinMode(rtc_pin, INPUT);
}

int Relogio::tempoAtual()
{
    DateTime now = rtc.now();
    return {now.hour(), now.minute()};
}
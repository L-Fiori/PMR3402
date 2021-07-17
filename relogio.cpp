#include <Arduino.h>

#include "definicoes_sistema.h"
#include "relogio.h"

//Relogio::Relogio(int pino)
//Relogio::Relogio
//{
    //this->rtc_pin = pino;
    //pinMode(rtc_pin, INPUT);
//}

DateTime Relogio::tempoAtual()
{
    DateTime now = rtc.now();
    return now;
}

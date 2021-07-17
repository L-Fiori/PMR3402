#include <Arduino.h>

#include "definicoes_sistema.h"
#include "relogio.h"

Relogio::Relogio()
{
}

DateTime Relogio::tempoAtual()
{
    DateTime agora = rtc.now();
    return agora;
}

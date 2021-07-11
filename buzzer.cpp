#include <Arduino.h>

#include "definicoes_sistema.h"
#include "buzzer.h"

tocarmusica::tocarmusica(int pino)
{
    this->buzzer_pin = pino;
}

void tocarmusica(void)
{
    tone(buzzer_pin,1500);
    delay(1000);
    noTone(buzzer_pin);
}
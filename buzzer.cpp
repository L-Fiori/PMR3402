#include <Arduino.h>
#include "definicoes_sistema.h"
#include "buzzer.h"

buzzer::buzzer(int pino)
{
    this->buzzer_pin = pino;
    pinMode(buzzer_pin,OUTPUT);
}

void tocarmusica(void)
{
    tone(buzzer_pin,494);
    delay(650);
    noTone(buzzer_pin);
    tone(buzzer_pin,554);
    delay(650);
    noTone(buzzer_pin);
    tone(buzzer_pin,440);
    delay(650);
    noTone(buzzer_pin);
    tone(buzzer_pin,220);
    delay(650);
    noTone(buzzer_pin);
    tone(buzzer_pin,330);
    delay(650);
    noTone(buzzer_pin);
}
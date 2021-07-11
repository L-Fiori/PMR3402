#include <Arduino.h>

#include "definicoes_sistema.h"
#include "buzzer.h"

int pino = b;

tocarmusica::tocarmusica(int pino)
{
    this->buzzer_pin = pino;
}

void tocarmusica(void)
{

}
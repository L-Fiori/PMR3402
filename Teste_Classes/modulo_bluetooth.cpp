#include <Arduino.h>

#include "definicoes_sistema.h"
#include "modulo_bluetooth.h"

ModuloBluetooth::ModuloBluetooth(int pinoRX, int pinoTX):bluetooth(pinoTX, pinoRX)
{
    this->bluetooth.begin(9600);
}

int ModuloBluetooth::recebeParametros()
{
    if(bluetooth.available()){
       return bluetooth.read();
    }
    else{
        return (-1);
    }
}
#include <Arduino.h>

#include "definicoes_sistema.h"
#include "modulo_bluetooth.h"

//ModuloBluetooth::ModuloBluetooth
//{}

String ModuloBluetooth::recebeParametros()
{
  Serialdata = "";
  Dabble.processInput();
  if(Terminal.available())
  {
    while (Terminal.available() != 0)
    {
      Serialdata = String(Serialdata + char(Terminal.read()));
    }
  }
  return Serialdata;
}

void ModuloBluetooth::escreveTela(String tela)
{
  Terminal.println(tela);
}

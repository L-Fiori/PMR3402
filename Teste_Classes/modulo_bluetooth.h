#ifndef MODULO_BLUETOOTH_H_INCLUDED
#define MODULO_BLUETOOTH_H_INCLUDED

#include "recebe_parametros_interface.h"
#include <SoftwareSerial.h>

class ModuloBluetooth : public RecebeParametrosInterface
{
public:
  ModuloBluetooth(int pinoRX, int pinoTX);
  int recebeParametros();

private:
  SoftwareSerial bluetooth;
};

#endif // MODULO_BLUETOOTH_H_INCLUDED
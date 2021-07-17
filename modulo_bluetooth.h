#ifndef MODULO_BLUETOOTH_H_INCLUDED
#define MODULO_BLUETOOTH_H_INCLUDED
#define CUSTOM_SETTINGS

#include "recebe_parametros_interface.h"
#include <Dabble.h>

class ModuloBluetooth : public RecebeParametrosInterface
{
public:
  ModuloBluetooth();
  String recebeParametros();
  void escreveTela(String tela);

private:
  String Serialdata;
};

#endif // MODULO_BLUETOOTH_H_INCLUDED

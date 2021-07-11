#ifndef BUZZER_H_INCLUDED
#define BUZZER_H_INCLUDED
#include "toca_musica_interface.h"

class Buzzer : public TocaMusicaInterface
{
public:
  Buzzer(int pino);
  void tocaMusica(void);

private:
  int buzzer_pin;
};

#endif // BUZZER_H_INCLUDED
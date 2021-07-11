#ifndef BUZZER_H_INCLUDED
#define BUZZER_H_INCLUDED
#include "toca_musica.h"

class Buzzer : public TocaMusica
{
public:
  /************************
   tocarmusica
  requisita o sinal sonoro
  entradas
    nenhuma
  saidas
    nenhuma
  *************************/
  void tocarmusica(void);

private:
    int buzzer_pin;

};

#endif // BUZZER_H_INCLUDED
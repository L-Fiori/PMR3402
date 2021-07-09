// Definicoes do sistema
#include "definicoes_sistema.h"

// Componentes
#include "buzzer.h"
#include "modulo_bluetooth.h"
#include "relogio.h"
#include "servo_motor.h"

/***********************************************************************
 Componentes
 ***********************************************************************/
Buzzer buz;
ModuloBluetooth mbt;
Relogio rel;
ServoMotor srv;

/***********************************************************************
 Estaticos
 ***********************************************************************/
int codigoEvento = NENHUM_EVENTO;
int eventoInterno = NENHUM_EVENTO;
int estado = ESPERA;
int codigoAcao;
int acao_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS];
int proximo_estado_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS];

/************************************************************************
 Main
 Loop principal de controle que executa a maquina de estados
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/

void setup()
{
	Serial.begin(9600);
}

void loop()
{
}
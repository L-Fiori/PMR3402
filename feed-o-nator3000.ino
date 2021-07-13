// Definicoes do sistema
#include "definicoes_sistema.h"

// Componentes
#include "buzzer.h"
#include "modulo_bluetooth.h"
#include "relogio.h"
#include "servo_motor.h"

/***********************************************************************
 Pinos
 ***********************************************************************/
#define PIN_BUZZER 8
#define PIN_RELOGIO 10
#define PIN_SERVO 11
#define PIN_RX 7
#define PIN_TX 9

/***********************************************************************
 Componentes
 ***********************************************************************/
Buzzer buz(PIN_BUZZER);
ModuloBluetooth mbt(PIN_RX, PIN_TX);
Relogio rel(PIN_RELOGIO);
ServoMotor srv(PIN_SERVO);

/***********************************************************************
 Estaticos
 ***********************************************************************/
int codigoEvento = NENHUM_EVENTO;
int eventoInterno = NENHUM_EVENTO;
int estado = DESCONECTADO;
int codigoAcao;
int acao_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS];
int proximo_estado_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS];

int executarAcao(int codigoAcao)
{
	int retval;

	retval = NENHUM_EVENTO;
	if (codigoAcao == NENHUMA_ACAO)
		return retval;

	switch (codigoAcao)
	{
	case A01:
		tmr.iniciar(true);
		break;
	case A02:
		sne.bip();
		com.notificar("Alarme em alerta");
		tmr.iniciar(false);
		break;
	case A03:
		com.notificar("Alarme desacionado");
		tmr.iniciar(false);
		break;
	case A04:
		com.notificar("Alarme desacionado");
		break;
	case A05:
		tmr.iniciar(true);
		break;
	case A06:
		sne.acionar(true);
		com.notificar("Invasao");
		tmr.iniciar(false);
		break;
	case A07:
		com.notificar("Alarme desacionado");
		tmr.iniciar(false);
		sne.acionar(false);
		break;
	} // switch

	return retval;
} // executarAcao

/************************************************************************
 iniciaMaquina de Estados
 Carrega a maquina de estados
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/
void iniciaMaquinaEstados()
{
	/*
		             Conectado          Parametros_recebidos     Horario_inicio      Horario_fim         Configurar_parametros   (EVENTOS)
	Desconectado	Configuracao / A01    
	Configuracao                             Operando / A02
	Operando                                                        Despejo / A03                            Configuracao / A05
	Despejo                                                                            Operando / A04
	(ESTADOS)
	*/

	int i;
	int j;

	for (i = 0; i < NUM_ESTADOS; i++)
	{
		for (j = 0; j < NUM_EVENTOS; j++)
		{
			acao_matrizTransicaoEstados[i][j] = NENHUMA_ACAO;
			proximo_estado_matrizTransicaoEstados[i][j] = i;
		}
	}
	proximo_estado_matrizTransicaoEstados[DESCONECTADO][CONECTADO] = CONFIGURACAO;
	acao_matrizTransicaoEstados[DESCONECTADO][CONECTADO] = A01;

	proximo_estado_matrizTransicaoEstados[CONFIGURACAO][PARAMETROS_RECEBIDOS] = OPERANDO;
	acao_matrizTransicaoEstados[CONFIGURACAO][PARAMETROS_RECEBIDOS] = A02;

	proximo_estado_matrizTransicaoEstados[OPERANDO][HORARIO_INICIO] = DESPEJO;
	acao_matrizTransicaoEstados[OPERANDO][HORARIO_INICIO] = A03;

	proximo_estado_matrizTransicaoEstados[OPERANDO][CONFIGURAR_PARAMETROS] = CONFIGURACAO;
	acao_matrizTransicaoEstados[OPERANDO][CONFIGURAR_PARAMETROS] = A05;

	proximo_estado_matrizTransicaoEstados[DESPEJO][HORARIO_FIM] = OPERANDO;
	acao_matrizTransicaoEstados[DESPEJO][HORARIO_FIM] = A04;

} // initStateMachine

/************************************************************************
 iniciaSistema
 Inicia o sistema ...
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/
void iniciaSistema()
{
	iniciaMaquinaEstados();
} // initSystem

/************************************************************************
 obterEvento
 Obtem um evento, que pode ser da IHM ou do alarme
 Parametros de entrada: nenhum
 Retorno: codigo do evento
*************************************************************************/

int obterEvento()
{
	int retval = NENHUM_EVENTO;

	DateTime now = rel.tempoAtual();
	int hora_rtc = now.hour();
	int min_rtc = now.minute();
	int seg_rtc = now.second();

	if (bluetooth.available() && estado == DESCONECTADO)
		return CONECTADO;

	if (mbt.recebeParametros() == 'A') // Acabou de configurar
		return PARAMETROS_RECEBIDOS;

	if (mbt.recebeParametros() == 'Q') // Quero configurar
		return CONFIGURAR_PARAMETROS;

	if (hora_rtc == hora_parametro && min_rtc == min_parametro && estado == OPERANDO)
		return HORARIO_INICIO;

	if (seg_rtc == 'tempo necessario' && estado == DESPEJO) // Tempo necessario depende do modo de operacao (porcao)
		return HORARIO_FIM;

	return retval;

} // obterEvento

/************************************************************************
 obterAcao
 Obtem uma acao da Matriz de transicao de estados
 Parametros de entrada: estado (int)
                        evento (int)
 Retorno: codigo da acao
*************************************************************************/
int obterAcao(int estado, int codigoEvento)
{
	return acao_matrizTransicaoEstados[estado][codigoEvento];
} // obterAcao

/************************************************************************
 obterProximoEstado
 Obtem o proximo estado da Matriz de transicao de estados
 Parametros de entrada: estado (int)
                        evento (int)
 Retorno: codigo do estado
*************************************************************************/
int obterProximoEstado(int estado, int codigoEvento)
{
	return proximo_estado_matrizTransicaoEstados[estado][codigoEvento];
} // obterAcao

/************************************************************************
 Main
 Loop principal de controle que executa a maquina de estados
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/
void setup()
{
	Serial.begin(9600);

	iniciaSistema();
	Serial.println("Feed-o-nator 3000 iniciado");
} // setup

void loop()
{
	if (eventoInterno == NENHUM_EVENTO)
	{
		codigoEvento = obterEvento();
	}
	else
	{
		codigoEvento = eventoInterno;
	}
	if (codigoEvento != NENHUM_EVENTO)
	{
		codigoAcao = obterAcao(estado, codigoEvento);
		estado = obterProximoEstado(estado, codigoEvento);
		eventoInterno = executarAcao(codigoAcao);
		Serial.print("Estado: ");
		Serial.print(estado);
		Serial.print(" Evento: ");
		Serial.print(codigoEvento);
		Serial.print(" Acao: ");
		Serial.println(codigoAcao);
	}
} // loop
}
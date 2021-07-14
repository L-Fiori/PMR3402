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
#define SERVO_ANGULO 30

/***********************************************************************
 Componentes
 ***********************************************************************/
Buzzer buz(PIN_BUZZER);
ModuloBluetooth mbt(PIN_RX, PIN_TX);
Relogio rel(PIN_RELOGIO);
ServoMotor srv(PIN_SERVO, SERVO_ANGULO);

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
		break;
	case A02:
		char modo = mbt.recebeParametros();

		if (modo == 'H')
		{
			char qtd_porcoes = mbt.recebeParametros(); // Provavel que precise transformar em int

			char[qtd_porcoes * 2] parametro; // Indices pares --> horas; indices impares --> minutos

			for (i = 0; i < qtd_porcoes; i++)
			{
				char hora_parametro = mbt.recebeParametros();
				char min_parametro = mbt.recebeParametros();

				parametro[2 * i] = hora_parametro;
				parametro[2 * i + 1] = min_parametro;
			}

			char tempo_abertura = mbt.recebeParametros();
		}

		elseif(modo == 'I')
		{
			char tempo_parametro = mbt.recebeParametros(); // Sempre em minutos
			char hora_inicial = mbt.recebeParametros();
			char min_inicial = mbt.recebeParametros();
			char tempo_abertura = mbt.recebeParametros();
		}
		break;
	case A03:
		srv.servoAngulo();
		break;
	case A04:
		srv.servoFecha();
		if (modo == 'I')
			tempo_corrido = 0;
		break;
	case A05:
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

int obterEvento(int hora_parametro, int min_parametro, int tempo_abertura)
{
	int retval = NENHUM_EVENTO;

	DateTime now = rel.tempoAtual();
	int hora_rtc = now.hour();
	int min_rtc = now.minute();
	int seg_rtc = now.second();

	if (estado == DESCONECTADO && bluetooth.available())
		return CONECTADO;

	if (mbt.recebeParametros() == 'A') // Acabou de configurar
		return PARAMETROS_RECEBIDOS;

	if (mbt.recebeParametros() == 'Q') // Quero configurar
		return CONFIGURAR_PARAMETROS;

	if (estado == OPERANDO)
	{
		if (modo == 'H')
		{
			if (hora_rtc == hora_parametro && min_rtc == min_parametro) // Precisa mudar pra comparar com um array e nao somente uma variavel simples.
				return HORARIO_INICIO;
		}
		elseif(modo == 'I')
		{
			if (hora_rtc == hora_inicial && min_rtc == min_inicial && seg_rtc == 0)
			{
				tempo_corrido = 0;
				// tempo_corrido = rel.tempoAtual() - ; contar o tempo a partir do horario de inicio
				return HORARIO_INICIO;
			}
			if (tempo_corrido == tempo_parametro)
				return HORARIO_INICIO;
		}
	}

	if (estado == DESPEJO && seg_rtc == tempo_abertura) // Tempo de abertura depende do modo de operacao (porcao)
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
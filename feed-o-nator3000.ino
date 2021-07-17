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
#define PIN_RX 3
#define PIN_TX 2
#define PIN_SCL A5 // Pino de cima do rtc
#define PIN_SDA A4 // Pino segundo de cima do rtc
#define SERVO_ANGULO 30

/***********************************************************************
 Componentes
 ***********************************************************************/
Buzzer buz(PIN_BUZZER);
ModuloBluetooth mbt;
//Relogio rel(PIN_RELOGIO);
Relogio rel;
//ServoMotor srv(PIN_SERVO, SERVO_ANGULO);
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
int tempo_abertura;
String modo = "";
String horario_inicial;
String intervalo;
int parametro[100];
int tempo_corrido;
int hora_rtc;
int min_rtc;
int seg_rtc;

String recebeParametro(){
  unsigned long time_agora;
  unsigned long timer;
  String tipo = "";
  
  time_agora = millis();
  timer = millis();
  while (timer < time_agora + 15000){
    timer =  millis();
    Serial.println(timer);
    tipo = mbt.recebeParametros();
    if (tipo != ""){
      break;
    }
  }

  return tipo;
}


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
		mbt.escreveTela("Digite H para modo horário ou I para modo intervalo");
    modo = recebeParametro();

		if (modo == "H")
		{
			mbt.escreveTela("Digite o número de porções diárias");
			String qtd_porcoes_str = recebeParametro();
			//qtd_porcoes = (qtd_porcoes);
      int qtd_porcoes = qtd_porcoes_str.toInt();

			//global int[qtd_porcoes * 2] parametro; // Indices pares --> horas; indices impares --> minutos
      parametro[qtd_porcoes * 2];

			for (int i = 0; i < qtd_porcoes; i++)
			{
				mbt.escreveTela("Digite o horário da porção no modelo HHMM");
				String hora_recebida = recebeParametro();
        int hora_recebida_int[4];
        for (int i = 0; i < sizeof(hora_recebida); i++){
          hora_recebida_int[i] = atoi(hora_recebida[i]);
        }
       
				parametro[2 * i] = hora_recebida_int[0]*10 + hora_recebida_int[1] ;
				parametro[2 * i + 1] = hora_recebida_int[2]*10 + hora_recebida_int[3];
			}

		}

		else if(modo == 'I')
		{
			mbt.escreveTela("Digite o horário de início da contagem do intervalo no formato HH:MM");
			horario_inicial = recebeParametro();
			mbt.escreveTela("Digite o intervalo entre despejos a partir do horário inicial em minutos");
			String intervalo = recebeParametro();
		}

		mbt.escreveTela("Digite o tamanho das porções (P, M ou G)");
		String tamanho_porcao = recebeParametro();

		if (tamanho_porcao == "P")
				tempo_abertura = 1;
		else if(tamanho_porcao == "M")
			    tempo_abertura = 2;
		else if(tamanho_porcao == "G")
			    tempo_abertura = 3;
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
	Desconectado	Configuracao / A02    
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
	acao_matrizTransicaoEstados[DESCONECTADO][CONECTADO] = A02;

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

	DateTime agora = rel.tempoAtual();
	int hora_rtc = agora.hour();
	int min_rtc = agora.minute();
	int seg_rtc = agora.second();
  //Serial.print("Hora:");
  //Serial.print(hora_rtc);
  //Serial.print(":");
  //Serial.print(min_rtc);
  //Serial.print(":");
  //Serial.println(seg_rtc);

	if (estado == DESCONECTADO )//&& bluetooth.available())
		return CONECTADO;

	// isso ainda não está bom, nao sabemos onde colocar a string "digite Q para configurar e A para terminei de configurar"
	
	if (mbt.recebeParametros() == 'A') // Acabou de configurar
		return PARAMETROS_RECEBIDOS;

	if (mbt.recebeParametros() == 'Q') // Quero configurar
		return CONFIGURAR_PARAMETROS;

	if (estado == OPERANDO)
	{
		if (modo == 'H')
		{
			int array_size = sizeof(parametro) / sizeof(parametro[0]);
			for (int i = 0; i < array_size; i = i + 2)
			{
				if (parametro[i] == hora_rtc && parametro[i + 1] == min_rtc && seg_rtc == 0)
				{
					return HORARIO_INICIO;
				}
			}
		}
		else if(modo == 'I')
		{
			if (seg_rtc == 0){
				delay(1000);
				tempo_corrido ++;
			}
			if (hora_rtc == (horario_inicial[0]+horario_inicial[1]) && min_rtc == (horario_inicial[3]+horario_inicial[4]) && seg_rtc == 0)
			{
				tempo_corrido = 0;
				return HORARIO_INICIO;
			}
			if (tempo_corrido == intervalo.toInt()){
				tempo_corrido = 0;
				return HORARIO_INICIO;
			}
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
	Dabble.begin(9600);
  if (! rel.rtc.begin()) {                         //Se o RTC nao for inicializado, faz
    Serial.println("RTC NAO INICIALIZADO");    //Imprime o texto
    while (1);                                 //Trava o programa
  }
  //rel.rtc.adjust(DateTime(2021, 7, 17, 17, 34, 30));
  delay(100);
	iniciaSistema();
	Serial.println("Feed-o-nator 3000 iniciado");
} // setup

void loop()
{
	if (eventoInterno == NENHUM_EVENTO)
	{
		codigoEvento = obterEvento();
    //Serial.println(estado);
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

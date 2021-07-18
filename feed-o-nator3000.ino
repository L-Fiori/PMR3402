// Definicoes do sistema
#include "definicoes_sistema.h"

// Componentes
#include "buzzer.h"
#include "modulo_bluetooth.h"
#include "relogio.h"
#include "servo_motor.h"

// Bibliotecas
#include <EEPROM.h>

/***********************************************************************
 Pinos
 ***********************************************************************/
#define PIN_BUZZER 8
#define PIN_SERVO 11
#define PIN_RX 3 // Bluetooth
#define PIN_TX 2 // Bluetooth
#define PIN_SCL A5 // Pino de cima do rtc
#define PIN_SDA A4 // Pino segundo de cima do rtc
#define SERVO_ANGULO 30

/***********************************************************************
 Componentes
 ***********************************************************************/
Buzzer buz(PIN_BUZZER);
ModuloBluetooth mbt;
Relogio rel;
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
int parametro[20];
int tempo_corrido;
int hora_rtc;
int min_rtc;
int seg_rtc;

String recebeParametro(){
  unsigned long timer_inicio;
  unsigned long timer_atual;
  String tipo = "";
  
  timer_inicio = millis();
  timer_atual = millis();
  while (timer_atual < timer_inicio + 15000){
    timer_atual =  millis();
    //Serial.println(timer);
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
  int modo_eeprom = 0;
  int modo_eeprom_l = 0;
  int qtd_porcoes = 0;
  String intervalo = "";

	retval = NENHUM_EVENTO;
	if (codigoAcao == NENHUMA_ACAO)
		return retval;

	switch (codigoAcao)
	{
	case A01:
		mbt.escreveTela("Digite H para modo horário ou I para modo intervalo");
    modo = recebeParametro();

		if (modo == "H")
		{
      modo_eeprom = 1; 
			mbt.escreveTela("Digite o número de porções diárias");
			String qtd_porcoes_str = recebeParametro(); // Espera receber o número de porções
      qtd_porcoes = qtd_porcoes_str.toInt(); // Transforma para inteiro

      //parametro[qtd_porcoes * 2]; // Indices pares --> horas; indices impares --> minutos

			for (int i = 0; i < qtd_porcoes; i++)
			{
				mbt.escreveTela("Digite o horário da porção no modelo HHMM");
				String hora_recebida_str = recebeParametro(); // Espera receber horário 
        int hora_recebida_int[4];
        for (int i = 0; i < sizeof(hora_recebida_str); i++){
          hora_recebida_int[i] =  String(hora_recebida_str[i]).toInt(); // Transforma para inteiro
        }
				parametro[2 * i] = hora_recebida_int[0]*10 + hora_recebida_int[1]; // Guarda as horas
				parametro[2 * i + 1] = hora_recebida_int[2]*10 + hora_recebida_int[3]; // Guarda os minutos
        Serial.print("Hora armz:");
        Serial.print(parametro[2*i]);
        Serial.print("Min armz:");
        Serial.println(parametro[2*i+1]);
			}

		}

		else if(modo == "I")
		{
      modo_eeprom = 2; 
			mbt.escreveTela("Digite o horário de início da contagem do intervalo no formato HHMM");
			horario_inicial = recebeParametro();
			mbt.escreveTela("Digite o intervalo entre despejos a partir do horário inicial em horas");
			intervalo = recebeParametro();
		}

		mbt.escreveTela("Digite o tamanho das porções (P, M ou G)");
		String tamanho_porcao = recebeParametro();

		if (tamanho_porcao == "P"){
      tempo_abertura = 1;
		}
				
		else if(tamanho_porcao == "M") {
      tempo_abertura = 2;
		}
			    
		else if(tamanho_porcao == "G"){
      tempo_abertura = 3;
		}
		break;
	case A02:
		srv.servoAngulo();
		break;
	case A03:
		srv.servoFecha();
		if (modo == "I")
			tempo_corrido = 0;
		break;
	} // switch
 
  // Escrevendo na eeprom
  if (modo_eeprom == 1){ // Modo = horario fixo
    EEPROM.write(0,modo_eeprom);
    EEPROM.write(1,qtd_porcoes);
    for (int i = 2; i <= 2*qtd_porcoes; i=i+2) {
      EEPROM.write(i,parametro[i-2]);
      EEPROM.write(i+1,parametro[i-1]);
    }
    EEPROM.write(2*qtd_porcoes+2,tempo_abertura);
  }
  else if(modo_eeprom == 2) { // Modo = intervalo
    EEPROM.write(0,modo_eeprom);
    int horario_eeprom = horario_inicial.toInt();
    EEPROM.write(1,horario_eeprom/100);
    EEPROM.write(2,horario_eeprom%100);
    EEPROM.write(3,intervalo.toInt());
    EEPROM.write(4,tempo_abertura);
  }

  // Leitura da eeprom
  modo_eeprom_l = EEPROM.read(0);
  if (modo_eeprom_l == 1){
    int qtd_porcoes_eeprom = EEPROM.read(1);
    for (int i = 2; i <= 2*qtd_porcoes_eeprom; i=i+2) {
      int hora_eeprom = EEPROM.read(i);
      int minuto_eeprom = EEPROM.read(i+1);
      Serial.print(hora_eeprom);
      Serial.print(":");
      Serial.println(minuto_eeprom);
    }
    int tempo_eeprom = EEPROM.read(2*qtd_porcoes_eeprom+2);
    Serial.print("  tempo: ");
    Serial.println(tempo_eeprom);
  }
  if (modo_eeprom_l == 2){
    int hora_eeprom = EEPROM.read(1);
    int minuto_eeprom = EEPROM.read(2);
    int intervalo_eeprom = EEPROM.read(3);
    int tempo_eeprom = EEPROM.read(4);
    Serial.print(hora_eeprom);
    Serial.print(":");
    Serial.print(minuto_eeprom);
    Serial.print("  intervalo: ");
    Serial.print(intervalo_eeprom);
    Serial.print("  tempo: ");
    Serial.println(tempo_eeprom);
  }
  
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
		             Conectado            Configurar           Horario_inicio        Horario_fim    (EVENTOS)
	Desconectado	 Operando / A01    
	Operando                            Operando / A01        Despejo / A02                      
	Despejo                                                                       Operando / A03
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
	proximo_estado_matrizTransicaoEstados[DESCONECTADO][CONECTADO] = OPERANDO;
	acao_matrizTransicaoEstados[DESCONECTADO][CONECTADO] = A01;

	proximo_estado_matrizTransicaoEstados[OPERANDO][CONFIGURAR] = OPERANDO;
	acao_matrizTransicaoEstados[OPERANDO][CONFIGURAR] = A01;

	proximo_estado_matrizTransicaoEstados[OPERANDO][HORARIO_INICIO] = DESPEJO;
	acao_matrizTransicaoEstados[OPERANDO][HORARIO_INICIO] = A02;

	proximo_estado_matrizTransicaoEstados[DESPEJO][HORARIO_FIM] = OPERANDO;
	acao_matrizTransicaoEstados[DESPEJO][HORARIO_FIM] = A03;

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
  int modo_eeprom = EEPROM.read(0);
  
	DateTime agora = rel.tempoAtual();
	int hora_rtc = agora.hour();
	int min_rtc = agora.minute();
	int seg_rtc = agora.second();
  static int flag_seg = seg_rtc;
  if (flag_seg != seg_rtc){
    Serial.print("Hora:");
    Serial.print(hora_rtc);
    Serial.print(":");
    Serial.print(min_rtc);
    Serial.print(":");
    Serial.println(seg_rtc);
    flag_seg = seg_rtc;
  }

 
	if (estado == DESCONECTADO )//&& bluetooth.available())
		return CONECTADO;

	// isso ainda não está bom, nao sabemos onde colocar a string "digite Q para configurar e A para terminei de configurar"
	//unsigned long tempo = 500;
  //String configuracao = recebeParametro(tempo);
	if (mbt.recebeParametros() == "Q") // Quero configurar
		return CONFIGURAR;

	if (estado == OPERANDO)
	{
		if (modo_eeprom == 1)
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
		else if(modo_eeprom == 2)
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
	Serial.begin(9600); // Começa a comunicação serial com o computador
	Dabble.begin(9600); // Começa a cominucação serial com o módulo bluetooth
  if (! rel.rtc.begin()) {                     //Se o RTC não for inicializado, faz
    Serial.println("RTC NAO INICIALIZADO");    //Imprime o texto
    while (1);                                 //Trava o programa
  }
  //rel.rtc.adjust(DateTime(2021, 7, 17, 17, 34, 30)); // Quando for necessário ajustar o horário do RTC
  delay(100);
	iniciaSistema();
	Serial.println("Feed-o-nator 3000 iniciado");
} // setup

void loop()
{ 
  int modo_eeprom_1 = EEPROM.read(0);
  Serial.println(modo_eeprom_1);
  if (modo_eeprom_1 == 2) {
    int hora_eeprom = EEPROM.read(1);
    int minuto_eeprom = EEPROM.read(2);
    int intervalo_eeprom = EEPROM.read(3);
    int tempo_eeprom = EEPROM.read(4);
    Serial.print(hora_eeprom);
    Serial.print(":");
    Serial.print(minuto_eeprom);
    Serial.print("  intervalo: ");
    Serial.print(intervalo_eeprom);
    Serial.print("  tempo: ");
    Serial.println(tempo_eeprom);
  }
  
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

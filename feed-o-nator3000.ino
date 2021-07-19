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
#define PIN_BUZZER 8 // Pino do buzzer
#define PIN_SERVO 10 // Pino do servo (PWM)
#define PIN_SCL A5 // Pino de cima do rtc
#define PIN_SDA A4 // Pino segundo de cima do rtc
#define PIN_RX 3 // Bluetooth RX
#define PIN_TX 2 // Bluetooth TX

/***********************************************************************
 Componentes
 ***********************************************************************/
ServoMotor srv(PIN_SERVO);
Buzzer buz(PIN_BUZZER);
ModuloBluetooth mbt;
Relogio rel;

/***********************************************************************
 Estaticos
 ***********************************************************************/
int proximo_estado_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS]; // Máquina de estados
int acao_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS]; // Máquina de estados
int eventoInterno = NENHUM_EVENTO; // Máquina de estados
int codigoEvento = NENHUM_EVENTO; // Máquina de estados
int codigoAcao = NENHUMA_ACAO; // Máquina de estados
int estado = OPERANDO; // Máquina de estados
byte vetor_intervalos[12]; // Vetor que contém todos os intervalos de despejo
byte parametro[18]; // Vetor para coletar parâmetros do modo 1 (por hora fixa)
byte tempo_abertura = 0; // Valor para o tempo de abertura (
byte flag_intervalo = 1; // Flag para modificar os intervalos de despejo
byte modo_eeprom; // Modo de despejo lido da EEPROM
byte qtd_porcoes; // Quantidades de porções para despejar no modo 1
String horario_inicial = ""; // String para guardar o horario inserido pelo usuário
String tamanho_porcao = ""; // Tamanho da porção P, M ou G
String intervalo = ""; // Intervalo de despejo do modo 2
String modo = ""; // Modo de despejo

// Função para receber os parâmetros do usuário
String recebeParametro(){
  unsigned long timer_inicio;
  unsigned long timer_atual;
  String tipo = "";
  // Espera 15 segundos ou até o usuário digitar algo
  timer_inicio = millis();
  timer_atual = millis();
  while (timer_atual < timer_inicio + 15000){
    timer_atual =  millis();
    tipo = mbt.recebeParametros();
    if (tipo != ""){
      return tipo;
    }
  }
  return "z";
}

// Função para configurar com os parâmetros do usuário
void configurar()
{
  mbt.escreveTela("Digite H para modo horario ou I para modo intervalo");
  modo = recebeParametro();
  if (modo == "z") return;

  if (modo == "H")
  {
    modo_eeprom = 1; 
    mbt.escreveTela("Digite o numero de porcoes diarias");
    String qtd_porcoes_str = recebeParametro(); // Espera receber o número de porções
    if (qtd_porcoes_str == "z") return;
    qtd_porcoes = qtd_porcoes_str.toInt(); // Transforma para inteiro

    for (int i = 0; i < qtd_porcoes; i++)
    {
      mbt.escreveTela("Digite o horario da porcao no modelo HHMM");
      String hora_recebida_str = recebeParametro(); // Espera receber horário
      if (hora_recebida_str == "z") return;
      byte hora_recebida_int[4];
      for (int i = 0; i < sizeof(hora_recebida_str); i++){
        hora_recebida_int[i] =  String(hora_recebida_str[i]).toInt(); // Transforma para inteiro
      }
      parametro[2 * i] = hora_recebida_int[0]*10 + hora_recebida_int[1]; // Guarda as horas
      parametro[2 * i + 1] = hora_recebida_int[2]*10 + hora_recebida_int[3]; // Guarda os minutos
    }
  }

  else if(modo == "I")
  {
    modo_eeprom = 2; 
    mbt.escreveTela("Digite o horario de inicio da contagem do intervalo no formato HHMM");
    horario_inicial = recebeParametro();
    if (horario_inicial == "z") return;
    mbt.escreveTela("Digite o intervalo entre despejos a partir do horario inicial em horas");
    intervalo = recebeParametro();
    if (intervalo == "z") return;
  }

  mbt.escreveTela("Digite o tamanho das porcoes (P, M ou G)");
  tamanho_porcao = recebeParametro();
  if (tamanho_porcao == "z") return;

  if (tamanho_porcao == "P")
    tempo_abertura = 7;   // Porção pequena -> 7 segundos de despejo
  else if(tamanho_porcao == "M")
    tempo_abertura = 10;  // Porção média -> 11 segundos de despejo
  else if(tamanho_porcao == "G")
    tempo_abertura = 15;  // Porção grande -> 15 segundos de despejo
}


int executarAcao(int codigoAcao)
{
  int retval;
  byte flag_escrever = 0;

  retval = NENHUM_EVENTO;
  if (codigoAcao == NENHUMA_ACAO)
    return retval;

  switch (codigoAcao)
  {
  case A01:
    configurar(); // Função para obter configurações do despejo via bluetooth
    flag_escrever = 1; // Flag para escrever nova configuração na EEPROM
    break;
  case A02:
    srv.servoAngulo(); // Abre o servo
    srv.servoAngulo(); // Abre o servo
    srv.servoAngulo(); // Abre o servo
    Serial.println("Servo aberto");
    break;
  case A03:
    srv.servoFecha(); // Fecha o servo
    srv.servoFecha(); // Fecha o servo
    srv.servoFecha(); // Fecha o servo
    buz.tocaMusica(); // Toca música indicando o despejo da ração
    Serial.println("Servo fechado");
    break;
  } // switch

  if (flag_escrever == 1) {
    // Escrita na eeprom
    if (modo_eeprom == 1){ // Modo = horario fixo
      EEPROM.write(0,modo_eeprom);
      EEPROM.write(1,qtd_porcoes); // Número de despejos
      for (int i = 2; i <= 2*qtd_porcoes; i=i+2) {
        EEPROM.write(i,parametro[i-2]);
        EEPROM.write(i+1,parametro[i-1]);
      }
      EEPROM.write(2*qtd_porcoes+2,tempo_abertura);
      flag_intervalo = 1; // Flag para construir o intervalo
    }
    else if(modo_eeprom == 2) { // Modo = intervalo
      EEPROM.write(0,modo_eeprom);
      int horario_eeprom = horario_inicial.toInt();
      EEPROM.write(1,horario_eeprom/100); // Hora
      EEPROM.write(2,horario_eeprom%100); // Minuto
      EEPROM.write(3,intervalo.toInt()); // Intervalo de despejo
      EEPROM.write(4,tempo_abertura); // Porção despejada
      flag_intervalo = 1; // Flag para construir o intervalo
    }
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
                Configurar        Horario_inicio      Horario_fim    (EVENTOS)  
  Operando    Operando / A01      Despejo / A02                      
  Despejo                                            Operando / A03
  (ESTADOS)
  */

  int i;
  int j;

  for (i = 0; i < NUM_ESTADOS; i++)
  {
    for (j = 0; j < NUM_EVENTOS; j++)
    {
      acao_matrizTransicaoEstados[i][j] = NENHUMA_ACAO;
      proximo_estado_matrizTransicaoEstados[i][j] = i; }
  }

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
  byte hora_rtc = agora.hour();
  byte min_rtc = agora.minute();
  byte seg_rtc = agora.second();
  static byte flag_seg = seg_rtc;
  if (flag_seg != seg_rtc){
    Serial.print("Hora:");
    Serial.print(hora_rtc);
    Serial.print(":");
    Serial.print(min_rtc);
    Serial.print(":");
    Serial.println(seg_rtc);
    if (seg_rtc%10 == 0) {
      mbt.escreveTela("Pressione Q para configurar o despejo"); // Escreve a mensagem a cada 10 segundos
    }
    flag_seg = seg_rtc; // Não entra no if até o próximo segundo
  }
  
  if (mbt.recebeParametros() == "Q") // Quero configurar
    return CONFIGURAR;

  if (estado == OPERANDO)
  {
    if (modo_eeprom == 1)
    {
      int qtd_porcoes_eeprom = EEPROM.read(1);
      int hora_eeprom = 0;
      int minuto_eeprom = 0;
      if (flag_intervalo == 1) {
        Serial.print("Intervalos atualmente configurados: ");
        for (int i = 2; i <= 2*qtd_porcoes_eeprom; i=i+2) {
          hora_eeprom = EEPROM.read(i);
          minuto_eeprom = EEPROM.read(i+1);
          vetor_intervalos[i-2] = hora_eeprom;
          vetor_intervalos[i-1] = minuto_eeprom;
          Serial.print(hora_eeprom);
          Serial.print(":");
          Serial.print(minuto_eeprom);
          Serial.print(", ");
        }
        tempo_abertura = EEPROM.read(2*qtd_porcoes_eeprom+2);
        Serial.println("");
        flag_intervalo = 0;
      }
      
      for (int i = 0; i < 2*qtd_porcoes_eeprom; i=i+2) {
        if (hora_rtc == vetor_intervalos[i] && min_rtc == vetor_intervalos[i+1] && seg_rtc == 0) {
          Serial.println("RACAO DESPEJADA POR HORA FIXA");
          mbt.escreveTela("RACAO DESPEJADA POR HORA FIXA");
          return HORARIO_INICIO; }
      }
    }
    else if(modo_eeprom == 2)
    {
      int hora_eeprom = EEPROM.read(1);
      int minuto_eeprom = EEPROM.read(2);
      int intervalo_eeprom = EEPROM.read(3);
      tempo_abertura = EEPROM.read(4);
      if (flag_intervalo == 1) {
        Serial.print("Intervalos atualmente configurados: ");
        for (int i = 0; i < (24/intervalo_eeprom); i++) {
          vetor_intervalos[i] = intervalo_eeprom*i + hora_eeprom%intervalo_eeprom;
          Serial.print(vetor_intervalos[i]);
          Serial.print(":");
          Serial.print(minuto_eeprom);
          Serial.print(", ");
        }
        Serial.println("");
        flag_intervalo = 0;
      }
      
      for (int i = 0; i < 24/intervalo_eeprom; i++){
        if (hora_rtc == vetor_intervalos[i] && min_rtc == minuto_eeprom && seg_rtc == 0) {
          Serial.println("RACAO DESPEJADA POR INTERVALO");
          mbt.escreveTela("RACAO DESPEJADA POR INTERVALO");
          return HORARIO_INICIO; }
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
    while (1); }                                //Trava o programa
  //rel.rtc.adjust(DateTime(2021, 7, 18, 20, 15, 00)); // Quando for necessário ajustar o horário do RTC
  delay(100);
  srv.servoFecha(); // Fecha o servo
  srv.servoFecha(); // Fecha o servo
  srv.servoFecha(); // Fecha o servo
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

#include <RTClib.h>
RTC_DS3231 rtc;

int rtc_pin = 8;

void setup() {
    pinMode(rtc_pin,INPUT);
}

int tempoAtual()
{
    DateTime now = rtc.now();
    Serial.print("Horas: "); //IMPRIME O TEXTO NA SERIAL
    Serial.print(now.hour(), DEC); //IMPRIME NO MONITOR SERIAL A HORA
    Serial.print(':'); //IMPRIME O CARACTERE NO MONITOR SERIAL
    Serial.print(now.minute(), DEC); //IMPRIME NO MONITOR SERIAL OS MINUTOS
}

void loop() {
  tempoAtual();
  delay(5000);
}

#ifndef DEFINICOES_SISTEMA_H_INCLUDED
#define DEFINICOES_SISTEMA_H_INCLUDED

#define true 1
#define false 0

#define NUM_ESTADOS 4
#define NUM_EVENTOS 5

// ESTADOS
#define DESCONECTADO 0
#define CONFIGURACAO 1
#define OPERANDO 2
#define DESPEJO 3

// EVENTOS
#define NENHUM_EVENTO -1
#define CONECTAR 0
#define PARAMETROS_RECEBIDOS 1
#define HORARIO_INICIO 2
#define HORARIO_FIM 3
#define CONFIGURAR_PARAMETROS 4

// ACOES
#define NENHUMA_ACAO -1
#define A01 0
#define A02 1
#define A03 2
#define A04 3
#define A05 4

#endif // DEFINICOES_SISTEMA_H_INCLUDED
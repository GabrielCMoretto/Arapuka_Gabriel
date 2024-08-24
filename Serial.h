// Puka - Serial.h

#include "Defines.h"

#ifndef SERIAL_H_
#define SERIAL_H_

/*
// Opções para Baudrate Serial
// SMCLK = 20 MHz
#define BR_2400      1
#define BR_4800      2
#define BR_9600      3
#define BR_19200     4
#define BR_38400     5
#define BR_57600     6
#define BR_74880     7
#define BR_115200    8
#define BR_230400    9      //230.400
#define BR_250k     10
#define BR_460800   11      //460.800
#define BR_500k     12
#define BR_921600   13      //921.600
#define BR_1000k    14
#define BR_1843k2   15      //1.843.200
#define BR_2000k    16
*/

// SERI USCI_A1(PC) integradas
extern volatile char seri_fila[SERI_FILA_TAM]; //Espaço para a fila serial de entrada
extern volatile int seri_pin, seri_pout;       //Ponteiros para usar a fila

void rec_msg(char vet[], int limite);
void ser1_data_hora(char *vet);
void ser1_linha(char *vt);
void ser1_crono(unsigned long x);

void ser1_double(double f, char prec);
void ser1_float(float f, char prec);

void ser1_dec32(long dt);
void ser1_dec32u(long dt);
void ser1_dec32nz(long dt);
void ser1_dec32unz(long dt);
void ser1_hex32(long dt);

void ser1_dec16(int dt);
void ser1_dec16u(unsigned int dt);
void ser1_dec16nz(int dt);
void ser1_dec16unz(unsigned int dt);
void ser1_hex16(int dt);

void ser1_bcd8(char dt);
void ser1_dec8(char dt);
void ser1_dec8u(char dt);
void ser1_dec8nz(char dt);
void ser1_dec8unz(char dt);
void ser1_hex8(char dt);

void ser1_str(char *msg);
void ser1_crlf(char x);
void ser1_spc(char x);
void ser1_char(char x);

char seri_cmdo_x(void);
char seri_cmdo(char *argc, char *argv, char limite);
char seri_cmdo2(char *argc, char *argv);
void seri_come_crlf(void);
void seri_come_ndec(void);
long seri_dec32u(void);
char seri_vazia(void);
char seri_xereta(char *cha);
void seri_config(void);
char seri_poe(char cha);
void seri_cheia(void);
char seri_tira(char *cha);
void seri_dump(void);
void ser1_config(char br);
void usci_a1_rx(void);
void usci_a1_tx(void);





#endif /* SERIAL_H_ */

// Puka - Defines.h

#ifndef DEFINES_H_
#define DEFINES_H_

#define TRUE    1   //Verdadeiro
#define FALSE   0   //Falso
#define CR      0xD //Carriage Return
#define LF      0xA //Line Feed

#define SMCLK   20000000L   //SMCLK = 20 MHz
#define ACLK    32768       //ACLK = 32768 Hz

// Filas
// USCI_A0=GPRS e USCI_A1=PC
#define SERI_FILA_TAM 128   //Tamanho da fila circular de entrada serial
#define GPRS_FILA_TAM 128   //Tamanho da fila circular de entrada do GPRS
#define GPS_FILA_TAM 128    //Tamanho da fila circular de entrada do GPS

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

// Buffer LCD
#define LCD_BUF_TAM  137    //Buffer para LCD com DMA 8 + 32x4 = 136
#define USCIB1_TXBUF_ADDR  (__SFR_FARPTR) 0x062E    //Endereço USCI_B1_TXBUF

// Parâmetros para as chaves
#define FECHADA     0      //Chave fechada
#define ABERTA      1      //Chave aberta
#define TRANSITO    2      //Mudança de estado
#define SW_CONT_MAX 5      //Máximo de contagens

// Parametros para a maquina de estados
#define DMT 0
#define VIG 1
#define SUS 2
#define ALT1 3
#define ALT2 4

#endif /* DEFINES_H_ */

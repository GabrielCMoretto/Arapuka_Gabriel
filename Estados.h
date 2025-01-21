#ifndef ESTADOS_H_
#define ESTADOS_H_

extern char serialMSG;
extern char estado;
extern int ax, ay, az, tp, gx, gy, gz;
extern int maiorx, menorx, maiory, menory, maiorz, menorz,roubado, alerta;
extern char vetor[14];
volatile unsigned int estado_i;
volatile unsigned int estado_y;
volatile char estado_comando[24];
char estado_puka[10];
long wr_address_mem;
char mem_vetor[20];
char mem_vt[10];
volatile int n_rd,m_rd;
extern volatile int receiveiterator;
extern volatile char receivestring[22];

void dormente ();
void vigilia ();
void suspeito ();
void alerta1 ();
void alerta2 ();
void sel_estado();
void status();
void estados_config();
void check_estado_address();
void apagar();
void ler_memoria();
void resete();
void ler_n();
void ler_n_m();
void rtc_configure();
char checkreceive(char x);
#endif

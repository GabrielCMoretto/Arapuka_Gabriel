// Gprs.h

#ifndef GPRS_H_
#define GPRS_H_

// GPRS USCI_A0(PC) integradas
extern volatile char gprs_fila[GPRS_FILA_TAM]; //Espaço para a fila serial de entrada
extern volatile int gprs_pin, gprs_pout;       //Ponteiros para usar a fila
extern volatile int receiveiterator;
extern volatile char receivestring[22];
extern char estado;
extern int state_cod;
extern char serialMSG;

void gprs_str(char *msg);
void gprs_char(char x);
void gprs_send_cmd(char *msg);
void gprs_send_msg(char *msg);
void gprs_config(void);
char gprs_poe(char cha);
void gprs_cheia(void);
char gprs_tira(char *cha);
void ser0_config(char br);
void set_values_gps();
void gprs_config_receive(char x);

#endif /* GPRS_H_ */

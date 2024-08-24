// Puka - Modos.h

#ifndef MODOS_H_
#define MODOS_H_

// Modos de Operação
#define MODO_TOT  21    //Modos: 0, 1, 2 , ..., 20
#define MODO_0    0     //Nada
#define MODO_1    1     //Livre
#define MODO_2    2     //Livre
#define MODO_3    3     //Livre
#define MODO_4    4     //Livre
#define MODO_5    5     //Livre
#define MODO_6    6     //Livre
#define MODO_7    7     //Livre
#define MODO_8    8     //Livre
#define MODO_9    9     //Livre
#define MODO_10  10     //Livre
#define MODO_11  11     //Livre
#define MODO_12  12     //Livre
#define MODO_13  13     //Livre
#define MODO_14  14     //Livre
#define MODO_15  15     //Livre
#define MODO_16  16     //Livre
#define MODO_17  17     //Livre
#define MODO_18  18     //Livre
#define MODO_19  19     //Livre
#define MODO_20  20     //Livre
#define MODO_99  99     //Não sei! Comando errado

extern char *modos_lista[];

char modo_0 (char modo);
char modo_1 (char modo);
char modo_2 (char modo);
char modo_3 (char modo);
char modo_4 (char modo);
char modo_5 (char modo);
char modo_6 (char modo);
char modo_7 (char modo);
char modo_8 (char modo);
char modo_9 (char modo);
char modo_sw (char modo);
char modo_leds (char modo);
char modo_lcd (char modo);
char modo_mpu (char modo);
char modo_rtc (char modo);
char modo_wq (char modo);
char modo_gps (char modo);
char modo_gprmc (char modo);
char modo_18 (char modo);
char modo_19 (char modo);
char modo_20 (char modo);
char modo_99 (char modo);
void modo_ser1 (char modo);

#endif /* MODOS_H_ */

// Puka - Rtc.h

#ifndef RTC_H_
#define RTC_H_

/////////////// RTC DS 3231 - Constantes
#define RTC_ADR  0x68  //Endere�o RTC DS3231
#define RTC_EWR  0xD0  //RTC para escrita (0x68<<1)
#define RTC_ERD  0xD1  //RTC para leitura (0x68<<1 + 1)

void rtc_wr_vet(char reg, char *vt, char qtd);
void rtc_rd_vet(char reg, char *vt, char qtd);
char rtc_rd(char reg);
void rtc_wr(char reg, char dado);
void rtc_estado();
char passou_1_hora();
void coloca_hora_dezena();
char passou_3_hora();
void atualiza_data_hora(int min, int hora);
char passou_1_min();
void coloca_min_dezena();

extern volatile char rtc_vetor[7];
extern volatile char rtc_msg[18];
extern volatile int ultimo_vetor_data_hora[6], atual_vetor_data_hora[6];
extern volatile char rtc_ultima_data_hora_1_hora[18];
extern volatile char rtc_ultima_data_hora_1_min[18];

#endif /* RTC_H_ */

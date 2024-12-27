// Strings.h

#ifndef STRINGS_H_
#define STRINGS_H_

void str_data_hora(char *vet, char *msg);
char str_cmp(char *s1, char *s2);
int str_len(char *msg);
void str_crono(unsigned long x, char *msg);
void str_rmvz_u(char *msg);
void str_rmvz_s(char *msg);
void str_double(double f, char prec, char *msg);
void str_float(float f, char prec, char *msg);
void str_dec32(long c, char *msg);
void str_dec32u(long c, char *msg);
void str_hex32(long c, char *msg);
void str_bcd16(int c, char *msg);
void str_dec16(int c, char *msg);
void str_dec16u(unsigned int c, char *msg);
void str_hex16(int c, char *msg);
void str_bcd8(char c, char *msg);
void str_dec8(signed char c, char *msg);
void str_dec8u(char c, char *msg);
void str_hex8(char c, char *msg);
long str_2_num(char *vt);
char eh_hexa(char letra);
char eh_dec(char letra);
char asc_nib(char asc);
long str_2_num(char *vt);
void str_maiusc(char *vt);
void IntToChar(int number, char *buffer);

#endif /* STRINGS_H_ */

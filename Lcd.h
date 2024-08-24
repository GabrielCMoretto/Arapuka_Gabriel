// Puka - Lcd.h

#ifndef LCD_H_
#define LCD_H_

// Opções para Baudrate Serial
// SMCLK = 20 MHz
#define SCL_50k       1
#define SCL_100k      2
#define SCL_200k      3
#define SCL_400k      4

extern volatile char i2c_tranca;   //Evitar duplo uso do I2C (porque LCD usa DMA)
extern volatile char lcd_adr;      //Endereço do PCF8574
extern volatile char lcd_tem;      //TRUE se LCD estiver presente
extern volatile char lcd_buf[];    //Buffer para LCD operar com DMA

void lcdb_data(char lin, char col, char *vet);
void lcdb_hora(char lin, char col, char *vet);
void lcdb_crono(char lin, char col, unsigned long x);
void lcdb_barra(char lin, long x, long max);

void lcdb_double(char lin, char col, double f, char prec);
void lcdb_float(char lin, char col, float f, char prec);

void lcdb_dec32(char lin, char col, long dt);
void lcdb_dec32u(char lin, char col, long dt);
void lcdb_dec32nz(char lin, char col, long dt);
void lcdb_dec32unz(char lin, char col, long dt);
void lcdb_hex32(char lin, char col, long dt);

void lcdb_dec16(char lin, char col, int dt);
void lcdb_dec16u(char lin, char col, unsigned int dt);
void lcdb_dec16nz(char lin, char col, int dt);
void lcdb_dec16unz(char lin, char col, unsigned int dt);
void lcdb_hex16(char lin, char col, int dt);

void lcdb_bcd8(char lin, char col, char dt);
void lcdb_dec8(char lin, char col, char dt);
void lcdb_dec8u(char lin, char col, char dt);
void lcdb_dec8nz(char lin, char col, char dt);
void lcdb_dec8unz(char lin, char col, char dt);
void lcdb_hex8(char lin, char col, char dt);

//void lcdb_str(char lin, char col, char *msg);
void lcdb_str(unsigned char lin, unsigned char col, char *msg);

void lcdb_char(char lin, char col, char letra);
void lcdb_apaga(void);

void lcd_atualiza(void);
void lcd_dma_config(void);
void lcd_str(char *msg);
void lcd_char(char letra);
void lcd_cursor(char pos);
void lcd_cmdo(char cmdo);
void lcd_write(char dado);
void lcd_inic(void);
void lcd_aux(char dado);
void lcd_delay(long limite);
char lcd_presente(void);
char i2c_trancar(void);
int i2c_teste_adr(char adr);
void i2c_config(char scl_freq);
void i2c_recupera(void);



#endif /* LCD_H_ */

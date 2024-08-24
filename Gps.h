// Puka - Gps.h

#ifndef GPS_H_
#define GPS_H_

extern volatile char gps_fila[GPS_FILA_TAM]; //Espaço para a fila serial de entrada
extern volatile int gps_pin, gps_pout;       //Ponteiros para usar a fila
extern volatile int gps_estado;              //Estado do receptor por software
extern volatile char gps_dado;               //Montar o dado que chega serialmente do GPS

// Só para teste
extern char gps_gprmc_ok[];
extern char gps_gprmc_nok[];

// GPS: serial por Software
// Variáveis para o prog principal colher as informações
extern volatile char gps_tranca;   //Prog principal coletando informações, perde uma atualização do GPS
extern volatile char gps_gprmc_novo;  //Indica que tem novos campo GPdados
extern volatile char gps_st;       //Estados para a busca do GPRMC
extern volatile char gps_sat;      //(TRUE/FALSE) Achou satélites?
extern volatile char gps_hora[7];  //hhmmss0
extern volatile char gps_data[7];  //ddmmyy0
extern volatile char gps_lat[11];  //ddmm.mmmmm0
extern volatile char gps_ns;       //(N/S) Latitude
extern volatile char gps_long[12]; //dddmm.mmmmm0
extern volatile char gps_ew;       //(E/W) Latitude
extern volatile char gps_gprmc[90];//$GPRMC receber os dados para extrair parâmetros

void copia_ate_virgula(char *ix, char *vt);
void gps_gprmc_extrai(void);
void gps_gprmc_busca(char dado);

char gps_xereta(char *cha);
void gps_config(void);
char gps_poe(char cha);
void gps_cheia(void);
char gps_tira(char *cha);
void gps_dump(void);







#endif /* GPS_H_ */

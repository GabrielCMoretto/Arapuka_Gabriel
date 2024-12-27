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

extern volatile char gps_msg[22];
extern float vol_lat, vol_long;
extern volatile char gps_ew2;
extern volatile long int menorlatitude, maiorlatitude, menorlongitude, maiorlongitude;
extern volatile long int lat, lataux, longt, longtaux;

void copia_ate_virgula(char *ix, char *vt);
void gps_gprmc_extrai(void);
void gps_gprmc_busca(char dado);

char gps_xereta(char *cha);
void gps_config(void);
char gps_poe(char cha);
void gps_cheia(void);
char gps_tira(char *cha);
void gps_dump(void);
void gps_estado_modo();
char calculo_GPRMC(void);
void separa_algarismos_gps_f(char vt[22]);
void set_values_gps();
void int_latitude_longitude();
char gps_furto();


#endif /* GPS_H_ */

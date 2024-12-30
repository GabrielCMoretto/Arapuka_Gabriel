// Puka - Globais.h

char gps_gprmc_ok[] =
        "083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A*57\r\n"; //Testar separação
char gps_gprmc_nok[] = ",V,,,,,,,,,,N*53\r\n"; //Testar separação - sem satélites

// Lista impressa quando se entra com "?"
volatile char modo;      //Modo de operação
char *modos_lista[21] = { "00) Nada\n",             //00
        "01) Livre_01\n",         //01
        "02) Livre_02\n",         //02
        "03) Livre_03\n",         //03
        "04) Livre_04\n",         //04
        "05) Livre_05\n",         //05
        "06) Livre_06\n",         //06
        "07) Livre_07\n",         //07
        "08) Livre_08\n",         //08
        "09) Livre_09\n",         //09
        "10) Testar SW\n",        //10 - SW
        "11) Testar LEDs\n",      //11 - LEDs
        "12) Testar LCD\n",       //12 - LCD
        "13) Testar MPU\n",       //13 - MPU
        "14) Testar RTC\n",       //14 - RTC
        "15) Testar WQ\n",        //15 - WQ
        "16) Testar GPS\n",       //16 - GPS
        "17) Testar GPRMC\n",     //17 - GPRMC
        "18) Livre_18\n",         //18
        "19) Testar ESP8266\n",   //19 - ESP8266
        "20) Livre_20\n" };        //20

// MPU 6050
volatile char mpu_tem;      //TRUE se LCD estiver presente

// Timers - Variáveis
volatile unsigned int crono_cont;   //Contador para o Cronômetro (TA1)
volatile unsigned char flag_c_seg;   //Flag Centésimos de segundo
volatile unsigned char flag_d_seg;   //Flag Décimos de segundo
volatile unsigned char flag_seg;     //Flag Segundo
volatile unsigned char cont_c_seg;   //Contar centésimos de segundo;
volatile unsigned char cont_d_seg;   //Contar décimos de segundo;
volatile unsigned char cont_seg;     //Contar segundos;
volatile unsigned char cont_min;     //Contar minutos;

// Variáveis para monitorar as chaves
volatile char sw1_cont;     //0, 1, ..., SW_CONT_MAX
volatile char sw1_estado;   //Instantâneo: ABERTA, TRANSITO, FECHADA
volatile char sw1;          //TRUE se chave acionada
volatile char sw2_cont;     //0, 1, ..., SW_CONT_MAX
volatile char sw2_estado;   //Instantâneo: ABERTA, TRANSITO, FECHADA
volatile char sw2;          //TRUE se chave acionada

////////////////////////////////////////////////////////////
///////////////////////  LCD  //////////////////////////////
////////////////////////////////////////////////////////////
volatile char i2c_tranca;   //TRUE indica que I2C está sendo usado
volatile char lcd_tem;      //TRUE se LCD estiver presente
volatile char lcd_adr;      //Endereço do PCF8574
volatile char lcd_flag;     //Indica que precisa atualizar LCD
volatile char lcd_buf[LCD_BUF_TAM]; //Buffer para todo LCD

////////////////////////////////////////////////////////////
/////////////////////// FILAS //////////////////////////////
////////////////////////////////////////////////////////////

// SERI: USCI_A1(PC) integradas
volatile char seri_fila[SERI_FILA_TAM]; //Espaço para a fila serial de entrada
volatile int seri_pin, seri_pout;       //Ponteiros para usar a fila

// GPRS: USCI_A0(PC) integradas
volatile char gprs_fila[GPRS_FILA_TAM]; //Espaço para a fila serial de entrada
volatile int gprs_pin, gprs_pout;       //Ponteiros para usar a fila

////////////////////////////////////////////////////////////
/////////////////////// FILAS //////////////////////////////
////////////////////////////////////////////////////////////

// GPS: serial por Software
// Variáveis para o prog principal colher as informações,strigs terminam com zero
volatile char gps_tranca; //Prog principal coletando informações, perde uma atualização do GPS
volatile char gps_gprmc_novo;   //Indica que tem novos dados
volatile char gps_st = 0;         //Estados para a busca do GPRMC
volatile char gps_sat;          //(TRUE/FALSE) Achou satélites?
volatile char gps_hora[11];     //hhmmss.sss0
volatile char gps_data[7];      //ddmmyy0
volatile char gps_lat[11];      //ddmm.mmmmm0
volatile char gps_ns;           //(N/S) Latitude
volatile char gps_long[12];     //dddmm.mmmmm0
volatile char gps_ew;           //(E/W) Latitude
volatile char gps_ew2;
volatile char gps_gprmc[90];   //$GPRMC receber os dados para extrair parâmetros

volatile char gps_fila[GPS_FILA_TAM];   //Espaço para a fila do GPS
volatile int gps_pin, gps_pout;         //Ponteiros para usar a fila
volatile int gps_estado;                //Estado do receptor por software
volatile char gps_dado;             //Montar o dado que chega serialmente do GPS
volatile char numero_telefone = "+5521979592145";

////////////////////////////////////////////////////////////
/////////// MAQUINA DE ESTADOS DO OPERA ////////////////////
////////////////////////////////////////////////////////////

char serialMSG = TRUE;
char estado = DMT;
int  ax, ay, az, tp, gx, gy, gz,maiorx = 0, menorx = 0, maiory = 0, menory = 0, maiorz = 0, menorz = 0,roubado=0, alerta=0;
volatile int amaiorx, amenorx, amaiory, amenory, amaiorz, amenorz;
volatile int maiorgx, menorgx, maiorgy, menorgy, maiorgz, menorgz;
char vetor[14];
volatile char toda_msg[70];
long wr_address_mem;
volatile char rtc_vetor[7];
volatile char rtc_msg[18];
volatile char gps_msg[22];
float vol_lat, vol_long;
volatile long int menorlatitude, maiorlatitude, menorlongitude, maiorlongitude;
volatile long int lat, lataux, longt, longtaux;
//ultimo data e hora salva              Data e hora atualizada
volatile int ultimo_vetor_data_hora[6], atual_vetor_data_hora[6]; //cada posição refere a cada dezena da data e hora vetor[0] = dia, vetor[1] = mes, vetor[2] = ano, vetor[3] = hora, vetor[4] = min, vetor[5] = seg
//ultima data e hora que foi mandado a mensagem de roubo, referente a 1 hora
volatile char rtc_ultima_data_hora_1_hora[18];
//ultima data e hora que foi mandado a mensagem de roubo, referente a 1 min
volatile char rtc_ultima_data_hora_1_min[18];

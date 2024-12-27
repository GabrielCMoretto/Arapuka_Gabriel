// Puka - Gps.c

// GPS simula porta serial por software
// Só recepção está implementada.
// P1.2 = GPS-TX
// Usa TA1 para sincronizar baud rate

#include <msp430.h>
#include "Defines.h"
#include "Gps.h"
#include "Gpio.h"
#include "Serial.h"

////////////////////////////////////////////////////
////////////////// Fila GPS ///////////////////////
///////////// MSP430 <== GPS ///////////////////////
/////// Interrupção chama gps_poe() ///////////////
/////// Ler a fila como gps_tira()  ///////////////
////////////////////////////////////////////////////

// Informa qual o próximo byte da fila SERI
// Não altera estado dos ponteiros
// Chamada fora das interrupções
// FALSE = fila vazia
char gps_xereta(char *cha)
{
    int pin, pout;
    __disable_interrupt();
    pout = gps_pout; //Copiar
    pin = gps_pin;   //ponteiros
    __enable_interrupt();
    pout++;
    if (pout == GPS_FILA_TAM)
        pout = 0;
    if (pout == pin)
    {
        return FALSE;
    }
    else
    {
        *cha = gps_fila[pout];
        return TRUE;
    }
}

// Colocar um byte na fila SERI
// Chamada dentro Interrupções de RX USCI_A1
char gps_poe(char cha)
{
    if (gps_pin == gps_pout)
    {
        gps_cheia();     //Msg de fila cheia
        return FALSE;     //SERI cheia
    }
    else
    {
        gps_fila[gps_pin++] = cha;
        if (gps_pin == GPS_FILA_TAM)
            gps_pin = 0;
        return TRUE;
    }
}

// Mensagem de Fila SERI Cheia
void gps_cheia(void)
{
    __disable_interrupt();
    ser1_str("\nERRO: GPS Cheia!\n");
    gps_dump();
    ser1_str("\nTravado!");
    while (TRUE)
        ;
}

// Tirar um byte da fila SERI
// Função chamada fora das interrupções
char gps_tira(char *cha)
{
    int pout_aux;
    __disable_interrupt();
    pout_aux = gps_pout + 1;
    if (pout_aux == GPS_FILA_TAM)
        pout_aux = 0;
    if (pout_aux == gps_pin)
    {
        __enable_interrupt();
        return FALSE;
    }
    else
    {
        *cha = gps_fila[pout_aux];
        gps_pout = pout_aux;
        __enable_interrupt();
        return TRUE;
    }
}

// Imprimir fila de entrada SERI
// Para debug, chamada quando fila enche
void gps_dump(void)
{
    int i;
    ser1_str("\nGPS:  Tamanho=");
    ser1_dec8u(GPS_FILA_TAM);
    ser1_str("  gps_pin=");
    ser1_dec8u(gps_pin);
    ser1_str("  gps_pout=");
    ser1_dec8u(gps_pout);
    ser1_crlf(1);
    for (i = 0; i < GPS_FILA_TAM; i += 16)
    {
        ser1_dec8u(i);
        ser1_char(':');
        ser1_char(' ');
        ser1_linha(&gps_fila[i]);
    }
}

// Copiar campo até encontrar vírgula
void copia_ate_virgula(char *ix, char *vt)
{
    char i = 0, j = 0;
    j = *ix;
    while (gps_gprmc[j] != ',')
    {
        vt[i] = gps_gprmc[j];
        i++;
        j++;
    }
    vt[i] = '\0';
    j++;
    *ix = j;
}

//                 1         2         3         4         5         6
//       0123456789012345678901234567890123456789012345678901234567890123 4 5
//$GPRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A*57\r\n";    //Testar separação
//                 1
//       0123456789012345 6 7
//$GPRMC,,V,,,,,,,,,,N*53\r\n";        //Testar separação - sem satélites

// Extrair dados do campo GPRMC
void gps_gprmc_extrai(void)
{
    char i = 0;
    //ser1_str("\nSeparar GPRMC:\n");

    /////////////////// Laço para teste
    while (gps_gprmc_ok[i] != CR)
    {
        gps_gprmc[i] = gps_gprmc_ok[i];
        //gps_gprmc[i]=gps_gprmc_nok[i];
        i++;
    }
    i = 0;
    ///////////////////////

    if (gps_tranca == FALSE)
    { //Se o prog princip não está acessando

        // Zerar todas as variaveis do GPS
        gps_hora[0] = '\0';
        gps_sat = '0';
        gps_lat[0] = '\0';
        gps_ns = '0';
        gps_long[0] = '\0';
        gps_ew = '0';
        gps_data[0] = '\0';

        if (gps_gprmc[i] != ',')
            copia_ate_virgula(&i, gps_hora);   //Hora
        else
            i++;
        if (gps_gprmc[i] != ',')
            gps_sat = gps_gprmc[i++];          //Status satélites
        if (gps_sat == 'A')
        {                                     //Só segue adiante se for válido
            i++;
            if (gps_gprmc[i] != ',')
                copia_ate_virgula(&i, gps_lat);    //Latitude
            if (gps_gprmc[i] != ',')
                gps_ns = gps_gprmc[i++];              //N S
            i++;
            if (gps_gprmc[i] != ',')
                copia_ate_virgula(&i, gps_long);     //Longitude
            if (gps_gprmc[i] != ',')
                gps_ew = gps_gprmc[i++];              //E W
            i++;
            while (gps_gprmc[i++] != ',')
                ;                                //Pular Speed
            while (gps_gprmc[i++] != ',')
                ;                                //Pular Curso
            if (gps_gprmc[i] != ',')
                copia_ate_virgula(&i, gps_data);     //Data
        }
        gps_gprmc_novo = TRUE; //Depois seria interessante levar para dentro do "if"
    }
}

// Função para buscar campo GPRMC
// É chamada a cada byte que chega
void gps_busca_gprmc(char dado)
{
    static char i = 0;
    switch (gps_st)
    {
    case 0:
        if (dado == '$')
            gps_st = 1;
        else
            gps_st = 0;
        break;
    case 1:
        if (dado == 'G')
            gps_st = 2;
        else
            gps_st = 0;
        break;
    case 2:
        if (dado == 'P')
            gps_st = 3;
        else
            gps_st = 0;
        break;
    case 3:
        if (dado == 'R')
            gps_st = 4;
        else
            gps_st = 0;
        break;
    case 4:
        if (dado == 'M')
            gps_st = 5;
        else
            gps_st = 0;
        break;
    case 5:
        if (dado == 'C')
            gps_st = 6;
        else
            gps_st = 0;
        break;
    case 6:
        if (dado == ',')
        {
            gps_st = 7;
            i = 0;
        }
        else
            gps_st = 0;
        break;
    case 7:
        if (dado == CR)
        {
            gps_gprmc[i] = '\0';
            gps_st = 0;
            gps_gprmc_extrai();
        }
        else
            gps_gprmc[i++] = dado;
        break;
    }
}

// ISR TA0CCR0
#pragma vector = 53
__interrupt void isr_ta0ccr0(void)
{
    if (gps_estado == 18)
    {    //Terminou?
        TA0CCTL0 &= ~CCIE;  //Desab Interrup TA1
        P1IFG &= ~BIT2;     //Apagar pedidos anteriores
        P1IE |= BIT2;     //Hab Interup P1.2
        gps_poe(gps_dado);  //Por na fila
        gps_busca_gprmc(gps_dado);  //Buscar pelo GPRMC
    }
    else
    {
        if ((gps_estado & BIT0) == 0)
        {   //é par
            gps_dado = gps_dado >> 1;
            if ((P1IN & BIT2) == BIT2)
                gps_dado |= BIT7;
            //gps_dado++;
        }
    }
    gps_estado++;
}

// ISR P1.2
// Detecta o START da serial enviada pelo GPS
#pragma vector = 47
__interrupt void isr_p1(void)
{
    P1IV;       //Apagar o pedido
    TA0CTL |= TACLR;
    TA0CCTL0 &= ~CCIFG; //Apagar possível pedido anterior
    TA0CCTL0 |= CCIE;
    P1IFG &= ~BIT2;
    P1IE &= ~BIT2;    //Desab Interup P1.2
    gps_estado = 0;       //Iniciar recepção
}

// Inicializar fila SERI
// Prepara a porta de GPIO (P1.2)
// Prepara Timer TA0 para operar 9.600
// 20E6/19.200 = 1.041
void gps_config(void)
{
    gps_pin = 1;
    gps_pout = 0;
    P1DIR &= ~BIT2;
    P1REN |= BIT2;
    P1OUT |= BIT2;   //Pullup
    P1IES |= BIT2;   //Interup flanco descida
    P1IFG |= ~BIT2;   //Apagar possível Interup anterior
    P1IE |= BIT2;   //Hab Interup
    TA0CTL = TASSEL_2 | MC_1;
    TA0CCR0 = 1041;   // 20E6/19.200 = 1.041
}
void gps_estado_modo()
{
    gps_config();
    calculo_GPRMC();
    separa_algarismos_gps_f(gps_msg);
}
char calculo_GPRMC(void)
{
    char x;
    float m1 = 0, m2 = 0;
    int d1 = 0, d2 = 0;
    gps_config();
    while (TRUE)
    {

        if (gps_tira(&x) == TRUE)
            //ser1_char(x);
            if (gps_gprmc_novo == TRUE)
            {
                gps_tranca = TRUE;        //Evitar que dados GPS sejam alterados
                gps_gprmc_novo = FALSE;
                d1 = 0;
                d2 = 0;
                m1 = 0;
                m2 = 0;
                d1 = d1 + ((gps_lat[0] - 48) * 10);
                d1 = d1 + (gps_lat[1] - 48);
                d2 = d2 + ((gps_long[0] - 48) * 100);
                d2 = d2 + ((gps_long[1] - 48) * 10);
                d2 = d2 + (gps_long[2] - 48);
                m1 = m1 + ((gps_lat[2] - 48) * 10);
                m1 = m1 + (gps_lat[3] - 48);
                m1 = m1 + ((gps_lat[5] - 48) / 10.);
                m1 = m1 + ((gps_lat[6] - 48) / 100.);
                m1 = m1 + ((gps_lat[7] - 48) / 1000.);
                m1 = m1 + ((gps_lat[8] - 48) / 10000.);
                m1 = m1 + ((gps_lat[9] - 48) / 100000.);
                m2 = m2 + ((gps_long[3] - 48) * 10);
                m2 = m2 + (gps_long[4] - 48);
                m2 = m2 + ((gps_long[6] - 48) / 10.);
                m2 = m2 + ((gps_long[7] - 48) / 100.);
                m2 = m2 + ((gps_long[8] - 48) / 1000.);
                m2 = m2 + ((gps_long[9] - 48) / 10000.);
                m2 = m2 + ((gps_long[10] - 48) / 100000.);
                vol_lat = d1 + m1 / 60;
                vol_long = d2 + m2 / 60;
                if (gps_ns == 'S')
                    vol_lat = vol_lat * (-1);
                if (gps_ew2 == 'W')
                    vol_long = vol_long * (-1);

                //separa_algarismos_gps(latitude,longitude);
                // ser1_str("\nGPRMC:");
                // ser1_str(" Hora=");     ser1_str(gps_hora);
                //ser1_str(" SAT=");      ser1_char(gps_sat);
                //ser1_str(" Lat=");      ser1_float(latitude, 6);
                //ser1_str(" NS=");       ser1_char(gps_ns);
                //ser1_str(" Long=");     ser1_float(longitude, 6);
                //ser1_str(" EW=");       ser1_char(gps_ew2);
                //ser1_str(" Data=");     ser1_str(gps_data);
                //ser1_crlf(1);
                gps_tranca = FALSE;        //Liberar alteraï¿½ï¿½o dados GPS
                TA0CTL = 0;   //Parar o timer, desligar a serial do GPS
                return;
            }
    }
}
//separa latitude e longitude em algarismos e salva na memï¿½ria  (não salva)
void separa_algarismos_gps_f(char vt[22])
{
    int a, b;
    volatile int i = 0, j = 0, k;
    volatile int lt, lt2; // algarismos inteiros da latitude
    volatile int lg, lg2, lg3; //algarismos inteiros da longitude
    volatile int dec; //parte decimal para envio
    volatile float declt, declg; //partes decimais 6 casas

    if (vol_lat < 0)
    {
        vt[0] = '-';
        vol_lat = (-1) * vol_lat;
        i++;
    }

    if (vol_long < 0)
    {
        vol_long = (-1) * vol_long;
        j = 1;
    }
    a = vol_lat;
    b = vol_long;

    lt = vol_lat;
    lt2 = vol_lat / 10; //dezena
    lt = lt - (10 * lt2); //unidade
    lg = vol_long;
    lg2 = vol_long / 10;
    lg3 = vol_long / 100; //centena
    lg2 = lg2 - (10 * lg3); //dezena
    lg = lg - (10 * lg2); //unidade
    vt[i] = lt2 + 0x30;
    i++;
    vt[i] = lt + 0x30;
    i++;
    vt[i] = '.';
    i++;
    declt = (vol_lat - a) * 10;
    for (k = 0; k < 6; k++)
    {
        dec = declt; //1 digito parte decimal latitude
        vt[i] = dec + 0x30;
        i++;
        declt = (declt - dec) * 10;
    }
    //fim da latitude e inï¿½cio da longitude
    vt[i] = ',';
    i++;
    if (j = 1)
    {
        vt[i] = '-';
        i++;
        j = 0;
    }
    vt[i] = lg3 + 0x30;
    i++;
    vt[i] = lg2 + 0x30;
    i++;
    vt[i] = lg + 0x30;
    i++;

    vt[i] = '.';
    i++;
    declg = (vol_long - b) * 10;
    for (k = 0; k < 6; k++)
    {
        dec = declg; //1 digito parte decimal longitude
        vt[i] = dec + 0x30;
        i++;
        declg = (declg - dec) * 10;
    }
    if (i < 20)
    {
        vt[i] = ' ';
        i++;
        vt[i] = ' ';
        i++;
    }
    else if (i < 21)
    {
        vt[i] = ' ';
        i++;
    }
}
//coloca os maiores e menores valores que o gps pode obter para não entrar em modo de furto
void set_values_gps()
{

    /*
     * Ruido obtido
     * latitude = 132 == 200
     * longitude = 237 == 300
     *
     * Ter cuidado, pois as vezes dá mais ruido do que obtido acima.
     */
    //chama a funçao responsavel por obter os valores atualizados do gps, armazena em um inteiro
    int_latitude_longitude();

    menorlatitude = lat - 250, maiorlatitude = lat + 250;
    menorlongitude = longt - 350, maiorlongitude = longt + 350;
}
//armazena os valores da latitude e longitude em variaveis globais
void int_latitude_longitude()
{
    gps_estado_modo();

    //latitude
    lat = (gps_msg[1] - '0') * 10000000 + (gps_msg[2] - '0') * 1000000
            + (gps_msg[4] - '0') * 100000;
    lataux = ((gps_msg[5] - '0'));
    lataux = (lataux * 1000);
    lataux = (lataux * 10); // Não estava dando certo fazer (gps_msg[5]-'0') * 10000
    lat += lataux + (gps_msg[6] - '0') * 1000 + (gps_msg[7] - '0') * 100
            + (gps_msg[8] - '0') * 10 + (gps_msg[9] - '0');

    //longitude
    //longt = gps_msg[12] * 100000000; //Não precisa no brasil os valores variam de -73.9855° a -34.7937°
    longt = (gps_msg[13] - '0') * 10000000 + (gps_msg[14] - '0') * 1000000
            + (gps_msg[16] - '0') * 100000;
    longtaux = (gps_msg[17] - '0');
    longtaux = (longtaux * 1000);
    longtaux = (longtaux * 10); // Não estava dando certo fazer (gps_msg[5]-'0') * 10000
    longt += longtaux + (gps_msg[18] - '0') * 1000 + (gps_msg[19] - '0') * 100
            + (gps_msg[20] - '0') * 10 + (gps_msg[21] - '0');
}
// Segue o mesmo principio do MPU
// Se os novos valores forem maiores que os valores coletado no repouso + ruido. significa que houve furto
char gps_furto()
{
    int i = 0, alerta = 0;

    //chama a funçao responsavel por obter os valores atualizados do gps, armazena em um inteiro
    int_latitude_longitude();

    if (lat < menorlatitude || lat > maiorlatitude)
        alerta++;
    if (longt < menorlongitude || longt > maiorlongitude)
        alerta++;

    if (alerta == 0)
        return FALSE;

    //recebeu o primeiro alerta de furto, entao checa durante 5 segundos
    else if (alerta != 0)
    {
        //gprs_complete_str("Alerta");
        //ser1_str("Alerta"); ser1_dec32u(lat);ser1_str("  ----- "); ser1_dec32u(longt); ser1_str("\n\r");
        while (i < 5)
        {
            //chama a funçao responsavel por obter os valores atualizados do gps, armazena em um inteiro
            int_latitude_longitude();

            if (lat < menorlatitude || lat > maiorlatitude)
                alerta++;
            else if (longt < menorlongitude || longt > maiorlongitude)
                alerta++;
            else
                alerta = 0;

            if (alerta == 0)
            {             //voltou para o estado normal
                return FALSE;
            }
            else if (alerta == 4)
            {       //foi furtado
                return TRUE;
            }
            i++;
            delay_10ms(200); //1 seg
        }
    }
}


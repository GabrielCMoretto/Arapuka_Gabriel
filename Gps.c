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
char gps_xereta(char *cha){
  int pin,pout;
  __disable_interrupt();
  pout=gps_pout; //Copiar
  pin=gps_pin;   //ponteiros
  __enable_interrupt();
  pout++;
  if (pout == GPS_FILA_TAM)    pout=0;
  if (pout == pin){
    return FALSE;
  }
  else{
    *cha=gps_fila[pout];
    return TRUE;
  }
}

// Colocar um byte na fila SERI
// Chamada dentro Interrupções de RX USCI_A1
char gps_poe(char cha){
  if (gps_pin == gps_pout){
    gps_cheia();     //Msg de fila cheia
    return FALSE;     //SERI cheia
  }
  else{
    gps_fila[gps_pin++]=cha;
    if (gps_pin==GPS_FILA_TAM)
      gps_pin=0;
    return TRUE;
  }
}

// Mensagem de Fila SERI Cheia
void gps_cheia(void){
  __disable_interrupt();
  ser1_str("\nERRO: GPS Cheia!\n");
  gps_dump();
  ser1_str("\nTravado!");
  while(TRUE);
}

// Tirar um byte da fila SERI
// Função chamada fora das interrupções
char gps_tira(char *cha){
  int pout_aux;
  __disable_interrupt();
  pout_aux=gps_pout+1;
  if (pout_aux == GPS_FILA_TAM)    pout_aux=0;
  if (pout_aux == gps_pin){
    __enable_interrupt();
    return FALSE;
  }
  else{
    *cha=gps_fila[pout_aux];
    gps_pout=pout_aux;
    __enable_interrupt();
    return TRUE;
  }
}

// Imprimir fila de entrada SERI
// Para debug, chamada quando fila enche
void gps_dump(void){
    int i;
    ser1_str("\nGPS:  Tamanho=");   ser1_dec8u(GPS_FILA_TAM);
    ser1_str("  gps_pin=");         ser1_dec8u(gps_pin);
    ser1_str("  gps_pout=");        ser1_dec8u(gps_pout);
    ser1_crlf(1);
    for (i=0; i<GPS_FILA_TAM; i+=16){
        ser1_dec8u(i);
        ser1_char(':');
        ser1_char(' ');
        ser1_linha(&gps_fila[i]);
    }
}

// Copiar campo até encontrar vírgula
void copia_ate_virgula(char *ix, char *vt){
    char i=0,j=0;
    j=*ix;
    while(gps_gprmc[j]!=','){
        vt[i]=gps_gprmc[j];
        i++;
        j++;
    }
    vt[i]='\0';
    j++;
    *ix=j;
}

//                 1         2         3         4         5         6
//       0123456789012345678901234567890123456789012345678901234567890123 4 5
//$GPRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A*57\r\n";    //Testar separação
//                 1
//       0123456789012345 6 7
//$GPRMC,,V,,,,,,,,,,N*53\r\n";        //Testar separação - sem satélites

// Extrair dados do campo GPRMC
void gps_gprmc_extrai(void){
    char i=0;
    //ser1_str("\nSeparar GPRMC:\n");


    /////////////////// Laço para teste
    while (gps_gprmc_ok[i] != CR){
        gps_gprmc[i]=gps_gprmc_ok[i];
        //gps_gprmc[i]=gps_gprmc_nok[i];
        i++;
    }
    i=0;
    ///////////////////////

    if (gps_tranca==FALSE){ //Se o prog princip não está acessando

        // Zerar todas as variaveis do GPS
        gps_hora[0]='\0';
        gps_sat='0';
        gps_lat[0]='\0';
        gps_ns='0';
        gps_long[0]='\0';
        gps_ew='0';
        gps_data[0]='\0';

        if (gps_gprmc[i]!=',')  copia_ate_virgula(&i,gps_hora);   //Hora
        else                    i++;
        if (gps_gprmc[i]!=',')  gps_sat=gps_gprmc[i++];          //Status satélites
        if (gps_sat == 'A'){                                     //Só segue adiante se for válido
            i++;
            if (gps_gprmc[i]!=',')    copia_ate_virgula(&i,gps_lat);    //Latitude
            if (gps_gprmc[i]!=',')  gps_ns=gps_gprmc[i++];              //N S
            i++;
            if (gps_gprmc[i]!=',')  copia_ate_virgula(&i,gps_long);     //Longitude
            if (gps_gprmc[i]!=',')  gps_ew=gps_gprmc[i++];              //E W
            i++;
            while (gps_gprmc[i++]!=',');                                //Pular Speed
            while (gps_gprmc[i++]!=',');                                //Pular Curso
            if (gps_gprmc[i]!=',')  copia_ate_virgula(&i,gps_data);     //Data
        }
        gps_gprmc_novo=TRUE;    //Depois seria interessante levar para dentro do "if"
    }
}

// Função para buscar campo GPRMC
// É chamada a cada byte que chega
void gps_busca_gprmc(char dado){
    static char i=0;
    switch(gps_st){
    case 0: if (dado=='$')  gps_st=1;
            else            gps_st=0;
            break;
    case 1: if (dado=='G')  gps_st=2;
            else            gps_st=0;
            break;
    case 2: if (dado=='P')  gps_st=3;
            else            gps_st=0;
            break;
    case 3: if (dado=='R')  gps_st=4;
            else            gps_st=0;
            break;
    case 4: if (dado=='M')  gps_st=5;
            else            gps_st=0;
            break;
    case 5: if (dado=='C')  gps_st=6;
            else            gps_st=0;
            break;
    case 6: if (dado==',')  {gps_st=7;  i=0;}
            else            gps_st=0;
            break;
    case 7: if (dado==CR)   {gps_gprmc[i]='\0';
                             gps_st=0;
                             gps_gprmc_extrai();
                            }
            else            gps_gprmc[i++]=dado;
            break;
    }
}

// ISR TA0CCR0
#pragma vector = 53
__interrupt void isr_ta0ccr0(void){
    if (gps_estado==18){    //Terminou?
        TA0CCTL0 &= ~CCIE;  //Desab Interrup TA1
        P1IFG &= ~BIT2;     //Apagar pedidos anteriores
        P1IE  |=  BIT2;     //Hab Interup P1.2
        gps_poe(gps_dado);  //Por na fila
        gps_busca_gprmc(gps_dado);  //Buscar pelo GPRMC
    }
    else{
        if ((gps_estado&BIT0)==0){   //é par
            gps_dado = gps_dado>>1;
            if ((P1IN&BIT2) == BIT2) gps_dado |= BIT7;
            //gps_dado++;
        }
    }
    gps_estado++;
}

// ISR P1.2
// Detecta o START da serial enviada pelo GPS
#pragma vector = 47
__interrupt void isr_p1(void){
    P1IV;       //Apagar o pedido
    TA0CTL |= TACLR;
    TA0CCTL0 &= ~CCIFG; //Apagar possível pedido anterior
    TA0CCTL0 |= CCIE;
    P1IFG &= ~BIT2;
    P1IE  &=  ~BIT2;    //Desab Interup P1.2
    gps_estado=0;       //Iniciar recepção
}

// Inicializar fila SERI
// Prepara a porta de GPIO (P1.2)
// Prepara Timer TA0 para operar 9.600
// 20E6/19.200 = 1.041
void gps_config(void){
  gps_pin=1;
  gps_pout=0;
  P1DIR &= ~BIT2;
  P1REN |=  BIT2;
  P1OUT |=  BIT2;   //Pullup
  P1IES |=  BIT2;   //Interup flanco descida
  P1IFG |= ~BIT2;   //Apagar possível Interup anterior
  P1IE  |=  BIT2;   //Hab Interup
  TA0CTL = TASSEL_2 | MC_1;
  TA0CCR0 = 1041;   // 20E6/19.200 = 1.041
}



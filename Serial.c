// Puka - Serial.c

// USCI_A1 <==> eZ-FET <==> PC

#include <msp430.h>
#include "Defines.h"
#include "Serial.h"
#include "Strings.h"

// Imprime Data e Hora
// Recebe no formato dos registradores do RTC
void ser1_data_hora(char *vet){
  char msg[18];
  str_data_hora(vet,msg);
  ser1_str(msg);
}


// Imprimir uma linha com 16 Hex e 16 char
// Serve para ver posições da memória
void ser1_linha(char *vt){
    int i;
    for (i=0; i<16; i++){
        ser1_hex8(vt[i]);
        ser1_char(' ');
    }
    ser1_char(' ');
    ser1_char(' ');

    for (i=0; i<16; i++){
        if (vt[i]>0x1F && vt[i]<0x80)   ser1_char(vt[i]);
        else                            ser1_char('.');
    }
    ser1_crlf(1);
}



////////////////////////////////////////////////////
///////////// USCI_A1 ==> PC ///////////////////////
/////////// Polling do UCA1IFG /////////////////////
////////////////////////////////////////////////////

// Escrever leitura do cronômetro
void ser1_crono(unsigned long x){
  char msg[15];
  str_crono(x, msg);
  ser1_str(msg);
}


////////////////////////////////////////////////////
////////////// float / double //////////////////////
////////////////////////////////////////////////////

// Escrever double, com sinal e sem zeros à esq
// + xxx xxx xxx xxx , ddd ddd ddd ddd ddd (usar char msg[30])
// Não imprime zeros à esquerda, exceto para 0.xxxx
// 12 posições = limite da parte inteira
// 15 posições = limite da parte fracionária
// Caso ultrapasse os limites imprime ### , ###
void ser1_double(double f, char prec){
  char msg[30];
  str_double(f, prec, msg);
  ser1_str(msg);
}

// Escrever float, com sinal e sem zeros à esq
// + xxx xxx xxx , ddd ddd ddd ddd (usar char msg[24])
// Não imprime zeros à esquerda, exceto para 0.xxxx
//  9 posições = limite da parte inteira
// 12 posições = limite da parte fracionária
// Caso ultrapasse os limites imprime ### , ###
void ser1_float(float f, char prec){
  char msg[24];
  str_float(f,prec, msg);
  ser1_str(msg);
}

////////////////////////////////////////////////////
//////////////// 32 bits ///////////////////////////
////////////////////////////////////////////////////

// Escrever 32 bits em Decimal, com sinal e com zeros à esq
void ser1_dec32(long dt){
  char msg[12];
  str_dec32(dt, msg);
  ser1_str(msg);
}

// Escrever 32 bits em Decimal, sem sinal e com zeros à esq
void ser1_dec32u(long dt){
  char msg[12];
  str_dec32u(dt, msg);
  ser1_str(msg);
}

// Escrever 32 bits em Decimal, com sinal e sem zeros à esq
void ser1_dec32nz(long dt){
  char msg[12];
  str_dec32(dt, msg);
  str_rmvz_s(msg);
  ser1_str(msg);
}

// Escrever 32 bits em Decimal, sem sinal e sem zeros à esq
void ser1_dec32unz(long dt){
  char msg[12];
  str_dec32u(dt, msg);
  str_rmvz_u(msg);
  ser1_str(msg);
}

// Escrever word (32 bits) em Hexa na posição atual
void ser1_hex32(long dt){
  char msg[9];
  str_hex32(dt, msg);
  ser1_str(msg);
}

////////////////////////////////////////////////////
//////////////// 16 bits ///////////////////////////
////////////////////////////////////////////////////

// Escrever 16 bits em Decimal, com sinal e com zeros à esq
void ser1_dec16(int dt){
  char msg[7];
  str_dec16(dt, msg);
  ser1_str(msg);
}

// Escrever 16 bits em Decimal, sem sinal e com zeros à esq
void ser1_dec16u(unsigned int dt){
  char msg[7];
  str_dec16u(dt, msg);
  ser1_str(msg);
}

// Escrever 16 bits em Decimal, com sinal e sem zeros à esq
void ser1_dec16nz(int dt){
  char msg[7];
  str_dec16(dt, msg);
  str_rmvz_s(msg);
  ser1_str(msg);
}

// Escrever 16 bits em Decimal, sem sinal e sem zeros à esq
void ser1_dec16unz(unsigned int dt){
  char msg[7];
  str_dec16u(dt, msg);
  str_rmvz_u(msg);
  ser1_str(msg);
}

// Escrever word (16 bits) em Hexa na posição atual
void ser1_hex16(int dt){
  char msg[5];
  str_hex16(dt, msg);
  ser1_str(msg);
}

////////////////////////////////////////////////////
///////////////// 8 bits ///////////////////////////
////////////////////////////////////////////////////

// Escrever 8 bits em BCD, com zeros à esq
void ser1_bcd8(char dt){
  char msg[3];
  str_bcd8(dt, msg);
  ser1_str(msg);
}

// Escrever 8 bits em Decimal, com sinal e com zeros à esq
void ser1_dec8(char dt){
  char msg[5];
  str_dec8(dt, msg);
  ser1_str(msg);
}

// Escrever 8 bits em Decimal, sem sinal e com zeros à esq
void ser1_dec8u(char dt){
  char msg[5];
  str_dec8u(dt, msg);
  ser1_str(msg);
}

// Escrever 8 bits em Decimal, com sinal e sem zeros à esq
void ser1_dec8nz(char dt){
  char msg[5];
  str_dec8(dt, msg);
  str_rmvz_s(msg);
  ser1_str(msg);
}

// Escrever 8 bits em Decimal, sem sinal e sem zeros à esq
void ser1_dec8unz(char dt){
  char msg[5];
  str_dec8u(dt, msg);
  str_rmvz_u(msg);
  ser1_str(msg);
}

// Escrever byte em Hexa
void ser1_hex8(char dt){
  char msg[3];
  str_hex8(dt, msg);
  ser1_str(msg);
}

// Imprimir espações
void ser1_spc(char x){
    char i=0;
    while (i < x){
        ser1_char(' ');
        i++;
    }
}

// Imprimir Retorno do Carro (0xD) e Nova Linha (0xA)
void ser1_crlf(char x){
    char i=0;
    while (i < x){
        ser1_char(CR);
        ser1_char(LF);
        i++;
    }
}

// Imprimir uma string na USCI_A1
// Polling TX_IFG=1
void ser1_str(char *msg){
    int i=0;
    while ( msg[i] != '\0'){
        ser1_char(msg[i++]);
    }
}

// Imprimir um caracter na USCI_A1
// Polling TX_IFG=1
void ser1_char(char x){
    UCA1TXBUF=x;
    while( (UCA1IFG&UCTXIFG) == 0);
}


////////////////////////////////////////////////////
/// Ler comandos que chegam pela Fila SERI /////////
////////// USCI_A1 <== PC //////////////////////////
////////////////////////////////////////////////////

// Função para verificar se foi teclado x ou X
// Se teclar X ou x, retorna TRUE.
// Se teclar algo diferente, retorna FALSE.
char seri_cmdo_x(void){
  char qtd,x,argc[4],argv[4];
  qtd=seri_cmdo(argc, argv, 3);
  str_maiusc(argv);
  if (qtd==1 && argv[0]=='X' && argv[1]=='\0')  return TRUE;
  return FALSE;
}


// Receber uma linha de comando da fila de entrada (seri)
// Linha de comando precisa ser terminada com CR ou LF
// argc[] = posição de início dos comandos
// argv[] = vetor com todos os comandos
// limite = quantidade máxima de caracteres a serem recebidos
// qtd = quantidade de comandos recebidos
// Argumentos separados por "branco"
char seri_cmdo(char *argc, char *argv, char limite){
  char x,qtd,i;
  char estado;      //modo=1, recebendo string
  i=0;
  estado=0;
  qtd=0;
  argc[0]=0;
  argv[0]='\0';
  seri_come_crlf();
  if (seri_vazia() == TRUE)  return qtd; //Fila vazia?
  while(TRUE){
    while (seri_tira(&x)==FALSE);
    if (estado == 1){   //Receber um argumento
      if (x=='\n' || x=='\r')        break;
      if (x==' '){
        argv[i++]='\0';
        estado=0;
      }
      else  argv[i++]=x;
    }
    else{             //Retirar espaços
      if (x=='\n' || x=='\r') break;
      if (x != ' '){
        argc[qtd]=i;
        argv[i++]=x;
        qtd++;
        estado=1;
      }
    }
    if (i==limite){
      i--;
      break;
    }
  }
  argv[i++]='\0';
  seri_come_crlf();
  return qtd;
}

void rec_msg(char vet[], int limite){
        char cont=0;
        char x=0;

        while(x!='\n' && x!='\r' && cont < limite){
            while (seri_tira(&x)==FALSE);
            vet[cont++]=x;
        }
}


//Consumir CR e LF
void seri_come_crlf(void){
    char x;
    while (TRUE){
        if (seri_xereta(&x) == FALSE) break;
        if ( x==CR || x==LF) seri_tira(&x);
        else                  break;
    }
}

// Consumir tudo o que não for número decimal
// Pode travar esperando um número
void seri_come_ndec(void){
    char x;
    while (TRUE){
        while (seri_xereta(&x) == FALSE) ;
        if ( x<'0' || x>'9') seri_tira(&x);
        else                  break;
    }
}


// Retirar da fila SERI um número decimal de até 32 bits sem sinal
long seri_dec32u(void){
    long x=0;;
    char z;
    seri_come_ndec();
    while(TRUE){
        while(seri_tira(&z) ==  FALSE);
        if (z>='0' && z<='9')   x= (10*x) + (z-'0');
        else                    return x;
    }
}


////////////////////////////////////////////////////
////////////////// Fila SERI ///////////////////////
///////////// USCI_A1 <== PC ///////////////////////
/////// Interrupção chama seri_poe() ///////////////
/////// Ler a fila como seri_tira()  ///////////////
////////////////////////////////////////////////////


// Verificar se a fila SERI está vazia
// TRUE = vazia e FALSE = não vazia
char seri_vazia(void){
    char pin, pout;
    __disable_interrupt();
    pin=seri_pin;   //Cópia dos ponteiros
    pout=seri_pout; //sem interrupções
    __enable_interrupt();
    pout=pout+1;
    if (pout == SERI_FILA_TAM)    pout=0;
    if (pout == pin)  return  TRUE;   //Vazia
    else              return  FALSE;  //Tem alguma coisa na fila
}

// Informa qual o próximo byte da fila SERI
// Não altera estado dos ponteiros
// Chamada fora das interrupções
// FALSE = fila vazia
char seri_xereta(char *cha){
  int pin,pout;
  __disable_interrupt();
  pout=seri_pout; //Copiar
  pin=seri_pin;   //ponteiros
  __enable_interrupt();
  pout++;
  if (pout == SERI_FILA_TAM)    pout=0;
  if (pout == pin){
    return FALSE;
  }
  else{
    *cha=seri_fila[pout];
    return TRUE;
  }
}

// Inicializar fila SERI (USCI_A0 e USCI_A1)
void seri_config(void){
  seri_pin=1;
  seri_pout=0;
}

// Colocar um byte na fila SERI
// Chamada dentro Interrupções de RX USCI_A1
char seri_poe(char cha){
  if (seri_pin == seri_pout){
    seri_cheia();     //Msg de fila cheia
    return FALSE;     //SERI cheia
  }
  else{
    seri_fila[seri_pin++]=cha;
    if (seri_pin==SERI_FILA_TAM)
      seri_pin=0;
    return TRUE;
  }
}

// Mensagem de Fila SERI Cheia
void seri_cheia(void){
  __disable_interrupt();
  ser1_str("\nERRO: SERI Cheia!\n");
  seri_dump();
  ser1_str("\nTravado!");
  while(TRUE);
}

// Tirar um byte da fila SERI
// Função chamada fora das interrupções
char seri_tira(char *cha){
  int pout_aux;
  __disable_interrupt();
  pout_aux=seri_pout+1;
  if (pout_aux == SERI_FILA_TAM)    pout_aux=0;
  if (pout_aux == seri_pin){
    __enable_interrupt();
    return FALSE;
  }
  else{
    *cha=seri_fila[pout_aux];
    seri_pout=pout_aux;
    __enable_interrupt();
    return TRUE;
  }
}

// Imprimir fila de entrada SERI
// Para debug, chamada quando fila enche
void seri_dump(void){
    int i;
    ser1_str("\nSERI:  Tamanho=");   ser1_dec8u(SERI_FILA_TAM);
    ser1_str("  seri_pin=");         ser1_dec8u(seri_pin);
    ser1_str("  seri_pout=");        ser1_dec8u(seri_pout);
    ser1_crlf(1);
    for (i=0; i<SERI_FILA_TAM; i+=16){
        ser1_dec8u(i);
        ser1_char(':');
        ser1_char(' ');
        ser1_linha(&seri_fila[i]);
    }
}


// USCI_A1: Configurar Serial 1 (MSP <=> PC), Hab int RX
// Usa SMCLK = 20 MHz
// P4.4 = TX (MSP ==> PC) e P4.5 = RX (MSP <== PC)
void ser1_config(char br){
    UCA1CTL1 =  UCSSEL_2 | UCSWRST;     //UCSI em Reset
    UCA1CTL0 = UCSPB;                   //1 Stop

    switch(br){
        case BR_4800:
        case BR_9600:    UCA1BRW = 2083;  UCA1MCTL=UCBRS_2|UCBRF_0;  break;
        case BR_19200:   UCA1BRW = 1041;  UCA1MCTL=UCBRS_6|UCBRF_0;  break;
        case BR_38400:   UCA1BRW = 520;   UCA1MCTL=UCBRS_7|UCBRF_0;  break;
        case BR_57600:   UCA1BRW = 347;   UCA1MCTL=UCBRS_2|UCBRF_0;  break;
        case BR_74880:
        case BR_115200:  UCA1BRW = 173;   UCA1MCTL=UCBRS_5|UCBRF_0;  break;
        case BR_230400:  UCA1BRW = 86;    UCA1MCTL=UCBRS_7|UCBRF_0;  break;
        case BR_250k:    UCA1BRW = 80;    UCA1MCTL=0;                break;
        case BR_500k:    UCA1BRW = 40;    UCA1MCTL=0;                break;
        case BR_1000k:   UCA1BRW = 20;    UCA1MCTL=0;                break;
        case BR_2000k:   UCA1BRW = 10;    UCA1MCTL=0;                break;
        default:         UCA1BRW = 2083;  UCA1MCTL=UCBRS_2|UCBRF_0;  break; //default = 9600 bps

    }
    P4DIR |=  BIT4;         //P4.4 saída
    P4DIR &= ~BIT5;         //P4.5 entrada
    P4SEL |= BIT5 | BIT4;   //Tx serial

    PMAPKEYID = 0X02D52;    //Liberar mapeamentp
    P4MAP4 = PM_UCA1TXD;    //TX=Sair por P4.4
    P4MAP5 = PM_UCA1RXD;    //RX=Receber por P4.5

    UCA1CTL1 &= ~ UCSWRST;  //UCSI sai de Reset
    //UCA1IE = UCTXIE|UCRXIE; //Hab interrupção  TX e RX
    UCA1IE = UCRXIE;            //Hab interrupção  RX
}

////////////////////////////////////////////////////
////////// Interrupções portas UART ////////////////
////////////////////////////////////////////////////

// USCI_A1 - ISR (USCI_A1 <==> PC)
#pragma vector = 46
/*__interrupt void ISR_USCI_A1(void){
    int n;
    n = __even_in_range(UCA1IV,0x4);
    switch(n){
        case 0x0: break;
        case 0x2: usci_a1_rx();  break; //RX
        case 0x4: usci_a1_tx();  break; //TX
    }
}*/

__interrupt void usci_a1_int(void){
    UCA1IV;                 //Apagar RXIFG
    seri_poe(UCA1RXBUF);   //Por dado na fila
}

// USCI_A1 <== PC recepção
void usci_a1_rx(void){
    seri_poe(UCA1RXBUF);
}

// USCI_A1 ==> PC recepção
void usci_a1_tx(void){
    ;
}




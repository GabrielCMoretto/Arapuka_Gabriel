// Puka - Lcd.c
// I2C (USCI_B1) P4.2 = SCL
// I2C (USCI_B1) P4.1 = SDA

#include <msp430.h>
#include "Defines.h"
#include "Timer.h"
#include "Serial.h"
#include "Strings.h"
#include "Lcd.h"

// Imprime a data, usa o vetor lido do RTC a partir do endereço
void lcdb_data(char lin, char col, char *vet){
    char msg[18];
    str_data_hora(vet,msg);
    msg[8]='\0';            //Limitar apenas à data dd/mm/aa0
    lcdb_str(lin,col,msg);
}

// Imprime a hora, usa o vetor lido do RTC a partir do endereço
void lcdb_hora(char lin, char col, char *vet){
    char msg[18];
    str_data_hora(vet,msg);
    lcdb_str(lin,col,&msg[9]);
}


// Escrever leitura do cronômetro
void lcdb_crono(char lin, char col, unsigned long x){
  char msg[15];
  str_crono(x, msg);
  lcdb_str(lin,col,msg);
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
void lcdb_double(char lin, char col, double f, char prec){
  char msg[30];
  str_double(f, prec, msg);
  lcdb_str(lin,col,msg);
}

// Escrever float, com sinal e sem zeros à esq
// + xxx xxx xxx , ddd ddd ddd ddd (usar char msg[24])
// Não imprime zeros à esquerda, exceto para 0.xxxx
//  9 posições = limite da parte inteira
// 12 posições = limite da parte fracionária
// Caso ultrapasse os limites imprime ### , ###
void lcdb_float(char lin, char col, float f, char prec){
  char msg[24];
  str_float(f,prec, msg);
  lcdb_str(lin,col,msg);
}

////////////////////////////////////////////////////
//////////////// 32 bits ///////////////////////////
////////////////////////////////////////////////////

// Escrever 32 bits em Decimal, com sinal e com zeros à esq
void lcdb_dec32(char lin, char col, long dt){
  char msg[12];
  str_dec32(dt, msg);
  lcdb_str(lin,col,msg);
}

// Escrever 32 bits em Decimal, sem sinal e com zeros à esq
void lcdb_dec32u(char lin, char col, long dt){
  char msg[12];
  str_dec32u(dt, msg);
  lcdb_str(lin,col,msg);
}

// Escrever 32 bits em Decimal, com sinal e sem zeros à esq
void lcdb_dec32nz(char lin, char col, long dt){
  char msg[12];
  str_dec32(dt, msg);
  str_rmvz_s(msg);
  lcdb_str(lin,col,msg);
}

// Escrever 32 bits em Decimal, sem sinal e sem zeros à esq
void lcdb_dec32unz(char lin, char col, long dt){
  char msg[12];
  str_dec32u(dt, msg);
  str_rmvz_u(msg);
  lcdb_str(lin,col,msg);
}

// Escrever word (32 bits) em Hexa na posição atual
void lcdb_hex32(char lin, char col, long dt){
  char msg[9];
  str_hex32(dt, msg);
  lcdb_str(lin,col,msg);
}


////////////////////////////////////////////////////
//////////////// 16 bits ///////////////////////////
////////////////////////////////////////////////////

// Escrever 16 bits em Decimal, com sinal e com zeros à esq
void lcdb_dec16(char lin, char col, int dt){
  char msg[7];
  str_dec16(dt, msg);
  lcdb_str(lin,col,msg);
}

// Escrever 16 bits em Decimal, sem sinal e com zeros à esq
void lcdb_dec16u(char lin, char col, unsigned int dt){
  char msg[7];
  str_dec16u(dt, msg);
  lcdb_str(lin,col,msg);
}

// Escrever 16 bits em Decimal, com sinal e sem zeros à esq
void lcdb_dec16nz(char lin, char col, int dt){
  char msg[7];
  str_dec16(dt, msg);
  str_rmvz_s(msg);
  lcdb_str(lin,col,msg);
}

// Escrever 16 bits em Decimal, sem sinal e sem zeros à esq
void lcdb_dec16unz(char lin, char col, unsigned int dt){
  char msg[7];
  str_dec16u(dt, msg);
  str_rmvz_u(msg);
  lcdb_str(lin,col,msg);
}

// Escrever word (16 bits) em Hexa na posição atual
void lcdb_hex16(char lin, char col, int dt){
  char msg[5];
  str_hex16(dt, msg);
  lcdb_str(lin,col,msg);
}


////////////////////////////////////////////////////
///////////////// 8 bits ///////////////////////////
////////////////////////////////////////////////////

// Escrever 8 bits em BCD, com zeros à esq
void lcdb_bcd8(char lin, char col, char dt){
  char msg[3];
  str_bcd8(dt, msg);
  lcdb_str(lin,col,msg);
}

// Escrever 8 bits em Decimal, com sinal e com zeros à esq
void lcdb_dec8(char lin, char col, char dt){
  char msg[5];
  str_dec8(dt, msg);
  lcdb_str(lin,col,msg);
}

// Escrever 8 bits em Decimal, sem sinal e com zeros à esq
void lcdb_dec8u(char lin, char col, char dt){
  char msg[5];
  str_dec8u(dt, msg);
  lcdb_str(lin,col,msg);
}

// Escrever 8 bits em Decimal, com sinal e sem zeros à esq
void lcdb_dec8nz(char lin, char col, char dt){
  char msg[5];
  str_dec8(dt, msg);
  str_rmvz_s(msg);
  lcdb_str(lin,col,msg);
}

// Escrever 8 bits em Decimal, sem sinal e sem zeros à esq
void lcdb_dec8unz(char lin, char col, char dt){
  char msg[5];
  str_dec8u(dt, msg);
  str_rmvz_u(msg);
  lcdb_str(lin,col,msg);
}

// Escrever byte em Hexa
void lcdb_hex8(char lin, char col, char dt){
  char msg[3];
  str_hex8(dt, msg);
  lcdb_str(lin,col,msg);
}

////////////////////////////////////////////////////
//////////////// LCDB - Básico /////////////////////
////////////////////////////////////////////////////

// Escrever uma string terminada em '\0'
// lin = 0 ou 1 e col = 0, 1, ..., 15
void lcdb_str(unsigned char lin, unsigned char col, char *msg){
    char i=0,pos,x;
    //pos= 4*col+68*lin+4;
    if (lin==0) lin=1;  //Segurança, garantir escrita dentro do buffer
    if (col==0) col=1;  //Segurança, garantir escrita dentro do buffer
    if (lin<3 && col<17){
        pos= 4*(col-1)+68*(lin-1)+4;
        while(msg[i] != '\0' && col<170){
            x=msg[i]&0xF0;
            lcd_buf[pos+0]=x|0xD;
            lcd_buf[pos+1]=x|0x9;
            x=(msg[i]&0xF)<<4;
            lcd_buf[pos+2]=x|0xD;
            lcd_buf[pos+3]=x|0x9;
            i++;
            pos+=4;
            col++;
        }
    }
    lcd_flag=TRUE;
}

// Escrever caractere no Buffer do LCD
// lin = 1 ou 2 e col = 1, 2, ..., 16
void lcdb_char(char lin, char col, char letra){
    char pos,x;
    if (lin==0) lin=1;  //Segurança, garantir escrita dentro do buffer
    if (col==0) col=1;  //Segurança, garantir escrita dentro do buffer
    if (col<17){
        //pos= 4*col+68*lin+4;
        pos= 4*(col-1)+68*(lin-1)+4;
        x=letra&0xF0;
        lcd_buf[pos+0]=x|0xD;
        lcd_buf[pos+1]=x|0x9;
        x=(letra&0xF)<<4;
        lcd_buf[pos+2]=x|0xD;
        lcd_buf[pos+3]=x|0x9;
        lcd_flag=TRUE;
    }
}


// Apagar todo o LCD, escreve brancos no Buffer do LCD
void lcdb_apaga(void){
    char i;
    for (i=4; i<68; i+=4){
        lcd_buf[i+ 0]=0x2D;  //Tudo com espaços (linha=0)
        lcd_buf[i+ 1]=0x29;
        lcd_buf[i+ 2]=0x0D;
        lcd_buf[i+ 3]=0x09;
        lcd_buf[i+68]=0x2D;  //Tudo com espaços (linha=1)
        lcd_buf[i+69]=0x29;
        lcd_buf[i+70]=0x0D;
        lcd_buf[i+71]=0x09;
    }
    lcd_flag=TRUE;
}



/////////////////////////////////////////////////////
///////////// LCD - DMA /////////////////////////////
/////////////////////////////////////////////////////

// Dispara atualização do LCD via DMA2
// Função chamada pela interrupção de TA2.0
// Se a interrupção de TA2.0 encontra lcd_flag=TRUE, chama esta função
void lcd_atualiza(void){
    if (lcd_tem==TRUE){              //Só se LCD estiver presente
        i2c_tranca=TRUE;            //Sinaliza que DMA vai usar I2C (Interrupt do DMA libera)
        UCB1IFG =0;                 //TXIFG=0
        DMA2CTL |= DMAEN;           //Enable=1
        UCB1I2CSA = lcd_adr;        //Endereço Escravo
        UCB1CTL1 |= UCTR | UCTXSTT; //Gerar START
    }
}

// Configurar LCD para operar com DMA 2
void lcd_dma_config(void){
    char i;
    for (i=0; i<LCD_BUF_TAM-4; i+=4){
        lcd_buf[i+0]=0x2D;  //Tudo com espaços
        lcd_buf[i+1]=0x29;
        lcd_buf[i+2]=0x0D;
        lcd_buf[i+3]=0x09;
    }
    lcd_buf[136]=0x0D;    //Forçar E=1

    lcd_buf[0]=0x8C;      //Lin=0, Col=0
    lcd_buf[1]=0x88;
    lcd_buf[2]=0x0C;
    lcd_buf[3]=0x08;
    lcd_buf[0+68]=0xCC;   //Lin=1, Col=0
    lcd_buf[1+68]=0xC8;
    lcd_buf[2+68]=0x0C;
    lcd_buf[3+68]=0x08;

    // Configurar DMA2
    DMACTL1 = DMA0TSEL_23;      //DMA2 Trigger = UCB1TXIFG
    DMA2CTL = DMADT_0      |    //DMAEN=0, Simples sem repetição
              DMAIE        |    //Hab Interrupção
              DMASRCBYTE   |    //Fonte em bytes
              DMADSTBYTE   |    //Destino em bytes
              DMASRCINCR_3 |    //Adr fonte incrementa
              DMADSTINCR_0;     //Adr destino fixo
    DMA2SA = &lcd_buf[0];       //Fonte = Buffer LCD
    DMA2DA = USCIB1_TXBUF_ADDR; //Endereço destino
    DMA2SZ = LCD_BUF_TAM;       //Qtd. de transferências
    lcd_flag=TRUE;
}


/////////////////////////////////////////////////////
///////////// LCD - Básico //////////////////////////
/////////////////////////////////////////////////////

// Escrever um caracter
void lcd_str(char *msg){
    char i=0;
    while (msg[i] != '\0')
        lcd_char(msg[i++]);
}

// Escrever um caracter RS=1
void lcd_char(char letra){
    char nib;
    nib=letra & 0xF0;
    lcd_write(nib | 0xD);
    lcd_write(nib | 0x9);
    nib=(letra & 0xF)<<4;
    lcd_write(nib | 0xD);
    lcd_write(nib | 0x9);
}

// Posicionar o cursor
void lcd_cursor(char pos){
    lcd_cmdo(0x80|pos);
}

// Escrever um comando RS=0
void lcd_cmdo(char cmdo){
    char nib;
    nib=cmdo & 0xF0;
    lcd_write(nib | 0xC);
    lcd_write(nib | 0x8);
    nib=(cmdo & 0xF)<<4;
    lcd_write(nib | 0xC);
    lcd_write(nib | 0x8);
}


// Escrever dado na porta
void lcd_write(char dado){
    if (lcd_tem==TRUE){             //Só se tem o LCD
        UCB1I2CSA = lcd_adr;        //Endereço Escravo
        UCB1CTL1 |= UCTR    |       //Mestre TX
                    UCTXSTT;        //Gerar START
        while ( (UCB1IFG & UCTXIFG) == 0)   ;          //Esperar TXIFG=1
        UCB1TXBUF = dado;                              //Escrever dado
        while ( (UCB1CTL1 & UCTXSTT) == UCTXSTT)   ;   //Esperar STT=0
        if ( (UCB1IFG & UCNACKIFG) == UCNACKIFG)       //NACK?
                    while(1);                          //Escravo gerou NACK
        UCB1CTL1 |= UCTXSTP;                        //Gerar STOP
        while ( (UCB1CTL1 & UCTXSTP) == UCTXSTP)   ;   //Esperar STOP
    }
}

// Incializar LCD modo 4 bits
void lcd_inic(void){

    // Descobrir endereço do PCF, default = 0
    //lcd_adr=0;
    //if (i2c_teste_adr(0x27)==TRUE)  lcd_adr=0x27;
    //if (i2c_teste_adr(0x3F)==TRUE)  lcd_adr=0x3F;

    // Preparar I2C para operar
    UCB1I2CSA = lcd_adr;    //Endereço Escravo
    UCB1CTL1 |= UCTR    |   //Mestre TX
                UCTXSTT;    //Gerar START
    while ( (UCB1IFG & UCTXIFG) == 0);          //Esperar TXIFG=1
    UCB1TXBUF = 0;                              //Saída PCF = 0;
    while ( (UCB1CTL1 & UCTXSTT) == UCTXSTT);   //Esperar STT=0
    if ( (UCB1IFG & UCNACKIFG) == UCNACKIFG)    //NACK?
                while(TRUE);

    // Começar inicialização do LCD
    lcd_aux(0);     delay_10ms(4);
    lcd_aux(3);     delay_10ms(1);
    lcd_aux(3);     delay_10ms(1);
    lcd_aux(3);     delay_10ms(1);
    lcd_aux(2);

    // Entrou em modo 4 bits
    lcd_aux(2);     lcd_aux(8);     //0x28
    lcd_aux(0);     lcd_aux(8);     //0x08
    lcd_aux(0);     lcd_aux(1);     //0x01
    lcd_aux(0);     lcd_aux(6);     //0x06
    //lcd_aux(0);     lcd_aux(0xF);   //0x0F - Cursor ativado
    lcd_aux(0);     lcd_aux(0xC);   //0x0C - Cursor desativado

    while ( (UCB1IFG & UCTXIFG) == 0)   ;          //Esperar TXIFG=1
    UCB1CTL1 |= UCTXSTP;                           //Gerar STOP
    while ( (UCB1CTL1 & UCTXSTP) == UCTXSTP)   ;   //Esperar STOP
    //lcd_delay(50);
}

// Auxiliar inicialização do LCD (RS=RW=0)
// *** Só serve para a inicialização ***
void lcd_aux(char dado){
    while ( (UCB1IFG & UCTXIFG) == 0);              //Esperar TXIFG=1
    UCB1TXBUF = ((dado<<4)&0XF0) | BIT3;            //PCF7:4 = dado;
    lcd_delay(20*50L);
    while ( (UCB1IFG & UCTXIFG) == 0);              //Esperar TXIFG=1
    UCB1TXBUF = ((dado<<4)&0XF0) | BIT3 | BIT2;     //E=1
    lcd_delay(20*50L);
    while ( (UCB1IFG & UCTXIFG) == 0);              //Esperar TXIFG=1
    UCB1TXBUF = ((dado<<4)&0XF0) | BIT3;            //E=0;
}

// Atraso usado na configuração do LCD
void lcd_delay(long limite){
    volatile long cont=0;
    while (cont++ < limite) ;
}

// Ver se tem LCD e descobrir o endereço
// TRUE se LCD estiver presente
// Atualiza as variáveis:
// lcd_tem = TRUE se LCD estiver presente
// lcd_adr = endreço da PCF8574
char lcd_presente(void){
    lcd_tem=FALSE;   //Supor que NÃO TEM LCD
    lcd_adr=0x27;
    if (i2c_teste_adr(lcd_adr)==TRUE)
        lcd_tem=TRUE;
    else{
        lcd_adr=0x3f;
        if(i2c_teste_adr(lcd_adr)==TRUE)
            lcd_tem=TRUE;
    }
    return lcd_tem;
}

// Trancar para poder usar I2C
// Verifica se I2C está destrancado, se positivo desab interrupt e tranca o I2C
// Pode imprimir mensagem de erro indicando que não conseguiu trancar
char i2c_trancar(void){
    char x=0;
    while(x<10){
        __disable_interrupt();
        if (i2c_tranca==TRUE){
            __enable_interrupt();
            delay_10ms(1);
            x++;
        }
        else{
            i2c_tranca=TRUE;
            __enable_interrupt();
            return TRUE;
        }
    }
    return FALSE;
}


// Testar endereço I2C
// TRUE se recebeu ACK e FALSE caso contrário
int i2c_teste_adr(char adr){
    UCB1I2CSA = adr;                            //Endereço do I2C
    UCB1CTL1 |= UCTR | UCTXSTT;                 //Gerar START, Mestre transmissor
    while ( (UCB1IFG & UCTXIFG) == 0);          //Esperar pelo START
    UCB1CTL1 |= UCTXSTP;                        //Gerar STOP
    while ( (UCB1CTL1 & UCTXSTP) == UCTXSTP);   //Esperar pelo STOP
    if ((UCB1IFG & UCNACKIFG) == 0)     return TRUE;
    else                                return FALSE;
}

// Configurar Pinos I2C - USCI_B1
// I2C (USCI_B1) P4.2 = SDA
// I2C (USCI_B1) P4.1 = SCL
void i2c_config(char scl_freq){
    UCB1CTL1 = UCSSEL_2 | UCSWRST;   //SMCLK e ressete
    UCB1CTL0 = UCSYNC |     //Síncrono
               UCMODE_3 |   //Modo I2C
               UCMST;       //Mestre

    switch(scl_freq){
        case SCL_50k:   UCB1BRW = 400;   break;  //SCL=40 kHz
        case SCL_100k:  UCB1BRW = 200;   break;  //SCL=100 kHz
        case SCL_200k:  UCB1BRW = 100;   break;  //SCL=200 kHz
        case SCL_400k:  UCB1BRW = 50;    break;  //SCL=400 kHz
        default:        UCB1BRW = 100;   break;  //SCL=200 kHz (default)
    }

    P4SEL |=  BIT2|BIT1;    // Use dedicated module
    //P4REN |=  BIT2|BIT1;    // Resistor enable
    //P4OUT |=  BIT2|BIT1;    // Pull-up

    PMAPKEYID = 0X02D52;    //Liberar mapeamento
//    P4MAP2 = PM_UCB1SDA;    //P4.2 = SDA
//    P4MAP1 = PM_UCB1SCL;    //P4.1 = SCL
      P4MAP1 = PM_UCB1SDA;    //P4.1 = SDA
      P4MAP2 = PM_UCB1SCL;    //P4.2 = SCL

    UCB1CTL1 = UCSSEL_2;    //SMCLK e remove ressete
}

// Tentar colocar I2C num estado conhecido
// P4.1=SCL e P4.2=SDA
void i2c_recupera(void){
    char sda,scl;
    int cont=1;
    P4DIR &= ~(BIT2|BIT1);  //SCL=SDA=entrada
    P4REN &= ~(BIT2|BIT1);  //SCL=SDA=Hi-Z (para o pullup dos dispositivos I2C)
    P4OUT &= ~BIT1;         //Pull down
    sda=(P4IN&BIT2)>>2;
    scl=(P4IN&BIT1)>>1;

    while( sda==0 || scl==0){
        ser1_crlf(1);
        ser1_dec16unz(cont++);
        if (scl==0) ser1_str(" SCL=LOW");
        if (sda==0) ser1_str(" SDA=LOW");
        P4DIR |= BIT1;
        P4OUT &= ~BIT1;         //SCL=0
        //P4REN |=  BIT1;     //Enviar um pulso SCL
        delay_10ms(1);
        P4OUT |= BIT1;        //SCL=1
        P4DIR |= BIT2;
        P4OUT &= ~BIT2;       //SDA=0
        delay_10ms(1);
        P4OUT |= BIT2;         //SDA=1
        delay_10ms(1);

        P4DIR &= ~(BIT2|BIT1);  //SCL=SDA=entrada
        sda=(P4IN&BIT2)>>2;
        scl=(P4IN&BIT1)>>1;
    }
    ser1_str("\nI2C Pronto");
}


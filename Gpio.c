// Puka - Gpio.c

#include <msp430.h>
#include "Defines.h"
#include "Gpio.h"

// Monitorar SW1 e SW2
// Chamada por interrupção a cada 10 mseg.
void sw_monitorar(void){

    //SW1
    if ( (P2IN&BIT1)==0){
        sw1_cont++; //Fechada
        if (sw1_cont>SW_CONT_MAX)   sw1_cont=SW_CONT_MAX;
    }
    else{
        sw1_cont--; //Aberta
        if (sw1_cont==255)          sw1_cont=0;
    }

    if (sw1_estado==ABERTA){
        if (sw1_cont==SW_CONT_MAX){
            sw1_estado=FECHADA;
            sw1=TRUE;
        }
    }
    else{
        if (sw1_cont==0)
            sw1_estado=ABERTA;
    }

    //SW2
    if ( (P1IN&BIT1)==0){
        sw2_cont++; //Fechada
        if (sw2_cont>SW_CONT_MAX)   sw2_cont=SW_CONT_MAX;
    }
    else{
        sw2_cont--; //Aberta
        if (sw2_cont==255)          sw2_cont=0;
    }

    if (sw2_estado==ABERTA){
        if (sw2_cont==SW_CONT_MAX){
            sw2_estado=FECHADA;
            sw2=TRUE;
        }
    }
    else{
        if (sw2_cont==0)
            sw2_estado=ABERTA;
    }

}



void led_VM(void){  P1OUT |=  BIT0;  }   //VM=aceso
void led_vm(void){  P1OUT &= ~BIT0;  }   //VM=apagado
void led_Vm(void){  P1OUT ^=  BIT0;  }   //VM=invertido
void led_VD(void){  P4OUT |=  BIT7;  }   //VD=aceso
void led_vd(void){  P4OUT &= ~BIT7;  }   //VD=apagado
void led_Vd(void){  P4OUT ^=  BIT7;  }   //VD=invertido
void SCP1(void)  {  P2OUT |=  BIT5;  }   //Scope1=Alto
void scp1(void)  {  P2OUT &= ~BIT5;  }   //Scope1=Baixo
void Scp1(void)  {  P2OUT ^=  BIT5;  }   //Scope1=Invertido
void SCP2(void)  {  P2OUT |=  BIT4;  }   //Scope2=Alto
void scp2(void)  {  P2OUT &= ~BIT4;  }   //Scope2=Baixo
void Scp2(void)  {  P2OUT ^=  BIT4;  }   //Scope2=Invertido

// Controle do SIM800L
void SIM_RST(void)  {  P6OUT |=  BIT0;  }   //SIM-RST=Alto
void sim_rst(void)  {  P6OUT &= ~BIT0;  }   //SIM-RST=Baixo
void SIM_rst(void)  {  P6OUT ^=  BIT0;  }   //SIM-RST=Inertido
void SIM_DTR(void)  {  P6OUT |=  BIT1;  }   //SIM-DTR=Alto
void sim_dtr(void)  {  P6OUT &= ~BIT1;  }   //SIM-DTR=Baixo
void SIM_dtr(void)  {  P6OUT ^=  BIT1;  }   //SIM-DTR=Invertido


void gpio_config(void){
    P2DIR &= ~BIT1;     //SW1
    P2REN |=  BIT1;     //SW1
    P2OUT |=  BIT1;     //SW1
    sw1_estado=ABERTA;
    sw1=FALSE;          //Nenhum acionamento

    P1DIR &= ~BIT1;     //SW2
    P1REN |=  BIT1;     //SW2
    P1OUT |=  BIT1;     //SW2
    sw2_estado=ABERTA;
    sw2=FALSE;          //Nenhum acionamento

    P1DIR |=  BIT0;     //Led1 = Vermelho
    P1OUT &= ~BIT0;     //Led1
    P4DIR |=  BIT7;     //Led2 = Verde
    P4OUT &= ~BIT7;     //Led2

    P2DIR |= BIT5 | BIT4;      //Scope, P2.5=Scope1 e P2.4=Scope2
    P2OUT &= ~(BIT5 | BIT4);   //Scope, P2.5=Scope1 e P2.4=Scope2

    P8DIR |= BIT2;      //G2 Controle lig/deslig 5VX
    P8OUT |= BIT2;      //G2=1, ligado 5VX

    P6DIR |= BIT1 | BIT0;       //Controle do SIM800L
    P6DIR &= ~(BIT1 | BIT0);    //Iniciam em ZERO?

}




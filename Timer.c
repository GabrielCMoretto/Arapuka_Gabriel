// Puka - Timer.

#include <msp430.h>
#include "Defines.h"
#include "Gpio.h"
#include "Timer.h"

////////////////////////////////////////////////////
/////////////// TA1: Cronômetro ////////////////////
////////////////////////////////////////////////////
// crono_inic()  --> inicializar cronômetro
// crono_calib() --> calibrar cronômetro
// crono_start() --> disparar cronômetro
// crono_stop()  --> parar cronômetro
// crono_zera()  --> zerar cronômetro
// crono_ler()   --> retorna valor instantâneo

// Ler valor cronometro
unsigned long crono_ler(void){
    unsigned long aux;
    __disable_interrupt();
    aux=((unsigned long)crono_cont<<16) | TA1R;
    __enable_interrupt();
    return aux;
}

// Inicializar cronometro SMCLK (20 MHz/20 = 1 MHz)
void crono_inic(void){
    TA1CTL = TASSEL_2 | ID_2 | TACLR | TAIE;  //1/4
    TA1EX0 = TAIDEX_4;                        //1/5
    crono_cont=0;
}

// Disparar cronometro
void crono_start(void){
    TA1CTL |= MC_2;
}

// Parar cronometro
void crono_stop(void){
    TA1CTL &= ~MC_2;
}

// Zerar cronometro
void crono_zera(void){
    TA1CTL |= TACLR;
    crono_cont=0;
}

// Determinar erro entre Start/Stop
unsigned char crono_calibra(void){
    unsigned char i,x=0;
    unsigned long tp;
    crono_zera();
    for (i=0; i<16; i++){
        crono_start();
        crono_stop();
    }
    tp = crono_ler();
    return tp>>4;
}

// Interrupção
#pragma vector = 48     //TIMER1_A1_VECTOR
__interrupt void isr_ta1ifg(void){
    TA1IV;
    crono_cont++;
}

////////////////////////////////////////////////////
/////////////// TA2: Base de Tempo /////////////////
////////////////////////////////////////////////////

// Atrasos múltiplos de 10 seg
// Tem erro max = 1 seg
void delay_seg(int dur){
    flag_seg=FALSE;
    while (flag_seg == FALSE);    //Sincronizar
    while (dur>0){
        flag_seg=FALSE;
        while (flag_seg == FALSE);
        dur--;
    }
}


// Atrasos múltiplos de 10 ms
// Tem erro max = 10 mseg
void delay_10ms(int dur){
    flag_c_seg=FALSE;
    while (flag_c_seg == FALSE);    //Sincronizar
    while (dur>0){
        flag_c_seg=FALSE;
        while (flag_c_seg == FALSE);
        dur--;
    }
}

// TA2 --> Base de tempo
// TA2.0 --> Interrompe a cada 10 mseg (100 Hz)
// Usa SMCLK/4 = 5 MHz, 50.000 contagens = 10 mseg
void ta2_config(void){
    TA2CTL = TASSEL_2 | ID_2 | MC_2;
    TA2CCTL0 = CCIE;
    TA2CCR0 = 50000;
}


////////////////////////////////////////////////////
///////////////// UCS: 20 MHz  /////////////////////
////////////////////////////////////////////////////

// Configurar relógios da CPU
// MCLK = SMCLK = 20 MHz
// ACLK = 32.768 Hz
void clk_20mhz(void){

    // Configure crystal ports
    P5SEL |= BIT2 | BIT3 | BIT4 | BIT5;     // Configure P5 to use Crystals

    // This should make XT1 startup in 500ms and XT2 in less than 1ms
    UCSCTL6 = XT2DRIVE_3 |    // Turn up crystal drive to
              XT1DRIVE_3 |    // speed up oscillator startup
              XCAP_3     |    // Use maximum capacitance (12pF)
//            XT1OFF     |    // Make sure XT1 and XT2
//            XT2OFF     |    // oscillators are active
//            SMCLKOFF   |    // Leave SMCLK ON
//            XT1BYPASS  |    // Not using external clock source
//            XT2BYPASS  |    // Not using external clock source
//            XTS        |    // XT1 low-frequency mode, which
              0;              // means XCAP bits will be used

    UCSCTL0 = 0x00;           // Let FLL manage DCO and MOD

    UCSCTL1 = DCORSEL_6 |      // Select DCO range to around 20MHz
//            DISMOD    |      // Enable modulator
                        0;

    UCSCTL2 = FLLD_0 | 4;      // (D=1) Set FLL dividers
//              FLLN(5);       // DCOCLK = 1 * 5 * FLLREF = 20 MHz  #define FLLN(x) ((x)-1)
    UCSCTL3 = SELREF_5 |       // Use XT2 (4 MHz) Oscillator for FLLREF
              FLLREFDIV_0;     // divided by 1

    UCSCTL5 = DIVPA_0 |        // Output dividers to 1
              DIVA_0  |        // ACLK  divided by 1
              DIVS_0  |        // SMCLK divided by 4
              DIVM_0;          // MCLK  divided by 1

    UCSCTL7 = 0;               // Clear XT2,XT1,DCO fault flags

    UCSCTL8 = SMCLKREQEN |     // Enable conditional requests for
              MCLKREQEN  |     // SMCLK, MCLK and ACLK. In case one
              ACLKREQEN;       // fails, another takes over

    do {                                                // Check if all clocks are oscillating
          UCSCTL7 &= ~( XT2OFFG   |  // Try to clear XT2,XT1,DCO fault flags,
                        XT1LFOFFG |  // system fault flags and check if
                        DCOFFG );    // oscillators are still faulty
          SFRIFG1 &= ~OFIFG;         //
    } while (SFRIFG1 & OFIFG);       // Exit only when everything is ok

    UCSCTL6 &= ~(XT1DRIVE_3 |        // Xtal is now stable,
                 XT2DRIVE_3);        // reduce drive strength (to save power)

    UCSCTL4 = SELA__XT1CLK |   // ACLK  = XT1 =>     32.768 Hz
              SELS__DCOCLK |   // SMCLK = DCO => 20.000.000 Hz
              SELM__DCOCLK;    // MCLK  = DCO => 20.000.000 Hz
}


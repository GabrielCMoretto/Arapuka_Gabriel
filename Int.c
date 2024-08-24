// Puka - Int.c
// Todas as rotinas de interrupção

#include <msp430.h>
#include "Defines.h"
#include "Timer.h"
#include "Lcd.h"
#include "Gpio.h"
#include "Int.h"


// ISR DMA, fim de transferência ==> Libera i2c_tranca
// DMA2 = LCD
#pragma vector = 50
__interrupt void isr_dma(void){
    int n;
    n = __even_in_range(DMAIV,0x10);

    // DMA2: LCD Note que não se espera a confirmação do STOP. Pode fazer isso?
    if (n==6){
        UCB1CTL1 |= UCTXSTP;    //Gerar STOP
        //while ( (UCB1CTL1 & UCTXSTP) == UCTXSTP);   //Esperar pelo STOP
        i2c_tranca=FALSE;       //Liberar tranca i2c
    }
}

// Base de tempo
// ISR TA2.0, a cada 10 mseg (100 Hz)
#pragma vector = 44
__interrupt void isr_ta2_0(void){
    TA2CCR0 += 50000;

    // Atualizar LCD
    if (i2c_tranca==FALSE){
        if (lcd_flag==TRUE){
            lcd_atualiza();
            lcd_flag=FALSE;  //<<====================
        }
    }

    // Monitorar chaves
    sw_monitorar();

    flag_c_seg=TRUE;
    cont_c_seg++;
    if (cont_c_seg == 10){
        cont_c_seg=0;
        cont_d_seg++;
        flag_d_seg=TRUE;
        if (cont_d_seg == 10){
            cont_d_seg=0;
            cont_seg++;
            flag_seg=TRUE;
            if (cont_seg == 60){
                cont_seg=0;
                cont_min++;
                if (cont_min == 60)
                    cont_min=0;
            }
        }
    }
}





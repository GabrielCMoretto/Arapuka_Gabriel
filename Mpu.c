// Puka - Mpu.c
// I2C (USCI_B1) P4.2 = SCL
// I2C (USCI_B1) P4.1 = SDA

// Endereço do MPU = 0x68+1

#include <msp430.h>
#include "Defines.h"
#include "Timer.h"
#include "Lcd.h"
#include "Serial.h"
#include "Mpu.h"


// Configurar o MPU
// Testa o endereço para ver se MPU está presente
char mpu_config(void){
    char x;
    mpu_tem=TRUE;
    x=i2c_teste_adr(MPU_ADR);
    if (x==FALSE){
        mpu_tem=FALSE;
        return FALSE;
    }
    mpu_wr(PWR_MGMT_1, 1);    //Acodar e Relógio=PLL gx
    delay_10ms(10);           //Esperar acordar
    mpu_wr(CONFIG, 6);        //Taxa = 1 kHz, Banda=5Hz
    mpu_wr(SMPLRT_DIV, 9);    //Taxa de amostr. = 100 Hz
    mpu_wr(GYRO_CONFIG, 0);   // +/- 250 graus/seg
    mpu_wr(ACCEL_CONFIG, 0);  // +/- 2g
    return TRUE;
}

// Ler sequência de dados do MPU
void mpu_rd_vet(char reg, char *vt, char qtd){
    char i;
    if (i2c_trancar()==FALSE){
        ser1_str("\nMPU nao tracou I2C.");
        while(1);
    }
    UCB1I2CSA = MPU_ADR;                //Endereço do MPU

    // Indicar registrador de onde começa a leitura
    UCB1CTL1 |= UCTR | UCTXSTT;                 //Mestre TX + Gerar START
    while ( (UCB1IFG & UCTXIFG) == 0);          //Esperar TXIFG=1
    UCB1TXBUF = reg;                            //Escrever registrador
    while ( (UCB1CTL1 & UCTXSTT) == UCTXSTT);   //STT=0?
    if ( (UCB1IFG & UCNACKIFG) == UCNACKIFG){   //NACK?
        P1OUT |= BIT0;                          //NACK=problema
        while(1);                               //Travar execução
    }

    // Configurar escravo transmissor
    UCB1CTL1 &= ~UCTR;         //Mestre RX
    UCB1CTL1 |= UCTXSTT;       //Gerar START Repetido
    while ( (UCB1CTL1 & UCTXSTT) == UCTXSTT);   //STT=0?

    // Ler a quantidade de dados, menos o último
    for (i=0; i<qtd-1; i++){
    while ((UCB1IFG & UCRXIFG) == 0);       //Esperar RX
          vt[i]=UCB1RXBUF;                  //Ler dado
    }

    // Ler o último dado e gerar STOP
    UCB1CTL1 |= UCTXSTP;                           //Gerar STOP
    while ((UCB1IFG & UCRXIFG) == 0);              //Esperar RX
    vt[i]=UCB1RXBUF;                               //Ler dado
    while ( (UCB1CTL1 & UCTXSTP) == UCTXSTP)   ;   //Esperar STOP
    i2c_tranca=FALSE;
}


// Ler um registrador do MPU
char mpu_rd(char reg){
    if (i2c_trancar()==FALSE){
        ser1_str("\nMPU nao tracou I2C.");
        while(1);
    }
    UCB1I2CSA = MPU_ADR;                //Endereço do MPU
    UCB1CTL1 |= UCTR | UCTXSTT;         //Mestre TX + Gerar START
    while ( (UCB1IFG & UCTXIFG) == 0);  //Esperar TXIFG=1
    UCB1TXBUF = reg;                    //Escrever registrador
    while ( (UCB1CTL1 & UCTXSTT) == UCTXSTT);   //STT=0?
    if ( (UCB1IFG & UCNACKIFG) == UCNACKIFG){   //NACK?
        P1OUT |= BIT0;                          //NACK=problema
        while(1);                               //Travar execução
    }

    // Configurar escravo transmissor
    UCB1CTL1 &= ~UCTR;         //Mestre RX
    UCB1CTL1 |= UCTXSTT;       //Gerar START Repetido
    while ( (UCB1CTL1 & UCTXSTT) == UCTXSTT);   //STT=0?
    UCB1CTL1 |= UCTXSTP;                        //Gerar STOP
    while ( (UCB1IFG & UCRXIFG) == 0);          //Esperar RX
    while ( (UCB1CTL1 & UCTXSTP) == UCTXSTP);   //Esperar STOP
    i2c_tranca=FALSE;
    return UCB1RXBUF;
}



// Escrever num registrador do MPU
void mpu_wr(char reg, char dado){
    if (i2c_trancar()==FALSE){
        ser1_str("\nMPU nao tracou I2C.");
        while(1);
    }
    UCB1I2CSA = MPU_ADR;        //Endereço do MPU
    UCB1CTL1 |= UCTR | UCTXSTT;                 //Mestre TX + START
    while ( (UCB1IFG & UCTXIFG) == 0);          //TXIFG=1?
    UCB1TXBUF = reg;                            //Escrever dado
    while ( (UCB1CTL1 & UCTXSTT) == UCTXSTT);   //Esperar STT=0
    if ( (UCB1IFG & UCNACKIFG) == UCNACKIFG){   //NACK?
        P1OUT |= BIT0;      //NACK=Sinalizar problema
        while(1);           //Travar execução
    }
    while ( (UCB1IFG & UCTXIFG) == 0);  //TXIFG=1?
    UCB1TXBUF = dado;                   //Escrever dado
    while ( (UCB1IFG & UCTXIFG) == 0);  //TXIFG=1?
    UCB1CTL1 |= UCTXSTP;                       //Gerar STOP
    while ( (UCB1CTL1 & UCTXSTP) == UCTXSTP);  //Esperar STOP
    i2c_tranca=FALSE;
}


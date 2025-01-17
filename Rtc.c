// Puka - Rtc.c
// I2C (USCI_B1) P4.2 = SCL
// I2C (USCI_B1) P4.1 = SDA

// Endereço do RTC = 0x68
// Endereço do MPU = 0x68+1

#include <msp430.h>
//#include "Defines.h"
//#include "Timer.h"
#include "Lcd.h"
#include "Serial.h"
#include "Rtc.h"

// Escrever uma sequência de dados no RTC
void rtc_wr_vet(char reg, char *vt, char qtd)
{
    char i;
    if (i2c_trancar() == FALSE)
    {
        ser1_str("\nRTC nao tracou I2C.");
        while (1)
            ;
    }
    UCB1I2CSA = RTC_ADR;                //Endereço do MPU

    // Indicar registrador de onde começa a leitura
    UCB1CTL1 |= UCTR | UCTXSTT;                 //Mestre TX + Gerar START
    while ((UCB1IFG & UCTXIFG) == 0)
        ;          //Esperar TXIFG=1
    UCB1TXBUF = reg;                            //Escrever registrador
    while ((UCB1CTL1 & UCTXSTT) == UCTXSTT)
        ;   //STT=0?
    if ((UCB1IFG & UCNACKIFG) == UCNACKIFG)
    {   //NACK?
        P1OUT |= BIT0;                          //NACK=problema
        while (1)
            ;                               //Travar execução
    }

    // Escrever a quantidade de dados
    for (i = 0; i < qtd; i++)
    {
        while ((UCB1IFG & UCTXIFG) == 0)
            ;   //Esperar TX
        UCB1TXBUF = vt[i];                  //Escrever dado
    }

    while ((UCB1IFG & UCTXIFG) == 0)
        ;       //Esperar última TX  <<<<=======

    // Gerar STOP
    UCB1CTL1 |= UCTXSTP;                           //Gerar STOP
    while ((UCB1CTL1 & UCTXSTP) == UCTXSTP)
        ;   //Esperar STOP
    i2c_tranca = FALSE;
}

// Ler sequência de dados do RTC
void rtc_rd_vet(char reg, char *vt, char qtd)
{
    char i;
    if (i2c_trancar() == FALSE)
    {
        ser1_str("\nRTC nao tracou I2C.");
        while (1)
            ;
    }
    UCB1I2CSA = RTC_ADR;                //Endereço do MPU

    // Indicar registrador de onde começa a leitura
    UCB1CTL1 |= UCTR | UCTXSTT;                 //Mestre TX + Gerar START
    while ((UCB1IFG & UCTXIFG) == 0)
        ;          //Esperar TXIFG=1
    UCB1TXBUF = reg;                            //Escrever registrador
    while ((UCB1CTL1 & UCTXSTT) == UCTXSTT)
        ;   //STT=0?
    if ((UCB1IFG & UCNACKIFG) == UCNACKIFG)
    {   //NACK?
        P1OUT |= BIT0;                          //NACK=problema
        while (1)
            ;                               //Travar execução
    }

    // Configurar escravo transmissor
    UCB1CTL1 &= ~UCTR;         //Mestre RX
    UCB1CTL1 |= UCTXSTT;       //Gerar START Repetido
    while ((UCB1CTL1 & UCTXSTT) == UCTXSTT)
        ;   //STT=0?

    // Ler a quantidade de dados, menos o último
    for (i = 0; i < qtd - 1; i++)
    {
        while ((UCB1IFG & UCRXIFG) == 0)
            ;       //Esperar RX
        vt[i] = UCB1RXBUF;                  //Ler dado
    }

    // Ler o último dado e gerar STOP
    UCB1CTL1 |= UCTXSTP;                           //Gerar STOP
    while ((UCB1IFG & UCRXIFG) == 0)
        ;              //Esperar RX
    vt[i] = UCB1RXBUF;                               //Ler dado
    while ((UCB1CTL1 & UCTXSTP) == UCTXSTP)
        ;   //Esperar STOP
    i2c_tranca = FALSE;
}
// Ler um registrador do RTC
char rtc_rd(char reg)
{
    if (i2c_trancar() == FALSE)
    {
        ser1_str("\nRTC nao tracou I2C.");
        while (1)
            ;
    }
    UCB1I2CSA = RTC_ADR;                //Endereço do RTC
    UCB1CTL1 |= UCTR | UCTXSTT;         //Mestre TX + Gerar START
    while ((UCB1IFG & UCTXIFG) == 0)
        ;  //Esperar TXIFG=1
    UCB1TXBUF = reg;                    //Escrever registrador
    while ((UCB1CTL1 & UCTXSTT) == UCTXSTT)
        ;   //STT=0?
    if ((UCB1IFG & UCNACKIFG) == UCNACKIFG)
    {   //NACK?
        P1OUT |= BIT0;                          //NACK=problema
        while (1)
            ;                               //Travar execução
    }

    // Configurar escravo transmissor
    UCB1CTL1 &= ~UCTR;         //Mestre RX
    UCB1CTL1 |= UCTXSTT;       //Gerar START Repetido
    while ((UCB1CTL1 & UCTXSTT) == UCTXSTT)
        ;   //STT=0?
    UCB1CTL1 |= UCTXSTP;                        //Gerar STOP
    while ((UCB1IFG & UCRXIFG) == 0)
        ;          //Esperar RX
    while ((UCB1CTL1 & UCTXSTP) == UCTXSTP)
        ;   //Esperar STOP
    i2c_tranca = FALSE;
    return UCB1RXBUF;
}

// Escrever num registrador do RTC
void rtc_wr(char reg, char dado)
{
    if (i2c_trancar() == FALSE)
    {
        ser1_str("\nRTC nao tracou I2C.");
        while (1)
            ;
    }
    UCB1I2CSA = RTC_ADR;        //Endereço do RTC
    UCB1CTL1 |= UCTR | UCTXSTT;                 //Mestre TX + START
    while ((UCB1IFG & UCTXIFG) == 0)
        ;          //TXIFG=1?
    UCB1TXBUF = reg;                            //Escrever dado
    while ((UCB1CTL1 & UCTXSTT) == UCTXSTT)
        ;   //Esperar STT=0
    if ((UCB1IFG & UCNACKIFG) == UCNACKIFG)
    {   //NACK?
        P1OUT |= BIT0;      //NACK=Sinalizar problema
        while (1)
            ;           //Travar execução
    }
    while ((UCB1IFG & UCTXIFG) == 0)
        ;  //TXIFG=1?
    UCB1TXBUF = dado;                   //Escrever dado
    while ((UCB1IFG & UCTXIFG) == 0)
        ;  //TXIFG=1?
    UCB1CTL1 |= UCTXSTP;                       //Gerar STOP
    while ((UCB1CTL1 & UCTXSTP) == UCTXSTP)
        ;  //Esperar STOP
    i2c_tranca = FALSE;
}
void rtc_estado()
{
    rtc_rd_vet(0, rtc_vetor, 7);
    str_data_hora(rtc_vetor, rtc_msg);
}
char passou_1_hora()
{
    coloca_hora_dezena();
    volatile unsigned int result1, result2, result;
    //checa se é o mesmo dia/mes/ano
    if ((atual_vetor_data_hora[0] == ultimo_vetor_data_hora[0])
            && (atual_vetor_data_hora[1] == ultimo_vetor_data_hora[1])
            && (atual_vetor_data_hora[2] == ultimo_vetor_data_hora[2]))
    {
        //transforma horas,min, seg em segundos e subtrai
        result1 = ((ultimo_vetor_data_hora[3] * 3600)
                + (ultimo_vetor_data_hora[4] * 60) + ultimo_vetor_data_hora[5]);
        result2 = ((atual_vetor_data_hora[3] * 3600)
                + (atual_vetor_data_hora[4] * 60) + atual_vetor_data_hora[5]);

        if (result1 > result2)
        {
            result = result1 - result2;
        }
        else if (result1 < result2)
        {
            result = result2 - result1;
        }
        //não passou nenhum segundo, garantir que result não vai receber valores loucos
        else
        {
            result = 0;
        }

        //if passou 1 hora = 3600 segundos
        if (result > 3600)
        {
            return TRUE;
        }
        return FALSE;
    }
    // caso não seja o mesmo dia
    else
    {
        //transforma horas,min, seg em segundos e subtrai
        result1 = (86400
                - ((ultimo_vetor_data_hora[3] * 3600)
                        + (ultimo_vetor_data_hora[4] * 60)
                        + ultimo_vetor_data_hora[5]));
        result2 = ((atual_vetor_data_hora[3] * 3600)
                + (atual_vetor_data_hora[4] * 60) + atual_vetor_data_hora[5]);
        //if passou 1 min
        if (result1 > result2)
        {
            result = result1 - result2;
        }
        else if (result1 < result2)
        {
            result = result2 - result1;
        }
        //não passou nenhum segundo, garantir que result não vai receber valores loucos
        else
        {
            result = 0;
        }

        //if passou 1 hora = 3600 segundos
        if (result > 3600)
        {
            return TRUE;
        }
        return FALSE;
    }

}
//coloca todos os valores para checagem se passou 1 hora em forma de dezenas de inteiros
void coloca_hora_dezena()
{

    //Colocando os valores das datas e horas atuais em dezenas de int
    atual_vetor_data_hora[0] = charToInt(rtc_msg[0]) * 10
            + charToInt(rtc_msg[1]);
    atual_vetor_data_hora[1] = charToInt(rtc_msg[3]) * 10
            + charToInt(rtc_msg[4]);
    atual_vetor_data_hora[2] = charToInt(rtc_msg[6]) * 10
            + charToInt(rtc_msg[7]);
    atual_vetor_data_hora[3] = charToInt(rtc_msg[9]) * 10
            + charToInt(rtc_msg[10]);
    atual_vetor_data_hora[4] = charToInt(rtc_msg[12]) * 10
            + charToInt(rtc_msg[13]);
    atual_vetor_data_hora[5] = charToInt(rtc_msg[15]) * 10
            + charToInt(rtc_msg[16]);

    //Colocando os valores das datas e horas antigas (da ultima contagem, se passou o tempo desejado) em dezenas de int
    ultimo_vetor_data_hora[0] = charToInt(rtc_ultima_data_hora_1_hora[0]) * 10
            + charToInt(rtc_ultima_data_hora_1_hora[1]);
    ultimo_vetor_data_hora[1] = charToInt(rtc_ultima_data_hora_1_hora[3]) * 10
            + charToInt(rtc_ultima_data_hora_1_hora[4]);
    ultimo_vetor_data_hora[2] = charToInt(rtc_ultima_data_hora_1_hora[6]) * 10
            + charToInt(rtc_ultima_data_hora_1_hora[7]);
    ultimo_vetor_data_hora[3] = charToInt(rtc_ultima_data_hora_1_hora[9]) * 10
            + charToInt(rtc_ultima_data_hora_1_hora[10]);
    ultimo_vetor_data_hora[4] = charToInt(rtc_ultima_data_hora_1_hora[12]) * 10
            + charToInt(rtc_ultima_data_hora_1_hora[13]);
    ultimo_vetor_data_hora[5] = charToInt(rtc_ultima_data_hora_1_hora[15]) * 10
            + charToInt(rtc_ultima_data_hora_1_hora[16]);
}
char passou_3_hora()
{
    coloca_hora_dezena();
    volatile unsigned int result1, result2, result;
    //checa se é o mesmo dia/mes/ano
    if ((atual_vetor_data_hora[0] == ultimo_vetor_data_hora[0])
            && (atual_vetor_data_hora[1] == ultimo_vetor_data_hora[1])
            && (atual_vetor_data_hora[2] == ultimo_vetor_data_hora[2]))
    {
        //transforma horas,min, seg em segundos e subtrai
        result1 = ((ultimo_vetor_data_hora[3] * 3600)
                + (ultimo_vetor_data_hora[4] * 60) + ultimo_vetor_data_hora[5]);
        result2 = ((atual_vetor_data_hora[3] * 3600)
                + (atual_vetor_data_hora[4] * 60) + atual_vetor_data_hora[5]);

        if (result1 > result2)
        {
            result = result1 - result2;
        }
        else if (result1 < result2)
        {
            result = result2 - result1;
        }
        //não passou nenhum segundo, garantir que result não vai receber valores loucos
        else
        {
            result = 0;
        }

        //if passou 1 hora = 3600 segundos
        if (result > 3600)
        {
            return TRUE;
        }
        return FALSE;
    }
    // caso não seja o mesmo dia
    else
    {
        //transforma horas,min, seg em segundos e subtrai
        result1 = (86400
                - ((ultimo_vetor_data_hora[3] * 3600)
                        + (ultimo_vetor_data_hora[4] * 60)
                        + ultimo_vetor_data_hora[5]));
        result2 = ((atual_vetor_data_hora[3] * 3600)
                + (atual_vetor_data_hora[4] * 60) + atual_vetor_data_hora[5]);
        //if passou 1 min
        if (result1 > result2)
        {
            result = result1 - result2;
        }
        else if (result1 < result2)
        {
            result = result2 - result1;
        }
        //não passou nenhum segundo, garantir que result não vai receber valores loucos
        else
        {
            result = 0;
        }

        //if passou 1 hora = 3600 segundos
        if (result > 10800)
        {
            return TRUE;
        }
        return FALSE;
    }

}
//Atualiza o vetor que contem a data e hora refente ao ultimo envio
// So para saber se passou 1 min e 1h
void atualiza_data_hora(int min, int hora)
{
    int i;
    //atualiza minutos
    if (min == TRUE)
    {
        for (i = 0; i < 18; i++)
        {
            rtc_ultima_data_hora_1_min[i] = rtc_msg[i];
        }
    }
    //atualiza hora
    if (hora == TRUE)
    {
        for (i = 0; i < 18; i++)
        {
            rtc_ultima_data_hora_1_hora[i] = rtc_msg[i];
        }
    }
}
char passou_1_min()
{
    volatile unsigned int result1, result2, result;
    coloca_min_dezena();

    //checa se é o mesmo dia/mes/ano
    if ((atual_vetor_data_hora[0] == ultimo_vetor_data_hora[0])
            && (atual_vetor_data_hora[1] == ultimo_vetor_data_hora[1])
            && (atual_vetor_data_hora[2] == ultimo_vetor_data_hora[2]))
    {
        //transforma horas,min, seg em segundos e subtrai
        result1 = ((ultimo_vetor_data_hora[3] * 3600)
                + (ultimo_vetor_data_hora[4] * 60) + ultimo_vetor_data_hora[5]);
        result2 = ((atual_vetor_data_hora[3] * 3600)
                + (atual_vetor_data_hora[4] * 60) + atual_vetor_data_hora[5]);

        if (result1 > result2)
        {
            result = result1 - result2;
        }
        else if (result1 < result2)
        {
            result = result2 - result1;
        }
        //não passou nenhum segundo, garantir que result não vai receber valores loucos
        else
        {
            result = 0;
        }

        //if passou 1 min
        if (result > 59)
        {
            return TRUE;
        }
        return FALSE;
    }
    // caso não seja o mesmo dia
    else
    {
        //transforma horas,min, seg em segundos e subtrai
        result1 = (86400
                - ((ultimo_vetor_data_hora[3] * 3600)
                        + (ultimo_vetor_data_hora[4] * 60)
                        + ultimo_vetor_data_hora[5]));
        result2 = ((atual_vetor_data_hora[3] * 3600)
                + (atual_vetor_data_hora[4] * 60) + atual_vetor_data_hora[5]);
        //if passou 1 min
        if (result1 > result2)
        {
            result = result1 - result2;
        }
        else if (result1 < result2)
        {
            result = result2 - result1;
        }
        //não passou nenhum segundo, garantir que result não vai receber valores loucos
        else
        {
            result = 0;
        }

        //if passou 1 min
        if (result > 59)
        {
            return TRUE;
        }
        return FALSE;
    }
}
//coloca todos os valores para checagem se passou 1 min em forma de dezenas de inteiros
void coloca_min_dezena()
{

    //Colocando os valores das datas e horas atuais em dezenas de int
    atual_vetor_data_hora[0] = charToInt(rtc_msg[0]) * 10
            + charToInt(rtc_msg[1]);
    atual_vetor_data_hora[1] = charToInt(rtc_msg[3]) * 10
            + charToInt(rtc_msg[4]);
    atual_vetor_data_hora[2] = charToInt(rtc_msg[6]) * 10
            + charToInt(rtc_msg[7]);
    atual_vetor_data_hora[3] = charToInt(rtc_msg[9]) * 10
            + charToInt(rtc_msg[10]);
    atual_vetor_data_hora[4] = charToInt(rtc_msg[12]) * 10
            + charToInt(rtc_msg[13]);
    atual_vetor_data_hora[5] = charToInt(rtc_msg[15]) * 10
            + charToInt(rtc_msg[16]);

    //Colocando os valores das datas e horas antigas (da ultima contagem, se passou o tempo desejado) em dezenas de int
    ultimo_vetor_data_hora[0] = charToInt(rtc_ultima_data_hora_1_min[0]) * 10
            + charToInt(rtc_ultima_data_hora_1_min[1]);
    ultimo_vetor_data_hora[1] = charToInt(rtc_ultima_data_hora_1_min[3]) * 10
            + charToInt(rtc_ultima_data_hora_1_min[4]);
    ultimo_vetor_data_hora[2] = charToInt(rtc_ultima_data_hora_1_min[6]) * 10
            + charToInt(rtc_ultima_data_hora_1_min[7]);
    ultimo_vetor_data_hora[3] = charToInt(rtc_ultima_data_hora_1_min[9]) * 10
            + charToInt(rtc_ultima_data_hora_1_min[10]);
    ultimo_vetor_data_hora[4] = charToInt(rtc_ultima_data_hora_1_min[12]) * 10
            + charToInt(rtc_ultima_data_hora_1_min[13]);
    ultimo_vetor_data_hora[5] = charToInt(rtc_ultima_data_hora_1_min[15]) * 10
            + charToInt(rtc_ultima_data_hora_1_min[16]);
}


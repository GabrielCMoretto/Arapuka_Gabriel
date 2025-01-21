// GPRS.c

#include <msp430.h>
#include "Defines.h"
#include "Gprs.h"
#include "Strings.h"
#include "Serial.h"
#include "Timer.h"

// Imprimir uma string na USCI_A0
// Polling TX_IFG=1
void gprs_str(char *msg)
{
    int i = 0;
    while (msg[i] != '\0')
    {
        gprs_char(msg[i++]);
    }
}

// Imprimir um caracter na USCI_A0
// Polling TX_IFG=1
void gprs_char(char x)
{
    UCA0TXBUF = x;
    while ((UCA0IFG & UCTXIFG) == 0)
        ;
}
// comando que envia um comando pro gprs
void gprs_send_cmd(char *vet)
{
    //ser1_str("\n[");
    //ser1_str(vet);
    //ser1_str("]\n");
    gprs_str(vet);
    delay_10ms(1);
    gprs_char(0x0D);
    delay_10ms(1);
    gprs_char(0x0A);
    delay_10ms(1);
    //while (gprs_tira(&x)==TRUE){
    //    ser1_char(x);
    //}
}
// comando que envia menssagem para um celular
void gprs_send_msg(char *vet)
{
    char x;
    gprs_config();
    gprs_send_cmd("AT+CMGF=1");
    gprs_send_cmd("AT+CMGS=\"+5521979592145\"");
    gprs_str(vet);
    delay_10ms(1);
    gprs_char(0x0A);
    delay_10ms(1);
    gprs_char(0x1A);
    delay_10ms(1);
    gprs_char(0x0D);
    delay_10ms(1);

    while (gprs_tira(&x) == TRUE)
    {
        ser1_char(x);
    }

}
//loop que coloca no serial oque chega do gprs
void loopserial()
{
    char x;
    while (gprs_tira(&x) == TRUE)
    {
        if (checkreceive(x) == TRUE)
        {
            delay_10ms(100);
            msg_handler(x);
            delay_10ms(100);
        }
        ser1_char(x);
    }
}
//ve se recebeu menssagem baseado no formato de mensagem que ele le do serial
char checkreceive(char x)
{
    if (x == receivestring[receiveiterator])
    {
        receiveiterator++;
        if (receiveiterator == 22)
        {
            receiveiterator = 0;
            return TRUE;
        }
        return FALSE;
    }
    else
    {
        receiveiterator = 0;
        return FALSE;
    }
}
//manda os comandos que servem para configurar o serial para mostrar as mensagens que chega no gprs
void gprs_config_receive(char x)
{
    gprs_send_cmd("AT+CMGF=1");
    while (gprs_tira(&x) == TRUE)
    {
        ser1_char(x);
    }
    delay_10ms(1);
    gprs_send_cmd("AT+CNMI=1,2,0,0,0");
    delay_10ms(1);

    gprs_send_cmd("AT+CMGD=1,4");
    while (gprs_tira(&x) == TRUE)
    {
        ser1_char(x);
    }
}
void msg_handler(char x)
{
    int cmd = FALSE;
    while (gprs_tira(&x) == TRUE)
    {
        ser1_char(x);
        if (cmd)
        {
            state_cod = x;
            cmd = FALSE;
            break;
        }
        if (x == 0x0A)
        {
            cmd = TRUE;
        }

    }

    if (state_cod == 48)
    {
        gprs_send_msg("Arapuka mudou para o modo Dormente");
        estado = DMT;
        serialMSG = TRUE;
    }
    if (state_cod == 49)
    {
        gprs_send_msg("Arapuka mudou para o modo Vigilia");
        estado = VIG;
        serialMSG = TRUE;
    }
    if (state_cod == 50)
    {
        gprs_send_msg("Arapuka mudou para o modo Suspeito");
        estado = SUS;
        serialMSG = TRUE;
    }
    if (state_cod == 51)
    {
        gprs_send_msg("Arapuka mudou para o modo Alerta 1");
        estado = ALT1;
        serialMSG = TRUE;
    }
    if (state_cod == 52)
    {
        gprs_send_msg("Arapuka mudou para o modo Alerta 2");
        estado = ALT2;
        serialMSG = TRUE;
    }
}
////////////////////////////////////////////////////
////////////////// Fila GPRS ///////////////////////
///////////// USCI_A0 <== GPRS /////////////////////
/////// Interrupção chama gprs_poe() ///////////////
/////// Ler a fila como gprs_tira()  ///////////////
////////////////////////////////////////////////////

// Inicializar fila GPRS (USCI_A0)
void gprs_config(void)
{
    gprs_pin = 1;
    gprs_pout = 0;
}

// Colocar um byte na fila GPRS
// Chamada dentro Interrupções de RX USCI_A0
char gprs_poe(char cha)
{
    if (gprs_pin == gprs_pout)
    {
        gprs_cheia();     //Msg de fila cheia
        return FALSE;     //SERI cheia
    }
    else
    {
        gprs_fila[gprs_pin++] = cha;
        if (gprs_pin == GPRS_FILA_TAM)
            gprs_pin = 0;
        return TRUE;
    }
}

// Mensagem de Fila GPRS Cheia
void gprs_cheia(void)
{
    __disable_interrupt();
    ser1_str("\nERRO: GPRS Cheia!\n");
    seri_dump();
    ser1_str("\nTravado!");
    while (TRUE)
        ;
}

// Tirar um byte da fila GPRS
// Função chamada fora das interrupções
char gprs_tira(char *cha)
{
    int pout_aux;
    __disable_interrupt();
    pout_aux = gprs_pout + 1;
    if (pout_aux == GPRS_FILA_TAM)
        pout_aux = 0;
    if (pout_aux == gprs_pin)
    {
        __enable_interrupt();
        return FALSE;
    }
    else
    {
        *cha = gprs_fila[pout_aux];
        gprs_pout = pout_aux;
        __enable_interrupt();
        return TRUE;
    }
}

// USCI_A0: Configurar Serial 1 (MSP <=> GPRS), Hab int RX
// Usa SMCLK = 20 MHz
// P3.3 = TX (MSP ==> GPRS) e P3.4 = RX (MSP <== GPRS)
void ser0_config(char br)
{
    UCA0CTL1 = UCSSEL_2 | UCSWRST;     //UCSI em Reset
    UCA0CTL0 = 0;                   //1 Stop

    switch (br)
    {
    case BR_4800:
    case BR_9600:
        UCA0BRW = 2083;
        UCA0MCTL = UCBRS_2 | UCBRF_0;
        break;
    case BR_19200:
        UCA0BRW = 1041;
        UCA0MCTL = UCBRS_6 | UCBRF_0;
        break;
    case BR_38400:
        UCA0BRW = 520;
        UCA0MCTL = UCBRS_7 | UCBRF_0;
        break;
    case BR_57600:
        UCA0BRW = 347;
        UCA0MCTL = UCBRS_2 | UCBRF_0;
        break;
    case BR_74880:
    case BR_115200:
        UCA0BRW = 173;
        UCA0MCTL = UCBRS_5 | UCBRF_0;
        break;
    case BR_230400:
        UCA0BRW = 86;
        UCA0MCTL = UCBRS_7 | UCBRF_0;
        break;
    case BR_250k:
        UCA0BRW = 80;
        UCA0MCTL = 0;
        break;
    case BR_500k:
        UCA0BRW = 40;
        UCA0MCTL = 0;
        break;
    case BR_1000k:
        UCA0BRW = 20;
        UCA0MCTL = 0;
        break;
    case BR_2000k:
        UCA0BRW = 10;
        UCA0MCTL = 0;
        break;
    default:
        UCA0BRW = 2083;
        UCA0MCTL = UCBRS_2 | UCBRF_0;
        break; //default = 9600 bps

    }
    P3DIR |= BIT3;         //P3.3 saída
    P3DIR &= ~BIT4;         //P3.4 entrada
    P3SEL |= BIT4 | BIT3;   //Tx serial

    UCA0CTL1 &= ~ UCSWRST;  //UCSI sai de Reset
    //UCA0IE = UCTXIE|UCRXIE; //Hab interrupção  TX e RX
    UCA0IE = UCRXIE;            //Hab interrupção  RX
}

// USCI_A0 - ISR (GPRS <==> USCI_A0)
#pragma vector = 56
__interrupt void ISR_USCI_A0(void)
{
    int n;
    n = __even_in_range(UCA0IV, 0x4);
    gprs_poe(UCA0RXBUF);
}


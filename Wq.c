// Puka - Wq.c
//
// (USCI_B0) P3.0 = WQ-SIMO
// (USCI_B0) P3.1 = WQ-SOMI
// (USCI_B0) P3.2 = WQ-CLK
// P2.6 = WQ-CS

// P3.0 (SIMO) ==> (SI)  U3.5
// P3.1 (SOMI) <== (SO)  U3.2
// P3.2 (SCK)  ==> (SCK) U3.6
// P2.6 (CS)   ==> (CS)  U3.1
//       3V3   ==> (Vcc) U3.8
//       3V3   ==> (SIO2) U3.3
//       3V3   ==> (HLD) U3.7
//       GND   ==> (GNC) U3.4

#include <msp430.h>
#include "Defines.h"
#include "Timer.h"
#include "Serial.h"
#include "Wq.h"

// Escrever um bloco na WQ
// Deve estar dentro de uma página de 256 bytes
// Ao terminar WEL=0
void wq_wr_blk(long adr, char *vet, int qtd)
{
    int i;
    while (wq_ocupado() == TRUE)
        ser1_char('*');
    wq_WEL();
    wq_cs();   //Selecionar SRAM
    spi_transf(WQ_PAGE_PROG);
    spi_transf((adr >> 16) & 0xFF);
    spi_transf((adr >> 8) & 0xFF);
    spi_transf(adr & 0xFF);
    for (i = 0; i < qtd; i++)
    {
        spi_transf(vet[i]);
    }
    wq_CS();      //De-selecionar Flash
    wq_wel();
    while (wq_ocupado())
        ;
}

// Ler uma certa quantidade de posições
void wq_rd_blk(long adr, char *vet, int qtd)
{
    long i;
    wq_cs();   //Selecionar Flash
    spi_transf(WQ_DT_RD);
    spi_transf((adr >> 16) & 0xFF);
    spi_transf((adr >> 8) & 0xFF);
    spi_transf(adr & 0xFF);
    for (i = 0; i < qtd; i++)
    {
        //delay(1000);
        vet[i] = spi_transf(0);
        //ser_dec32unz(i);
        //ser_spc(1);
        //ser_hex8(vet[i]);
        //ser_spc(1);
        //ser_crlf(1);
    }
    wq_CS();      //De-selecionar Flash

}

// Apagar toda a memória - Demora!
void wq_erase_chip(void)
{
    int i = 0;
    //ser1_str("\nErase Chip");
    //ser1_crlf(1);
    wq_WEL();
    wq_cs();      //Selecionar Flash
    spi_transf(WQ_CHIP_ERASE); //0x60
    wq_CS();      //De-selecionar SRAM
    while (TRUE)
    {
        if (wq_ocupado() == FALSE)
            break;
        delay_seg(1);
        ser1_dec16unz(++i);
        ser1_spc(1);
        if ((i % 10) == 0)
            ser1_crlf(1);
    }
    ser1_str("\nPronto\n");
}

// Apagar (0xFF) um bloco de 64KB
// Endereço = aaaa aaaa   0000 0000   0000 0000 (24 bits)
void wq_erase_64k(long adr)
{
    adr &= 0xFF0000L;        //Ficar dentro pag de 64KB
    wq_WEL();
    wq_cs();   //Selecionar Flash
    spi_transf(WQ_BLK_ERASE_64K);
    spi_transf((adr >> 16) & 0xFF);
    spi_transf((adr >> 8) & 0xFF);
    spi_transf(adr & 0xFF);
    wq_CS();      //De-selecionar SRAM
    while (wq_ocupado() == TRUE)
    {
        delay_10ms(10);
        ser1_char('.');
    }
}

// Apagar (0xFF) um setor de 4 KB
// Endereço = aaaa aaaa   aaaa 0000   0000 0000 (24 bits)
// 4095 setores de 4 KB
// Ao terminar WEL=0 automaticamente
void wq_erase_4k(long adr)
{
    adr &= 0xFFF000L;        //Ficar dentro pag de 4KB
    wq_WEL();
    wq_cs();   //Selecionar Flash
    spi_transf(WQ_SEC_ERASE_4K);
    spi_transf((adr >> 16) & 0xFF);
    spi_transf((adr >> 8) & 0xFF);
    spi_transf(adr & 0xFF);
    wq_CS();      //De-selecionar SRAM
    while (wq_ocupado() == TRUE)
    {
        delay_10ms(1);
        ser1_char('.');
    }
}

// WQ está ocupado?
// Ocupado=TRUE Livre=FALSE
char wq_ocupado(void)
{
    char x;
    wq_cs();   //Selecionar SRAM
    spi_transf(WQ_SR1_RD);
    x = spi_transf(0);
    wq_CS();      //De-selecionar SRAM
    if ((x & 1) == 0)
        return FALSE;
    else
        return TRUE;
}

// WEL = 0 --> Write Enable = 0
// Bloqueia as escritas
void wq_wel(void)
{
    wq_cs();   //Selecionar SRAM
    spi_transf(WQ_WEL_RST);
    wq_CS();      //De-selecionar SRAM
}

// WEL = 1 --> Write Enable = 0
// Permite as escritas
void wq_WEL(void)
{
    wq_cs();   //Selecionar SRAM
    spi_transf(WQ_WEL_SET);
    wq_CS();      //De-selecionar SRAM
}

// Ler Reg de Status 2
char wq_sr2_rd(void)
{
    char d;
    wq_cs();      //Selecionar Flash
    spi_transf(WQ_SR2_RD);
    d = spi_transf(0);
    wq_CS();      //De-selecionar Flash
    return d;
}

// Ler Reg de Status 1
char wq_sr1_rd(void)
{
    char d;
    wq_cs();      //Selecionar Flash
    spi_transf(WQ_SR1_RD);
    d = spi_transf(0);
    wq_CS();      //De-selecionar Flash
    return d;
}

// Retorna
// vt[0] = 0xEF = Manufacturer ID
// vt[1] = 0x15 = Device ID
// Deve
void w25_manuf_dev_id(char *vt)
{
    wq_cs();   //Selecionar Flash
    spi_transf(WQ_MANUF_ID);     //JEDEC
    spi_transf(0);
    spi_transf(0);
    spi_transf(0);
    vt[0] = spi_transf(0);
    vt[1] = spi_transf(0);
    wq_CS();      //De-selecionar SRAM
}

void wq_cs(void)
{
    P2OUT &= ~BIT6;
}   //Habilitar o CS
void wq_CS(void)
{
    P2OUT |= BIT6;
}   //Desabilitar o CS

// Enviar e receber um byte pela SPI
char spi_transf(char x)
{
    UCB0IFG &= ~UCRXIFG;   //Zerar Esperar receber

    while ((UCB0IFG & UCTXIFG) == 0)
        ;   //Esperar transmitir
    UCB0TXBUF = x;
    while ((UCB0IFG & UCRXIFG) == 0)
        ;   //Esperar receber
    return UCB0RXBUF;
}

// Configurar SPI USCI_B0
void spi_config(void)
{
    UCB0CTL1 = UCSSEL_2 | UCSWRST;  //Reset=1
    //UCB0CTL0 = UCCKPL | UCCKPH | UCMSB | UCMST | UCSYNC;  //==>Não funciona
    //UCB0CTL0 = UCMSB | UCMST | UCSYNC;                    //=>Não funciona
    UCB0CTL0 = UCCKPL | UCMSB | UCMST | UCSYNC;           //==>OK
    //UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;           //==>OK
    UCB0BRW = SMCLK / 500000L;    //CLK=400kHz (20Mhz/500k = 400)
    //UCB0BRW = SMCLK/100000L;    //CLK=400kHz (20Mhz/100k = 2000)
    P3SEL |= (BIT2 | BIT1 | BIT0);  //Destinar ao SPI
    P2DIR |= BIT6;              //P2.6 = CS
    P2OUT |= BIT6;              //P2.6 = High
    UCB0CTL1 = UCSSEL_2;        //Reset=0
}
//Salva na memoria, chamada por Suspeito, alerta 1 e 2
//Salva o estado atual e todos os outros dados
void salvar_memoria()
{
    unsigned int i = 0, j = 0;
    char vetor[100];
    volatile long wr = wr_address_mem;
    char estado_puka[5];
    estado_puka[0] = estado + '0';
    estado_puka[1] = '\0';

    rtc_estado();
    gps_estado_modo();
    todos_dados(TRUE);

    //coloca o estado atual no vetor que sera salvo na memoria
    while (estado_puka[i] != '\0')
    {
        vetor[i] = estado_puka[i];
        i++;
    }

    while (toda_msg[j] != '\0')
    {
        vetor[i] = toda_msg[j];
        i++, j++;
    }
    vetor[i] = '\n', i++, vetor[i] = '\r', i++, vetor[i] = '\0';

    wr = wr_address_mem;

    wq_wr_blk(wr, vetor, i);

    wr_address_mem += 128;

    save_estado();
}

flash_save(uint32_t adress, uint8_t *data, uint16_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++)
    {
        save_data(data[i], adress + i);
    }
}


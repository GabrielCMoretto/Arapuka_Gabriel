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
#include "Strings.h"

// Configurar o MPU
// Testa o endereço para ver se MPU está presente
char mpu_config(void)
{
    char x;
    mpu_tem = TRUE;
    x = i2c_teste_adr(MPU_ADR);
    if (x == FALSE)
    {
        mpu_tem = FALSE;
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
void mpu_rd_vet(char reg, char *vt, char qtd)
{
    char i;
    if (i2c_trancar() == FALSE)
    {
        ser1_str("\nMPU nao tracou I2C.");
        while (1)
            ;
    }
    UCB1I2CSA = MPU_ADR;                //Endereço do MPU

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

// Ler um registrador do MPU
char mpu_rd(char reg)
{
    if (i2c_trancar() == FALSE)
    {
        ser1_str("\nMPU nao tracou I2C.");
        while (1)
            ;
    }
    UCB1I2CSA = MPU_ADR;                //Endereço do MPU
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

// Escrever num registrador do MPU
void mpu_wr(char reg, char dado)
{
    if (i2c_trancar() == FALSE)
    {
        ser1_str("\nMPU nao tracou I2C.");
        while (1)
            ;
    }
    UCB1I2CSA = MPU_ADR;        //Endereço do MPU
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
//Armazena os valores de ax, ay, az, gx, gy e gz
void mpu_values()
{
    char vetor[14];
    mpu_rd_vet(ACCEL_XOUT_H, vetor, 14);    //Ler 14 regs

    ax = vetor[0];
    ax = (ax << 8) + vetor[1];
    ay = vetor[2];
    ay = (ay << 8) + vetor[3];
    az = vetor[4];
    az = (az << 8) + vetor[5];
    tp = vetor[6];
    tp = (tp << 8) + vetor[7];
    gx = vetor[8];
    gx = (gx << 8) + vetor[9];
    gy = vetor[10];
    gy = (gy << 8) + vetor[11];
    gz = vetor[12];
    gz = (gz << 8) + vetor[13];
}
//coloca Modo, horario, latitude, longitude
// Se vier o argumento TRUE, significa que é para colocar os valores do MPU (usada para salvar na memoria)
void todos_dados(int mpu)
{
    unsigned int i, y;
    char buffer[5]; //8 bits podem representar apenas 3 caracteres, contando com '\0' == 4. Porem pode ter negativo entao coloquei mais 1 posição

    //salvar rtc
    for (i = 0; i < 17; i++)
    {
        toda_msg[i] = rtc_msg[i];
    }
    toda_msg[17] = ' ';

    //salvar gps
    //latitude
    for (i = 18; i < 29; i++)
    {
        toda_msg[i] = gps_msg[i - 18];
    }
    if (gps_msg[0] == '+')
    {
        toda_msg[29] = 'N';
    }
    else if (gps_msg[0] == '-')
    {
        toda_msg[29] = 'S';
    }
    else{
        toda_msg[29] = '?';
    }
    toda_msg[30] = '_';

    //longitude
    for (i = 31; i < 42; i++)
    {
        toda_msg[i] = gps_msg[i - 20];
    }
    toda_msg[42] = ',';
    if (gps_msg[11] == '+')
    {
        toda_msg[43] = 'E';
    }
    else if (gps_msg[11] == '-')
    {
        toda_msg[43] = 'W';
    }
    else{
        toda_msg[43] = '?';
    }
    //receber caracter nulo na ultima posição
    toda_msg[44] = '\0';

    //salvar acelerometro e giroscopio
    //Precisa colocar o caractere de espaço na posição 44 e guardar os novos valores referentes ao MPU
    if (mpu == TRUE)
    {
        toda_msg[44] = ' ';
        i = 45;

        mpu_8bits();

        //Acelerometro
        toda_msg[i++] = 'A';
        toda_msg[i++] = '=';
        toda_msg[i++] = ' ';
        //ax
        IntToChar(ax, buffer);
        y = 0;
        while (buffer[y] != '\0')
        {
            toda_msg[i++] = buffer[y++];
        }
        toda_msg[i++] = ' ';

        //ay
        IntToChar(ay, buffer);
        y = 0;
        while (buffer[y] != '\0')
        {
            toda_msg[i++] = buffer[y++];
        }
        toda_msg[i++] = ' ';

        //az
        IntToChar(az, buffer);
        y = 0;
        while (buffer[y] != '\0')
        {
            toda_msg[i++] = buffer[y++];
        }
        toda_msg[i++] = ',';
        toda_msg[i++] = ' ';

        //Giroscopio
        toda_msg[i++] = 'G';
        toda_msg[i++] = '=';
        toda_msg[i++] = ' ';

        //gx
        IntToChar(gx, buffer);
        y = 0;
        while (buffer[y] != '\0')
        {
            toda_msg[i++] = buffer[y++];
        }
        toda_msg[i++] = ' ';

        //gy
        IntToChar(gy, buffer);
        y = 0;
        while (buffer[y] != '\0')
        {
            toda_msg[i++] = buffer[y++];
        }
        toda_msg[i++] = ' ';

        //gz
        IntToChar(gz, buffer);
        y = 0;
        while (buffer[y] != '\0')
        {
            toda_msg[i++] = buffer[y++];
        }
        toda_msg[i] = '\0';

    }
}
//pega apenas os 8 bits mais significativos do MPU e salva nas variaveis globais
//Função sera chamada quando for gravar todos os dados na memoria
void mpu_8bits()
{

    char x;

    x = i2c_teste_adr(MPU_ADR);
    if (x == FALSE)
    {
        x = i2c_teste_adr(MPU_ADR);
    }

    mpu_values();
    //transformando em valores de 8 bits com sinal
    ax = ax >> 8;
    ay = ay >> 8;
    az = az >> 8;
    gx = gx >> 8;
    gy = gy >> 8;
    gz = az >> 8;
}
//Coloca os valores do estado de repouso do mpu e defino os maiores valores que cada variavel pode obter com MPU em repouso
//sera chamada no estado dormente, onde ira definir os valores para as variaveis de aceleração e giroscopio
//Tambem será utilizada quando for salvar na memoria e precisar pegar valores recentes. Não tera problema pois a checagem do MPU era principalmente
// para caracterizar o Furto
void repouso_values_mpu()
{
    char x;

    x = i2c_teste_adr(MPU_ADR);
    if (x == FALSE)
    {
        return;
    }

    mpu_values();

    /*
     * //valores obtidos de um MPU especifico, é preciso verificar esse ruido para todos os outros
     * ax= +16874  +16932  == 58
     * ay= -00502  -00430  == 72
     * az= -01834  -01742  == 92
     *
     * gx= -00130  -00120  == 10
     * gy= -00293  -00281  == 12
     * gz= -00320  -00311  == 9
     */

    //Na aceleração irei colocar valores bem maiores do que do ruido, pois qualquer movimento pequeno no objeto
    // estava dando o primeiro sinal de alerta a fim de caracterizar o furto
    amaiorx = ax + 300;
    amenorx = ax - 300;
    amaiory = ay + 300;
    amenory = ay - 300;
    amaiorz = az + 300;
    amenorz = az - 300;

    maiorgx = gx + 10;
    menorgx = gx - 10;
    maiorgy = gy + 12;
    menorgy = gy - 12;
    maiorgz = gz + 9;
    menorgz = gz - 10;
}
// checa se houve mudança do acelerometro, caracterizando furto
// Se a primeira checagem der TRUE, checa durante 5 seg se ainda obtem valores diferentes do estado de repouso, dessa maneira a função retorna
// que o dispositivo foi roubado
char acel_furto()
{

    char x;
    int i = 0, alerta = 0;

    x = i2c_teste_adr(MPU_ADR);
    if (x == FALSE)
    {
        return FALSE;
    }

    mpu_values();

    //Checa se houve aleração por parte do acelerometro
    if (ax < amenorx || ax > amaiorx)
        alerta++;
    else if (ay < amenory || ay > amaiory)
        alerta++;
    else if (az < amenorz || az > amaiorz)
        alerta++;
    else
        alerta = 0;

    if (alerta == 0)
    {
        return FALSE;
    }
    //recebeu o primeiro sinal que está com valores diferentes do repouso
    //começa a checagem por 5 segundos
    //a cada segundo checa, Caso pare de acusar falha, retorna false, caso contrario checa até encerrar 5 segundos, se for verdadeiro retorna TRUE
    //
    else if (alerta != 0)
    {
        //gprs_str("alerta\n\r");
        while (i < 5)
        {
            mpu_values();

            //Checa se houve aleração por parte do gisroscopio
            if (ax < amenorx || gx > amaiorx)
                alerta++;
            else if (ay < amenory || ay > amaiory)
                alerta++;
            else if (az < amenorz || az > amaiorz)
                alerta++;
            else
                alerta = 0;

            if (alerta == 0)
            {             //voltou para o estado normal
                return FALSE;
            }
            else if (alerta == 4)
            {       //foi furtado
                return TRUE;
            }
            i++;
            delay_10ms(200); //1 seg
        }
    }
}
// checa se houve mudança do giroscopio, caracterizando furto
// Se a primeira checagem der TRUE, checa durante 5 seg se ainda obtem valores diferentes do estado de repouso, dessa maneira a função retorna
// que o dispositivo foi roubado
char giro_furto(){
    char x;
    int i=0, alerta=0;

    x=i2c_teste_adr(MPU_ADR);
    if (x == FALSE){
        return FALSE;
    }

    mpu_values();

    //Checa se houve aleração por parte do gisroscopio
    if(gx < menorgx || gx > maiorgx) alerta++;
    else if(gy < menorgy || gy > maiorgy) alerta++;
    else if(gz < menorgz || gz > maiorgz) alerta++;
    else alerta = 0;

    if(alerta == 0){
        return FALSE;
    }
    //recebeu o primeiro sinal que está com valores diferentes do repouso
    //começa a checagem por 5 segundos
    //a cada segundo checa, Caso pare de acusar falha, retorna false, caso contrario checa até encerrar 5 segundos, se for verdadeiro retorna TRUE
    //
    else if(alerta != 0){
        //gprs_str("alerta\n\r");
        while(i<5){
            mpu_values();

            //Checa se houve aleração por parte do gisroscopio
            if(gx < menorgx || gx > maiorgx) alerta++;
            else if(gy < menorgy || gy > maiorgy) alerta++;
            else if(gz < menorgz || gz > maiorgz) alerta++;
            else alerta = 0;

            if(alerta == 0){             //voltou para o estado normal
                return FALSE;
            }
            else if(alerta == 4){       //foi furtado
                return TRUE;
            }
            i++;
            delay_10ms(200); //1 seg
        }
    }
}

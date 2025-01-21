#include "Estados.h"
#include "Defines.h"
#include "Mpu.h"
#include "Gprs.h"
#include "Lcd.h"
#include <msp430.h>

void dormente()
{
    char x;
    lcdb_str(1, 1, "Dormente       ");
    estado_puka[0] = 'D', estado_puka[1] = 'M', estado_puka[2] = 'T', estado_puka[3] =
            ' ', estado_puka[4] = '-', estado_puka[5] = ' ', estado_puka[6] =
            '\0';
    //gprs_send_msg(estado_puka);
    gprs_send_msg("Arapuka em estado Dormente");

    while (TRUE)
    {
        sel_estado();
    }

}
void vigilia()
{

    lcdb_str(1, 1, "Vigilia        ");
    estado_puka[0] = 'V', estado_puka[1] = 'I', estado_puka[2] = 'G', estado_puka[3] =
            ' ', estado_puka[4] = '-', estado_puka[5] = ' ', estado_puka[6] =
            '\0';
    //gprs_send_msg(estado_puka);
    gprs_send_msg("Arapuka em estado de Vigilia");
    salvar_memoria();

    //definindo os valores do repouso do Arapuka
    repouso_values_mpu();
    set_values_gps();

    while (TRUE)
    {
        sel_estado();

        //se foi furtado entra em Alerta 1
        if (acel_furto() == TRUE || giro_furto() == TRUE || gps_furto() == TRUE)
        {
            suspeito();
        }
    }

}
void suspeito()
{
    lcdb_str(1, 1, "Suspeito       ");
    estado_puka[0] = 'S', estado_puka[1] = 'P', estado_puka[2] = 'T', estado_puka[3] =
            ' ', estado_puka[4] = '-', estado_puka[5] = ' ', estado_puka[6] =
            '\0';
    //gprs_send_msg(estado_puka);
    gprs_send_msg("Arapuka em estado Suspeito");
    salvar_memoria();

    gps_estado_modo();
    todos_dados(TRUE);
    gprs_send_msg(toda_msg);

    rtc_estado();
    atualiza_data_hora(FALSE, TRUE);        //S� preciso atualizar a ultima hora

    int i = 0;
    while (TRUE)
    {
        sel_estado();
        rtc_estado();

        //envia mensagem a cada 3h
        if (passou_1_hora() == TRUE)
        {
            i++;
            if (i == 3)
            {
                atualiza_data_hora(FALSE, TRUE); //S� preciso atualizar a ultima hora
                gps_estado_modo();
                todos_dados(TRUE);
                gprs_send_msg(toda_msg);

                //SALVA NA MEMORIA
                salvar_memoria();
                i = 0;
            }
        }
    }

}
void alerta1()
{
    lcdb_str(1, 1, "Alerta 1       ");
    estado_puka[0] = 'A', estado_puka[1] = 'L', estado_puka[2] = 'T', estado_puka[3] =
            '1', estado_puka[4] = ' ', estado_puka[5] = '-', estado_puka[6] =
            ' ', estado_puka[7] = '\0';
    gprs_send_msg(estado_puka);
    gprs_send_msg("Arapuka em estado de Alerta 1");
    salvar_memoria();

    gps_estado_modo();
    todos_dados(TRUE);
    gprs_send_msg(toda_msg);

    rtc_estado();
    atualiza_data_hora(FALSE, TRUE);        //S� preciso atualizar a ultima hora

    while (TRUE)
    {
        sel_estado();
        rtc_estado();

        if (passou_1_hora() == TRUE)
        {
            atualiza_data_hora(FALSE, TRUE); //S� preciso atualizar a ultima hora
            gps_estado_modo();
            todos_dados(TRUE);
            gprs_send_msg(toda_msg);

            //SALVA NA MEMORIA
            salvar_memoria();
        }
    }

}
void alerta2()
{
    lcdb_str(1, 1, "Alerta 2       ");
    estado_puka[0] = 'A', estado_puka[1] = 'L', estado_puka[2] = 'T', estado_puka[3] =
            '2', estado_puka[4] = ' ', estado_puka[5] = '-', estado_puka[6] =
            ' ', estado_puka[7] = '\0';
    gprs_send_msg(estado_puka);
    gprs_send_msg("Arapuka em estado de Alerta 2");
    salvar_memoria();

    gps_estado_modo();
    todos_dados(TRUE);
    gprs_send_msg(toda_msg);

    rtc_estado();
    atualiza_data_hora(TRUE, TRUE);         //S� preciso atualizar a ultima hora
    while (TRUE)
    {
        sel_estado();
        rtc_estado();

        if (passou_1_min() == TRUE)
        {
            atualiza_data_hora(TRUE, FALSE); //atualiza apenas o minuto

            gps_estado_modo();
            todos_dados(TRUE);
            gprs_send_msg(toda_msg);
        }

        if (passou_1_hora() == TRUE)
        {
            atualiza_data_hora(FALSE, TRUE); //S� preciso atualizar a ultima hora
            gps_estado_modo();
            todos_dados(TRUE);
            gprs_send_msg(toda_msg);

            //SALVA NA MEMORIA
            salvar_memoria();
        }
    }

}
//Analisar comando recebido do ESP32, vou fazer um vetor receber cada caractere, ent�o analisar qual a mensagem recebida
void sel_estado()
{
    char start_record = FALSE;
    char x;

    while (gprs_tira(&x) == TRUE)
    {
        if (checkreceive(x) == TRUE)
        {
            estado_y = 1, estado_i = 0; // estado_y==1 --> Posso selecionar o estado, pois j� recebi o comando
        }

        if (estado_y && x == '\n')
        {
            start_record = TRUE;
        }
    }
    delay_10ms(100);
    while (gprs_tira(&x) == TRUE)
    {
        if (start_record)
        { //garantir de receber apenas o que realmente quero, estava vindo caracteres loucos
            estado_comando[estado_i++] = x; //armazena os caracteres dentro de estado_comando
        }
    }

    if (estado_y == 1)
    {
        estado_y = 0;
        if (estado_comando[0] == '#' && estado_comando[1] == 'R'
                && estado_comando[2] == 'S' && estado_comando[3] == 'T'
                && estado_comando[4] == '#')
        {
            resete();
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'D'
                && estado_comando[2] == 'M' && estado_comando[3] == 'T'
                && estado_comando[4] == '#')
        {
            dormente();
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'V'
                && estado_comando[2] == 'I' && estado_comando[3] == 'G'
                && estado_comando[4] == '#')
        {
            vigilia();
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'A'
                && estado_comando[2] == 'L' && estado_comando[3] == 'T'
                && estado_comando[4] == '1' && estado_comando[5] == '#')
        {
            alerta1();
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'A'
                && estado_comando[2] == 'L' && estado_comando[3] == 'T'
                && estado_comando[4] == '2' && estado_comando[5] == '#')
        {
            alerta2();
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'S'
                && estado_comando[2] == 'P' && estado_comando[3] == 'T'
                && estado_comando[4] == '#')
        {
            suspeito();
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'A'
                && estado_comando[2] == 'P' && estado_comando[3] == 'G'
                && estado_comando[4] == '#')
        {
            apagar();
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'R'
                && estado_comando[2] == 'T' && estado_comando[3] == 'C')
        {
            rtc_configure();
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'R'
                && estado_comando[2] == 'D' && estado_comando[3] == ' ')
        { //Checa se � o comando de leitura das posi��es de memoria
            ler_memoria();          // Funcao que decide se vai ler_n ou ler_n_m
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'S'
                && estado_comando[2] == 'T' && estado_comando[3] == 'A'
                && estado_comando[4] == 'T' && estado_comando[5] == '#')
        {
            status();
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'g'
                && estado_comando[2] == 'r' && estado_comando[3] == 'e'
                && estado_comando[7] == 'o' && estado_comando[8] == 'n')
        {
            led_VD();
            gprs_send_msg("LED Verde ON");
            lcdb_str(1, 1, "Led Verde ON   ");
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'g'
                && estado_comando[2] == 'r' && estado_comando[3] == 'e'
                && estado_comando[7] == 'o' && estado_comando[8] == 'f'
                && estado_comando[9] == 'f')
        {
            led_vd();
            gprs_send_msg("LED Verde Off");
            lcdb_str(1, 1, "Led Verde OFF  ");
        }

        else if (estado_comando[0] == '#' && estado_comando[1] == 'r'
                && estado_comando[2] == 'e' && estado_comando[3] == 'd'
                && estado_comando[5] == 'o' && estado_comando[6] == 'n')
        {
            led_VM();
            gprs_send_msg("Led Vermelho ON");
            lcdb_str(1, 1, "Led RED ON     ");
        }
        else if (estado_comando[0] == '#' && estado_comando[1] == 'r'
                && estado_comando[2] == 'e' && estado_comando[3] == 'd'
                && estado_comando[5] == 'o' && estado_comando[6] == 'f'
                && estado_comando[7] == 'f')
        {
            led_vm();
            gprs_send_msg("Led Vermelho Off");
            lcdb_str(1, 1, "Led RED OFF   ");
        }
        else
        {
            lcdb_str(1, 1, "Codigo Errado  ");
            code_erro();
        }
    }

}
// retorna todos os dados para o ESP32
//envia o estado atual, data/hora, localizacao e acel/gir
void status()
{
    lcdb_str(1, 1, "Status         ");
    rtc_estado();
    gps_estado_modo();
    //argumento FALSE para receber apenas os valores do RTC + GPS
    todos_dados(TRUE);
    //gprs_send_msg(estado_puka);                       //Envia o estado atual
    gprs_send_msg(toda_msg);
}
void estados_config()
{
    estado_i = 0, estado_y = 0;
    wr_address_mem = 0;
}
//Ler qual o ultimo estado salvo e pega o wr_address_mem da ultima escrita
void check_estado_address()
{
    unsigned int i;
    long address = 0;
    char vetor[5]; //tamanho necessario

    //che
    while (TRUE)
    {
        wq_rd_blk(address, vetor, 5);         //ler apenas as primeiras posi��es
        if (vetor[0] < ' ' || vetor[0] > 'z')
            break;    //significa que nao tem mais registro
        address += 128;
    }

    wr_address_mem = address;

    //se tiver apenas o primeiro registro, vai darruim. Esse IF evita da erro
    if (address >= 128)
        address -= 128;

    wq_rd_blk(address, vetor, 5);
    //checa qual o ultimo estado para poder voltar
    if (vetor[0] == 'S' && vetor[1] == 'P' && vetor[2] == 'T')
    {
        suspeito();
    }
    else if (vetor[0] == 'V' && vetor[1] == 'I' && vetor[2] == 'G')
    {
        vigilia();
    }
    else if (vetor[0] == 'A' && vetor[1] == 'L' && vetor[2] == 'T'
            && vetor[3] == '1')
    {
        alerta1();
    }
    else if (vetor[0] == 'A' && vetor[1] == 'L' && vetor[2] == 'T'
            && vetor[3] == '2')
    {
        alerta2();
    }
    else
    {
        dormente();
    }
}
void apagar()
{
    lcdb_str(1, 1, "Apagar Memoria ");
    wq_erase_chip();
    gprs_send_msg("Toda Memoria apagada");
}
//Decide qual fun��o chamar, se chama a de ler_n ou ler_n_m
void ler_memoria()
{
    lcdb_str(1, 1, "Ler Memoria    ");
    unsigned int i, z, entrou = 0;
    int j = 1;

    n_rd = 0, m_rd = 0;

    //Ao percorrer os caracteres, checar quem encontra primeiro
    for (i = 4; i < 24; i++)
    {
        //ler_n
        if (estado_comando[i] == '#' && entrou == 0)
        {
            i--;
            while (i > 3)
            {
                n_rd += (estado_comando[i] - '0') * j;
                i--;
                j *= 10;
            }
            entrou = 1;
            ler_n();               //passa posi��o do fim do vetor
        }
        //Ler_n_m
        else if (estado_comando[i] == ' ' && entrou == 0)
        {
            ///// n
            z = i;    //onde esta o caractere de espaco
            i--;
            while (i > 3)
            {
                n_rd += (estado_comando[i] - '0') * j;
                i--;
                j *= 10;
            }

            ///// m
            i = z, j = 1;
            while (estado_comando[i] != '#')
                i++;
            i--;
            while (i > z)
            {
                m_rd += (estado_comando[i] - '0') * j;
                i--;
                j *= 10;
            }
            entrou = 1;
            ler_n_m();
        }
    }
}
// Realiza o resete do MSP por software
void resete()
{
    lcdb_str(1, 1, "Resete         ");
    PMMCTL0 = PMMPW | PMMSWPOR; // Configura��o para realizar um software reset
}
void code_erro()
{
    gprs_send_msg("ERROR, comando invalido.");
}
void ler_n()
{
    unsigned int i;
    char vt[100];

    long address = 0;

    //ler memoria 128 em 128 registros e mostrar no gprs
    for (i = 0; i < n_rd; i++)
    {
        wq_rd_blk(address, vt, 100);  //ler as 128 primeiras posi��es da memoria
        gprs_send_msg(vt);
        address += 128;   //pula para os proximo 128 registros
        delay_10ms(400);
    }
}
void ler_n_m(int n, int m)
{
    unsigned int i;
    int cont;
    char vt[100];

    long address = 0, wr = 0;

    cont = (m_rd - n_rd) + 1; // pega ate o registro igual a m, ou seja, pega o registro m tambem
    address = (n_rd * 128) - 128; //pega do registro n

    //ler memoria 128 em 128 registros e mostrar no gprs
    for (i = 0; i < cont; i++)
    {
        wq_rd_blk(address, vt, 100); //ler as 100 primeiras posi��es da memoria, so precisa ler isso
        gprs_send_msg(vt);
        address += 128;   //pula para os proximo 128 registros
        delay_10ms(400);
    }
}
void rtc_configure()
{
    lcdb_str(1, 1, "RTC Configure  ");
    char vetor[3];

    //Data (dd/mm/aa)
    vetor[0] = 16 * (estado_comando[5] - 0x30) + (estado_comando[6] - 0x30); //Dia
    vetor[1] = 16 * (estado_comando[8] - 0x30) + (estado_comando[9] - 0x30); //Mes
    vetor[2] = 16 * (estado_comando[11] - 0x30) + (estado_comando[12] - 0x30); //Ano
    rtc_wr_vet(4, vetor, 3);

    // Hora (hh:mm:ss)
    vetor[2] = 16 * (estado_comando[14] - 0x30) + (estado_comando[15] - 0x30); //Horas
    vetor[1] = 16 * (estado_comando[17] - 0x30) + (estado_comando[18] - 0x30); //Minutos
    vetor[0] = 16 * (estado_comando[20] - 0x30) + (estado_comando[21] - 0x30); //Segundos
    rtc_wr_vet(0, vetor, 3);
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

#include "Estados.h"
#include "Defines.h"
#include "Mpu.h"
#include "Gprs.h"
#include "Lcd.h"
#include "Wq.h"

void dormente()
{
    if (serialMSG)
    {
        ser1_str("Estado:Dormente\n");
        lcdb_apaga();
        lcdb_str(1, 1, "Opera:Dormente");
        serialMSG = FALSE;
        salvar_memoria();
        delay_10ms(50);
    }

}
void vigilia()
{
    if (serialMSG)
    {
        ser1_str("Estado:Vigilia\n");
        lcdb_apaga();
        lcdb_str(1, 1, "Opera:Vigilia");
        serialMSG = FALSE;
        //definindo os valores do repouso do Arapuka
        repouso_values_mpu();
        set_values_gps();
        salvar_memoria();
        delay_10ms(1);
    }
    //se foi furtado entra em Alerta 1
    if (acel_furto() == TRUE || giro_furto() == TRUE || gps_furto() == TRUE)
    {
        estado = SUS;
        serialMSG = TRUE;
    }

}
void suspeito(char x)
{
    if (serialMSG)
    {
        ser1_str("Estado:Suspeito\n");
        lcdb_apaga();
        lcdb_str(1, 1, "Opera:Suspeito");
        serialMSG = FALSE;
        //gprs_complete_str(estado_puka);
        salvar_memoria();

        gps_estado_modo();
        todos_dados(TRUE);
        gprs_send_msg(toda_msg);

        rtc_estado();
        atualiza_data_hora(FALSE, TRUE);    //Só preciso atualizar a ultima hora
    }

    rtc_estado();

    //envia mensagem a cada 3h
    if (passou_3_hora() == TRUE)
    {
        atualiza_data_hora(FALSE, TRUE); //Só preciso atualizar a ultima hora
        gps_estado_modo();
        todos_dados(TRUE);
        gprs_send_msg(toda_msg);

        //SALVA NA MEMORIA
        salvar_memoria();
    }

}
void alerta1(char x)
{
    if (serialMSG)
    {
        ser1_str("Estado:Alerta 1\n");
        serialMSG = FALSE;
        //gprs_complete_str(estado_puka);
        lcdb_apaga();
        lcdb_str(1, 1, "Opera:Alerta 1");
        salvar_memoria();

        gps_estado_modo();
        todos_dados(TRUE);
        gprs_send_msg(toda_msg);

        rtc_estado();
        atualiza_data_hora(FALSE, TRUE);    //Só preciso atualizar a ultima hora
    }

    rtc_estado();

    if (passou_1_hora() == TRUE)
    {
        atualiza_data_hora(FALSE, TRUE); //Só preciso atualizar a ultima hora
        gps_estado_modo();
        todos_dados(TRUE);
        gprs_send_msg(toda_msg);

        //SALVA NA MEMORIA
        salvar_memoria();
    }

}
void alerta2(char x)
{
    if (serialMSG)
    {
        ser1_str("Estado:Alerta 2\n");
        serialMSG = FALSE;
        lcdb_apaga();
        lcdb_str(1, 1, "Opera:Alerta 2");
        salvar_memoria();

        gps_estado_modo();
        todos_dados(TRUE);
        gprs_send_msg(toda_msg);

        rtc_estado();
        atualiza_data_hora(TRUE, TRUE);     //Só preciso atualizar a ultima hora
    }
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
        atualiza_data_hora(FALSE, TRUE); //Só preciso atualizar a ultima hora
        gps_estado_modo();
        todos_dados(TRUE);
        gprs_send_msg(toda_msg);

        //SALVA NA MEMORIA
        salvar_memoria();
    }

}
void carregar_estado()
{
    char bytes[256];
    char byte;

    wq_rd_blk(0x3FFFE00, bytes, 256);
    byte = bytes[0];
    wr_address_mem = ((long) bytes[1] << 24) | ((long) bytes[2] << 16)
            | ((long) bytes[3] << 8) | (long) bytes[4];

    // Validar o byte lido (verificar se é um número)
    if (byte >= '0' && byte <= '4')
    {
        estado = byte - '0';  // Converter o caractere para número
    }
    else
    {
        estado = 0;  // Valor padrão em caso de falha na leitura
    }
}
//salva estado na ultima pagina
void save_estado()
{
    int i;
    char buffer[256];  // Buffer para armazenar os dados do setor
    long sector_address = 0x3FFFE00; // Alinhar ao início do setor do ultimo endereco
    char data[256];
    data[0] = estado + '0';

    data[1] = (wr_address_mem >> 24) & 0xFF;
    data[2] = (wr_address_mem >> 16) & 0xFF;
    data[3] = (wr_address_mem >> 8) & 0xFF;
    data[4] = wr_address_mem & 0xFF;

    // Ler todo o setor atual para o buffer
    wq_rd_blk(sector_address, buffer, 256);

    // Apagar o setor
    wq_erase_4k(sector_address);

    // Reescrever o setor inteiro com os dados atualizados
    wq_wr_blk(sector_address, data, 256);

}

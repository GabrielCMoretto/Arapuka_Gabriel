#include "Estados.h"
#include "Defines.h"
#include "Mpu.h"
#include "Gprs.h"
#include "Lcd.h"

void dormente()
{
    if (serialMSG)
    {
        ser1_str("Estado:Dormente\n");
        lcdb_apaga();
        lcdb_str(1, 1, "Opera:Dormente");
        serialMSG = FALSE;
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

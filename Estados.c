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
    estado = VIG;
    serialMSG = TRUE;
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
void suspeito()
{
    if (serialMSG)
    {
        ser1_str("Estado:Suspeito\n");
        lcdb_apaga();
        lcdb_str(1, 1, "Opera:Suspeito");
        serialMSG = FALSE;
    }
}
void alerta1()
{
    if (serialMSG)
    {
        ser1_str("Estado:Alerta 1\n");
        serialMSG = FALSE;
    }
}
void alerta2()
{
    if (serialMSG)
    {
        ser1_str("Estado:Alerta 2\n");
        serialMSG = FALSE;
    }
}

#include "Estados.h"
#include "Defines.h"
#include "Mpu.h"

void dormente()
{
    if (serialMSG)
    {
        ser1_str("Estado:Dormente\n");
        serialMSG = FALSE;
    }
    estado = VIG;
    serialMSG = TRUE;
}
void vigilia()
{
    if (serialMSG)
    {
        ser1_str("Estado:Vigilia\n");
        serialMSG = FALSE;
    }
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

    if (gx < menorx || gx > maiorx)
        alerta++;
    else if (gy < menory || gy > maiory)
        alerta++;
    else if (gz < menorz || gz > maiorz)
        alerta++;
    else
        alerta = 0;

    ser1_str("Alerta: ");
    ser1_dec16(alerta);
    ser1_crlf(1);

    ser1_str("Gx: ");
    ser1_dec16(gx);
    ser1_crlf(1);

    ser1_str("Gy: ");
    ser1_dec16(gy);
    ser1_crlf(1);

    ser1_str("Gz: ");
    ser1_dec16(gz);
    ser1_crlf(1);

    if (alerta > 4)
    {
        roubado = 1;
        lcdb_apaga();
        ser1_str("Dispositivo entrando no modo suspeito!");
        ser1_crlf(1);
        delay_10ms(400);
        estado = SUS;
        serialMSG = TRUE;
    }
    delay_10ms(100);
}
void suspeito()
{
    if (serialMSG)
    {
        ser1_str("Estado:Suspeito\n");
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

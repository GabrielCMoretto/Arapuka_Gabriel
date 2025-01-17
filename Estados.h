#ifndef ESTADOS_H_
#define ESTADOS_H_

extern char serialMSG;
extern char estado;
extern int ax, ay, az, tp, gx, gy, gz;
extern int maiorx, menorx, maiory, menory, maiorz, menorz,roubado, alerta;
extern char vetor[14];

void dormente ();
void vigilia ();
void suspeito (char x);
void alerta1 (char x);
void alerta2 (char x);
#endif

// Strings.c

#include <msp430.h>
#include "Defines.h"
#include "Strings.h"


// Msg com 18 posições         01234567890123456
// Gerar string com Data Hora (dd/mm/aa hh:mm:ss)
// Segue o formato dos regs do RTC
// vet[7] = conteúdo dos endereços de 0 até 6 do DS3231
void str_data_hora(char *vet, char *msg){
  str_bcd8(vet[4],&msg[ 0]); msg[ 2]='/';
  str_bcd8(vet[5],&msg[ 3]); msg[ 5]='/';
  str_bcd8(vet[6],&msg[ 6]); msg[ 8]=' ';
  str_bcd8(vet[2],&msg[ 9]); msg[11]=':';
  str_bcd8(vet[1],&msg[12]); msg[14]=':';
  str_bcd8(vet[0],&msg[15]); msg[17]='\0';
}



// Comparar duas strings
char str_cmp(char *s1, char *s2){
    char i=0;
    while(s1[i]!='\0'){
        if (s1[i]!=s2[i])   return FALSE;
        i++;
    }
    if (s1[i]==s2[i])   return TRUE;
    return FALSE;
}

// Medir tamanho de uma string.
// Não conta o zero final
int str_len(char *msg){
   int x=0;
   while(msg[x] != '\0')   x++;
   return x;
}

// String para Cronômetro
// 4.294,967.296 s --> 14 posições com o '\0'
void str_crono(unsigned long x, char *msg){
    int t;
    char i,j,vt[12];
    for (i=0; i<12; i++)    msg[i]='\0';    //Marcar tudo com '\0'
    str_dec32u(x,vt);
    str_rmvz_u(vt);
    t=str_len(vt);
    msg[14]='\0';
    i=0;
    for(j=0; j<=t; j++){
        if (j==4 || j==7 || j==10)   msg[13-i++]='.';
        msg[13-i++]=vt[t-j];
    }

    //Descolar para esquerda, removendo os '\0'
    j=0;
    while(msg[j] == '\0')  j++;  //Onde acabam os zeros
    i=0;
    while(msg[j] != '\0') msg[i++]=msg[j++];
    msg[i]=msg[j];
    if (msg[0]=='\0'){
      msg[0]='0'; //Só tem zeros?
      msg[1]='\0';
    }
}

// Remove zeros à esquerda, sem sinal
void str_rmvz_u(char *msg){
  int i,j;
  j=0;
  while(msg[j] == '0')  j++;  //Onde acabam os zeros
  i=0;
  while(msg[j] != '\0') msg[i++]=msg[j++];
  msg[i]=msg[j];
  if (msg[0]=='\0'){
    msg[0]='0'; //Só tem zeros?
    msg[1]='\0';
  }
}

// Remove zeros à esquerda, com sinal
void str_rmvz_s(char *msg){
  int i,j;
  j=1;
  while(msg[j] == '0')  j++;  //Onde acabam os zeros
  i=1;
  while(msg[j] != '\0') msg[i++]=msg[j++];
  msg[i]=msg[j];
  if (msg[1]=='\0') msg[0]='0'; //Só tem zeros?
}

////////////////////////////////////////////////////
/////////// float / double /////////////////////////
////////////////////////////////////////////////////

// Imprimir double float = + xxx xxx xxx xxx , ddd ddd ddd ddd ddd (usar char msg[30])
// Não imprime zeros à esquerda, exceto para 0.xxxx
// 12 posições = limite da parte inteira
// 15 posições = limite da parte fracionária
// Caso ultrapasse os limites imprime ### , ###
void str_double(double f, char prec, char *msg){
  char i,ix,aux;
  double fabs,fx,faux;
  long dv=1;
  fabs=f;
  if (f<0)  fabs=-1*f;
  if (fabs >= 1E12 || (prec>15)){  //Ultrapassou limite?
    for (i=0; i<7; i++)
      msg[i]='#';
    msg[3]='.';
    msg[7]='\0';
  }
  else{
    ix=0;
    //Imprimir sinal
    if (f<0)  msg[ix++]='-';
    else      msg[ix++]='+';
    //Tamanho da parte inteira
    fx=fabs;
    while( (fx-dv) >0){
      dv*=10;
    }
    dv/=10;

    //Imprimir parte inteira
    faux=0;
    while(fx>=1){
      aux=fx/dv;
      faux=10*faux+aux;     //compor parte inteira
      msg[ix++]=aux+0x30;
      fx=fx-(aux*dv);
      dv=dv/10;
    }
    //Caso com zeros, ex: 10,1
    while(dv!=0){
      msg[ix++]='0';
      dv = dv/10;
    }
    //Caso +.123 (falta um zero)
    if (ix==1)  msg[ix++]='0';
    //msg[ix++]=',';    //Vírgula
    msg[ix++]='.';      //Ponto

    //Parte fracionária
    fx=fabs-faux;
    for(i=0; i<prec; i++){
      fx*=10;
      aux=fx;
      msg[ix++]=aux+0x30;
      fx=fx-aux;
    }
    msg[ix]='\0';
  }
}

// Imprimir float = + xxx xxx xxx , ddd ddd ddd ddd (usar char msg[24])
// Não imprime zeros à esquerda, exceto para 0.xxxx
//  9 posições = limite da parte inteira
// 12 posições = limite da parte fracionária
// Caso ultrapasse os limites imprime ### , ###
void str_float(float f, char prec, char *msg){
  char i,ix,aux;
  float fabs,fx,faux;
  long dv=1;
  fabs=f;
  if (f<0)  fabs=-1*f;
  if (fabs >= 1E9 || (prec>12)){  //Ultrapassou limite?
    for (i=0; i<7; i++)
      msg[i]='#';
    msg[3]='.';
    msg[7]='\0';
  }
  else{
    ix=0;
    //Imprimir sinal
    if (f<0)  msg[ix++]='-';
    else      msg[ix++]='+';
    //Tamanho da parte inteira
    fx=fabs;
    while( (fx-dv) >0){
      dv*=10;
    }
    dv/=10;

    //Imprimir parte inteira
    faux=0;
    while(fx>=1){
      aux=fx/dv;
      faux=10*faux+aux;     //compor parte inteira
      msg[ix++]=aux+0x30;
      fx=fx-(aux*dv);
      dv=dv/10;
    }
    //Caso com zeros, ex: 10,1
    while(dv!=0){
      msg[ix++]='0';
      dv = dv/10;
    }
    //Caso +.123 (falta um zero)
    if (ix==1)  msg[ix++]='0';
    //msg[ix++]=',';    //Vírgula
    msg[ix++]='.';      //Ponto

    //Parte fracionária
    fx=fabs-faux;
    for(i=0; i<prec; i++){
      fx*=10;
      aux=fx;
      msg[ix++]=aux+0x30;
      fx=fx-aux;
    }
    msg[ix]='\0';
  }
}

////////////////////////////////////////////////////
//////////////// 32 bits ///////////////////////////
////////////////////////////////////////////////////

// dec32 - Decinal 32 bits com sinal e com zeros à esquerda
// msg = +4 294 967 295 \0 - 12 posições
void str_dec32(long c, char *msg){
  char i,aux;
  long dv=1000000000L;
  unsigned long x;
  if (c<0)  msg[0]='-';
  else      msg[0]='+';
  x=abs(c);
  for (i=1; i<10; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// dec32u - Decinal 8 bits sem sinal e com zeros à esquerda
// msg = 4 294 967 295 \0 - 11 posições
void str_dec32u(long c, char *msg){
  char i,aux;
  unsigned long x,dv=1000000000L;
  x=c;
  for (i=0; i<9; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// Gerar string hexa 32 bits sem sinal
// msg = xxxx xxxx \0 - 9 posições
void str_hex32(long c, char *msg){
    char i,aux;
    for (i=8; i>0; i--){
        aux=c&0xF;
        if (aux>9)  msg[i-1]=aux+0x37;
        else        msg[i-1]=aux+0x30;
        c = c>>4;
    }
    msg[8]='\0';
}

////////////////////////////////////////////////////
//////////////// 16 bits ///////////////////////////
////////////////////////////////////////////////////

// bcd16 - BCD em 16 bits, sem sinal e zeros à esquerda
// msg = 1234\0 - 5 posições
void str_bcd16(int c, char *msg){
  msg[0]=0x30+(c>>12);
  msg[1]=0x30+((c>>8)&0xF);
  msg[2]=0x30+((c>>4)&0xF);
  msg[3]=0x30+(c&0xF);
  msg[4]=0;
}

// dec16 - Decinal 16 bits com sinal e zeros à esquerda
// msg = +12345\0 - 7 posições
void str_dec16(int c, char *msg){
  char i;
  int x,aux,dv=10000;
  if (c<0)  msg[0]='-';
  else      msg[0]='+';
  x=abs(c);
  for (i=1; i<5; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// dec16u - Decinal 8 bits sem sinal e zeros à esquerda
// msg = 65535\0 - 6 posições
void str_dec16u(unsigned int c, char *msg){
  char i;
  unsigned int x,aux,dv=10000;
  x=c;
  for (i=0; i<4; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// Gerar string hexa 16 bits sem sinal
void str_hex16(int c, char *msg){
    char i,aux;
    for (i=4; i>0; i--){
        aux=c&0xF;
        if (aux>9)  msg[i-1]=aux+0x37;
        else        msg[i-1]=aux+0x30;
        c = c>>4;
    }
    msg[4]='\0';
}

////////////////////////////////////////////////////
///////////////// 8 bits ///////////////////////////
////////////////////////////////////////////////////

// bcd8 - BCD em 8 bits, sem sinal e zeros à esquerda
// msg = 12\0 - 3 posições
void str_bcd8(char c, char *msg){
  msg[0]=0x30+(c>>4);
  msg[1]=0x30+(c&0xF);
  msg[2]=0;
}

// dec8 - Decinal 8 bits com sinal e zeros à esquerda
// msg = +123\0 - 5 posições
void str_dec8(signed char c, char *msg){
  char i,x,aux,dv=100;
  if (c<0)  msg[0]='-';
  else      msg[0]='+';
  x=abs(c);
  for (i=1; i<3; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// dec8u - Decinal 8 bits sem sinal e zeros à esquerda
// msg = 123\0 - 4 posições
void str_dec8u(char c, char *msg){
  char i,x,aux,dv=100;
  x=c;
  for (i=0; i<2; i++){
    aux=x/dv;
    msg[i]=aux+0x30;
    x=x-(aux*dv);
    dv=dv/10;
  }
  msg[i++]=x+0x30;
  msg[i]='\0';
}

// Gerar string hexa 8 bits sem sinal
void str_hex8(char c, char *msg){
    char i,aux;
    for (i=2; i>0; i--){
        aux=c&0xF;
        if (aux>9)  msg[i-1]=aux+0x37;
        else        msg[i-1]=aux+0x30;
        c = c>>4;
    }
    msg[2]='\0';
}






// Converter string para número
// Retorna long
// Aceita Hexadecinal 0xABCD ou decimal
// Se houver caracter inválido, retorna ZERO
long str_2_num(char *vt){
    char i,positivo=TRUE;
  long num=0;
  str_maiusc(vt);       //Converter letras para maiúsculas
  i=0;
  while (vt[i]=='0')  i++;  //Remover zeros à esquerda

  //Converter Hexa
  if (vt[i]=='X'){
    i++;
    while(vt[i] != '\0'){
      if (eh_hexa(vt[i]) == TRUE){
        num = (num<<4)+asc_nib(vt[i]);
        i++;
      }
      else{
        num=0;
        break;
      }
    }
  }

  //Converter Decimal
  else{
    if (vt[i]=='-'){  positivo=FALSE; i++; }
    if (vt[i]=='+'){  positivo=TRUE;  i++; }
    while(vt[i] != '\0'){
      if (eh_dec(vt[i]) == TRUE){
        num = 10*num+vt[i]-0x30;
        i++;
      }
      else{
        num=0;
        break;
      }
    }
    if (positivo==FALSE)  num = -1*num;
  }
  return num;
}

// Testar se um byte é uma Hexa válido
// TRUE  --> caso seja um hexa
// False --> caso não seja
char eh_hexa(char letra){
  if (letra<'0' || letra>'F') return FALSE;
  if (letra>'9' && letra<'A') return FALSE;
  return TRUE;
}

// Testar se um byte é uma Decimal válido
// TRUE  --> caso seja um Decimal
// False --> caso não seja
char eh_dec(char letra){
  if (letra>'9' || letra<'0') return FALSE;
  return TRUE;
}

// Converter ASCII em nibble
char asc_nib(char asc){
  if (asc<'A')  return asc-0x30;
  else          return asc-0x37;
}

// Converter letras para maiúsculas
void str_maiusc(char *vt){
  char i=0;
  while (vt[i] != '\0'){
    if (vt[i]>('a'-1) && vt[i]<('z'+1))  vt[i] &= ~BIT5;
    i++;
  }
}


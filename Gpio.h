// Puka - Gpio.h

#ifndef GPIO_H_
#define GPIO_H_

extern volatile char sw1_cont;     //0, 1, ..., SW_CONT_MAX
extern volatile char sw1_estado;   //ABERTA, TRANSITO, FECHADA
extern volatile char sw1;          //TRUE se chave acionada
extern volatile char sw2_cont;     //0, 1, ..., SW_CONT_MAX
extern volatile char sw2_estado;   //ABERTA, TRANSITO, FECHADA
extern volatile char sw2;          //TRUE se chave acionada

void sw_monitorar(void);
void led_VM(void);
void led_vm(void);
void led_Vm(void);
void led_VD(void);
void led_vd(void);
void led_Vd(void);
void SCP1(void);
void scp1(void);
void Scp1(void);
void SCP2(void);
void scp2(void);
void Scp2(void);
void SIM_RST(void);
void sim_rst(void);
void SIM_rst(void);
void SIM_DTR(void);
void sim_dtr(void);
void SIM_dtr(void);

void gpio_config(void);


#endif /* GPIO_H_ */

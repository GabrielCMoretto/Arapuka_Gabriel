// Puka - Mpu.h

#ifndef MPU_H_
#define MPU_H_

// MPU-6050 algumas constantes
#define MPU_ADR         0x69 //Endereço I2C do MPU
#define MPU_WHO         0x68 //Resposta ao Who am I
#define SMPLRT_DIV      0x19
#define CONFIG          0x1A
#define GYRO_CONFIG     0x1B
#define ACCEL_CONFIG    0x1C
#define ACCEL_XOUT_H    0x3B
#define PWR_MGMT_1      0x6B
#define WHO_AM_I        0x75 //Registrador Who am I

extern volatile char mpu_tem;      //TRUE se MPU6050 estiver presente
extern int  ax, ay, az, tp, gx, gy, gz;
extern volatile char toda_msg[70];
extern volatile char rtc_msg[18];
extern volatile char gps_msg[22];
extern volatile int amaiorx, amenorx, amaiory, amenory, amaiorz, amenorz;
extern volatile int maiorgx, menorgx, maiorgy, menorgy, maiorgz, menorgz;

char mpu_config(void);
void mpu_rd_vet(char reg, char *vt, char qtd);
char mpu_rd(char reg);
void mpu_wr(char reg, char dado);
void mpu_values();
void todos_dados(int mpu);
void mpu_8bits();
void repouso_values_mpu();
char acel_furto();
char giro_furto();

#endif /* MPU_H_ */

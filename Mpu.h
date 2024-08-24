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

char mpu_config(void);
void mpu_rd_vet(char reg, char *vt, char qtd);
char mpu_rd(char reg);
void mpu_wr(char reg, char dado);



#endif /* MPU_H_ */

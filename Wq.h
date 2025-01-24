// Puka - Wq.h

#ifndef WQ_H_
#define WQ_H_

#include <stdint.h>

// Constantes para a W25Q32
#define WQ_WEL_SET       0x06  //Fazer WEL=1 Write Enable
#define WQ_WEL_RST       0x04  //Fazer WEL=0 Write Disable
#define WQ_SR1_RD        0x05  //Ler Reg 1
#define WQ_SR2_RD        0x35  //Ler Reg 2
#define WQ_SR_WR         0x01  //Escrever SR1 e SR0 (Seq: D7-D0 e D8-D15)
#define WQ_DT_RD         0x03  //Ler posições da memória
#define WQ_PAGE_PROG     0x02  //A23-A0 e D7-D0
#define WQ_BLK_ERASE_64K 0xD8  //A23-A0
#define WQ_BLK_ERASE_32K 0x52  //A23-A0
#define WQ_SEC_ERASE_4K  0x20  //A23-A0
#define WQ_CHIP_ERASE    0x60  //ou 0xC7
#define WQ_CHIP_ERASE1   0xC7  //ou 0x60
#define WQ_ERASE_SUSP    0x75  //
#define WQ_ERASE_RESUME  0x7A  //
#define WQ_POWER_DWN     0xB9  //
#define WQ_MANUF_ID      0x90  //Manufacturer ID
#define WQ_UNIQUE_ID     0x4B  //Unique ID Number
#define WQ_JEDEC_ID      0x9f  //Código do fabricante

void wq_wr_blk(long adr, char *vet, int qtd);
void wq_rd_blk(long adr, char *vet, int qtd);
void wq_erase_chip(void);
void wq_erase_64k(long adr);
void wq_erase_4k(long adr);
char wq_ocupado(void);
void wq_wel(void);
void wq_WEL(void);
char wq_sr2_rd(void);
char wq_sr1_rd(void);
void w25_manuf_dev_id(char *vt);
void wq_cs(void);
void wq_CS(void);
char spi_transf(char x);
void spi_config(void);
void salvar_memoria();
char save_data(char dt, long adr);

flash_save(uint32_t adress, uint8_t* data, uint16_t size);

extern volatile char toda_msg[70];
extern volatile long wr_address_mem;
extern char estado;
#endif /* WQ_H_ */

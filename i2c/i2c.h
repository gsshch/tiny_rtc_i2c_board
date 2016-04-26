/*
 * i2c.h
 *
 * Created: 2016-04-11
 * Author: alex.rodzevski@gmail.com
 */ 


#ifndef I2C_H_
#define I2C_H_

void i2c_init(void);
void i2c_set_clk(unsigned long f_cpu, uint32_t frequency);
int i2c_rd_byte(uint8_t cli_addr, uint8_t reg_addr, uint8_t *dat);
int i2c_wr_byte(uint8_t cli_addr, uint8_t reg_addr, uint8_t dat);
int i2c_rd_blk(uint8_t cli_addr, uint8_t reg_addr, uint8_t *buf, uint8_t len);
int i2c_wr_blk(uint8_t cli_addr, uint8_t reg_addr, uint8_t *buf, uint8_t len);
int i2c_rd_idx16_blk(uint8_t cli_addr, uint16_t reg_addr,
                        uint8_t *buf, uint8_t len);
int i2c_wr_idx16_blk(uint8_t cli_addr, uint16_t reg_addr,
                        uint8_t *dat, uint8_t len);

#endif /* I2C_H_ */
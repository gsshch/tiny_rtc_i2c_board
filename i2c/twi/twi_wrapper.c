/*
 * twi_wrapper.c
 *
 * Description: This file contains the twi wrapper which in this case wraps the
 * Arduino twi-library to primarily work as an i2c master.
 *
 * Created: 2016-04-11
 * Author: alex.rodzevski@gmail.com
 */ 

#include <util/twi.h>
#include <string.h>
#include "twi.h"

#define USE_BUSY_WAIT   (uint8_t)1
#define SEND_STOP_BIT   (uint8_t)1

void i2c_init(unsigned long f_cpu)
{
        twi_init(f_cpu);
}

void i2c_set_clk(unsigned long f_cpu, uint32_t frequency)
{
        /* TODO: Add frequency limit check */
        TWBR = ((f_cpu / frequency) - 16) / 2;
}

int i2c_rd_byte(uint8_t cli_addr, uint8_t reg_addr, uint8_t *dat)
{
        uint8_t ret;

        ret = twi_writeTo(cli_addr, &reg_addr, 1, USE_BUSY_WAIT, SEND_STOP_BIT);
        if (ret != 0)
                return ret;
        ret = twi_readFrom(cli_addr, dat, 1, SEND_STOP_BIT);
        if (ret == 0)
                return -1;
        return 0;
}

int i2c_wr_byte(uint8_t cli_addr, uint8_t reg_addr, uint8_t dat)
{
        uint8_t buf[2];

        buf[0] = reg_addr;
        buf[1] = dat;
        return twi_writeTo(cli_addr, buf, 2, USE_BUSY_WAIT, SEND_STOP_BIT);
}

int i2c_rd_blk(uint8_t cli_addr, uint8_t reg_addr, uint8_t *buf, uint8_t len)
{
        uint8_t ret;

        if ((len - 1) >= TWI_BUFFER_LENGTH)
                return -1;

        ret = twi_writeTo(cli_addr, &reg_addr, 1, USE_BUSY_WAIT, SEND_STOP_BIT);
        if (ret != 0)
                return ret;
        ret = twi_readFrom(cli_addr, buf, len, SEND_STOP_BIT);
        if (ret == 0)
                return -1;
        return 0;
}

int i2c_wr_blk(uint8_t cli_addr, uint8_t reg_addr, uint8_t *dat, uint8_t len)
{
        uint8_t buf[TWI_BUFFER_LENGTH];

        if ((len - 1) >= TWI_BUFFER_LENGTH)
                return -1;

        buf[0] = reg_addr;
        memcpy(&buf[1], dat, len);
        return twi_writeTo(cli_addr, buf, (len + 1),
                        USE_BUSY_WAIT, SEND_STOP_BIT);
}

int i2c_rd_idx16_blk(uint8_t cli_addr, uint16_t reg_addr,
                                uint8_t *buf, uint8_t len)
{
        uint8_t ret;

        if ((len - 2) >= TWI_BUFFER_LENGTH)
                return -1;

        ret = twi_writeTo(cli_addr, (uint8_t *)&reg_addr, 2,
                                USE_BUSY_WAIT, SEND_STOP_BIT);
        if (ret != 0)
                return ret;
        ret = twi_readFrom(cli_addr, buf, len, SEND_STOP_BIT);
        if (ret == 0)
                return -1;
        return 0;
}

int i2c_wr_idx16_blk(uint8_t cli_addr, uint16_t reg_addr,
                                uint8_t *dat, uint8_t len)
{
        uint8_t buf[TWI_BUFFER_LENGTH];

        if ((len - 2) >= TWI_BUFFER_LENGTH)
                return -1;

        buf[0] = (uint8_t)(0x00FF & reg_addr);
        buf[1] = (uint8_t)((0xFF00 & reg_addr) >> 8);
        memcpy(&buf[2], dat, len);
        return twi_writeTo(cli_addr, buf, (len + 2),
                                USE_BUSY_WAIT, SEND_STOP_BIT);
}
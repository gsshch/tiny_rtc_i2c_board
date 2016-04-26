/*
 * rtc.c
 *
 * Created: 2016-04-19 12:55:02
 *  Author: alex.rodzevski
 */ 
#include <stdio.h>
#include <string.h>
#include "rtc.h"
#include "../i2c/i2c.h"
#include "../common.h"

/* DS1307 RTC register for start/stop time counter */
#define RTC_REG_START_TIME      (uint8_t)0x00

/* DS1307 RTC register address pointing at the internal RAM-buffer */
#define RTC_REG_RAM_BUF_START   (uint8_t)0x08

/* Memory block sizes, in bytes */
#define RTC_RAM_SIZE            (uint8_t)56


void rtc_init(void)
{
        uint8_t rtc_reg[RTC_RAM_SIZE];

        /* Clear the RTC RAM buffer */
        memset(rtc_reg, 0, RTC_RAM_SIZE);
        /* TODO: Clear the whole RTC RAM buffer with a block write */
        i2c_wr_addr_blk(DS1307, RTC_REG_RAM_BUF_START, rtc_reg, RTC_RAM_SIZE);

        /* TODO: Start the RTC clock by writing '0' to RTC_REG_START_TIME */
        i2c_wr_addr_byte(DS1307, RTC_REG_START_TIME, 0);
}

void rtc_get_time_var(struct rtc_time_var *var)
{
        uint8_t sec, min;
        uint8_t rtc_dat[2];

        /* TODO: Read out current RTC time from the register RTC_REG_START_TIME,
         * the register size is two bytes.
         * RTC_REG_RAM_BUF_START. The block size is defined by "len".
         */
        i2c_rd_addr_blk(DS1307, RTC_REG_START_TIME, rtc_dat, sizeof(rtc_dat));
        sec = rtc_dat[0];
        min = rtc_dat[1];

        var->sec_1 = (sec & 0x0F);
        var->sec_10 = ((sec & 0x70) >> 4);
        var->min_1 = (min & 0x0F);
        var->min_10 = ((min & 0x70) >> 4);
}

int rtc_get_ram_buf(uint8_t *buf, uint8_t len)
{
        if (len >= RTC_RAM_SIZE)
                return -1;
        /* TODO: Read out a block of data from RTC RAM starting from register
         * RTC_REG_RAM_BUF_START. The block size is defined by "len".
         */
        return i2c_rd_addr_blk(DS1307, RTC_REG_RAM_BUF_START, buf, len);
}

int rtc_set_ram_buf(uint8_t *buf, uint8_t len)
{
        if (len >= RTC_RAM_SIZE)
                return -1;
        /* TODO: Write a block of data to RTC RAM starting from register
         * RTC_REG_RAM_BUF_START. The block size is defined by "len".
         */
        return i2c_wr_addr_blk(DS1307, RTC_REG_RAM_BUF_START, buf, len);
}
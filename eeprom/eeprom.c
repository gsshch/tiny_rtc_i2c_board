/*
 * eeprom.c
 *
 * Created: 2016-04-19 12:55:43
 *  Author: alex.rodzevski
 */
#include <stdio.h>
#include "eeprom.h"
#include "../i2c/i2c.h"
#include "../common.h"

int eeprom_get_page(uint8_t page_index, uint8_t *dat)
{
        uint16_t reg_addr;

        if (page_index >= EEPROM_NBR_PAGES)
                return -1;

        reg_addr = (page_index * EEPROM_PAGE_SIZE);
        return i2c_rd_addr16_blk(AT24C32, reg_addr, dat, EEPROM_PAGE_SIZE);
}

int eeprom_set_page(uint8_t page_index, uint8_t *dat)
{
        uint16_t reg_addr;

        if (page_index >= EEPROM_NBR_PAGES)
                return -1;

        reg_addr = (page_index * EEPROM_PAGE_SIZE);
        return i2c_wr_addr16_blk(AT24C32, reg_addr, dat, EEPROM_PAGE_SIZE);
}

int eeprom_get_data(uint16_t reg_idx, uint8_t *buf, uint16_t len)
{
        if ((reg_idx + len) > EEPROM_TOTAL_SIZE)
                return -1;

        return i2c_rd_addr16_blk(AT24C32, reg_idx, buf, len);
}

int eeprom_set_data(uint16_t reg_idx, uint8_t *buf, uint16_t len)
{
        uint8_t page_overflow_len;
        uint8_t page_rest_len;
        uint8_t ret;

        /* Note! The length is limited to a page size only. If needed multi-page
         * handling this needs to be adjusted.
         */
        if (len > EEPROM_PAGE_SIZE || (reg_idx + len) > EEPROM_TOTAL_SIZE)
                return -1;

        /* Page boundary handling. When crossing the page boundary the new page
         * needs to be explicitly addressed, else current page will be over-
         * written.
         */
        if ((reg_idx % EEPROM_PAGE_SIZE) + len > EEPROM_PAGE_SIZE) {
                page_overflow_len = ((reg_idx % EEPROM_PAGE_SIZE) + len) -
                                                        EEPROM_PAGE_SIZE;
                page_rest_len = (len > page_overflow_len ?
                                        len - page_overflow_len :
                                                page_overflow_len - len);
                ret = i2c_wr_addr16_blk(AT24C32, reg_idx, buf, page_rest_len);
                if (ret)
                        return ret;
                _delay_ms(100);
                ret = i2c_wr_addr16_blk(AT24C32,
                                        (reg_idx + len) - page_overflow_len,
                                        &buf[page_rest_len], page_overflow_len);
        } else {
                ret = i2c_wr_addr16_blk(AT24C32, reg_idx, buf, len);
        }
        return ret;   
}
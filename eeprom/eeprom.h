/*
 * eeprom.h
 *
 * Created: 2016-04-19 12:55:28
 *  Author: alex.rodzevski
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#define EEPROM_TOTAL_SIZE       (uint16_t)4096  /* Bytes */
#define EEPROM_PAGE_SIZE        (uint8_t)32     /* Bytes */
#define EEPROM_NBR_PAGES        (uint8_t)128    /* 4096 / 32 */

int eeprom_get_page(uint8_t page_index, uint8_t *dat);
int eeprom_set_page(uint8_t page_index, uint8_t *dat);
int eeprom_get_data(uint16_t reg_idx, uint8_t *buf, uint16_t len);
int eeprom_set_data(uint16_t reg_idx, uint8_t *buf, uint16_t len);

#endif /* EEPROM_H_ */
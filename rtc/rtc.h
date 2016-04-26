/*
 * rtc.h
 *
 * Created: 2016-04-19 12:55:13
 *  Author: alex.rodzevski
 */ 


#ifndef RTC_H_
#define RTC_H_


/* RTC time variable struct */
struct rtc_time_var {
        uint8_t sec_10;
        uint8_t sec_1;
        uint8_t min_10;
        uint8_t min_1;
};

void rtc_init(void);
void rtc_get_time_var(struct rtc_time_var *var);
int rtc_get_ram_buf(uint8_t *buf, uint8_t len);
int rtc_set_ram_buf(uint8_t *buf, uint8_t len);

#endif /* RTC_H_ */
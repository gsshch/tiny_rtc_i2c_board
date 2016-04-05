# Atmel Studio firmware for the Tiny RTC board.
----

## Project brief
> The Tiny RTC board incorporates two I2C modules, one RTC IC (DS1307) and
> one EEPROM IC (AT24C32), which is ideal for learning how to setup a
> multi-client I2C bus communication. The main purpose of this repository is
> to provide an I2C reference solution running on Arduino HW implemented in
> Atmel Studio.

----
## TWI - I2C
> This firmware uses the C-implementation of the Arduino TWI-library (minor
> patch for the SDA/SCL pull-ups) which is then abstracted by an I2C-wrapper.
> The test and verifications has been done on an Arduino Mega which holds an
> ATmega2560 MCU. Due to trademark issues the Atmel MCUs names it's I2C function
> to TWI (Two Wire Interface) but the functions are more or less identical (and
> compatible with each other). More information about this can be found [here]
(http://www.i2c-bus.org/twi-bus).


----
## HW Info
> The are many variants of the board but, essentially, the ICs and the pin-outs
> are the same. A good description of the board's functions and accompanied
> connections can be found [here](http://www.hobbyist.co.nz/?q=real_time_clock).
> 
> 
> [RTC DS1307 datasheet](http://datasheets.maximintegrated.com/en/ds/DS1307.pdf)
> 
> 
> [EEPROM AT24C32 datasheet](http://www.atmel.com/images/doc0336.pdf)


A cheap place to buy the board from:


http://www.elecrow.com/sensor-c-111/misc-c-111_115/tiny-rtc-for-arduino-p-323.html

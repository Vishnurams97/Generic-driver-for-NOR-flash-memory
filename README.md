## Summary
This is basic Storage driver for Flash ICs (here I have used Winbond 8MByte NOR Flash) which can be very useful when porting File Systems Libraries(eg, FATS) to any microcontrollers implementing external storage.

#### This driver implements a FIFO based storing and fetching of application data.

#### It is developed with Nordic Semiconductor(nrf52 ARM Cortex M4) IC using Standard SPI peripheral.

#### These are some test results during development(from my personal views, may not be accurate!).

## Requirements
* 8MB/16MB/32MB NOR flash from Winbond.
* NRF52 series devboards/custom boards.
* nrf52 SDK

<code>Time Taken for(Standard SPI 8MHz Clock/8MB Storage),</code>
* whole chip read -> 3 secs approx
* whole chip program -> 9 secs approx
* Erase Sector -> 20ms approx
* Erase Block -> 300ms approx

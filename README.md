## This is basic Storage driver for Flash ICs (Here I have used Winbond 8MByte NOR Flash) which can be very useful when porting File Systems Libraries(eg, FATS) to any microcontrollers implementing external storage.

#### This driver actually implements a FIFO based storing and fetching of application data.

#### It is developed with Nordic Semiconductor(nrf52 ARM Cortex M4) IC using Standard SPI peripheral.

#### These are some test results during development(from my personal views, may not be accurate!).

<code>Time Taken for(Standard SPI 8MHz Clock/8MB Storage),
            whole chip read -> 3 secs approx
            whole chip program -> 11 secs approx
            Erase Sector -> 20ms approx
            Erase Block -> 300ms approx</code>

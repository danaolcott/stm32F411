# stm32F411
Projects using the STM32F411 Nucleo Board

It started out as a simple blinking LED project and turned into something more complex.  The Nucleo board has headers that allow shields made for the Arduino to be used.  I decided to combine an LCD shield from DF Robot and an SDCard shield from SeedStudio (v3.1).  The LCD shield is pretty cool and has a 128x64 single pixel display, a user button, 5 position joystick, and an LED that is routed to the power supply.  The LCD is stacked on top of the SD Card shield.  Both of them use the same SPI interface pins and the 3.3v supply was not routed up through the SDCard shield.  To get them to all work together, I had to make a few modifications to each of the sheilds.

Modifications to the LCD shield:
- cut the header pin from the shield button and solder jumper wire to ### to allow the button on the shield to be used for something other than the reset button.
- solder bridge accross ### so I could use analog pin ### instead of the one supplied.  The reason for this is that I was using I2c for EEPROM memory chip and the I2C conflicts with the analog pin used for the user joystick (see Games repo for more information).
- solder jumper wire from the SPI cs pin (D10 - arduino, PB6 - stm32 lables) to D6 (PB10) and cut the pin on the header so the D10 signal does not make it to the board (see the next section about using the sdcard shield).  This allows the LCD to be stacked on and SDCard shield.
- Remove the current limiting resistor on the power LED, rotate, resolder, and solder jumper to pin ####.  This allows the power led to used as a general debug LED.  Note:  The Nucleo board has only 1 usable led (connected to Arduino Pin D13), which commonly shared with the SPI peripheral on many shields made for the Arduino, which makes it pretty useless.

Modifications to the SD Card Shield:
- Remove R1-R6 and make solder bridges across R1, R2, R4.  These were set up as voltage dividers so the SD Card signals would be 3.3v insted of 5v.  These were no longer needed since digital output from the Nucleo board is 3.3v.
- Install jumper wire from 3.3v output of the regulator to the 3.3v pin output on the top of the shield.  For one reason or another, the 3.3v output pin on the top of the shield does not carry through for other shields to be stacked on top.

STM32 Standard Libraries:
The projects in this repo all use the Standard Libraries from STM.  I realize these might be considered outdated since STM has the HAL libs, the STM Cube Tool for generated code, etc.  I decided to use the Standard Libraries after trying to get an SDCard to work using the HAL libs in SPI mode.  After many hours of failure, a simple check with the scope monitoring the CS and SCK lines revealed the massive amount of time wasted between the time the CS line went low and the start of the first clock cycle.  I'm not exactly sure this was the source of it not working, but after switching to a more stripped down approach (Standard Libs), I was able to get the sdcard to work.

Projects:
These are all a work in progress, often a collection of items from other projects with no particular direction in mind.

Images:







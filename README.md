# stm32F411
Projects using the STM32F411 Nucleo Board

It started out as a simple blinking LED project and turned into something more complex.  The Nucleo board has headers that allow shields made for the Arduino to be used.  I decided to combine an LCD shield from DF Robot and an SDCard shield from SeedStudio (v3.1).  The LCD shield is pretty cool and has a 128x64 single pixel display, a user button, 5 position joystick, and an LED that is routed to the power supply.  The LCD is stacked on top of the SD Card shield.  Both of them use the same SPI interface pins and the 3.3v supply was not routed up through the SDCard shield.  To get them to all work together, I had to make a few modifications to each of the sheilds.

Modifications to the LCD shield:
- cut the header pin that connects to the reset signal.  Solder jumper wire from the cut header pin (shield button) to D2 (PA10).  This allows the button on the shield to be used as user button rather than a reset button.
- make a solder bridge between pins A0 and A1 (see top of the lcd shield).  Cut the header pin on A0 to prevent the signal from passing into the nucleo board.  The joystick should be configured to use A1 (PA1 on nucleo) as analog input.  Note:  This modification is not needed.  I did this when using the shield on the Atmel SAM70 development board and there was a pin conflict.
- solder jumper wire from the SPI cs pin (D10 - arduino, PB6 - stm32 label) to D6 (PB10) and cut the pin on the header at D10.  Configure the CS pin to work on D6 (PB10).  This allows the LCD shield and the SDCard shield to use the same SPI but different CS pins.
- Remove the current limiting resistor on the power LED, rotate, resolder, and solder jumper to pin A3 (PB0 on the nucleo label).  This allows the power led to used as a general debug LED.

Modifications to the SD Card Shield:
- Remove R1-R6 and make solder bridges across R1, R2, R4.  These are no longer needed since digital output signals from the Nucleo board are 3.3v.
- Install jumper wire from 3.3v output of the regulator to the 3.3v pin output on the top of the shield.  Note:  The 3.3v output pin on the top of the shield does not carry through for other shields to be stacked on top.  Without the 3.3v signal, the LCD will not work.

Other Hardware Added:
- This board was also used in the Games repo with added hardware for sound and i2c EEPROM data storage for high score, number of times the game played, etc.  The DAC was made using 5 resistors connected to PB1, PB2, PB13-PB15.  The EEPROM chip was connected to PB8 and PB9.

- 
STM32 Standard Libraries:
- The projects in this repo all use the Standard Libraries from STM.  I realize these might be considered outdated since STM has the HAL libs, the STM Cube Tool for generated code, etc.  I decided to use the Standard Libraries after trying to get an SDCard to work using the HAL libs in SPI mode.  After many hours of failure, a simple check with the scope monitoring the CS and SCK lines revealed the massive amount of time wasted between the time the CS line went low and the start of the first clock cycle.  I'm not exactly sure this was the source of it not working, but after switching to a more stripped down approach (Standard Libs), I was able to get the sdcard to work.

Projects:
These are all a work in progress, often a collection of items from other projects with no particular direction in mind.

Images:







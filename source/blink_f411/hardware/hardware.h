/*
 * hardware.h
 *
 *  Created on: Jan 27, 2024
 *      Author: henry
 *
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

///////////////////////////////////////////
//buttons
#define userButton_Pin GPIO_Pin_13
#define userButton_GPIO_Port GPIOC
#define userButton_EXTI_IRQn EXTI15_10_IRQn

#define shieldButton_Pin GPIO_Pin_10
#define shieldButton_GPIO_Port GPIOA
#define shieldButton_EXTI_IRQn EXTI15_10_IRQn

//leds - note: green led on the board is shared
//with the SPI interface
//routed the shield led to PB0 (A3 on arduino header)
#define shieldLed_Pin GPIO_Pin_0
#define shieldLed_GPIO_Port GPIOB

////////////////////////////////////////////
//adc - joystick - NOTE: use PA1 when using the
//stm32f411 Nucleo board
#define ADC_A0_Pin GPIO_Pin_0
#define ADC_A0_GPIO_Port GPIOA
#define ADC_A1_Pin GPIO_Pin_1
#define ADC_A1_GPIO_Port GPIOA

////////////////////////////////////////////
//uart2 - PA2 and PA3 - connected to usb
//to serial pass through on stlink
#define USART2_Tx_Pin GPIO_Pin_2
#define USART2_Rx_Pin GPIO_Pin_3
#define USART2_GPIO_Port GPIOA

////////////////////////////////////////////
//uart1
//PA15 and PB7 - uart1 tx / rx
#define USART1_Tx_Pin GPIO_Pin_15
#define USART1_Rx_Pin GPIO_Pin_7
#define USART1_GPIO_TxPort GPIOA
#define USART1_GPIO_RxPort GPIOB



////////////////////////////////////////////
//lcd controls
#define LCD_CMD_Pin GPIO_Pin_7
#define LCD_CMD_GPIO_Port GPIOC
#define LCD_Backlight_Pin GPIO_Pin_8
#define LCD_Backlight_GPIO_Port GPIOA
#define LCD_Reset_Pin GPIO_Pin_9
#define LCD_Reset_GPIO_Port GPIOA

//spi interface for lcd and SDCard
#define SPI1_CS_LCD_Pin GPIO_Pin_10
#define SPI1_CS_GPIO_Port GPIOB

#define SPI1_CS_SDCARD_Pin GPIO_Pin_6
#define SPI1_CS_GPIO_Port GPIOB

#define SPI1_SCK_Pin GPIO_Pin_5
#define SPI1_MISO_Pin GPIO_Pin_6
#define SPI1_MOSI_Pin GPIO_Pin_7
#define SPI1_GPIO_Port GPIOA


//i2c for memory chip
#define I2C1_SCL_Pin GPIO_Pin_8
#define I2C1_SDA_Pin GPIO_Pin_9
#define I2C1_GPIO_Port GPIOB


///////////////////////////////////////////
//DAC Pins - these are for updated sound board
//that uses 8 bits, aligned bottom to top
//
//port masks:
//portB mask - clears the dac bits on portB
//  0000 1111 1111 1001

//read port B
// & the mask
//bit 0 - (value & 0x01) << 13
//bit 1 - ((value >> 1) & 0x01) << 14
//bit 2 - ((value >> 2) & 0x01) << 15
//bit 3 - ((value >> 3) & 0x01) << 1
//bit 4 - ((value >> 4) & 0x01) << 2
//bit 5 - ((value >> 5) & 0x01) << 12

#define DAC_Bit0_Pin GPIO_Pin_13
#define DAC_Bit0_GPIO_Port GPIOB

#define DAC_Bit1_Pin GPIO_Pin_14
#define DAC_Bit1_GPIO_Port GPIOB

#define DAC_Bit2_Pin GPIO_Pin_15
#define DAC_Bit2_GPIO_Port GPIOB

#define DAC_Bit3_Pin GPIO_Pin_1
#define DAC_Bit3_GPIO_Port GPIOB

#define DAC_Bit4_Pin GPIO_Pin_2
#define DAC_Bit4_GPIO_Port GPIOB

#define DAC_Bit5_Pin GPIO_Pin_12
#define DAC_Bit5_GPIO_Port GPIOB

#define DAC_Bit6_Pin GPIO_Pin_11
#define DAC_Bit6_GPIO_Port GPIOA

#define DAC_Bit7_Pin GPIO_Pin_12
#define DAC_Bit7_GPIO_Port GPIOA

//////////////////////////////////////
//port A mask:
//clears the dac bits on port A
//  1110 0111 1111 1111

//read reg port A
// & the mask to it to clear the dac bits
//add bits 6 and 7

//bit 6 = ((value >> 6) & 0x01) << 11
//bit 7 = ((value >> 7) & 0x01) << 12



//////////////////////////////////////////////
//OLD BOARD
//dac pins - 5 bit dac for sound
//add 2 more dac pins for 7 bit dac
//PB12 - add 10k for bit -1
//PA11 - add 20k for bit -2
//adjust all the bit names up by 2
//ie, make pb13 bit 2, make pb14 bit 3, etc.

//#define DAC_Bit5_Pin GPIO_Pin_1
//#define DAC_Bit5_GPIO_Port GPIOB
//#define DAC_Bit6_Pin GPIO_Pin_2
//#define DAC_Bit6_GPIO_Port GPIOB
//#define DAC_Bit2_Pin GPIO_Pin_13
//#define DAC_Bit2_GPIO_Port GPIOB
//#define DAC_Bit3_Pin GPIO_Pin_14
//#define DAC_Bit3_GPIO_Port GPIOB
//#define DAC_Bit4_Pin GPIO_Pin_15
//#define DAC_Bit4_GPIO_Port GPIOB

//#define DAC_Bit0_Pin GPIO_Pin_11
//#define DAC_Bit0_GPIO_Port GPIOA
//#define DAC_Bit1_Pin GPIO_Pin_12
//#define DAC_Bit1_GPIO_Port GPIOB
/////////////////////////////////////////////

//////////////////////////////////////////////
//I dont think these are used for anything yet
//but they would map to a board layer under the
//nucleo board
//
//not sure what these are for - pwm3 / ch3 and ch4
#define GPIO_PC8_Pin GPIO_Pin_8
#define GPIO_PC8_GPIO_Port GPIOC
#define GPIO_PC9_Pin GPIO_Pin_9
#define GPIO_PC9_GPIO_Port GPIOC

//not sure what these are for
//align with digital pins, or also
//mapped to SPI3 - data IO and clock lines
//wonder if there's sd card here?
#define GPIO_D3_Pin GPIO_Pin_3
#define GPIO_D3_GPIO_Port GPIOB
#define GPIO_D5_Pin GPIO_Pin_4
#define GPIO_D5_GPIO_Port GPIOB
#define GPIO_D4_Pin GPIO_Pin_5
#define GPIO_D4_GPIO_Port GPIOB


//note:  pc10, pc11, pc12 - SPI3 data and clock lines - outside connector
//PD2 would be a good CS Line for SPI 3

//PC8 and PC9 - outside connector - mapped to PWM3, channels 3 (PC8) and 4 (PC9)

//PA15 and PB7 - uart1 tx / rx
//PA13 and PA14 - nothing....  no peripherals listed.

//pa11 and pb12 - add 2 more resistors for 2 more dac bits
//10k

//for the new board - add PA12 for 1 more dac bit for total of 8
//



#endif /* HARDWARE_H_ */

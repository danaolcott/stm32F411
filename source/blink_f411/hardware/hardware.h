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

////////////////////////////////////////////
//adc - joystick
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
//lcd controls
#define LCD_CMD_Pin GPIO_Pin_7
#define LCD_CMD_GPIO_Port GPIOC
#define LCD_Backlight_Pin GPIO_Pin_8
#define LCD_Backlight_GPIO_Port GPIOA
#define LCD_Reset_Pin GPIO_Pin_9
#define LCD_Reset_GPIO_Port GPIOA

//spi interface for lcd
#define SPI1_CS_Pin GPIO_Pin_6
#define SPI1_CS_GPIO_Port GPIOB

#define SPI1_SCK_Pin GPIO_Pin_5
#define SPI1_MISO_Pin GPIO_Pin_6
#define SPI1_MOSI_Pin GPIO_Pin_7
#define SPI1_GPIO_Port GPIOA

//i2c for memory chip
#define I2C1_SCL_Pin GPIO_Pin_8
#define I2C1_SDA_Pin GPIO_Pin_9
#define I2C1_GPIO_Port GPIOB

//dac pins - 5 bit dac for sound
#define DAC_Bit3_Pin GPIO_Pin_1
#define DAC_Bit3_GPIO_Port GPIOB
#define DAC_Bit4_Pin GPIO_Pin_2
#define DAC_Bit4_GPIO_Port GPIOB
#define DAC_Bit0_Pin GPIO_Pin_13
#define DAC_Bit0_GPIO_Port GPIOB
#define DAC_Bit1_Pin GPIO_Pin_14
#define DAC_Bit1_GPIO_Port GPIOB
#define DAC_Bit2_Pin GPIO_Pin_15
#define DAC_Bit2_GPIO_Port GPIOB



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




#endif /* HARDWARE_H_ */

//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Gabby Williams
//
//       LAB NAME:  Lab 8: Analog to Digital Converter (part1)
//
//      FILE NAME:  lab8_p1main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code will show the ADC values of channel 7 on the second row of LCD
//    . On the first row of the LCD, "Status: Dark" or "Status: Light" is 
//    displayed based on whether the value is above or below a specific 
//    threshold value, in this case it's 500. To stop the program, press PB1
//    and it would display "Program Stopped" on the first row of the LCD.
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include "LaunchPad.h"
#include "clock.h"
#include <ti/devices/msp/msp.h>
#include "lcd1602.h"
#include "adc.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab8_part1(void);
void config_pb1_interrupts(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define ADC_THRESHOLD   (500)

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool g_pb1_pressed = false;
bool g_pb2_pressed = false;

// Define a structure to hold different data types

int main(void)
{
  // Configure the LaunchPad board
  clock_init_40mhz();
  launchpad_gpio_init();
  dipsw_init();
  I2C_mstr_init();
  lcd1602_init();
  leds_init();
  lcd_clear();
  OPA0_init(7);
  OPA0_enable();
  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);
  config_pb1_interrupts();

  // enter your code here
  run_lab8_part1();
 

  // Endless loop to prevent program from ending
  while (1)
    ;

} /* main */

void config_pb1_interrupts(void)
{ 
  GPIOB->POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;
  GPIOB->CPU_INT.ICLR  = GPIO_CPU_INT_ICLR_DIO18_CLR;
  GPIOB->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;

  NVIC_SetPriority(GPIOB_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOB_INT_IRQn);
}//config_pb1_interrupt

void GROUP1_IRQHandler(void)
{
  uint32_t group_gpio_iidx;
  uint32_t gpio_mis;

  do 
  {
    group_gpio_iidx = CPUSS->INT_GROUP[1].IIDX;
    switch (group_gpio_iidx)
    {
      case (CPUSS_INT_GROUP_IIDX_STAT_INT0): //GPIOA
        gpio_mis = GPIOA->CPU_INT.MIS;
        if((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) == GPIO_CPU_INT_MIS_DIO15_SET)
        {
        g_pb2_pressed = true;
        GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
        }

        break;
      case (CPUSS_INT_GROUP_IIDX_STAT_INT1): //GPIOB
        gpio_mis = GPIOB->CPU_INT.MIS;
        if((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) == GPIO_CPU_INT_MIS_DIO18_SET)
        {
          g_pb1_pressed = true;
          GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
        }
        break;
    }


  }while (group_gpio_iidx != 0);
}

void run_lab8_part1(void)
{
  uint16_t adc_value = 0;
  bool done = false;
  char *dark_msg = "Status: Dark    ";
  char *light_msg = "Status: Light   ";

  while (!done)
  {
    adc_value = ADC0_in(7);

    lcd_set_ddram_addr(LCD_LINE1_ADDR);

    if (adc_value < ADC_THRESHOLD)
    {
      lcd_write_string(dark_msg);
    }
    else
    {
      lcd_write_string(light_msg);
    }

    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("ADC = ");
    lcd_write_doublebyte(adc_value);
    msec_delay(200);

    if (g_pb1_pressed) 
    {
      done = true;
    }

  }

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Program Stopped");
 

}


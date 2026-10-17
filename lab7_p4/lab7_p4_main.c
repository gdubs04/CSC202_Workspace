//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Gabby Williams
//
//       LAB NAME:  lab7: part4
//
//      FILE NAME:  lab7_p4_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code ... *** COMPLETE THIS BASED ON LAB REQUIREMENTS ***
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include "LaunchPad.h"
#include "clock.h"
#include "lcd1602.h"
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab7_part4(void);
void config_pb1_interrupts(void);
void config_pb2_interrupts(void);
void GROUP1_IRQHandler(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
volatile bool pb1_pressed = false;
volatile bool pb2_pressed = false;

// Define a structure to hold different data types

int main(void)
{
  // Configure the LaunchPad board
  clock_init_40mhz();
  launchpad_gpio_init();
  I2C_mstr_init();
  lcd1602_init();
  lcd_clear();
  leds_init();
  leds_disable();
  seg7_init();
  seg7_off();

  // enter your code here
  config_pb1_interrupts();
  config_pb2_interrupts();

  __enable_irq();
  run_lab7_part4();
  __disable_irq();
  leds_disable();
  seg7_off();

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Program Stopped");

  // Endless loop to prevent program from ending
  while (1)
    ;

} /* main */

void config_pb1_interrupts(void)
{
  GPIOB->CPU_INT.ICLR  = GPIO_CPU_INT_ICLR_DIO18_CLR;
  GPIOB->POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;
  GPIOB->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;

  NVIC_SetPriority(GPIOB_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOB_INT_IRQn);
}

void config_pb2_interrupts(void)
{
  GPIOA->CPU_INT.ICLR  = GPIO_CPU_INT_ICLR_DIO15_CLR;
  GPIOA->POLARITY15_0  = GPIO_POLARITY15_0_DIO15_RISE;
  GPIOA->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;

  NVIC_SetPriority(GPIOA_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOA_INT_IRQn);
}

void GROUP1_IRQHandler(void)
{
  uint32_t status;
  uint32_t gpio_mis;

  do
  {
    status = CPUSS->INT_GROUP[1].IIDX;

    switch (status)
    {
      case CPUSS_INT_GROUP_IIDX_STAT_INT0:
        gpio_mis = GPIOA->CPU_INT.MIS;

        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) != 0)
        {
          pb2_pressed         = true;
          GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
        }
        break;

      case CPUSS_INT_GROUP_IIDX_STAT_INT1:
        gpio_mis = GPIOB->CPU_INT.MIS;

        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) != 0)
        {
          pb1_pressed         = true;
          GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
        }
        break;

      default:
        break;
    }

  } while (status != 0);
}

void run_lab7_part4(void)
{
  uint8_t cntr       = 0;
  bool    done       = false;
  bool    display_on = false;
  char    message[3];

  while (!done)
  {

    if (pb1_pressed)
    {
      pb1_pressed = false;

      msec_delay(20);

      done = true;
    }

    if (pb2_pressed)
    {
      pb2_pressed = false;

      msec_delay(20);

      display_on = !display_on;

      lcd_set_ddram_addr(LCD_LINE2_ADDR);
      lcd_write_string("                ");

      if (display_on)
      {
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        lcd_write_string("PB2 PRESSED");
      }
    }

    if (!done)
    {

      sprintf(message, "%02d", cntr);
      lcd_set_ddram_addr(LCD_LINE1_ADDR + 7);
      lcd_write_string(message);

      leds_on(cntr);

      msec_delay(200);

      cntr++;

      if (cntr > 99)
      {
        cntr = 0;
      }
    }
  }
}

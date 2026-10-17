//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Gabby Williams
//
//       LAB NAME:  Lab 7: Part 3
//
//      FILE NAME:  lab7_p3_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code will have a counter incrementing 0-99 displayed on the LCD while
//    an ISR counts the number of dip switches in the ON position where the
//    number on DIG0 seg7 displays 0-4.
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
void run_lab7_part3(void);
void SysTick_Handler(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define MSPM0_CLOCK_FREQUENCY (40E6)
#define SYST_TICK_PERIOD (10.24E-3)
#define SYST_TICK_PERIOD_COUNT (SYST_TICK_PERIOD * MSPM0_CLOCK_FREQUENCY)

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

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
  leds_disable();
  seg7_init();
  seg7_off();
  lcd_clear();
  sys_tick_init(SYST_TICK_PERIOD_COUNT);

  // enter your code here
  run_lab7_part3();

  __disable_irq();
  sys_tick_disable();
  leds_disable();
  seg7_off();
  dipsw_deinit();
  lcd_clear();

  // Endless loop to prevent program from ending
  while (1)
    ;

} /* main */

void SysTick_Handler(void)
{
  uint8_t switches;
  uint8_t mask;
  uint8_t count = 0;

  switches = dipsw_read();

  for (mask = 0x01; mask <= 0x08; mask <<= 1)
  {
    if ((switches & mask) != 0)
    {
      count++;
    }
  }

  seg7_hex(count, SEG7_DIG0_ENABLE_IDX);
}

void run_lab7_part3(void)
{
  uint8_t counter = 0;
  bool    done    = false; // flag
  char    message[3];

  while (!done)
  {
    sprintf(message, "%02d", counter);

    lcd_set_ddram_addr(LCD_LINE1_ADDR + 7);
    lcd_write_string(message);

    msec_delay(200);

    counter++;

    if (counter > 99)
    {
      counter = 0;
    }
  }
}

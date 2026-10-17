//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  TBD
//
//       LAB NAME:  TBD
//
//      FILE NAME:  main.c
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
#include <stdlib.h>
#include <stdbool.h>


//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "LaunchPad.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void sys_tick_init(uint32_t period);
void half_sec_delay(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
volatile uint32_t g_current_tick_count = 0;

// Define a structure to hold different data types

int main(void)
{
  // Configure the LaunchPad board
  clock_init_40mhz();
  launchpad_gpio_init();
  seg7_init();
 
  // enter your code here
sys_tick_init(409600);
 
 // Endless loop to prevent program from ending
 while (1)
 {
  seg7_hex(0x9, SEG7_DIG1_ENABLE_IDX);
  half_sec_delay();
  seg7_off();
  half_sec_delay();
 }
 return 0;

} /* main */

void half_sec_delay(void)
{
  uint32_t start_ticks = g_current_tick_count;

  while (g_current_tick_count - start_ticks <  49)
    ;
}

void SysTick_Handler(void)
{
  g_current_tick_count++;
}

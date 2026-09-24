//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Gabby Williams
//
//       LAB NAME:  Lab 4: Interfacing to Output Device
//
//      FILE NAME:  lab4_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//    Learn how to control LEDS and a seven-segment display.
//    Configure and manipulate the IOMUX and GPIO modules.
//    Write and debug code to effectively control hardware components.
//    This code ... *** COMPLETE THIS BASED ON LAB REQUIREMENTS ***
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

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab4_part1(void);
void run_lab4_part2(void);
void run_lab4_part3(void);
void run_lab4_part4(void);
void run_lab4_part5(void);
void run_lab4_part6(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

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
  leds_init();
  leds_enable();

  // enter your code here
  run_lab4_part1();
  msec_delay(1000);

  run_lab4_part2();
  msec_delay(500);

  run_lab4_part3();

  leds_disable();
  seg7_init();
  run_lab4_part4();
  msec_delay(500);

  run_lab4_part5();
  msec_delay(500);

  run_lab4_part6();

  // Endless loop to prevent program from ending
  while (1)
    ;

} /* main */

void run_lab4_part1(void)
{
  // enter your code here
  led_on(LED_BAR_LD2_IDX);
  led_on(LED_BAR_LD3_IDX);
  led_on(LED_BAR_LD4_IDX);
  led_on(LED_BAR_LD5_IDX);

  msec_delay(1000);
  led_off(LED_BAR_LD2_IDX);
  led_off(LED_BAR_LD3_IDX);
  led_off(LED_BAR_LD4_IDX);
  led_off(LED_BAR_LD5_IDX);
}

void run_lab4_part2(void)
{
  uint8_t loop_cntr = 0;
  uint8_t led_idx   = 0;

  while (loop_cntr < 5)
  {
    for (led_idx = LED_BAR_LD0_IDX; led_idx <= LED_BAR_LD7_IDX; led_idx++)
    {
      led_on(led_idx);
      msec_delay(200);
      led_off(led_idx);
    }

    for (led_idx = LED_BAR_LD6_IDX; led_idx > LED_BAR_LD0_IDX; led_idx--)
    {
      led_on(led_idx);
      msec_delay(200);
      led_off(led_idx);
    }
    loop_cntr = loop_cntr + 1;
  } // while
  led_on(LED_BAR_LD0_IDX);
  msec_delay(200);
  led_off(LED_BAR_LD0_IDX);
}

void run_lab4_part3(void)
{
  uint8_t loop_cntr = 0;
  uint8_t count     = 0;

  while (loop_cntr < 2)
  {

    leds_on(count);

    if (loop_cntr == 0)
    {
      msec_delay(100);
    }
    else
    {
      msec_delay(50);
    }
    if (count == 255)
    {
      loop_cntr++;
      msec_delay(500);
    }
    count++;
  }

  msec_delay(500);

  leds_off();
}

void run_lab4_part4(void)
{
  seg7_on(0x38, SEG7_DIG0_ENABLE_IDX);
  msec_delay(1000);

  seg7_off();
  msec_delay(500);
}

void run_lab4_part5(void)
{
  uint8_t loop_cntr = 0;

  while (loop_cntr < 4)
  {
    seg7_on(0x66, SEG7_DIG2_ENABLE_IDX);
    msec_delay(3000);
    seg7_off();
    msec_delay(2000);

    loop_cntr++;
  }

  seg7_off();
}

void run_lab4_part6(void)
{
  uint8_t loop_cntr = 0;

  while (loop_cntr < 200)
  {

    seg7_hex(0xC, SEG7_DIG0_ENABLE_IDX);

    msec_delay(6);
    seg7_hex(0xA, SEG7_DIG1_ENABLE_IDX);

    msec_delay(6);

    seg7_hex(0xF, SEG7_DIG2_ENABLE_IDX);

    msec_delay(6);

    seg7_hex(0xE, SEG7_DIG3_ENABLE_IDX);

    msec_delay(6);
    loop_cntr++;
  }
}
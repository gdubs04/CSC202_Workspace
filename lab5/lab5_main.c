//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Gabby Williams
//
//       LAB NAME:  Lab 5: Interfacing to Input Device
//
//      FILE NAME:  lab5_main.c
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
void run_lab5_part1(void);
void run_lab5_part2(void);
void run_lab5_part3(void);
void run_lab5_part4(void);

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
  seg7_init();
  dipsw_init();
  lpsw_init();
  keypad_init();

  // enter your code here

  run_lab5_part1();
  msec_delay(500);

  run_lab5_part2();
  msec_delay(500);
  leds_off();
  leds_enable();

  run_lab5_part3();
  msec_delay(500);
  leds_off();
  leds_enable();

  run_lab5_part4();




  // Endless loop to prevent program from ending
  while (1)
    ;
}
/* main */

void run_lab5_part1(void)
{
  bool    display_is_on = false;
  uint8_t press_cnt     = 0;

  seg7_off();

  while (press_cnt < 3)
  {
    if (is_pb_down(PB1_IDX))
    {
      if (display_is_on)
      {
        seg7_off();
        display_is_on = false;
        press_cnt++;
      }

      else
      {
        seg7_hex(0x03, SEG7_DIG0_ENABLE_IDX);
        display_is_on = true;
        
      }

      msec_delay(5);
      while (is_pb_down(PB1_IDX))
      {
      }
      msec_delay(5);
    }
  }

  leds_off();
  seg7_off();
}

void run_lab5_part2(void)
{
  uint8_t low_nibble  = 0;
  uint8_t high_nibble = 0;
  uint8_t seg7_data   = 0;
  uint8_t loop_cnt    = 0;

  typedef enum
  {
    GET_LOW = 0,
    GET_HIGH,
    DISPLAY,
  } fsm_state_t;

  fsm_state_t state = GET_LOW;

  while (loop_cnt < 3)
  {
    switch (state)
    {
      case GET_LOW:

        low_nibble = dipsw_read();

        if (is_lpsw_down(LP_SW2_IDX))
        {
          msec_delay(10);

          while (is_lpsw_down(LP_SW2_IDX))
          {}
          msec_delay(10);
          
            
          state = GET_HIGH;
        }

        break;

      case GET_HIGH:

        high_nibble = dipsw_read();

        if (is_lpsw_down(LP_SW2_IDX))
        {
          msec_delay(10);

          while (is_lpsw_down(LP_SW2_IDX))
          {}
          msec_delay(10);

          seg7_data = ((high_nibble & 0x0F) << 4) | (low_nibble & 0x0F);

          state = DISPLAY;
        }
        break;

      case DISPLAY:
        if (is_pb_down(PB1_IDX))
        {
          seg7_on(seg7_data, SEG7_DIG2_ENABLE_IDX);
        }
        else
        {
          seg7_on(seg7_data, SEG7_DIG0_ENABLE_IDX);
        }

        if (is_lpsw_down(LP_SW2_IDX))
        {
          msec_delay(10);

          while (is_lpsw_down(LP_SW2_IDX))
          {}
          msec_delay(10);

          seg7_off();
          loop_cnt++;
          state = GET_LOW;
        }
        break;

      default:
        state = GET_LOW;
        break;
    }
  }

  
  seg7_off();
}

void run_lab5_part3 (void)
{
  uint8_t key_press;
  uint8_t loop_cnt = 0;
  

  while(loop_cnt < 8)
  {
    key_press = getkey_pressed();
    leds_on(key_press);
    msec_delay(5);
    wait_no_key_pressed();
    msec_delay(5);

    loop_cnt++;

  }
  
  leds_off();

}

void run_lab5_part4(void)
{
  uint8_t loop_cnt = 0;
  uint8_t key_press;
  uint8_t flash_cnt = 0;

  while (loop_cnt < 4)
  {
    key_press = keypad_scan();

    if (key_press != NO_KEY_PRESSED)
    {
      for (flash_cnt = 0; flash_cnt < key_press; flash_cnt++)
      {
        leds_on(0xFF);
        msec_delay(500);

        leds_off();
        msec_delay(500);
      }
      msec_delay(5);
      wait_no_key_pressed();
      msec_delay(5);
    }
  }
  leds_off();
}

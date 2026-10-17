//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Gabby Williams
//
//       LAB NAME:  Lab6: Interfacing to Liquid Crystal Display (LCD)
//
//      FILE NAME:  lab6_main.c
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
#include "lcd1602.h"



//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab6_part1A(void);
void wait_for_pb_pressed(uint8_t pb_idx);
void wait_for_pb_released(uint8_t pb_idx);
void run_lab6_part2(void);
void run_lab6_part3(void);
void run_lab6_part4(void);




//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define DELAY_DEBOUNCE (20)
#define DEBOUNCE_DELAY (1000)
#define TOTAL_CHAR_LCD (32)
#define CHAR_PER_LCD (16)

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
  I2C_mstr_init();
  lcd1602_init();
  dipsw_init();

  // enter your code here
  run_lab6_part1A();
  msec_delay(DEBOUNCE_DELAY);

  wait_for_pb_pressed(PB2_IDX);
  lcd_clear();
  msec_delay(DEBOUNCE_DELAY);
  wait_for_pb_released(PB2_IDX);
  run_lab6_part2();

  
  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string("Press PB 2");

  wait_for_pb_pressed(PB2_IDX);
  msec_delay(DEBOUNCE_DELAY);
  lcd_clear();
  wait_for_pb_released(PB2_IDX);

  lcd_write_string("Running Part 3");
  msec_delay(DEBOUNCE_DELAY);
  lcd_clear();
  
  run_lab6_part3();

  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string("Press PB 2");

  wait_for_pb_pressed(PB2_IDX);
  msec_delay(DEBOUNCE_DELAY);
  lcd_clear();
  wait_for_pb_released(PB2_IDX);

  lcd_write_string("Running Part 4");
  msec_delay(DEBOUNCE_DELAY);
  keypad_init();

  run_lab6_part4();

  // Endless loop to prevent program from ending
  while (1)
    ;

} /* main */


void wait_for_pb_pressed(uint8_t pb_idx)
{
    while (is_pb_up(pb_idx))
    {

    }

    msec_delay(DELAY_DEBOUNCE);
}

void wait_for_pb_released(uint8_t pb_idx)
{
    while (is_pb_down(pb_idx))
    {
      
    }

    msec_delay(DELAY_DEBOUNCE);
}


void run_lab6_part1A(void)
{
  uint8_t letter = 0;

  for (letter = 'A'; letter <= 'Z'; letter++)
  {
    lcd_write_char(letter);

    if (letter == 'P')
    {
      lcd_set_ddram_addr(LCD_LINE2_ADDR);
    }
  }
}

void run_lab6_part2(void)
{
  lcd_write_string("Running part 2");
  msec_delay(1000);

  uint32_t value_32bit = 1234567890;
  uint16_t value_16bit = 1234;
  uint8_t  value_8bit  = 123;

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_5);
  lcd_write_quadbyte(value_32bit);

  wait_for_pb_pressed(PB1_IDX);
  wait_for_pb_released(PB1_IDX);

  lcd_set_ddram_addr(LCD_LINE2_ADDR + LCD_CHAR_POSITION_6);
  lcd_write_doublebyte(value_16bit);

  wait_for_pb_pressed(PB1_IDX);
  wait_for_pb_released(PB1_IDX);

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_7);
  lcd_write_byte(value_8bit);

  wait_for_pb_pressed(PB1_IDX);
  wait_for_pb_released(PB1_IDX);

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_5);
  lcd_write_string("Part 2 done");
}

void run_lab6_part3(void)
{

  uint16_t timer_count = 100;
  bool     done        = false;

  lcd_clear();

  while (!done)
  {

    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_5);

    lcd_write_doublebyte(timer_count);

    if (is_pb_down(PB1_IDX))
    {
      timer_count = 100;

      wait_for_pb_released(PB1_IDX);
    }

    if (is_pb_down(PB2_IDX))
    {
      done = true;

      wait_for_pb_released(PB2_IDX);
      lcd_clear();
      lcd_set_ddram_addr(LCD_LINE1_ADDR);
      lcd_write_string("Part 3 Done");
    }
    else
    {

      msec_delay(200);

      if (timer_count == 0)
      {
        timer_count = 100;
      }
      else
      {
        timer_count--;
      }
    }
  }
}

void run_lab6_part4(void)
{
  bool done = false;
  uint8_t key;
  uint8_t char_count = 0;

  lcd_clear();

  while (!done)
  {
    if (is_pb_down(PB2_IDX))
    {

      done = true;

      wait_for_pb_released(PB2_IDX);
      lcd_clear();
      lcd_set_ddram_addr(LCD_LINE1_ADDR);
      lcd_write_string("Program Stopped");
    }

    else if (is_pb_down(PB1_IDX))
    {
      lcd_clear();

      char_count = 0;

      wait_for_pb_released(PB1_IDX);
    }

    else
    {
      key = keypad_scan();

      if (key != NO_KEY_PRESSED)
      {
        if (char_count >= TOTAL_CHAR_LCD)
        {
          lcd_clear();
          char_count = 0;
        }

        if (char_count < CHAR_PER_LCD)
        {

          lcd_set_ddram_addr(LCD_LINE1_ADDR + char_count);
        }

        else
        {

          lcd_set_ddram_addr(LCD_LINE2_ADDR + (char_count - CHAR_PER_LCD));
        }

        hex_to_lcd(key);

        char_count++;

        msec_delay(DELAY_DEBOUNCE);
        wait_no_key_pressed();
        msec_delay(DELAY_DEBOUNCE);
      }
    }
  }
}

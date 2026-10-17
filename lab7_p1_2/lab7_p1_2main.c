//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Gabby Williams
//
//       LAB NAME:  Lab 7: Working with Interrupts
//
//      FILE NAME:  lab7_p1_2main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code generates: Part1: SOS in morse code while scrolling a message across 
//    the LCD.
//    Part 2: Repeats part 1 but with a message that is too long for one LCD.
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
#include "lcd1602.h"
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab7_part1(void);
void run_lab7_part2(void);
void wait_for_pb_pressed(uint8_t pb_idx);
void wait_for_pb_released(uint8_t pb_idx);
void lcd_write_string_window(char *string, uint8_t start_lcd_addr, uint8_t max_lcd_addr);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define MSPM0_CLOCK_FREQUENCY (40E6)
#define SYST_TICK_PERIOD (10.25E-3)
#define SYST_TICK_PERIOD_COUNT (SYST_TICK_PERIOD * MSPM0_CLOCK_FREQUENCY)

#define NUM_STATES (18)
#define DELAY_DEBOUNCE (20)
#define DEBOUNCE_DELAY (1000)

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
// Defines the seven segment display for SOS
const uint8_t seg7_letter_code[] = {
    0x6D, 0x00, 0x6D, 0x00, 0x6D, 0x00, // S
    0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x00, // O
    0x6D, 0x00, 0x6D, 0x00, 0x6D, 0x00  // S
};

// This is the 1:3 ratio for dots:dashes for Morse code.
// The delay times in terms of number SysTick interrupts.
const uint8_t delay_count[] = {
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, // dots
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, // dash
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x30  // dots
};

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
  lcd_clear();
  sys_tick_init(SYST_TICK_PERIOD_COUNT);

  // enter your code here
  run_lab7_part1();
  sys_tick_disable();
  seg7_off();

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string("Press PB 2");
  msec_delay(1000);
  wait_for_pb_pressed(PB2_IDX);
  lcd_clear();
  wait_for_pb_released(PB2_IDX);

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Running Part 2");
  msec_delay(1000);
  lcd_clear();

  sys_tick_init(SYST_TICK_PERIOD_COUNT);
  run_lab7_part2();
  sys_tick_disable();
  seg7_off();

  

  // Endless loop to prevent program from ending
  while (1)
    ;

} /* main */

void SysTick_Handler(void)
{
  static uint16_t delay_time = 1;
  static uint16_t code_index = 0;

  delay_time--;
  if (delay_time == 0)
  {
    seg7_on(seg7_letter_code[code_index], SEG7_DIG0_ENABLE_IDX);

    delay_time = delay_count[code_index];
    code_index++;

    if (code_index == NUM_STATES)
    {
      code_index = 0;
    }
  }
}

// for pbs
void wait_for_pb_pressed(uint8_t pb_idx)
{
  while (is_pb_up(pb_idx))
  {
  }

  msec_delay(DELAY_DEBOUNCE);
}

void wait_for_pb_released(uint8_t pb_idx)
{
  msec_delay(DELAY_DEBOUNCE);
  while (is_pb_down(pb_idx))
  {
  }

  msec_delay(DELAY_DEBOUNCE);
}

void run_lab7_part1(void)
{
  char   *message = "Microcontrollers are fun.";
  bool    done    = false;
  uint8_t add     = 0;
  uint8_t idx     = 0;

  while (!done)
  {
    for (add = 0x4F; (add > 0x40 )&& !done; add--)
    {
      lcd_clear();
      lcd_set_ddram_addr(add);
      lcd_write_string(message);
      msec_delay(200);

      if (is_pb_down(PB1_IDX))
      {
        done = true;
      }
      lcd_clear();
    }
    idx = 0;

    while ((message[idx] != '\0') && !done)
    {
      lcd_set_ddram_addr(0x40);
      lcd_write_string(message + idx);
      msec_delay(200);

      if (is_pb_down(PB1_IDX))
      {
        msec_delay(DELAY_DEBOUNCE);
        done = true;
      }
      lcd_clear();
      idx++;
    }
  }

  wait_for_pb_released(PB1_IDX);

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_5);
  lcd_write_string("Part 1 done");
  msec_delay(1000);
}

void lcd_write_string_window(char *string, uint8_t start_lcd_addr, uint8_t max_lcd_addr)
{
  uint8_t address = start_lcd_addr;
  uint8_t index = 0;
  char one_char[2];

  one_char[1] = '\0';
  lcd_set_ddram_addr(address);

  while (string[index] != '\0' && address <= max_lcd_addr)
  {
    one_char[0] = string[index];
    lcd_write_string(one_char);

    index++;
    address++;
  }
}


void run_lab7_part2(void)
{
  char   *message = "Microcontrollers are fun. I love programming in MSPM0+ assembly code!!!";
  bool    done    = false;
  uint8_t add     = 0;
  uint8_t idx     = 0;


  while (!done)
  {
    for (add = 0x4F; (add > 0x40 && !done); add--)
    {
      lcd_clear();
      lcd_write_string_window(message, add, 0x4F);
      msec_delay(200);

      if (is_pb_down(PB1_IDX))
      {
        msec_delay(DELAY_DEBOUNCE);
        if (is_pb_down(PB1_IDX))
        {
          done = true;
        }
      }
    }
    idx = 0;

    while (message[idx] != '\0' && !done)
    {
      lcd_clear();
      lcd_write_string_window(message + idx, 0x40, 0x4F);
      msec_delay(200);

      if (is_pb_down(PB1_IDX))
      {
        msec_delay(DELAY_DEBOUNCE);
        if (is_pb_down(PB1_IDX))
        {
          done = true;
        }
      }
      idx++;
    }
  }
  
  wait_for_pb_released(PB1_IDX);

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_5);
  lcd_write_string("Part 2 done");
  msec_delay(1000);
  lcd_clear();
}

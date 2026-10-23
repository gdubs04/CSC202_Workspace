//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Gabby Williams
//
//       LAB NAME:  Lab 8: Analog to Digital Converter (part2)
//
//      FILE NAME:  lab8_p2main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code will create a program that lights the LEDS up that corresponds
//    to the amount of voltage coming through the onboard potentiometer. So, if
//    the pot gives off no voltage, no LEDS should be lit. Increasing the 
//    by turning the potentiometer causes the next LED to turn on (LD0 to LD7)
//    across the board. This behavior continues until all LEDS are on.
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
#include "adc.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab8_part2(void);
void config_pb1_interrupts(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

#define NUM_OF_LEDS               (8+1)
#define ADC_COUNT_PER_LED         (uint16_t)(4096 / NUM_OF_LEDS)

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
  lcd_clear();
  leds_init();
  leds_enable();
  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);
  config_pb1_interrupts();
 
  // enter your code here
  run_lab8_part2();
 
 // Endless loop to prevent program from ending
 while (1);

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

void run_lab8_part2(void)
{
  uint16_t adc_pot_value = 0;
  uint8_t led_num = 0;
  bool done = false;
  uint8_t idx = 0;
  char *adc_msg  = "ADC =    ";
  char *led_msg = "LED =   ";

  while (!done)
  {
    adc_pot_value = ADC0_in(7);

    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string(adc_msg);
    lcd_write_doublebyte(adc_pot_value);

    led_num = (uint16_t)(adc_pot_value / ADC_COUNT_PER_LED);


    leds_off();

    for (idx = 0; idx < led_num; idx++) 
    {
      led_on(idx);
    }

    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string(led_msg);
    lcd_write_doublebyte(led_num);
    msec_delay(200);

    if (g_pb1_pressed)
    {
      done = true;
      g_pb1_pressed = false;
    }

  }

  leds_off();
  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Program Stopped");

}

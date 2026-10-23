//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Gabby Williams
//
//       LAB NAME:  Lab 8: Analog to Digital Converter (part3)
//
//      FILE NAME:  lab8_p3main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code will create a program that lights the LEDS up that corresponds
//    to the amount of voltage coming though the onboard potentiometer. If the
//    pot gives off no voltage, no LEDs should be lit. Increasing the voltage by
//    turning the potentiometer causes the next LED to turn on (left to right)
//    across the board. This behavior will continue until all LEDs are on.
//    The ADC returns a value between 0 and 1023. Since we have 8 LEDs, we need
//    to divide the ADC value into nine ranges where each range
//    corresponds to an LED. The ADC value is converted to Celsius by dividing
//    the sensor reading by 2, then the Celsius value is converted into
//    Fahrenheit.
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
#include "adc.h"
#include "clock.h"
#include "lcd1602.h"
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab8_part3(void);
void config_pb1_interrupts(void);
void config_pb2_interrupts(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define NUM_OF_LEDS               (8+1)
#define ADC_COUNT_PER_LED         (uint16_t)(4096 / NUM_OF_LEDS)
#define DEGREE_SYMBOL_LCD "\xDF"
#define CONVERT_TO_FAHRENHEIT(x)  ((x) * 9 / 5 + 32)
#define TEMP_SENSOR_ADC0_CHANNEL  (5)

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool g_pb1_pressed = false;
bool g_pb2_pressed = false;
uint16_t g_temp_adc_value;


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
  config_pb2_interrupts();

  // enter your code here
  run_lab8_part3();

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
} // config_pb1_interrupt

void config_pb2_interrupts(void)
{
  GPIOA->POLARITY15_0  = GPIO_POLARITY15_0_DIO15_RISE;
  GPIOA->CPU_INT.ICLR  = GPIO_CPU_INT_ICLR_DIO15_CLR;
  GPIOA->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;

  NVIC_SetPriority(GPIOA_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOA_INT_IRQn);
} // config_pb2_interrupt

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
        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) ==
            GPIO_CPU_INT_MIS_DIO15_SET)
        {
          g_temp_adc_value    = ADC0_in(TEMP_SENSOR_ADC0_CHANNEL);
          g_pb2_pressed       = true;
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

void run_lab8_part3(void)
{
  uint16_t deg_c = 0;
  uint16_t deg_f = 0;

  uint16_t adc_pot_value = 0;
  uint8_t led_num = 0;
  bool done = false;
  uint8_t idx = 0;

  uint16_t temp_value = 0;
  char    *adc_msg  = "ADC =    ";
  char    *temp_msg1 = "Temp =";
  char    *temp_msg2 = DEGREE_SYMBOL_LCD " F";

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

    if (g_pb1_pressed) 
    {
      done = true;
      g_pb1_pressed = false;
    }

    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string(temp_msg1);

    if (g_pb2_pressed)
    {
      deg_c = g_temp_adc_value >> 1;

      deg_f = (deg_c * 9 / 5) + 32;

      lcd_set_ddram_addr(LCD_LINE2_ADDR + 6);
      lcd_write_byte(deg_f);
      lcd_write_string(temp_msg2);
      

      g_pb2_pressed = false;

    }

  }
  leds_off();
  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Program Stopped");
}
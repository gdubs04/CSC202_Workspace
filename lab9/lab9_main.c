//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Gabby Williams
//
//       LAB NAME:  Lab 9: Pulse Width Modulation and Finite State Machine
//                  Design
//
//      FILE NAME:  lab9_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This project runs on the LP_MSPM0G3507 LaunchPad board interfacing to
//    the CSC202 Expansion board.
//
//    This code utilizes PWM to control an LED, an active buzzer and a passive
//    buzzer. When either the duty cycle or the frequency changes then the 
//    output of the PWM signal changes, which affects the brightness of the led
//    and the sound produced by the buzzers. The keypad is used to adjust the 
//    duty cycle while th potentiometer controls the duty cycle or frequency
//    depending on the current mode. 
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
#include "demo_melodies.h"
#include "lcd1602.h"
#include "pitches.h"
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void    run_lab9_part1(void);
void    config_pb1_interrupts(void);
void    config_pb2_interrupts(void);
void    GROUP1_IRQHandler(void);
void    update_lcd_line1(char *string);
void    update_lcd_line2(uint16_t frequency, uint8_t duty_cycle);
void    update_lcd_line2A(uint16_t frequency, uint8_t duty_cycle);
void    motor0_set_pwm_freq(uint16_t pwm_frequency);
uint8_t Check_for_duty_cycle(void);
void    enable_active_buzzer(void);
void    enable_passive_buzzer(void);
void    disable_active_buzzer(void);
void    disable_passive_buzzer(void);


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define FIFTY_PERCENT_DC (50)

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
  I2C_mstr_init();
  lcd1602_init();
  lcd_clear();
  leds_init();
  leds_enable();
  keypad_init();
  dipsw_init();
  dipsw_filter_enable();
  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);

  motor0_init();
  motor0_pwm_init(4000, 0);
  motor0_pwm_enable();

  disable_active_buzzer();
  disable_passive_buzzer();

  config_pb1_interrupts();
  config_pb2_interrupts();

  // enter your code here
  run_lab9_part1();

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
      case (CPUSS_INT_GROUP_IIDX_STAT_INT0): // GPIOA
        gpio_mis = GPIOA->CPU_INT.MIS;
        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) ==
            GPIO_CPU_INT_MIS_DIO15_SET)
        {
          g_pb2_pressed       = true;
          GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
        }

        break;
      case (CPUSS_INT_GROUP_IIDX_STAT_INT1): // GPIOB
        gpio_mis = GPIOB->CPU_INT.MIS;
        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) ==
            GPIO_CPU_INT_MIS_DIO18_SET)
        {

          g_pb1_pressed       = true;
          GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
        }
        break;
    }

  } while (group_gpio_iidx != 0);
}

void update_lcd_line1(char *string)
{
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string(string);
}

void update_lcd_line2A(uint16_t frequency, uint8_t duty_cycle)
{
  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string("F=");
  lcd_write_doublebyte(frequency); 
  lcd_write_string(" Dur=");
  lcd_write_byte(duty_cycle);
  
}

void update_lcd_line2(uint16_t frequency, uint8_t duty_cycle)
{
  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string("F=");
  lcd_write_doublebyte(frequency);
  lcd_write_string(" DC=");
  lcd_write_byte(duty_cycle);
  lcd_write_char('%');
  
}

void motor0_set_pwm_freq(uint16_t pwm_frequency)
{
  // Determine what the timer clock divider and prescaler are
  uint32_t tim_clk_divider = TIMA0->CLKDIV + 1;
  uint32_t tim_clk_scaler = (TIMA0->COMMONREGS.CPS & GPTIMER_CPS_PCNT_MASK) + 1;
  // Now determine what the timer clock frequency is
  uint32_t tim_clk = get_bus_clock_freq() / (tim_clk_divider * tim_clk_scaler);
  // Update the load value based on new PWM frequency
  uint32_t load_value = (tim_clk / pwm_frequency);
  // Disable timer and update the load register
  motor0_pwm_disable();
  TIMA0->COUNTERREGS.LOAD = (load_value - 1) & GPTIMER_LOAD_LD_MASK;
}

uint8_t Check_for_duty_cycle(void)
{

  static uint8_t duty_cycle = 0;
  uint8_t        key_press;

  key_press = keypad_scan();

  if (key_press != NO_KEY_PRESSED)
  {
    duty_cycle = key_press * 100 / 15;

    msec_delay(5);
    wait_no_key_pressed();
    msec_delay(5);
  }
  return (duty_cycle);
}

void enable_active_buzzer(void)
{
  led_off(LED_BAR_LD1_IDX);
}

void enable_passive_buzzer(void)
{
  led_off(LED_BAR_LD2_IDX);
}

void disable_active_buzzer(void)
{
  led_on(LED_BAR_LD1_IDX);
}

void disable_passive_buzzer(void)
{
  led_on(LED_BAR_LD2_IDX);
}
void play_note(uint16_t frequency, uint16_t duration_ms)
{
  // Only enable the PWM if we have a frequency
  if (frequency != 0)
  {
    motor0_set_pwm_freq(frequency);
    motor0_set_pwm_dc(FIFTY_PERCENT_DC);
    motor0_pwm_enable();
    msec_delay(duration_ms);
    motor0_pwm_disable();
  } /* if */
} /* play_note */

void run_lab9_part1(void)
{

  bool done = false;

  uint16_t adc_pot_value = 0;
  uint8_t  switch_value  = 0;
  uint8_t  duty_cycle    = 0;
  uint16_t frequency     = 50;
  uint8_t  note_idx      = 0;

  typedef enum
  {
    Buzzer_IDLE,
    Active_DC_Test,
    Passive_DC_Test,
    Active_DC_Sweep,
    Passive_FREQ_Sweep,
    Passive_Melody,
  } fsm_state_t;

  fsm_state_t present_State = Buzzer_IDLE;
  fsm_state_t next_State    = Buzzer_IDLE;

  while (!done)
  {
    char *buzzer_msg = "Buzzers Disabled";
    char *ATest_msg  = "Active DC Test  ";
    char *PTest_msg  = "Passive DC Test ";
    char *ASweep_msg = "Active DC Sweep ";
    char *PSweep_msg = "Passive F Sweep ";
    char *Melody_msg = "Play Melody     ";

    if (g_pb1_pressed)
    {
      done          = true;
      g_pb1_pressed = false;
    }
    if (g_pb2_pressed)
    {
      present_State = next_State;
      msec_delay(100);
      g_pb2_pressed = false;
    }

    if (!done)
    {
      switch (present_State)
      {
        case (Buzzer_IDLE):
          frequency = 50;
          duty_cycle = 0;
          next_State = Active_DC_Test;
          duty_cycle = Check_for_duty_cycle();
          motor0_set_pwm_freq(frequency);
          motor0_set_pwm_dc(duty_cycle);
          motor0_pwm_enable();
          update_lcd_line1(buzzer_msg);
          update_lcd_line2(frequency, duty_cycle);
          disable_active_buzzer();
          disable_passive_buzzer();

          break;

        case (Active_DC_Test):

          next_State = Passive_DC_Test;
          duty_cycle = Check_for_duty_cycle();
          motor0_set_pwm_freq(frequency);
          motor0_set_pwm_dc(duty_cycle);
          motor0_pwm_enable();
          update_lcd_line1(ATest_msg);
          update_lcd_line2(frequency, duty_cycle);
          enable_active_buzzer();
          disable_passive_buzzer();

          break;

        case (Passive_DC_Test):

          next_State = Active_DC_Sweep;
          duty_cycle = Check_for_duty_cycle();
          motor0_set_pwm_freq(frequency);
          motor0_set_pwm_dc(duty_cycle);
          motor0_pwm_enable();
          update_lcd_line1(PTest_msg);
          update_lcd_line2(frequency, duty_cycle);
          disable_active_buzzer();
          enable_passive_buzzer();
          break;

        case (Active_DC_Sweep):

          next_State = Passive_FREQ_Sweep;
          duty_cycle = ADC0_in(7) * 100 / 4095;
          motor0_set_pwm_freq(frequency);
          motor0_set_pwm_dc(duty_cycle);
          motor0_pwm_enable();
          update_lcd_line1(ASweep_msg);
          update_lcd_line2(frequency, duty_cycle);
          enable_active_buzzer();
          disable_passive_buzzer();
          break;

        case (Passive_FREQ_Sweep):

          next_State = Passive_Melody;
          duty_cycle = 50;
          frequency  = ADC0_in(7);
          motor0_set_pwm_freq(frequency);
          motor0_set_pwm_dc(duty_cycle);
          motor0_pwm_enable();
          update_lcd_line1(PSweep_msg);
          update_lcd_line2(frequency, duty_cycle);
          disable_active_buzzer();
          enable_passive_buzzer();
          break;

        case (Passive_Melody):

          next_State = Buzzer_IDLE;

          play_note(imperial_march[note_idx].freq,
                    imperial_march[note_idx].duration);

          update_lcd_line1(Melody_msg);
          update_lcd_line2A(imperial_march[note_idx].freq, imperial_march[note_idx].duration);
          disable_active_buzzer();
          enable_passive_buzzer();
          note_idx++;
          if (note_idx == IMPERIAL_MARCH_LENGTH)
            note_idx = 0;
          break;

          
      }

    } // if(!done)

  } // while(!done)

  motor0_pwm_disable();
  disable_active_buzzer();
  disable_passive_buzzer();

  leds_off();
  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string("Program Stopped");
} // main
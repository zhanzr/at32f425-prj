#include "at32f425.h"
#include "at32f425_clock.h"
#include "i2c_application.h"

#include "oled.h"

#include "custom_at32f425_board.h"

__IO uint32_t g_Ticks;                                

#define I2Cx_ADDRESS                     0xAA

#define I2Cx_PORT                        I2C1
#define I2Cx_CLK                         CRM_I2C1_PERIPH_CLOCK
#define I2Cx_DMA                         DMA1
#define I2Cx_DMA_CLK                     CRM_DMA1_PERIPH_CLOCK
                                         
#define I2Cx_SCL_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK
#define I2Cx_SCL_GPIO_PIN                GPIO_PINS_6
#define I2Cx_SCL_GPIO_PinsSource         GPIO_PINS_SOURCE6  
#define I2Cx_SCL_GPIO_PORT               GPIOB
#define I2Cx_SCL_GPIO_MUX                GPIO_MUX_1     
                                         
#define I2Cx_SDA_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK
#define I2Cx_SDA_GPIO_PIN                GPIO_PINS_7   
#define I2Cx_SDA_GPIO_PinsSource         GPIO_PINS_SOURCE7      
#define I2Cx_SDA_GPIO_PORT               GPIOB
#define I2Cx_SDA_GPIO_MUX                GPIO_MUX_1 

i2c_handle_type hi2cx;

enum I2C_FREQ_CONF {
    I2C_FREQ_10KHZ = 0x2170FAFA,
    I2C_FREQ_50KHZ = 0x80E06565,
    I2C_FREQ_100KHZ = 0x80E03030,
    I2C_FREQ_200KHZ = 0X20E0355F,
    I2C_FREQ_INVALID = UINT32_MAX,
};

/**
  * @brief  compare whether the valus of buffer 1 and buffer 2 are equal.
  * @param  buffer1: buffer 1 address.
            buffer2: buffer 2 address.
  * @retval 0: equal.
  *         1: unequal.
  */
uint32_t buffer_compare(uint8_t* buffer1, uint8_t* buffer2, uint32_t len)
{
  uint32_t i;
  
  for(i = 0; i < len; i++)
  {
    if(buffer1[i] != buffer2[i])
    {
      return 1;
    }
  }

  return 0;
}

int main(void) {
	  i2c_status_type i2c_status;

  /* config nvic priority group */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
	
//  system_clock_config();

  uint32_t test_tick_0;
  uint32_t test_tick_1;

  /* System timer configuration */
  SysTick_Config(system_core_clock / 1000);

  uart_print_init(115200);
  at32_board_init();
	
  hi2cx.i2cx = I2Cx_PORT;
  /* i2c config */
  i2c_config(&hi2cx);
	
  printf("AT START F425 Board @ %u MHz\n", system_core_clock / (1000000));
  printf("Boot Mem:%02X\n", scfg_mem_map_get());

  printf("I2C test start\n");

  printf("F425@ %u MHz\n", system_core_clock / (1000000));

  MD066_Init();
  MD066_Clear();

  while (1) {
		  MD066_Init();
		
    MD066_Demo();

 printf("P1 %u MHz, Ticks:%u\n", system_core_clock/(1000000), g_Ticks);

    test_tick_0 = g_Ticks;
    while ((test_tick_0 + 200) > g_Ticks) {
      __NOP();
      __WFI();
    }
    at32_led_toggle(LED2);
 printf("P2 %u MHz, Ticks:%u\n", system_core_clock/(1000000), g_Ticks);

    test_tick_0 = g_Ticks;
    while ((test_tick_0 + 400) > g_Ticks) {
      __NOP();
      __WFI();
    }
    at32_led_toggle(LED3);
 printf("P3 %u MHz, Ticks:%u\n", system_core_clock/(1000000), g_Ticks);

    test_tick_0 = g_Ticks;
    while ((test_tick_0 + 800) > g_Ticks) {
      __NOP();
      __WFI();
    }
    at32_led_toggle(LED4);
		 printf("P4 %u MHz, Ticks:%u\n", system_core_clock/(1000000), g_Ticks);

  }
}

/**
  * @brief  initializes peripherals used by the i2c.
  * @param  none
  * @retval none
  */
void i2c_lowlevel_init(i2c_handle_type* hi2c)
{
  gpio_init_type gpio_init_structure;
  
  if(hi2c->i2cx == I2Cx_PORT)
  {
    /* i2c periph clock enable */
    crm_periph_clock_enable(I2Cx_CLK, TRUE);    
    crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);
    
    /* gpio configuration */  
    gpio_pin_mux_config(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_GPIO_PinsSource, I2Cx_SCL_GPIO_MUX);
    
    gpio_pin_mux_config(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_GPIO_PinsSource, I2Cx_SDA_GPIO_MUX);
    
    /* configure i2c pins: scl */
    gpio_init_structure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_structure.gpio_mode           = GPIO_MODE_MUX;
    gpio_init_structure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_init_structure.gpio_pull           = GPIO_PULL_UP;  
                                            
    gpio_init_structure.gpio_pins           = I2Cx_SCL_GPIO_PIN;
    gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_init_structure);

    /* configure i2c pins: sda */
    gpio_init_structure.gpio_pins           = I2Cx_SDA_GPIO_PIN;
    gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_init_structure); 
    
    /* config i2c */ 
    i2c_init(hi2c->i2cx, 0, I2C_FREQ_200KHZ);
    
    i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, I2Cx_ADDRESS);
  }
}

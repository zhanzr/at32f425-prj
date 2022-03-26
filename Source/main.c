#include "at32f425.h"
#include "custom_at32f425_board.h"
#include "at32f425_clock.h"

__IO uint32_t g_Ticks;

int main(void) {
  system_clock_config();

  uint32_t lockTick;
  uint32_t deltaTick;
  
  /* System timer configuration */
  SysTick_Config(system_core_clock / 1000);
  
	uart_print_init(115200);
	at32_board_init();
	
  printf("AT START F425 Board @%u MHz\n", system_core_clock/(1000000));
	printf("Boot Mem:%02X\n", scfg_mem_map_get());
  while (1) {
		printf("%u MHz, Ticks:%u\n", system_core_clock/(1000000), g_Ticks);
		
    lockTick = g_Ticks;
    while ((lockTick + 200) > g_Ticks) {
      __NOP();
      __WFI();
    }		
		at32_led_toggle(LED2);
		
    lockTick = g_Ticks;
    while ((lockTick + 400) > g_Ticks) {
      __NOP();
      __WFI();
    }
		at32_led_toggle(LED3);
		
		lockTick = g_Ticks;
    while ((lockTick + 800) > g_Ticks) {
      __NOP();
      __WFI();
    }
		at32_led_toggle(LED4);
  }
}

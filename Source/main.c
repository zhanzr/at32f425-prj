#include "at32f425.h"
#include "at32f425_clock.h"
#include "custom_at32f425_board.h"

__IO uint32_t g_Ticks;

#define TEST_LOOP 5000
#define CRC32_REF_RESULT 0xE5DFCF6D

#define BUFFER_SIZE 120
static const uint32_t data_buffer[BUFFER_SIZE] = {
    0xc33dd31c, 0xe37ff35e, 0x129022f3, 0x32d24235, 0x52146277, 0x7256b5ea,
    0x4a755a54, 0x6a377a16, 0x0af11ad0, 0x2ab33a92, 0xed0fdd6c, 0xcd4dbdaa,
    0xbb3bab1a, 0x6ca67c87, 0x5cc52c22, 0x3c030c60, 0x1c41edae, 0xfd8fcdec,
    0xad8b9de8, 0x8dc97c26, 0x5c644c45, 0x3ca22c83, 0x1ce00cc1, 0xef1fff3e,
    0x95a88589, 0xf56ee54f, 0xd52cc50d, 0x34e224c3, 0x04817466, 0x64475424,
    0x78066827, 0x18c008e1, 0x28a3cb7d, 0xdb5ceb3f, 0xfb1e8bf9, 0x9bd8abbb,
    0xdf7caf9b, 0xbfba8fd9, 0x9ff86e17, 0x7e364e55, 0x2e933eb2, 0x0ed11ef0,
    0xa35ad3bd, 0xc39cf3ff, 0xe3de2462, 0x34430420, 0x64e674c7, 0x44a45485,
    0xad2abd0b, 0x8d689d49, 0x7e976eb6, 0x5ed54ef4, 0x2e321e51, 0x0e70ff9f,
    0xefbedfdd, 0xcffcbf1b, 0x9f598f78, 0x918881a9, 0xb1caa1eb, 0xd10cc12d,
    0xe16f1080, 0x00a130c2, 0x20e35004, 0x40257046, 0x83b99398, 0xa3fbb3da,
    0x00001021, 0x20423063, 0x408450a5, 0x60c670e7, 0x9129a14a, 0xb16bc18c,
    0x569546b4, 0xb75ba77a, 0x97198738, 0xf7dfe7fe, 0xc7bc48c4, 0x58e56886,
    0x4405a7db, 0xb7fa8799, 0xe75ff77e, 0xc71dd73c, 0x26d336f2, 0x069116b0,
    0x76764615, 0x5634d94c, 0xc96df90e, 0xe92f99c8, 0xb98aa9ab, 0x58444865,
    0x78a70840, 0x18612802, 0xc9ccd9ed, 0xe98ef9af, 0x89489969, 0xa90ab92b,
    0xd1ade1ce, 0xf1ef1231, 0x32732252, 0x52b54294, 0x72f762d6, 0x93398318,
    0xa56ab54b, 0x85289509, 0xf5cfc5ac, 0xd58d3653, 0x26721611, 0x063076d7,
    0x8d689d49, 0xf7dfe7fe, 0xe98ef9af, 0x063076d7, 0x93398318, 0xb98aa9ab,
    0x4ad47ab7, 0x6a961a71, 0x0a503a33, 0x2a12dbfd, 0xfbbfeb9e, 0x9b798b58};

static const uint32_t bswap32_data_buffer[BUFFER_SIZE] = {
    0x1CD33DC3, 0x5EF37FE3, 0xF3229012, 0x3542D232, 0x77621452, 0xEAB55672,
    0x545A754A, 0x167A376A, 0xD01AF10A, 0x923AB32A, 0x6CDD0FED, 0xAABD4DCD,
    0x1AAB3BBB, 0x877CA66C, 0x222CC55C, 0x600C033C, 0xAEED411C, 0xECCD8FFD,
    0xE89D8BAD, 0x267CC98D, 0x454C645C, 0x832CA23C, 0xC10CE01C, 0x3EFF1FEF,
    0x8985A895, 0x4FE56EF5, 0x0DC52CD5, 0xC324E234, 0x66748104, 0x24544764,
    0x27680678, 0xE108C018, 0x7DCBA328, 0x3FEB5CDB, 0xF98B1EFB, 0xBBABD89B,
    0x9BAF7CDF, 0xD98FBABF, 0x176EF89F, 0x554E367E, 0xB23E932E, 0xF01ED10E,
    0xBDD35AA3, 0xFFF39CC3, 0x6224DEE3, 0x20044334, 0xC774E664, 0x8554A444,
    0x0BBD2AAD, 0x499D688D, 0xB66E977E, 0xF44ED55E, 0x511E322E, 0x9FFF700E,
    0xDDDFBEEF, 0x1BBFFCCF, 0x788F599F, 0xA9818891, 0xEBA1CAB1, 0x2DC10CD1,
    0x80106FE1, 0xC230A100, 0x0450E320, 0x46702540, 0x9893B983, 0xDAB3FBA3,
    0x21100000, 0x63304220, 0xA5508440, 0xE770C660, 0x4AA12991, 0x8CC16BB1,
    0xB4469556, 0x7AA75BB7, 0x38871997, 0xFEE7DFF7, 0xC448BCC7, 0x8668E558,
    0xDBA70544, 0x9987FAB7, 0x7EF75FE7, 0x3CD71DC7, 0xF236D326, 0xB0169106,
    0x15467676, 0x4CD93456, 0x0EF96DC9, 0xC8992FE9, 0xABA98AB9, 0x65484458,
    0x4008A778, 0x02286118, 0xEDD9CCC9, 0xAFF98EE9, 0x69994889, 0x2BB90AA9,
    0xCEE1ADD1, 0x3112EFF1, 0x52227332, 0x9442B552, 0xD662F772, 0x18833993,
    0x4BB56AA5, 0x09952885, 0xACC5CFF5, 0x53368DD5, 0x11167226, 0xD7763006,
    0x499D688D, 0xFEE7DFF7, 0xAFF98EE9, 0xD7763006, 0x18833993, 0xABA98AB9,
    0xB77AD44A, 0x711A966A, 0x333A500A, 0xFDDB122A, 0x9EEBBFFB, 0x588B799B,
};

__IO uint32_t crc_value = 0;

extern uint32_t xcrc32(const uint8_t *buf, size_t len, uint32_t init);

int main(void) {
  system_clock_config();

  uint32_t test_tick_0;
  uint32_t test_tick_1;

  /* System timer configuration */
  SysTick_Config(system_core_clock / 1000);

  uart_print_init(115200);
  at32_board_init();
  /* enable crc clock */
  crm_periph_clock_enable(CRM_CRC_PERIPH_CLOCK, TRUE);

  printf("AT START F425 Board @ %u MHz\n", system_core_clock / (1000000));
  printf("Boot Mem:%02X\n", scfg_mem_map_get());

  printf("CRC test start\n");

  {
    crc_data_reset();
    crc_value = crc_block_calculate((uint32_t *)data_buffer, BUFFER_SIZE);
    printf("Hardware:\t%08X\n", crc_value);
    crc_value = xcrc32((const uint8_t *)bswap32_data_buffer,
                       sizeof(bswap32_data_buffer), 0xffffffff);
    printf("Software:\t%08X\n", crc_value);
  }
  {
    crc_data_reset();
    crc_value =
        crc_block_calculate((uint32_t *)bswap32_data_buffer, BUFFER_SIZE);
    printf("Hardware:\t%08X\n", crc_value);
    crc_value =
        xcrc32((const uint8_t *)data_buffer, sizeof(data_buffer), 0xffffffff);
    printf("Software:\t%08X\n", crc_value);
  }

  test_tick_0 = g_Ticks;
  for (uint32_t i = 0; i < TEST_LOOP; ++i) {
    crc_data_reset();

    /* compute the crc of "data_buffer" */
    crc_value = crc_block_calculate((uint32_t *)data_buffer, BUFFER_SIZE);
    if (crc_value == CRC32_REF_RESULT) {
      continue;
    } else {
      printf("error %08X\n", crc_value);
      break;
    }
  }
  test_tick_1 = g_Ticks;

  printf("Hardware CRC test end[%08X], %u, %u, [%u]\n", crc_value, test_tick_0,
         test_tick_1, (test_tick_1 - test_tick_0));

  test_tick_0 = g_Ticks;
  for (uint32_t i = 0; i < TEST_LOOP; ++i) {
    crc_value = xcrc32((const uint8_t *)bswap32_data_buffer,
                       sizeof(bswap32_data_buffer), 0xffffffff);

    if (crc_value == CRC32_REF_RESULT) {
      continue;
    } else {
      printf("error %08X\n", crc_value);
      break;
    }
  }
  test_tick_1 = g_Ticks;

  printf("Software CRC test end[%08X], %u, %u, [%u]\n", crc_value, test_tick_0,
         test_tick_1, (test_tick_1 - test_tick_0));

  printf("F425@ %u MHz\n", system_core_clock / (1000000));

  while (1) {
    // printf("%u MHz, Ticks:%u\n", system_core_clock/(1000000), g_Ticks);

    test_tick_0 = g_Ticks;
    while ((test_tick_0 + 200) > g_Ticks) {
      __NOP();
      __WFI();
    }
    at32_led_toggle(LED2);

    test_tick_0 = g_Ticks;
    while ((test_tick_0 + 400) > g_Ticks) {
      __NOP();
      __WFI();
    }
    at32_led_toggle(LED3);

    test_tick_0 = g_Ticks;
    while ((test_tick_0 + 800) > g_Ticks) {
      __NOP();
      __WFI();
    }
    at32_led_toggle(LED4);
  }
}

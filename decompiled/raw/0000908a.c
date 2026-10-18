/* Original address: 0x0000908a; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Startup mepc target initializes drivers then calls nonreturning application */

void main(void)

{
  gp = &__global_pointer;
  NVIC_PriorityGroupConfig(2);
  SystemCoreClockUpdate();
  delay_init();
  debug_usart2_init(0x1c200);
  measurement_uart_dma_init(0x2580);
  buttons_gpio_exti_init();
  battery_inputs_init();
  display_init();
  filesystem_resources_init();
  tim3_periodic_init(9999,0x383f);
                    /* WARNING: Subroutine does not return */
  application_loop();
}


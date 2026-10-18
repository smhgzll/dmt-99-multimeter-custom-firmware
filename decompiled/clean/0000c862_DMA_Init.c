/* Semantic review: high. Channel peripheral memory count and CFGR struct fields.
 * Address 0x0000c862; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c862; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Channel peripheral memory count and CFGR struct fields */

void DMA_Init(DMA_Channel_Registers *channel,DMA_Init_Recovered *configuration)

{
  gp = &__global_pointer;
  channel->CFGR =
       configuration->direction | configuration->circular | configuration->peripheral_increment |
       configuration->memory_increment | configuration->peripheral_width |
       configuration->memory_width | configuration->priority | configuration->memory_to_memory |
       channel->CFGR & 0xffff800f;
  channel->CNTR = configuration->count;
  channel->PADDR = configuration->peripheral_address;
  channel->MADDR = configuration->memory_address;
  return;
}


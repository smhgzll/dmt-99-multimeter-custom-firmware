/* Original address: 0x0000cdac; Ghidra pseudocode, not compilable reconstructed source. */

/* high: SPI configuration mask struct offsets CRC polynomial */

void SPI_Init(SPI_Registers *spi,SPI_Init_Recovered *configuration)

{
  gp = &__global_pointer;
  spi->CTLR1 = configuration->direction | configuration->mode | configuration->data_size |
               configuration->clock_polarity | configuration->clock_phase | configuration->nss |
               configuration->prescaler | configuration->first_bit | spi->CTLR1 & 0x3040;
  spi->I2SCFGR = spi->I2SCFGR & 0xf7ff;
  spi->CRCR = configuration->crc_polynomial;
  return;
}


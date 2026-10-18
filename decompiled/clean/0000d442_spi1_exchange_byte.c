/* Semantic review: high. SPI1 TXE RXNE loops bounded by 201 polls.
 * Address 0x0000d442; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000d442; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: SPI1 TXE RXNE loops bounded by 201 polls */

uchar spi1_exchange_byte(uchar data)

{
  char cVar1;
  uchar uVar2;
  int iVar3;
  
  gp = &__global_pointer;
  cVar1 = -0x37;
  while (iVar3 = SPI_I2S_GetFlagStatus(&SPI1,2), iVar3 == 0) {
    cVar1 = cVar1 + -1;
    if (cVar1 == '\0') {
      return '\0';
    }
  }
  SPI_I2S_SendData(&SPI1,data);
  cVar1 = -0x37;
  do {
    iVar3 = SPI_I2S_GetFlagStatus(&SPI1,1);
    if (iVar3 != 0) {
      uVar2 = SPI_I2S_ReceiveData(&SPI1);
      return uVar2;
    }
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  return '\0';
}


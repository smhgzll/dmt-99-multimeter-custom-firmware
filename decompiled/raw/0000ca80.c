/* Original address: 0x0000ca80; Ghidra pseudocode, not compilable reconstructed source. */

/* high: GPIO configuration nibbles and pullup-pulldown register logic */

void GPIO_Init(GPIO_Registers *port,GPIO_Init_Recovered *configuration)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint32_t uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  
  gp = &__global_pointer;
  uVar6 = configuration->mode;
  uVar7 = uVar6 & 0xf;
  if ((uVar6 & 0x10) != 0) {
    uVar7 = uVar7 | configuration->speed;
  }
  uVar1 = (uint)configuration->pin;
  if ((configuration->pin & 0xff) != 0) {
    uVar4 = port->CFGLR;
    uVar3 = 0;
    do {
      uVar2 = 1 << (uVar3 & 0x1f);
      if (uVar2 == (uVar1 & uVar2)) {
        uVar4 = uVar7 << (uVar3 << 2 & 0x1f) | ~(0xf << (uVar3 << 2 & 0x1f)) & uVar4;
        if (uVar6 == 0x28) {
          port->BCR = uVar2;
        }
        else if (uVar6 == 0x48) {
          port->BSHR = uVar2;
        }
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 != 8);
    port->CFGLR = uVar4;
  }
  if (0xff < uVar1) {
    uVar4 = port->CFGHR;
    uVar3 = 8;
    do {
      uVar2 = 1 << (uVar3 & 0x1f);
      if (uVar2 == (uVar1 & uVar2)) {
        uVar5 = uVar3 * 4 - 0x20;
        uVar4 = ~(0xf << (uVar5 & 0x1f)) & uVar4 | uVar7 << (uVar5 & 0x1f);
        if (uVar6 == 0x28) {
          port->BCR = uVar2;
        }
        else if (uVar6 == 0x48) {
          port->BSHR = uVar2;
        }
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 != 0x10);
    port->CFGHR = uVar4;
  }
  return;
}


/* Original address: 0x0001e59e; Ghidra pseudocode, not compilable reconstructed source. */

void _memcpy(void *destination,void *source,uint count)

{
  undefined1 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  
  gp = &__global_pointer;
  puVar4 = (undefined4 *)((int)destination + count);
  if (((((uint)source ^ (uint)destination) & 3) == 0) && (3 < count)) {
    for (; ((uint)destination & 3) != 0; destination = (void *)((int)destination + 1)) {
      uVar1 = *(undefined1 *)source;
      source = (void *)((int)source + 1);
      *(undefined1 *)destination = uVar1;
    }
    for (; destination < (undefined4 *)((uint)puVar4 & 0xfffffffc) + -8;
        destination = (void *)((int)destination + 0x24)) {
      uVar2 = *(undefined4 *)((int)source + 4);
      uVar11 = *(undefined4 *)((int)source + 8);
      uVar10 = *(undefined4 *)((int)source + 0xc);
      uVar9 = *(undefined4 *)((int)source + 0x10);
      uVar8 = *(undefined4 *)((int)source + 0x14);
      uVar3 = *(undefined4 *)((int)source + 0x18);
      uVar7 = *(undefined4 *)((int)source + 0x1c);
      *(undefined4 *)destination = *(undefined4 *)source;
      uVar6 = *(undefined4 *)((int)source + 0x20);
      *(undefined4 *)((int)destination + 4) = uVar2;
      *(undefined4 *)((int)destination + 8) = uVar11;
      *(undefined4 *)((int)destination + 0xc) = uVar10;
      *(undefined4 *)((int)destination + 0x10) = uVar9;
      *(undefined4 *)((int)destination + 0x14) = uVar8;
      *(undefined4 *)((int)destination + 0x18) = uVar3;
      *(undefined4 *)((int)destination + 0x1c) = uVar7;
      *(undefined4 *)((int)destination + 0x20) = uVar6;
      source = (undefined4 *)((int)source + 0x24);
    }
    for (; destination < (undefined4 *)((uint)puVar4 & 0xfffffffc);
        destination = (void *)((int)destination + 4)) {
      uVar2 = *(undefined4 *)source;
      source = (void *)((int)source + 4);
      *(undefined4 *)destination = uVar2;
    }
  }
  if (puVar4 <= destination) {
    return;
  }
  do {
    uVar1 = *(undefined1 *)source;
    puVar5 = (undefined4 *)((int)destination + 1);
    source = (void *)((int)source + 1);
    *(undefined1 *)destination = uVar1;
    destination = puVar5;
  } while (puVar5 < puVar4);
  return;
}


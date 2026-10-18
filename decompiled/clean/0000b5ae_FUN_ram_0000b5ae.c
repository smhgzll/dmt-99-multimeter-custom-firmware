/* Semantic review: unresolved. Unknown; inspect pseudocode and callers.
 * Address 0x0000b5ae; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000b5ae; Ghidra pseudocode, not compilable reconstructed source. */

void FUN_ram_0000b5ae(void)

{
  gp = &__global_pointer;
  if (application_view == '\x01') {
    button_event = 5;
  }
  else {
    DAT_ram_20000200 = 0;
    button_event = 3;
  }
  return;
}


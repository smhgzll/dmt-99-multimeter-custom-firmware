/* Semantic review: high. GCC t0-return register-save helper; 48-byte frame.
 * Address 0x00006608; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00006608; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: This is an inlined function */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* high: GCC t0-return register-save helper; 48-byte frame */

void wch_save_10(void)

{
  code *UNRECOVERED_JUMPTABLE;
  
  gp = &__global_pointer;
                    /* WARNING: Could not recover jumptable at 0x0000662e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)();
  return;
}


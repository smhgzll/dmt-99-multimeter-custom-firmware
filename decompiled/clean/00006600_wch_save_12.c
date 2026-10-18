/* Semantic review: high. GCC t0-return register-save helper; 64-byte frame.
 * Address 0x00006600; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x00006600; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: This is an inlined function */
/* WARNING: Unknown calling convention -- yet parameter storage is locked */
/* high: GCC t0-return register-save helper; 64-byte frame */

void wch_save_12(void)

{
  code *UNRECOVERED_JUMPTABLE;
  
  gp = &__global_pointer;
                    /* WARNING: Could not recover jumptable at 0x0000662e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*UNRECOVERED_JUMPTABLE)();
  return;
}


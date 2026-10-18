/* Semantic review: high. Vector slot 3; infinite self-loop.
 * Address 0x0000cb8c; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000cb8c; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Vector slot 3; infinite self-loop */

void HardFault_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


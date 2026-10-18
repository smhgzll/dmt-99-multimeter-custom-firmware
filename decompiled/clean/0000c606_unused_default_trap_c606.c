/* Semantic review: high. Two-byte self-loop inside the contiguous WCH weak-handler stub region; no vector points here.
 * Address 0x0000c606; original logic and decompiler warnings preserved.
 * Readable pseudocode; maintained C implementations live in reconstructed/. */
/* Original address: 0x0000c606; Ghidra pseudocode, not compilable reconstructed source. */

/* high: Two-byte self-loop inside the contiguous WCH weak-handler stub region; no vector points
   here */

void unused_default_trap_c606(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


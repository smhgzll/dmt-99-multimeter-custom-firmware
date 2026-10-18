/* Original address: 0x0000997e; Ghidra pseudocode, not compilable reconstructed source. */

/* WARNING: Function: wch_save_2 replaced with injection: wch_save_2 */
/* high: Combines high/low nibbles across bytes 2 through 6 into four symbols */

void measurement_decode_segments(uchar *symbols)

{
  uint uVar1;
  
  gp = &__global_pointer;
  uVar1 = measurement_segment_decode
                    (measurement_dma_frame.status_or_segments0 & 0xfffffff0 |
                     measurement_dma_frame.packed_segments[0] & 0xf);
  *symbols = (uchar)uVar1;
  uVar1 = measurement_segment_decode
                    (measurement_dma_frame.packed_segments[0] & 0xfffffff0 |
                     measurement_dma_frame.packed_segments[1] & 0xf);
  symbols[1] = (uchar)uVar1;
  uVar1 = measurement_segment_decode
                    (measurement_dma_frame.packed_segments[1] & 0xfffffff0 |
                     measurement_dma_frame.packed_segments[2] & 0xf);
  symbols[2] = (uchar)uVar1;
  uVar1 = measurement_segment_decode
                    (measurement_dma_frame.packed_segments[2] & 0xfffffff0 |
                     measurement_dma_frame.packed_segments[3] & 0xf);
  symbols[3] = (uchar)uVar1;
  return;
}


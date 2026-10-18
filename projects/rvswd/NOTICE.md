# Protocol reference attribution

The short-frame implementation was written for this diagnostic after studying:

- Nicolai Electronics, `esp32-component-rvswd`, Copyright (c) 2025,
  [MIT license](https://github.com/Nicolai-Electronics/esp32-component-rvswd/blob/main/LICENSE).
  Its separate, pre-existing local reference directory retains its original license.
- Charles Lohr, Copyright 2023, `ch32fun` / ESP32-S2 programmer,
  [source and MIT/X11 or NewBSD license notice](https://github.com/cnlohr/esp32s2-cookbook/blob/master/ch32v003programmer/main/ch32v003_swio.h).
- The [wch-protocols knowledge base](https://github.com/ch32-riscv-ug/wch-protocols)
  for observed frame boundaries and unresolved compatibility details.

No upstream programmer, embedded target firmware, Flash algorithm, or target
program buffer is compiled or distributed as part of the diagnostic binary.
ESP-IDF dependencies retain their own upstream licenses.

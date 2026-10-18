# 0.4.0 emulator screenshots

The actual RV32 BIN executed with menu/game inputs through GPIO PC8/9/10/11/12.
Panel outputs were saved as PNG; no host drawing calls replaced target rendering.

![Startup menu](clean-menu.png)
![God on, sound off](sound-off.png)
![Center-button fire](fire.png)

held-fire.png: center held2.3 seconds; firing continued without opening a menu.
pause.png: the menu opened only after PC8+downPC10 were held together2 seconds.

![God protection](god-protects.png)
![Muted fire](muted-fire.png)

God/wall/corpse scenes use controlled SRAM actor positions.
The god fixture places a previously confirmed attacking human enemy nearby;
health stays100. Muted fire started no PWM tone, checked through MMIO rather
than inferred from the image.
wall-blocks-shot.png demonstrates blocked sight/shots/noise wake-up.
corpse-removed.png accelerates the removal timer in a fixture;
the actual80-tick lifetime was checked in C tests.
These are not physical-device/FPS evidence.

PNG hashes: screenshots.json.
[Detailed test report](../../../TEST_RESULTS.md).

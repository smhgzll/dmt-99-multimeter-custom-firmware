#ifndef DMT_CONFIG_H
#define DMT_CONFIG_H
#if defined(STOCK_COMPATIBLE) && defined(CUSTOM_DMT99)
#error Select exactly one DMT99 configuration
#endif
#if !defined(STOCK_COMPATIBLE) && !defined(CUSTOM_DMT99)
#define STOCK_COMPATIBLE 1
#endif
#ifdef CUSTOM_DMT99
#define DMT_BUILD_NAME "CUSTOM_DMT99"
#else
#define DMT_BUILD_NAME "STOCK_COMPATIBLE"
#endif
/* Release features (firmware-update image, simulator-release): the last
 * measurement mode survives power-off and the
 * About page shows DMT_PRODUCT_NAME and the firmware version. Parity builds
 * (DMT_RELEASE_FEATURES unset) behave exactly as V2.4. Turkish is planned. */
#if defined(DMT_RELEASE_FEATURES) && DMT_RELEASE_FEATURES
#define DMT_ABOUT_FW_VERSION 1
/* Installable themes: THEME.PKG on the bootloader disk, theme manager on the
 * About page (ui/theme*.c). */
#define DMT_THEMES 1
/* Image-free measurement screen with colours and digit size set on the meter
 * (ui/simple_screen.c, About > ENTER). */
#define DMT_SIMPLE_SCREEN 1
#endif
#if defined(CUSTOM_DMT99) || (defined(DMT_RELEASE_FEATURES) && DMT_RELEASE_FEATURES)
#define DMT_LAST_MODE 1
#endif
#ifndef DMT_PRODUCT_NAME
#define DMT_PRODUCT_NAME "POTAT-99"
#endif
#endif

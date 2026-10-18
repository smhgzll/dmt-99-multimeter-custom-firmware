# Unknown ASIC candidates

The recovered CH32-facing interface is USART1, with 10-byte outgoing commands and 13-byte incoming display/event
records. It transports LCD segment-like status rather than a verified raw ADC conversion word. A programmable
measurement SoC can implement a product-specific protocol, so this interface is insufficient to name the chip.

| Candidate | Compatible evidence | Missing or incompatible evidence | Assessment |
|---|---|---|---|
| SD7502 | Vendor datasheet documents UART, multimeter functions and LQFP64 | No manufacturer-fixed AA55/5AA5 protocol found; actual package/pin connectivity unavailable | Plausible, not identified |
| SD78P952 | Vendor selection catalog lists multimeter SoC and LQFP64/100 | Detailed datasheet/protocol not recovered here; no observed chip ID; cannot match pinout | Plausible, evidence weaker |
| SD78P953 | Vendor datasheet documents UART, multimeter functions, dual ADC and LQFP48/64/100 | Its power-measurement capability is not demonstrated by this BIN; no unique protocol signature | Plausible, not identified |
| Other/custom programmed SoC | Product-specific UART and segment frames fit an independently programmed peer | No silicon identification or analog-side evidence | Remains possible |

No candidate is positively ruled out by the available firmware. A pin/package mismatch could rule one out,
but those measurements are not available. Marketing use in another FNIRSI device has not been used as proof.
Public datasheets establish peripheral capabilities, not the actual OTP application executing inside the peer.

Primary material consulted/downloaded:

- [SD7502 vendor datasheet](https://sdicmicro.cn/upload/img/2021-08/611627e6a2699.pdf), local PDF and text in `references/`.
- [SD78P953 vendor datasheet](https://sdicmicro.cn/upload/img/2022-11/6371b0eeeaeef.pdf), local PDF and text in `references/`.
- [SDIC product selection catalog](https://sdicmicro.cn/upload/img/2023-05/64633c119f860.pdf), SD78P952 package/capability comparison.

A failed download from the vendor's www host returned HTML; that response is retained with an `.html` suffix,
and was replaced by a verified PDF from the working vendor host. No missing datasheet was silently substituted.

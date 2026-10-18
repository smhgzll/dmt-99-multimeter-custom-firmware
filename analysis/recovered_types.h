/* Evidence-backed types used by Ghidra; do not imply recovered original typedef names. */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef struct { uint32_t CFGLR,CFGHR,INDR,OUTDR,BSHR,BCR,LCKR; } GPIO_Registers;
typedef struct { uint32_t CTLR,CFGR0,INTR,APB2PRSTR,APB1PRSTR,AHBPCENR,APB2PCENR,APB1PCENR,BDCTLR,RSTSCKR,AHBRSTR,CFGR2; } RCC_Registers;
typedef struct { uint16_t CTLR1,pad0,CTLR2,pad1,STATR,pad2,DATAR,pad3,CRCR,pad4,RCRCR,pad5,TCRCR,pad6,I2SCFGR,pad7,I2SPR,pad8; } SPI_Registers;
typedef struct { uint16_t STATR,pad0,DATAR,pad1,BRR,pad2,CTLR1,pad3,CTLR2,pad4,CTLR3,pad5,GPR,pad6; } USART_Registers;
typedef struct { uint32_t CFGR,CNTR,PADDR,MADDR; } DMA_Channel_Registers;
typedef struct { uint16_t pin,pad;uint32_t speed,mode; } GPIO_Init_Recovered;
typedef struct { uint32_t baud;uint16_t word_length,stop_bits,parity,mode,flow_control,pad; } USART_Init_Recovered;
typedef struct { uint16_t direction,mode,data_size,clock_polarity,clock_phase,nss,prescaler,first_bit,crc_polynomial; } SPI_Init_Recovered;
typedef struct { uint32_t peripheral_address,memory_address,direction,count,peripheral_increment,memory_increment,peripheral_width,memory_width,circular,priority,memory_to_memory; } DMA_Init_Recovered;
typedef struct { uint8_t first,second,status_or_segments0,packed_segments[4],unit_flags0,unit_flags1,unknown[4]; } Measurement_Wire13;
typedef struct { uint16_t metadata,magnitude; } History_Record4;
typedef enum { Event_PC10=1,Event_PC11=2,Event_PC9=3,Event_PC12=4,Event_PC8_Short=5,Event_PC8_Long=6,Event_PC2=8 } Button_Event;

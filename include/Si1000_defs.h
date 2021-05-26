//-----------------------------------------------------------------------------
// Si1000_defs.h
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Register/bit definitions for the Si100x family.
//
//
// Target:         Si100x
// Tool chain:     Keil, SDCC
// Command Line:   None
//
//
// Release 1.0
//    -EZRadioPRO definitions created by Ken Berrenger.
//    -26 JAN 2010  (FB)
//

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------

#ifndef INCLUDE_SI1000_DEFS
#define INCLUDE_SI1000_DEFS

#include <stdbool.h>

//-----------------------------------------------------------------------------
// Byte Registers
//-----------------------------------------------------------------------------

__sfr __at(0x80) P0;        // Port 0 Latch
__sfr __at(0x81) SP;        // Stack Pointer
__sfr __at(0x82) DPL;       // Data Pointer Low
__sfr __at(0x83) DPH;       // Data Pointer High
__sfr __at(0x84) SPI1CFG;   // SPI1 Configuration
__sfr __at(0x85) SPI1CKR;   // SPI1 Clock Rate Control
__sfr __at(0x85) TOFFL;     // Temperature Offset Low
__sfr __at(0x86) SPI1DAT;   // SPI1 Data
__sfr __at(0x86) TOFFH;     // Temperature Offset High
__sfr __at(0x87) PCON;      // Power Control
__sfr __at(0x88) TCON;      // Timer/Counter Control
__sfr __at(0x89) TMOD;      // Timer/Counter Mode
__sfr __at(0x8A) TL0;       // Timer/Counter 0 Low
__sfr __at(0x8B) TL1;       // Timer/Counter 1 Low
__sfr __at(0x8C) TH0;       // Timer/Counter 0 High
__sfr __at(0x8D) TH1;       // Timer/Counter 1 High
__sfr __at(0x8E) CKCON;     // Clock Control
__sfr __at(0x8F) PSCTL;     // Program Store R/W Control
__sfr __at(0x90) P1;        // Port 1 Latch
__sfr __at(0x91) TMR3CN;    // Timer/Counter 3 Control
__sfr __at(0x91) CRC0DAT;   // CRC0 Data
__sfr __at(0x92) TMR3RLL;   // Timer/Counter 3 Reload Low
__sfr __at(0x92) CRC0CN;    // CRC0 Control
__sfr __at(0x93) TMR3RLH;   // Timer/Counter 3 Reload High
__sfr __at(0x93) CRC0IN;    // CRC0 Input
__sfr __at(0x94) TMR3L;     // Timer/Counter 3 Low
__sfr __at(0x95) CRC0FLIP;  // CRC0 Flip
__sfr __at(0x95) TMR3H;     // Timer/Counter 3 High
__sfr __at(0x96) DC0CF;     // DC0 (DC/DC Converter) Configuration
__sfr __at(0x96) CRC0AUTO;  // CRC0 Automatic Control
__sfr __at(0x97) DC0CN;     // DC0 (DC/DC Converter) Control
__sfr __at(0x97) CRC0CNT;   // CRC0 Automatic Flash Sector Count
__sfr __at(0x98) SCON0;     // UART0 Control
__sfr __at(0x99) SBUF0;     // UART0 Data Buffer
__sfr __at(0x9A) CPT1CN;    // Comparator1 Control
__sfr __at(0x9B) CPT0CN;    // Comparator0 Control
__sfr __at(0x9C) CPT1MD;    // Comparator1 Mode Selection
__sfr __at(0x9D) CPT0MD;    // Comparator0 Mode Selection
__sfr __at(0x9E) CPT1MX;    // Comparator1 Mux Selection
__sfr __at(0x9F) CPT0MX;    // Comparator0 Mux Selection
__sfr __at(0xA0) P2;        // Port 2 Latch
__sfr __at(0xA1) SPI0CFG;   // SPI0 Configuration
__sfr __at(0xA2) SPI0CKR;   // SPI0 Clock Rate Control
__sfr __at(0xA3) SPI0DAT;   // SPI0 Data
__sfr __at(0xA4) P0MDOUT;   // Port 0 Output Mode Configuration
__sfr __at(0xA4) P0DRV;     // Port 0 Drive Strength
__sfr __at(0xA5) P1MDOUT;   // Port 1 Output Mode Configuration
__sfr __at(0xA5) P1DRV;     // Port 1 Drive Strength
__sfr __at(0xA6) P2MDOUT;   // Port 2 Output Mode Configuration
__sfr __at(0xA6) P2DRV;     // Port 2 Drive Strength
__sfr __at(0xA7) SFRPAGE;   // SFR Page
__sfr __at(0xA8) IE;        // Interrupt Enable
__sfr __at(0xA9) CLKSEL;    // Clock Select
__sfr __at(0xAA) EMI0CN;    // EMIF Control
__sfr __at(0xAB) EMI0CF;    // EMIF Configuration
__sfr __at(0xAC) RTC0ADR;   // RTC0 Address
__sfr __at(0xAD) RTC0DAT;   // RTC0 Data
__sfr __at(0xAE) RTC0KEY;   // RTC0 Key
__sfr __at(0xAF) EMI0TC;    // EMIF Timing Control
__sfr __at(0xAF) ONESHOT;   // ONESHOT Timing Control
__sfr __at(0xB0) SPI1CN;    // SPI1 Control
__sfr __at(0xB1) OSCXCN;    // External Oscillator Control
__sfr __at(0xB2) OSCICN;    // Internal Oscillator Control
__sfr __at(0xB3) OSCICL;    // Internal Oscillator Calibration
__sfr __at(0xB5) PMU0CF;    // PMU0 Configuration
__sfr __at(0xB6) FLSCL;     // Flash Scale Register
__sfr __at(0xB7) FLKEY;     // Flash Lock And Key
__sfr __at(0xB8) IP;        // Interrupt Priority
__sfr __at(0xB9) IREF0CN;   // Current Reference IREF0 Control
__sfr __at(0xBA) ADC0AC;    // ADC0 Accumulator Configuration
__sfr __at(0xBA) ADC0PWR;   // ADC0 Burst Mode Power-Up Time
__sfr __at(0xBB) ADC0MX;    // AMUX0 Channel Select
__sfr __at(0xBC) ADC0CF;    // ADC0 Configuration
__sfr __at(0xBD) ADC0TK;    // ADC0 Tracking Control
__sfr __at(0xBD) ADC0L;     // ADC0 Low
__sfr __at(0xBE) ADC0H;     // ADC0 High
__sfr __at(0xBF) P1MASK;    // Port 1 Mask
__sfr __at(0xC0) SMB0CN;    // SMBus0 Control
__sfr __at(0xC1) SMB0CF;    // SMBus0 Configuration
__sfr __at(0xC2) SMB0DAT;   // SMBus0 Data
__sfr __at(0xC3) ADC0GTL;   // ADC0 Greater-Than Compare Low
__sfr __at(0xC4) ADC0GTH;   // ADC0 Greater-Than Compare High
__sfr __at(0xC5) ADC0LTL;   // ADC0 Less-Than Compare Word Low
__sfr __at(0xC6) ADC0LTH;   // ADC0 Less-Than Compare Word High
__sfr __at(0xC7) P0MASK;    // Port 0 Mask
__sfr __at(0xC8) TMR2CN;    // Timer/Counter 2 Control
__sfr __at(0xC9) REG0CN;    // Voltage Regulator (REG0) Control
__sfr __at(0xCA) TMR2RLL;   // Timer/Counter 2 Reload Low
__sfr __at(0xCB) TMR2RLH;   // Timer/Counter 2 Reload High
__sfr __at(0xCC) TMR2L;     // Timer/Counter 2 Low
__sfr __at(0xCD) TMR2H;     // Timer/Counter 2 High
__sfr __at(0xCE) PCA0CPM5;  // PCA0 Module 5 Mode Register
__sfr __at(0xCF) P1MAT;     // Port 1 Match
__sfr __at(0xD0) PSW;       // Program Status Word
__sfr __at(0xD1) REF0CN;    // Voltage Reference Control
__sfr __at(0xD2) PCA0CPL5;  // PCA0 Capture 5 Low
__sfr __at(0xD3) PCA0CPH5;  // PCA0 Capture 5 High
__sfr __at(0xD4) P0SKIP;    // Port 0 Skip
__sfr __at(0xD5) P1SKIP;    // Port 1 Skip
__sfr __at(0xD6) P2SKIP;    // Port 2 Skip
__sfr __at(0xD7) P0MAT;     // Port 0 Match
__sfr __at(0xD8) PCA0CN;    // PCA0 Control
__sfr __at(0xD9) PCA0MD;    // PCA0 Mode
__sfr __at(0xDA) PCA0CPM0;  // PCA0 Module 0 Mode Register
__sfr __at(0xDB) PCA0CPM1;  // PCA0 Module 1 Mode Register
__sfr __at(0xDC) PCA0CPM2;  // PCA0 Module 2 Mode Register
__sfr __at(0xDD) PCA0CPM3;  // PCA0 Module 3 Mode Register
__sfr __at(0xDE) PCA0CPM4;  // PCA0 Module 4 Mode Register
__sfr __at(0xDF) PCA0PWM;   // PCA0 PWM Configuration
__sfr __at(0xE0) ACC;       // Accumulator
__sfr __at(0xE1) XBR0;      // Port I/O Crossbar Control 0
__sfr __at(0xE2) XBR1;      // Port I/O Crossbar Control 1
__sfr __at(0xE3) XBR2;      // Port I/O Crossbar Control 2
__sfr __at(0xE4) IT01CF;    // INT0/INT1 Configuration
__sfr __at(0xE5) FLWR;      // Flash Write Only Register
__sfr __at(0xE6) EIE1;      // Extended Interrupt Enable 1
__sfr __at(0xE7) EIE2;      // Extended Interrupt Enable 2
__sfr __at(0xE8) ADC0CN;    // ADC0 Control
__sfr __at(0xE9) PCA0CPL1;  // PCA0 Capture 1 Low
__sfr __at(0xEA) PCA0CPH1;  // PCA0 Capture 1 High
__sfr __at(0xEB) PCA0CPL2;  // PCA0 Capture 2 Low
__sfr __at(0xEC) PCA0CPH2;  // PCA0 Capture 2 High
__sfr __at(0xED) PCA0CPL3;  // PCA0 Capture 3 Low
__sfr __at(0xEE) PCA0CPH3;  // PCA0 Capture 3 High
__sfr __at(0xEF) RSTSRC;    // Reset Source Configuration/Status
__sfr __at(0xF0) B;         // B Register
__sfr __at(0xF1) P0MDIN;    // Port 0 Input Mode Configuration
__sfr __at(0xF2) P1MDIN;    // Port 1 Input Mode Configuration
__sfr __at(0xF3) P2MDIN;    // Port 2 Input Mode Configuration
__sfr __at(0xF4) SMB0ADR;   // SMBus Slave Address
__sfr __at(0xF5) SMB0ADM;   // SMBus Slave Address Mask
__sfr __at(0xF6) EIP1;      // Extended Interrupt Priority 1
__sfr __at(0xF7) EIP2;      // Extended Interrupt Priority 2
__sfr __at(0xF8) SPI0CN;    // SPI0 Control
__sfr __at(0xF9) PCA0L;     // PCA0 Counter Low
__sfr __at(0xFA) PCA0H;     // PCA0 Counter High
__sfr __at(0xFB) PCA0CPL0;  // PCA0 Capture 0 Low
__sfr __at(0xFC) PCA0CPH0;  // PCA0 Capture 0 High
__sfr __at(0xFD) PCA0CPL4;  // PCA0 Capture 4 Low
__sfr __at(0xFE) PCA0CPH4;  // PCA0 Capture 4 High
__sfr __at(0xFF) VDM0CN;    // VDD Monitor Control

//-----------------------------------------------------------------------------
// 16-bit Register Definitions (might not be supported by all compilers)
//-----------------------------------------------------------------------------

__sfr16 __at(((0x82 + 1U) << 8) | 0x82) DP;      // Data Pointer
__sfr16 __at(((0x85 + 1U) << 8) | 0x85) TOFF;    // Temperature Sensor Offset
__sfr16 __at(((0x92 + 1U) << 8) | 0x92) TMR3RL;  // Timer 3 Reload
__sfr16 __at(((0x94 + 1U) << 8) | 0x94) TMR3;    // Timer 3 Counter
__sfr16 __at(((0xBD + 1U) << 8) | 0xBD) ADC0;    // ADC0 Data
__sfr16 __at(((0xC3 + 1U) << 8) | 0xC3) ADC0GT;  // ADC0 Greater-Than Compare
__sfr16 __at(((0xC5 + 1U) << 8) | 0xC5) ADC0LT;  // ADC0 Less-Than Compare
__sfr16 __at(((0xCA + 1U) << 8) | 0xCA) TMR2RL;  // Timer 2 Reload
__sfr16 __at(((0xCC + 1U) << 8) | 0xCC) TMR2;    // Timer 2 Counter
__sfr16 __at(((0xD2 + 1U) << 8) |
             0xD2) PCA0CP5;  // PCA0 Module 5 Capture/Compare
__sfr16 __at(((0xE9 + 1U) << 8) |
             0xE9) PCA0CP1;  // PCA0 Module 1 Capture/Compare
__sfr16 __at(((0xEB + 1U) << 8) |
             0xEB) PCA0CP2;  // PCA0 Module 2 Capture/Compare
__sfr16 __at(((0xED + 1U) << 8) |
             0xED) PCA0CP3;                    // PCA0 Module 3 Capture/Compare
__sfr16 __at(((0xF9 + 1U) << 8) | 0xF9) PCA0;  // PCA0 Counter
__sfr16 __at(((0xFB + 1U) << 8) |
             0xFB) PCA0CP0;  // PCA0 Module 0 Capture/Compare
__sfr16 __at(((0xFD + 1U) << 8) |
             0xFD) PCA0CP4;  // PCA0 Module 4 Capture/Compare

//-----------------------------------------------------------------------------
// Indirect RTC Register Addresses
//-----------------------------------------------------------------------------

#define CAPTURE0 0x00  // RTC address of CAPTURE0 register
#define CAPTURE1 0x01  // RTC address of CAPTURE1 register
#define CAPTURE2 0x02  // RTC address of CAPTURE2 register
#define CAPTURE3 0x03  // RTC address of CAPTURE3 register
#define RTC0CN 0x04    // RTC address of RTC0CN register
#define RTC0XCN 0x05   // RTC address of RTC0XCN register
#define RTC0XCF 0x06   // RTC address of RTC0XCF register
#define RTC0PIN 0x07   // RTC address of RTC0PIN register
#define ALARM0 0x08    // RTC address of ALARM0 register
#define ALARM1 0x09    // RTC address of ALARM1 register
#define ALARM2 0x0A    // RTC address of ALARM2 register
#define ALARM3 0x0B    // RTC address of ALARM3 register

//-----------------------------------------------------------------------------
// Address Definitions for Bit-addressable Registers
//-----------------------------------------------------------------------------

#define SFR_P0 0x80
#define SFR_TCON 0x88
#define SFR_P1 0x90
#define SFR_CRC0CN 0x92
#define SFR_SCON0 0x98
#define SFR_P2 0xA0
#define SFR_IE 0xA8
#define SFR_SPI1CN 0xB0
#define SFR_IP 0xB8
#define SFR_SMB0CN 0xC0
#define SFR_TMR2CN 0xC8
#define SFR_PSW 0xD0
#define SFR_PCA0CN 0xD8
#define SFR_ACC 0xE0
#define SFR_ADC0CN 0xE8
#define SFR_B 0xF0
#define SFR_SPI0CN 0xF8

//-----------------------------------------------------------------------------
// Bit Definitions
//-----------------------------------------------------------------------------

// TCON 0x88
__sbit __at(SFR_TCON + 7) TF1;  // Timer 1 Overflow Flag
__sbit __at(SFR_TCON + 6) TR1;  // Timer 1 On/Off Control
__sbit __at(SFR_TCON + 5) TF0;  // Timer 0 Overflow Flag
__sbit __at(SFR_TCON + 4) TR0;  // Timer 0 On/Off Control
__sbit __at(SFR_TCON + 3) IE1;  // Ext. Interrupt 1 Edge Flag
__sbit __at(SFR_TCON + 2) IT1;  // Ext. Interrupt 1 Type
__sbit __at(SFR_TCON + 1) IE0;  // Ext. Interrupt 0 Edge Flag
__sbit __at(SFR_TCON + 0) IT0;  // Ext. Interrupt 0 Type

// CRC0CN 0x92
__sbit __at(SFR_CRC0CN + 4) CRC0SEL;
__sbit __at(SFR_CRC0CN + 3) CRC0INIT;
__sbit __at(SFR_CRC0CN + 2) CRC0VAL;

// SCON0 0x98
__sbit __at(SFR_SCON0 + 7) S0MODE;  // UART0 Mode
                                    // Bit6 UNUSED
__sbit __at(SFR_SCON0 + 5) MCE0;    // UART0 MCE
__sbit __at(SFR_SCON0 + 4) REN0;    // UART0 RX Enable
__sbit __at(SFR_SCON0 + 3) TB80;    // UART0 TX Bit 8
__sbit __at(SFR_SCON0 + 2) RB80;    // UART0 RX Bit 8
__sbit __at(SFR_SCON0 + 1) TI0;     // UART0 TX Interrupt Flag
__sbit __at(SFR_SCON0 + 0) RI0;     // UART0 RX Interrupt Flag

// IE 0xA8
__sbit __at(SFR_IE + 7) EA;     // Global Interrupt Enable
__sbit __at(SFR_IE + 6) ESPI0;  // SPI0 Interrupt Enable
__sbit __at(SFR_IE + 5) ET2;    // Timer 2 Interrupt Enable
__sbit __at(SFR_IE + 4) ES0;    // UART0 Interrupt Enable
__sbit __at(SFR_IE + 3) ET1;    // Timer 1 Interrupt Enable
__sbit __at(SFR_IE + 2) EX1;    // External Interrupt 1 Enable
__sbit __at(SFR_IE + 1) ET0;    // Timer 0 Interrupt Enable
__sbit __at(SFR_IE + 0) EX0;    // External Interrupt 0 Enable

// SPI1CN 0xB0
__sbit __at(SFR_SPI1CN + 7) SPIF1;    // SPI1 Interrupt Flag
__sbit __at(SFR_SPI1CN + 6) WCOL1;    // SPI1 Write Collision Flag
__sbit __at(SFR_SPI1CN + 5) MODF1;    // SPI1 Mode Fault Flag
__sbit __at(SFR_SPI1CN + 4) RXOVRN1;  // SPI1 RX Overrun Flag
__sbit __at(SFR_SPI1CN + 3) NSS1MD1;  // SPI1 Slave Select Mode 1
__sbit __at(SFR_SPI1CN + 2) NSS1MD0;  // SPI1 Slave Select Mode 0
__sbit __at(SFR_SPI1CN + 1) TXBMT1;   // SPI1 TX Buffer Empty Flag
__sbit __at(SFR_SPI1CN + 0) SPI1EN;   // SPI1 Enable

// IP 0xB8
// Bit7 UNUSED
__sbit __at(SFR_IP + 6) PSPI0;  // SPI0 Priority
__sbit __at(SFR_IP + 5) PT2;    // Timer 2 Priority
__sbit __at(SFR_IP + 4) PS0;    // UART0 Priority
__sbit __at(SFR_IP + 3) PT1;    // Timer 1 Priority
__sbit __at(SFR_IP + 2) PX1;    // External Interrupt 1 Priority
__sbit __at(SFR_IP + 1) PT0;    // Timer 0 Priority
__sbit __at(SFR_IP + 0) PX0;    // External Interrupt 0 Priority

// SMB0CN 0xC0
__sbit __at(SFR_SMB0CN + 7) MASTER;   // SMBus0 Master/Slave
__sbit __at(SFR_SMB0CN + 6) TXMODE;   // SMBus0 Transmit Mode
__sbit __at(SFR_SMB0CN + 5) STA;      // SMBus0 Start Flag
__sbit __at(SFR_SMB0CN + 4) STO;      // SMBus0 Stop Flag
__sbit __at(SFR_SMB0CN + 3) ACKRQ;    // SMBus0 Acknowledge Request
__sbit __at(SFR_SMB0CN + 2) ARBLOST;  // SMBus0 Arbitration Lost
__sbit __at(SFR_SMB0CN + 1) ACK;      // SMBus0 Acknowledge Flag
__sbit __at(SFR_SMB0CN + 0) SI;       // SMBus0 Interrupt Pending Flag

// TMR2CN 0xC8
__sbit __at(SFR_TMR2CN + 7) TF2H;     // Timer 2 High Byte Overflow Flag
__sbit __at(SFR_TMR2CN + 6) TF2L;     // Timer 2 Low Byte Overflow Flag
__sbit __at(SFR_TMR2CN + 5) TF2LEN;   // Timer 2 Low Byte Interrupt Enable
__sbit __at(SFR_TMR2CN + 4) TF2CEN;   // Timer 2 Lfo Capture Enable
__sbit __at(SFR_TMR2CN + 3) T2SPLIT;  // Timer 2 Split Mode Enable
__sbit __at(SFR_TMR2CN + 2) TR2;      // Timer 2 On/Off Control
__sbit __at(SFR_TMR2CN + 1) T2RCLK;   // Timer 2 Capture Mode
__sbit __at(SFR_TMR2CN + 0) T2XCLK;   // Timer 2 External Clock Select

// PSW 0xD0
__sbit __at(SFR_PSW + 7) CY;   // Carry Flag
__sbit __at(SFR_PSW + 6) AC;   // Auxiliary Carry Flag
__sbit __at(SFR_PSW + 5) F0;   // User Flag 0
__sbit __at(SFR_PSW + 4) RS1;  // Register Bank Select 1
__sbit __at(SFR_PSW + 3) RS0;  // Register Bank Select 0
__sbit __at(SFR_PSW + 2) OV;   // Overflow Flag
__sbit __at(SFR_PSW + 1) F1;   // User Flag 1
__sbit __at(SFR_PSW + 0) P;    // Accumulator Parity Flag

// PCA0CN 0xD8
__sbit __at(SFR_PCA0CN + 7) CF;    // PCA0 Counter Overflow Flag
__sbit __at(SFR_PCA0CN + 6) CR;    // PCA0 Counter Run Control Bit
__sbit __at(SFR_PCA0CN + 5) CCF5;  // PCA0 Module 5 Interrupt Flag
__sbit __at(SFR_PCA0CN + 4) CCF4;  // PCA0 Module 4 Interrupt Flag
__sbit __at(SFR_PCA0CN + 3) CCF3;  // PCA0 Module 3 Interrupt Flag
__sbit __at(SFR_PCA0CN + 2) CCF2;  // PCA0 Module 2 Interrupt Flag
__sbit __at(SFR_PCA0CN + 1) CCF1;  // PCA0 Module 1 Interrupt Flag
__sbit __at(SFR_PCA0CN + 0) CCF0;  // PCA0 Module 0 Interrupt Flag

// ADC0CN 0xE8
__sbit __at(SFR_ADC0CN + 7) AD0EN;    // ADC0 Enable
__sbit __at(SFR_ADC0CN + 6) BURSTEN;  // ADC0 Burst Enable
__sbit __at(SFR_ADC0CN + 5) AD0INT;   // ADC0 EOC Interrupt Flag
__sbit __at(SFR_ADC0CN + 4) AD0BUSY;  // ADC0 Busy Flag
__sbit __at(SFR_ADC0CN + 3) AD0WINT;  // ADC0 Window Interrupt Flag
__sbit __at(SFR_ADC0CN + 2) AD0CM2;   // ADC0 Convert Start Mode Bit 2
__sbit __at(SFR_ADC0CN + 1) AD0CM1;   // ADC0 Convert Start Mode Bit 1
__sbit __at(SFR_ADC0CN + 0) AD0CM0;   // ADC0 Convert Start Mode Bit 0

// SPI0CN 0xF8
__sbit __at(SFR_SPI0CN + 7) SPIF0;    // SPI0 Interrupt Flag
__sbit __at(SFR_SPI0CN + 6) WCOL0;    // SPI0 Write Collision Flag
__sbit __at(SFR_SPI0CN + 5) MODF0;    // SPI0 Mode Fault Flag
__sbit __at(SFR_SPI0CN + 4) RXOVRN0;  // SPI0 RX Overrun Flag
__sbit __at(SFR_SPI0CN + 3) NSS0MD1;  // SPI0 Slave Select Mode 1
__sbit __at(SFR_SPI0CN + 2) NSS0MD0;  // SPI0 Slave Select Mode 0
__sbit __at(SFR_SPI0CN + 1) TXBMT0;   // SPI0 TX Buffer Empty Flag
__sbit __at(SFR_SPI0CN + 0) SPI0EN;   // SPI0 Enable

//-----------------------------------------------------------------------------
// Interrupt Priorities
//-----------------------------------------------------------------------------

#define INTERRUPT_INT0 0            // External Interrupt 0
#define INTERRUPT_TIMER0 1          // Timer0 Overflow
#define INTERRUPT_INT1 2            // External Interrupt 1
#define INTERRUPT_TIMER1 3          // Timer1 Overflow
#define INTERRUPT_UART0 4           // Serial Port 0
#define INTERRUPT_TIMER2 5          // Timer2 Overflow
#define INTERRUPT_SPI0 6            // Serial Peripheral Interface 0
#define INTERRUPT_SMBUS0 7          // SMBus0 Interface
#define INTERRUPT_RTC0ALARM 8       // RTC0 (SmaRTClock) Alarm
#define INTERRUPT_ADC0_WINDOW 9     // ADC0 Window Comparison
#define INTERRUPT_ADC0_EOC 10       // ADC0 End Of Conversion
#define INTERRUPT_PCA0 11           // PCA0 Peripheral
#define INTERRUPT_COMPARATOR0 12    // Comparator0
#define INTERRUPT_COMPARATOR1 13    // Comparator1
#define INTERRUPT_TIMER3 14         // Timer3 Overflow
#define INTERRUPT_VDDMON 15         // VDD Monitor Early Warning
#define INTERRUPT_PORT_MATCH 16     // Port Match
#define INTERRUPT_RTC0_OSC_FAIL 17  // RTC0 (smaRTClock) Osc. Fail
#define INTERRUPT_SPI1 18           // Serial Peripheral Interface 1

//-----------------------------------------------------------------------------
// SFR Page Definitions
//-----------------------------------------------------------------------------
#define CONFIG_PAGE 0x0F  // SYSTEM AND PORT CONFIGURATION PAGE
#define LEGACY_PAGE 0x00  // LEGACY SFR PAGE
#define CRC0_PAGE 0x0F    // CRC0
#define TOFF_PAGE 0x0F    // TEMPERATURE SENSOR OFFSET PAGE

//=============================================================================
//
// EZRadioPRO Register Defines
//
// The register names are defined exactly as listed in the Function/
// Description field of the Si1000 data sheet. Upper Case is used for
// constants. Spaces and miscellaneous characters are replaced with
// underscores. EZRADIOPRO_ prefix added for Si1000.
//
//=============================================================================
#define EZRADIOPRO_DEVICE_TYPE 0x00
#define EZRADIOPRO_DEVICE_VERSION 0x01
#define EZRADIOPRO_DEVICE_STATUS 0x02
#define EZRADIOPRO_INTERRUPT_STATUS_1 0x03
#define EZRADIOPRO_INTERRUPT_STATUS_2 0x04
#define EZRADIOPRO_INTERRUPT_ENABLE_1 0x05
#define EZRADIOPRO_INTERRUPT_ENABLE_2 0x06
#define EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_1 0x07
#define EZRADIOPRO_OPERATING_AND_FUNCTION_CONTROL_2 0x08
#define EZRADIOPRO_CRYSTAL_OSCILLATOR_LOAD_CAPACITANCE 0x09
#define EZRADIOPRO_MICROCONTROLLER_OUTPUT_CLOCK 0x0A
#define EZRADIOPRO_GPIO0_CONFIGURATION 0x0B
#define EZRADIOPRO_GPIO1_CONFIGURATION 0x0C
#define EZRADIOPRO_GPIO2_CONFIGURATION 0x0D
#define EZRADIOPRO_IO_PORT_CONFIGURATION 0x0E
#define EZRADIOPRO_ADC_CONFIGURATION 0x0F
#define EZRADIOPRO_ADC_SENSOR_AMPLIFIER_OFFSET 0x10
#define EZRADIOPRO_ADC_VALUE 0x11
#define EZRADIOPRO_TEMPERATURE_SENSOR_CONTROL 0x12
#define EZRADIOPRO_TEMPERATURE_VALUE_OFFSET 0x13
#define EZRADIOPRO_WAKE_UP_TIMER_PERIOD_1 0x14
#define EZRADIOPRO_WAKE_UP_TIMER_PERIOD_2 0x15
#define EZRADIOPRO_WAKE_UP_TIMER_PERIOD_3 0x16
#define EZRADIOPRO_WAKE_UP_TIMER_VALUE_1 0x17
#define EZRADIOPRO_WAKE_UP_TIMER_VALUE_2 0x18
#define EZRADIOPRO_LOW_DUTY_CYCLE_MODE_DURATION 0x19
#define EZRADIOPRO_LOW_BATTERY_DETECTOR_THRESHOLD 0x1A
#define EZRADIOPRO_BATTERY_VOLTAGE_LEVEL 0x1B
#define EZRADIOPRO_IF_FILTER_BANDWIDTH 0x1C
#define EZRADIOPRO_AFC_LOOP_GEARSHIFT_OVERRIDE 0x1D
#define EZRADIOPRO_AFC_TIMING_CONTROL 0x1E
#define EZRADIOPRO_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE 0x1F
#define EZRADIOPRO_CLOCK_RECOVERY_OVERSAMPLING_RATIO 0x20
#define EZRADIOPRO_CLOCK_RECOVERY_OFFSET_2 0x21
#define EZRADIOPRO_CLOCK_RECOVERY_OFFSET_1 0x22
#define EZRADIOPRO_CLOCK_RECOVERY_OFFSET_0 0x23
#define EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1 0x24
#define EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0 0x25
#define EZRADIOPRO_RECEIVED_SIGNAL_STRENGTH_INDICATOR 0x26
#define EZRADIOPRO_RSSI_THRESHOLD 0x27
#define EZRADIOPRO_ANTENNA_DIVERSITY_REGISTER_1 0x28
#define EZRADIOPRO_ANTENNA_DIVERSITY_REGISTER_2 0x29
#define EZRADIOPRO_DATA_ACCESS_CONTROL 0x30
#define EZRADIOPRO_EZMAC_STATUS 0x31
#define EZRADIOPRO_HEADER_CONTROL_1 0x32
#define EZRADIOPRO_HEADER_CONTROL_2 0x33
#define EZRADIOPRO_PREAMBLE_LENGTH 0x34
#define EZRADIOPRO_PREAMBLE_DETECTION_CONTROL 0x35
#define EZRADIOPRO_SYNC_WORD_3 0x36
#define EZRADIOPRO_SYNC_WORD_2 0x37
#define EZRADIOPRO_SYNC_WORD_1 0x38
#define EZRADIOPRO_SYNC_WORD_0 0x39
#define EZRADIOPRO_TRANSMIT_HEADER_3 0x3A
#define EZRADIOPRO_TRANSMIT_HEADER_2 0x3B
#define EZRADIOPRO_TRANSMIT_HEADER_1 0x3C
#define EZRADIOPRO_TRANSMIT_HEADER_0 0x3D
#define EZRADIOPRO_TRANSMIT_PACKET_LENGTH 0x3E
#define EZRADIOPRO_CHECK_HEADER_3 0x3F
#define EZRADIOPRO_CHECK_HEADER_2 0x40
#define EZRADIOPRO_CHECK_HEADER_1 0x41
#define EZRADIOPRO_CHECK_HEADER_0 0x42
#define EZRADIOPRO_HEADER_ENABLE_3 0x43
#define EZRADIOPRO_HEADER_ENABLE_2 0x44
#define EZRADIOPRO_HEADER_ENABLE_1 0x45
#define EZRADIOPRO_HEADER_ENABLE_0 0x46
#define EZRADIOPRO_RECEIVED_HEADER_3 0x47
#define EZRADIOPRO_RECEIVED_HEADER_2 0x48
#define EZRADIOPRO_RECEIVED_HEADER_1 0x49
#define EZRADIOPRO_RECEIVED_HEADER_0 0x4A
#define EZRADIOPRO_RECEIVED_PACKET_LENGTH 0x4B
#define EZRADIOPRO_ANALOG_TEST_BUS 0x50
#define EZRADIOPRO_DIGITAL_TEST_BUS 0x51
#define EZRADIOPRO_TX_RAMP_CONTROL 0x52
#define EZRADIOPRO_PLL_TUNE_TIME 0x53
#define EZRADIOPRO_CALIBRATION_CONTROL 0x55
#define EZRADIOPRO_MODEM_TEST 0x56
#define EZRADIOPRO_CHARGEPUMP_TEST 0x57
#define EZRADIOPRO_CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE 0x58
#define EZRADIOPRO_DIVIDER_CURRENT_TRIMMING 0x59
#define EZRADIOPRO_VCO_CURRENT_TRIMMING 0x5A
#define EZRADIOPRO_VCO_CALIBRATION_OVERRIDE 0x5B
#define EZRADIOPRO_SYNTHESIZER_TEST 0x5C
#define EZRADIOPRO_BLOCK_ENABLE_OVERRIDE_1 0x5D
#define EZRADIOPRO_BLOCK_ENABLE_OVERRIDE_2 0x5E
#define EZRADIOPRO_BLOCK_ENABLE_OVERRIDE_3 0x5F
#define EZRADIOPRO_CHANNEL_FILTER_COEFFICIENT_ADDRESS 0x60
#define EZRADIOPRO_CHANNEL_FILTER_COEFFICIENT_VALUE 0x61
#define EZRADIOPRO_CRYSTAL_OSCILLATOR_CONTROL_TEST 0x62
#define EZRADIOPRO_RC_OSCILLATOR_COARSE_CALIBRATION_OVERRIDE 0x63
#define EZRADIOPRO_RC_OSCILLATOR_FINE_CALIBRATION_OVERRIDE 0x64
#define EZRADIOPRO_LDO_CONTROL_OVERRIDE 0x65
#define EZRADIOPRO_LDO_LEVEL_SETTING 0x66
#define EZRADIOPRO_DELTASIGMA_ADC_TUNING_1 0x67
#define EZRADIOPRO_DELTASIGMA_ADC_TUNING_2 0x68
#define EZRADIOPRO_AGC_OVERRIDE_1 0x69
#define EZRADIOPRO_AGC_OVERRIDE_2 0x6A
#define EZRADIOPRO_GFSK_FIR_FILTER_COEFFICIENT_ADDRESS 0x6B
#define EZRADIOPRO_GFSK_FIR_FILTER_COEFFICIENT_VALUE 0x6C
#define EZRADIOPRO_TX_POWER 0x6D
#define EZRADIOPRO_TX_DATA_RATE_1 0x6E
#define EZRADIOPRO_TX_DATA_RATE_0 0x6F
#define EZRADIOPRO_MODULATION_MODE_CONTROL_1 0x70
#define EZRADIOPRO_MODULATION_MODE_CONTROL_2 0x71
#define EZRADIOPRO_FREQUENCY_DEVIATION 0x72
#define EZRADIOPRO_FREQUENCY_OFFSET_1 0x73
#define EZRADIOPRO_FREQUENCY_OFFSET_2 0x74
#define EZRADIOPRO_FREQUENCY_BAND_SELECT 0x75
#define EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_1 0x76
#define EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_0 0x77
#define EZRADIOPRO_FREQUENCY_HOPPING_CHANNEL_SELECT 0x79
#define EZRADIOPRO_FREQUENCY_HOPPING_STEP_SIZE 0x7A
#define EZRADIOPRO_TX_FIFO_CONTROL_1 0x7C
#define EZRADIOPRO_TX_FIFO_CONTROL_2 0x7D
#define EZRADIOPRO_RX_FIFO_CONTROL 0x7E
#define EZRADIOPRO_FIFO_ACCESS 0x7F
//------------------------------------------------------------------------------------------------
// new registers for B1 radio
#define EZRADIOPRO_AFC_LIMITER 0x2A
#define EZRADIOPRO_AFC_CORRECTION 0x2B
#define EZRADIOPRO_OOK_COUNTER_VALUE_1 0x2C
#define EZRADIOPRO_OOK_COUNTER_VALUE_2 0X2D
#define EZRADIOPRO_SLICER_PEAK_HOLD 0X2E
#define EZRADIOPRO_ADC8_CONTROL 0x4F
#define EZRADIOPRO_INVALID_PREAMBLE_THRESHOLD_AND_PA_MISC 0x54
#define EZRADIOPRO_MISCELLANEOUS_SETTINGS 0x78
#define EZRADIOPRO_TURN_AROUND_AND_15_4_LENGTH_COMPLIANCE 0x7B
//=============================================================================
//
// Register Bit Masks
//
//=============================================================================
// EZRADIOPRO_DEVICE_TYPE                                      0x00
#define EZRADIOPRO_DT_MASK 0x1F
// EZRADIOPRO_DEVICE_VERSION                                   0x01
#define EZRADIOPRO_VC_MASK 0x1F
// EZRADIOPRO_DEVICE_STATUS                                    0x02
#define EZRADIOPRO_CPS_MASK 0x03
#define EZRADIOPRO_LOCKDET 0x04
#define EZRADIOPRO_FREQERR 0x08
#define EZRADIOPRO_HEADERR 0x10
#define EZRADIOPRO_RXFFEM 0x20
#define EZRADIOPRO_FFUNFL 0x40
#define EZRADIOPRO_FFOVFL 0x80

// EZRADIOPRO_INTERRUPT_STATUS_1                               0x03
#define EZRADIOPRO_ICRCERROR 0x01
#define EZRADIOPRO_IPKVALID 0x02
#define EZRADIOPRO_IPKSENT 0x04
#define EZRADIOPRO_IEXT 0x08
#define EZRADIOPRO_IRXFFAFULL 0x10
#define EZRADIOPRO_ITXFFAEM 0x20
#define EZRADIOPRO_ITXFFAFULL 0x40
#define EZRADIOPRO_IFFERR 0x80

// EZRADIOPRO_INTERRUPT_STATUS_2                               0x04
#define EZRADIOPRO_IPOR 0x01
#define EZRADIOPRO_ICHIPRDY 0x02
#define EZRADIOPRO_ILBD 0x04
#define EZRADIOPRO_IWUT 0x08
#define EZRADIOPRO_IRSSI 0x10
#define EZRADIOPRO_IPREAINVAL 0x20
#define EZRADIOPRO_IPREAVAL 0x40
#define EZRADIOPRO_ISWDET 0x80

// EZRADIOPRO_INTERRUPT_ENABLE_1                               0x05
#define EZRADIOPRO_ENCRCERROR 0x01
#define EZRADIOPRO_ENPKVALID 0x02
#define EZRADIOPRO_ENPKSENT 0x04
#define EZRADIOPRO_ENEXT 0x08
#define EZRADIOPRO_ENRXFFAFULL 0x10
#define EZRADIOPRO_ENTXFFAEM 0x20
#define EZRADIOPRO_ENTXFFAFULL 0x40
#define EZRADIOPRO_ENFFERR 0x80

// EZRADIOPRO_INTERRUPT_ENABLE_2                               0x06
#define EZRADIOPRO_ENPOR 0x01
#define EZRADIOPRO_ENCHIPRDY 0x02
#define EZRADIOPRO_ENLBDI 0x04  // added I to make unique
#define EZRADIOPRO_ENWUT 0x08
#define EZRADIOPRO_ENRSSI 0x10
#define EZRADIOPRO_ENPREAINVAL 0x20
#define EZRADIOPRO_ENPREAVAL 0x40
#define EZRADIOPRO_ENSWDET 0x80

// EZRADIOPRO_OPERATING_FUNCTION_CONTROL_1                     0x07
#define EZRADIOPRO_XTON 0x01
#define EZRADIOPRO_PLLON 0x02
#define EZRADIOPRO_RXON 0x04
#define EZRADIOPRO_TXON 0x08
#define EZRADIOPRO_X32KSEL 0x10
#define EZRADIOPRO_ENWT 0x20
#define EZRADIOPRO_ENLBD 0x40
#define EZRADIOPRO_SWRES 0x80

// EZRADIOPRO_OPERATING_FUNCTION_CONTROL_2                     0x08
#define EZRADIOPRO_FFCLRTX 0x01
#define EZRADIOPRO_FFCLRRX 0x02
#define EZRADIOPRO_ENLDM 0x04
#define EZRADIOPRO_AUTOTX 0x08
#define EZRADIOPRO_RXMPK 0x10
#define EZRADIOPRO_ANTDIV_MASK 0xE0

// EZRADIOPRO_CRYSTAL_OSCILLATOR_LOAD_CAPACITANCE              0x09
#define EZRADIOPRO_XLC_MASK 0x7F
#define EZRADIOPRO_XTALSHFT 0x80

// EZRADIOPRO_MICROCONTROLLER_OUTPUT_CLOCK                     0x0A
#define EZRADIOPRO_MCLK_MASK 0x07
#define EZRADIOPRO_ENLFC 0x08
#define EZRADIOPRO_CLKT_MASK 0x30

// EZRADIOPRO_GPIO0_CONFIGURATION                              0x0B
#define EZRADIOPRO_GPIO0_MASK 0x1F
#define EZRADIOPRO_PUP0 0x20
#define EZRADIOPRO_GPIO0DRV_MASK 0xC0

// EZRADIOPRO_GPIO1_CONFIGURATION                              0x0C
#define EZRADIOPRO_GPIO1_MASK 0x1F
#define EZRADIOPRO_PUP1 0x20
#define EZRADIOPRO_GPIO1DRV_MASK 0xC0

// EZRADIOPRO_GPIO2_CONFIGURATION                              0x0D
#define EZRADIOPRO_GPIO2_MASK 0x1F
#define EZRADIOPRO_PUP2 0x20
#define EZRADIOPRO_GPIO2DRV_MASK 0xC0

// EZRADIOPRO_IO_PORT_CONFIGURATION                            0x0E
#define EZRADIOPRO_DIO_MASK 0x07
#define EZRADIOPRO_ITSDO 0x08
#define EZRADIOPRO_EXTITST_MASK 0x70

// EZRADIOPRO_ADC_CONFIGURATION                                0x0F
#define EZRADIOPRO_ADCGAIN_MASK 0x03
#define EZRADIOPRO_ADCREF_MASK 0x0C
#define EZRADIOPRO_ADCSEL_MASK 0x70
#define EZRADIOPRO_ADCSTART 0x80  // W
#define EZRADIOPRO_ADCDONE 0x80   // R

// EZRADIOPRO_ADC_SENSOR_AMPLIFIER_OFFSET                      0x10
#define EZRADIOPRO_ADCOFFS_MASK 0x0F

// EZRADIOPRO_ADC_VALUE                                        0x11
// no bits or mask

// EZRADIOPRO_TEMPERATURE_SENSOR_CONTROL                       0x12
#define EZRADIOPRO_TSTRIM_MASK 0x0F
#define EZRADIOPRO_ENTSTRIM 0x10
#define EZRADIOPRO_ENTSOFFS 0x20
#define EZRADIOPRO_TSRANGE_MASK 0xC0

// EZRADIOPRO_TEMPERATURE_VALUE_OFFSET                         0x13
// no bits or mask

// EZRADIOPRO_WAKE_UP_TIMER_PERIOD_1                           0x14
#define EZRADIOPRO_WTD_MASK 0x03
#define EZRADIOPRO_WTR_MASK 0x3C

// EZRADIOPRO_WAKE_UP_TIMER_PERIOD_2                           0x15
// no bits or mask

// EZRADIOPRO_WAKE_UP_TIMER_PERIOD_3                           0x16
#define EZRADIOPRO_WTM_MASK 0x80

// EZRADIOPRO_WAKE_UP_TIMER_VALUE_1                            0x17
// no bits or mask

// EZRADIOPRO_WAKE_UP_TIMER_VALUE_2                            0x18
// no bits or mask

// EZRADIOPRO_LOW_DUTY_CYCLE_MODE_DURATION                     0x19
// no bits or mask

// EZRADIOPRO_LOW_BATTERY_DETECTOR_THRESHOLD                   0x1A
#define EZRADIOPRO_LBDT_MASK 0x1F

// EZRADIOPRO_BATTERY_VOLTAGE_LEVEL                            0x1B
#define EZRADIOPRO_VBAT_MASK 0x1F

// EZRADIOPRO_I_F_FILTER_BANDWIDTH                             0x1C
#define EZRADIOPRO_FILSET_MASK 0x0F
#define EZRADIOPRO_NDEC_MASK 0x70
#define EZRADIOPRO_DWN3_BYPASS 0x80

// EZRADIOPRO_A_F_C_LOOP_GEARSHIFT_OVERRIDE                    0x1D
#define EZRADIOPRO_AFCGEARH_MASK 0x3F
#define EZRADIOPRO_ENAFC 0x40
#define EZRADIOPRO_AFCBD 0x80

// EZRADIOPRO_A_F_C_TIMING_CONTROL                             0x1E
#define EZRADIOPRO_LGWAIT_MASK 0x07
#define EZRADIOPRO_SHWAIT_MASK 0x38

// EZRADIOPRO_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE                0x1F
#define EZRADIOPRO_CRSLOW_MASK 0x07
#define EZRADIOPRO_CRFAST_MASK 0x38
#define EZRADIOPRO_RXREADY 0x40

// EZRADIOPRO_CLOCK_RECOVERY_OVERSAMPLING_RATIO                0x20
// no bits or mask

// EZRADIOPRO_CLOCK_RECOVERY_OFFSET_2 0x21
#define EZRADIOPRO_NCOFF_MASK 0x0F
#define EZRADIOPRO_STALLCTRL 0x10
#define EZRADIOPRO_RXOSR_MASK 0xE0

// EZRADIOPRO_CLOCK_RECOVERY_OFFSET_1                          0x22
// no bits or mask

// EZRADIOPRO_CLOCK_RECOVERY_OFFSET_0                          0x23
// no bits or mask

// EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1                0x24
#define EZRADIOPRO_CRGAIN_MASK 0x07

// EZRADIOPRO_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0                0x25
// no bits or mask

// EZRADIOPRO_RECEIVED_SIGNAL_STRENGTH_INDICATOR               0x26
// no bits or mask

// EZRADIOPRO_RSSI_THRESHOLD                                   0x27
// no bits or mask

// EZRADIOPRO_ANTENNA_DIVERSITY_REGISTER_1                     0x28
// no bits or mask

// EZRADIOPRO_ANTENNA_DIVERSITY_REGISTER_2                     0x29
// no bits or mask

// EZRADIOPRO_DATA_ACCESS_CONTROL                              0x30
#define EZRADIOPRO_CRC_MASK 0x03
#define EZRADIOPRO_CRC_16 0x01
#define EZRADIOPRO_ENCRC 0x04
#define EZRADIOPRO_ENPACTX 0x08
#define EZRADIOPRO_CRCDONLY 0x20
#define EZRADIOPRO_LSBFRST 0x40
#define EZRADIOPRO_ENPACRX 0x80

// EZRADIOPRO_EZ_MAC_STATUS                                    0x31
#define EZRADIOPRO_PKSENT 0x01
#define EZRADIOPRO_PKTX 0x02
#define EZRADIOPRO_CRCERROR 0x04
#define EZRADIOPRO_PKVALID 0x08
#define EZRADIOPRO_PKRX 0x10
#define EZRADIOPRO_PKSRCH 0x20
#define EZRADIOPRO_RXCRC1 0x40

// EZRADIOPRO_HEADER_CONTROL_1                                 0x32
#define EZRADIOPRO_HDCH_MASK 0x0F
#define EZRADIOPRO_BCEN_MASK 0xF0
#define EZRADIOPRO_BCEN 0xF0
#define EZRADIOPRO_DISABLE_HFILTERS 0x00

// EZRADIOPRO_HEADER_CONTROL_2                                 0x33
#define EZRADIOPRO_PREALEN_MASK 0x01
#define EZRADIOPRO_SYNCLEN_MASK 0x06
#define EZRADIOPRO_FIXPKLEN 0x08
#define EZRADIOPRO_HDLEN_MASK 0x70
#define EZRADIOPRO_SYNCLEN_1BYTE 0x00
#define EZRADIOPRO_SYNCLEN_2BYTE 0x02
#define EZRADIOPRO_SYNCLEN_3BYTE 0x04
#define EZRADIOPRO_SYNCLEN_4BYTE 0x06
#define EZRADIOPRO_HDLEN_0BYTE 0x00
#define EZRADIOPRO_HDLEN_1BYTE 0x10
#define EZRADIOPRO_HDLEN_2BYTE 0x20
#define EZRADIOPRO_HDLEN_3BYTE 0x30
#define EZRADIOPRO_HDLEN_4BYTE 0x40
#define EZRADIOPRO_SKIPSYN 0x80

// EZRADIOPRO_PREAMBLE_LENGTH                                  0x34
// no bits or mask

// EZRADIOPRO_SYNC_WORD_3                                      0x36
// no bits or mask

// EZRADIOPRO_SYNC_WORD_2                                      0x37
// no bits or mask

// EZRADIOPRO_SYNC_WORD_1                                      0x38
// no bits or mask

// EZRADIOPRO_SYNC_WORD_0                                      0x39
// no bits or mask

// EZRADIOPRO_TRANSMIT_HEADER_3                                0x3A
// no bits or mask

// EZRADIOPRO_TRANSMIT_HEADER_2                                0x3B
// no bits or mask

// EZRADIOPRO_TRANSMIT_HEADER_1                                0x3C
// no bits or mask

// EZRADIOPRO_TRANSMIT_HEADER_0                                0x3D
// no bits or mask

// EZRADIOPRO_TRANSMIT_PACKET_LENGTH                           0x3E
// no bits or mask

// EZRADIOPRO_CHECK_HEADER_3                                   0x3F
// no bits or mask

// EZRADIOPRO_CHECK_HEADER_2                                   0x40
// no bits or mask

// EZRADIOPRO_CHECK_HEADER_1                                   0x41
// no bits or mask

// EZRADIOPRO_CHECK_HEADER_0                                   0x42
// no bits or mask

// EZRADIOPRO_HEADER_ENABLE_3                                  0x43
// no bits or mask

// EZRADIOPRO_HEADER_ENABLE_2                                  0x44
// no bits or mask

// EZRADIOPRO_HEADER_ENABLE_1                                  0x45
// no bits or mask

// EZRADIOPRO_HEADER_ENABLE_0                                  0x46
// no bits or mask

// EZRADIOPRO_RECEIVED_HEADER_3                                0x47
// no bits or mask

// EZRADIOPRO_RECEIVED_HEADER_2                                0x48
// no bits or mask

// EZRADIOPRO_RECEIVED_HEADER_1                                0x49
// no bits or mask

// EZRADIOPRO_RECEIVED_HEADER_0                                0x4A
// no bits or mask

// EZRADIOPRO_RECEIVED_PACKET_LENGTH                           0x4B
// no bits or mask

// EZRADIOPRO_ANALOG_TEST_BUS                                  0x50
#define EZRADIOPRO_ATB_MASK 0x1F

// EZRADIOPRO_DIGITAL_TEST_BUS                                 0x51
#define EZRADIOPRO_DTB_MASK 0x2F
#define EZRADIOPRO_ENSCTEST 0x40

// EZRADIOPRO_TX_RAMP_CONTROL                                  0x52
#define EZRADIOPRO_TXRAMP_MASK 0x03
#define EZRADIOPRO_LDORAMP_MASK 0x0C
#define EZRADIOPRO_TXMOD_MASK 0x70

// EZRADIOPRO_PLL_TUNE_TIME                                    0x53
#define EZRADIOPRO_PLLT0_MASK 0x07
#define EZRADIOPRO_PLLTS_MASK 0xF8

// EZRADIOPRO_CALIBRATION_CONTROL                              0x55
#define EZRADIOPRO_SKIPVCO 0x01
#define EZRADIOPRO_VCOCAL 0x02
#define EZRADIOPRO_VCOCALDP 0x04
#define EZRADIOPRO_RCCAL 0x08
#define EZRADIOPRO_ENRCFCAL 0x10
#define EZRADIOPRO_ADCCALDONE 0x20
#define EZRADIOPRO_XTALSTARTHALF 0x40

// EZRADIOPRO_MODEM_TEST                                       0x56
#define EZRADIOPRO_IQSWITCH 0x01
#define EZRADIOPRO_REFCLKINV 0x02
#define EZRADIOPRO_REFCLKSEL 0x04
#define EZRADIOPRO_AFCPOL 0x10
#define EZRADIOPRO_DTTYPE 0x20
#define EZRADIOPRO_SLICFBYP 0x40
#define EZRADIOPRO_BCRFBYP 0x80

// EZRADIOPRO_CHARGEPUMP_TEST                                  0x57
#define EZRADIOPRO_CDCCUR_MASK 0x07
#define EZRADIOPRO_CDCONLY 0x08
#define EZRADIOPRO_CPFORCEDN 0x10
#define EZRADIOPRO_CPFORCEUP 0x20
#define EZRADIOPRO_FBDIV_RST 0x40
#define EZRADIOPRO_PFDRST 0x80

// EZRADIOPRO_CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE             0x58
#define EZRADIOPRO_CPCORR_MASK 0x1F
#define EZRADIOPRO_CPCORROV 0x20
#define EZRADIOPRO_CPCURR_MASK 0xC0

// EZRADIOPRO_DIVIDER_CURRENT_TRIMMING                         0x59
#define EZRADIOPRO_D1P5TRIM_MASK 0x03
#define EZRADIOPRO_D2TRIM_MASK 0x0C
#define EZRADIOPRO_D3TRIM_MASK 0x30
#define EZRADIOPRO_FBDIVHC 0x40
#define EZRADIOPRO_TXCORBOOSTEN 0x80

// EZRADIOPRO_VCO_CURRENT_TRIMMING                             0x5A
#define EZRADIOPRO_VCOCUR_MASK 0x03
#define EZRADIOPRO_VCOCORR_MASK 0x3C
#define EZRADIOPRO_VCOCORROV 0x40
#define EZRADIOPRO_TXCURBOOSTEN 0x80

// EZRADIOPRO_VCO_CALIBRATION_OVERRIDE                         0x5B
#define EZRADIOPRO_VCOCAL_MASK 0x7F
#define EZRADIOPRO_VCOCALOV 0x80  // W
#define EZRADIOPRO_VCDONE 0x80    // R

// EZRADIOPRO_SYNTHESIZER_TEST                                 0x5C
#define EZRADIOPRO_DSRST 0x01
#define EZRADIOPRO_DSRSTMOD 0x02
#define EZRADIOPRO_DSORDER_MASK 0x0C
#define EZRADIOPRO_DSMOD 0x10
#define EZRADIOPRO_ENOLOOP 0x20
#define EZRADIOPRO_VCOTYPE 0x40
#define EZRADIOPRO_DSMDT 0x80

// EZRADIOPRO_BLOCK_ENABLE_OVERRIDE_1                          0x5D
#define EZRADIOPRO_ENMX2 0x01
#define EZRADIOPRO_ENBF12 0x02
#define EZRADIOPRO_ENDV32 0x04
#define EZRADIOPRO_ENBF5 0x08
#define EZRADIOPRO_ENPA 0x10
#define EZRADIOPRO_ENPGA 0x20
#define EZRADIOPRO_ENLNA 0x40
#define EZRADIOPRO_ENMIX 0x80

// EZRADIOPRO_BLOCK_ENABLE_OVERRIDE_2                          0x5E
#define EZRADIOPRO_PLLRESET 0x01
#define EZRADIOPRO_ENBF2 0x02
#define EZRADIOPRO_ENBF11 0x04
#define EZRADIOPRO_ENBF3 0x08
#define EZRADIOPRO_ENBF4 0x10
#define EZRADIOPRO_ENMX3 0x20
#define EZRADIOPRO_ENLDET 0x40
#define EZRADIOPRO_ENDS 0x80

// EZRADIOPRO_BLOCK_ENABLE_OVERRIDE_3                          0x5F
#define EZRADIOPRO_ENBG 0x01
#define EZRADIOPRO_ENCP 0x02
#define EZRADIOPRO_ENVCO 0x04
#define EZRADIOPRO_DVBSHUNT 0x08
#define EZRADIOPRO_ENDV1P5 0x10
#define EZRADIOPRO_ENDV2 0x20
#define EZRADIOPRO_ENDV31 0x40
#define EZRADIOPRO_ENFRDV 0x80

// EZRADIOPRO_CHANNEL_FILTER_COEFFICIENT_ADDRESS               0x60
#define EZRADIOPRO_CHFILADD_MASK 0x0F

// EZRADIOPRO_CHANNEL_FILTER_COEFFICIENT_VALUE                 0x61
#define EZRADIOPRO_CHFILVAL_MASK 0x3F

// EZRADIOPRO_CRYSTAL_OSCILLATOR_CONTROL_TEST                  0x62
#define EZRADIOPRO_ENBUF 0x01
#define EZRADIOPRO_BUFOVR 0x02
#define EZRADIOPRO_ENAMP2X 0x04
#define EZRADIOPRO_ENBIAS2X 0x08
#define EZRADIOPRO_CLKHYST 0x10
#define EZRADIOPRO_PWST_MASK 0xE0

// EZRADIOPRO_RC_OSCILLATOR_COARSE_CALIBRATION_OVERRIDE        0x63
#define EZRADIOPRO_RCC_MASK 0x7F
#define EZRADIOPRO_RCCOV 0x80

// EZRADIOPRO_RC_OSCILLATOR_FINE_CALIBRATION_OVERRIDE          0x64
#define EZRADIOPRO_RCF_MASK 0x7F
#define EZRADIOPRO_RCFOV 0x80

// EZRADIOPRO_LDO_CONTROL_OVERRIDE   0x65
#define EZRADIOPRO_ENDIGPWDN 0x01
#define EZRADIOPRO_ENDIGLDO 0x02
#define EZRADIOPRO_ENPLLLDO 0x04
#define EZRADIOPRO_ENRFLDO 0x08
#define EZRADIOPRO_ENIFLDO 0x10
#define EZRADIOPRO_ENVCOLDO 0x20
#define EZRADIOPRO_ENBIAS 0x40
#define EZRADIOPRO_ENSPOR 0x80

// EZRADIOPRO_LDO_LEVEL_SETTING   0x66
#define EZRADIOPRO_DIGLVL_MASK 0x07
#define EZRADIOPRO_ENRC32 0x10
#define EZRADIOPRO_ENTS 0x20
#define EZRADIOPRO_ENXTAL 0x40
#define EZRADIOPRO_ENOVR 0x80

// EZRADIOPRO_DELTASIGMA_ADC_TUNING_1                          0x67
#define EZRADIOPRO_ADCTUNE_MASK 0x0F
#define EZRADIOPRO_ADCTUNEOVR 0x10
#define EZRADIOPRO_ENADC 0x20
#define EZRADIOPRO_ENREFDAC 0x40
#define EZRADIOPRO_ADCRST 0x80

// EZRADIOPRO_DELTASIGMA_ADC_TUNING_2                          0x68
#define EZRADIOPRO_DSADCREF_MASK 0x07  // added DS to make unique
#define EZRADIOPRO_ADCOLOOP 0x08
#define EZRADIOPRO_ENVCM 0x10

// EZRADIOPRO_AGC_OVERRIDE_1                                   0x69
#define EZRADIOPRO_PGA_MASK 0x0F
#define EZRADIOPRO_LNAGAIN 0x10
#define EZRADIOPRO_AGCEN 0x20

// EZRADIOPRO_AGC_OVERRIDE_2   0x6A
#define EZRADIOPRO_PGATH_MASK 0x03
#define EZRADIOPRO_LNACOMP_MASK 0x3F
#define EZRADIOPRO_AGCSLOW 0x40
#define EZRADIOPRO_AGCOVPM 0x80

// EZRADIOPRO_GFSK_FIR_FILTER_COEFFICIENT_ADDRESS              0x6B
#define EZRADIOPRO_FIRADD_MASK 0x07

// EZRADIOPRO_GFSK_FIR_FILTER_COEFFICIENT_VALUE                0x6C
#define EZRADIOPRO_FIRVAL_MASK 0x3F

// EZRADIOPRO_TX_POWER   0x6D
#define EZRADIOPRO_TXPOW_MASK 0x07

// EZRADIOPRO_TX_DATA_RATE_1                                   0x6E
// no bits or mask

// EZRADIOPRO_TX_DATA_RATE_0                                   0x6F
// no bits or mask

// EZRADIOPRO_MODULATION_MODE_CONTROL_1                        0x70
#define EZRADIOPRO_ENWHITE 0x01
#define EZRADIOPRO_ENMANCH 0x02
#define EZRADIOPRO_ENMANINV 0x04
#define EZRADIOPRO_MANPPOL 0x08
#define EZRADIOPRO_ENPHPWDN 0x10
#define EZRADIOPRO_TXDTRTSCALE 0x20

// EZRADIOPRO_MODULATION_MODE_CONTROL_2                        0x71
#define EZRADIOPRO_MODTYP_MASK 0x03
#define EZRADIOPRO_FD_MASK 0x04
#define EZRADIOPRO_ENINV 0x08
#define EZRADIOPRO_DTMOD_MASK 0x30
#define EZRADIOPRO_TRCLK_MASK 0xC0
#define EZRADIOPRO_MODTYP_GFSK 0x03
#define EZRADIOPRO_FIFO_MODE 0x20
#define EZRADIOPRO_TX_DATA_CLK_GPIO 0x40

// EZRADIOPRO_FREQUENCY_DEVIATION                              0x72
// no bits or mask

// EZRADIOPRO_FREQUENCY_OFFSET_1                               0x73
// no bits or mask

// EZRADIOPRO_FREQUENCY_OFFSET_2                               0x74
#define EZRADIOPRO_FO_MASK 0x03

// EZRADIOPRO_FREQUENCY_BAND_SELECTRESERVED                    0x75
#define EZRADIOPRO_FB_MASK 0x1F
#define EZRADIOPRO_HBSEL 0x20
#define EZRADIOPRO_SBSEL 0x40

// EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_1                      0x76
// no bits or mask

// EZRADIOPRO_NOMINAL_CARRIER_FREQUENCY_0                      0x77
// no bits or mask

// EZRADIOPRO_FREQUENCY_HOPPING_CHANNEL_SELECT                 0x79
// no bits or mask

// EZRADIOPRO_FREQUENCY_HOPPING_STEP_SIZE                      0x7A
// no bits or mask

// EZRADIOPRO_TX_FIFO_CONTROL_1                                0x7C
#define EZRADIOPRO_TXAFTHR_MASK 0x3F

// EZRADIOPRO_TX_FIFO_CONTROL_2                                0x7D
#define EZRADIOPRO_TXAETHR_MASK 0x3F

// EZRADIOPRO_RX_FIFO_CONTROL                                  0x7E
#define EZRADIOPRO_RXAFTHR_MASK 0x3F
#define EZRADIOPRO_RESERVED 0x40

// EZRADIOPRO_FIFO_ACCESS                                      0x7F
// no bits or mask

//-----------------------------------------------------------------------------
// Header File PreProcessor Directive
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------

#endif /* INCLUDE_SI1000_DEFS */

/**
 * @file main.c
 * @author metrichf
 * @date 2025-02-12
 * @brief Main function
 */

/* ======= Includes ========*/
#include <stdint.h> /* For uint8_t definition */
#include <xc.h>

/* ======== Define and Pragma Directives   ======== */

// CONFIG  PIC12F675 Configuration Bit Settings
#pragma config FOSC =                                                          \
    INTRCIO // Oscillator Selection bits (INTOSC oscillator: I/O function on
            // GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF  // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF // MCLR
#pragma config BOREN = OFF // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP =                                                            \
    OFF // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD =                                                           \
    OFF // Data Code Protection bit (Data memory code protection is disabled)

/*define clock freq*/
#define _XTAL_FREQ 4000000 // 4MHZ internal crystal

// Constants
#define cModeFreq1K 1
#define cModeFreq2K 2
#define cModeFreq5K 3
#define cModeFreq10K 4
#define cModeFreqVar 5

// Variables
unsigned int i;
unsigned char iMode;
unsigned int wIn;
#define In_Mode GP3
#define Out_Osc_1 GP1
#define Out_Osc_2 GP2
#define Out_Led_1 GP4
#define Out_Led_2 GP5

void Osc_1KHz() {
  Out_Led_1 = 1;
  Out_Led_2 = 0;
  for (i = 0; i < 1000; ++i) {
    Out_Osc_1 = !Out_Osc_1;
    Out_Osc_2 = !Out_Osc_1;
    __delay_us(480);
  }
}

void Osc_2KHz() {
  Out_Led_1 = 1;
  Out_Led_2 = 0;
  for (i = 0; i < 2000; ++i) {
    Out_Osc_1 = !Out_Osc_1;
    Out_Osc_2 = !Out_Osc_1;
    __delay_us(230);
  }
}

void Osc_5KHz() {
  Out_Led_1 = 0;
  Out_Led_2 = 1;
  for (i = 0; i < 5000; ++i) {
    Out_Osc_1 = !Out_Osc_1;
    Out_Osc_2 = !Out_Osc_1;
    __delay_us(81);
  }
}

void Osc_10KHz() {
  Out_Led_1 = 0;
  Out_Led_2 = 1;
  for (i = 0; i < 10000; ++i) {
    Out_Osc_1 = !Out_Osc_1;
    Out_Osc_2 = !Out_Osc_1;
    __delay_us(30);
  }
}

void Osc_Var() {
  Osc_1KHz();
  __delay_us(10);
  Osc_2KHz();
  __delay_us(10);
  Osc_5KHz();
  __delay_us(10);
  Osc_10KHz();
  __delay_us(10);
}

void setup() {
  // Initialize hardware settings
  CMCON = 0x07; // Comparators OFF
  OPTION_REG = 0;
  OPTION_REGbits.nGPPU = 1; // Disable pullup

  INTCON = 0;
  IOC = 0;

  TRISIO = 0x09;
  TRISIObits.TRISIO0 = 1;
  TRISIObits.TRISIO1 = 0;
  TRISIObits.TRISIO2 = 0;
  TRISIObits.TRISIO3 = 1; // Always input
  TRISIObits.TRISIO4 = 0;
  TRISIObits.TRISIO5 = 0;

  ANSEL = 0;          // AN0 only enabled (last 4 bits are enable bits {AN3:AN0}
  ANSELbits.ANS0 = 1; // Set as analog input
  ANSELbits.ANS1 = 0; // Set as digital I/O
  ANSELbits.ANS2 = 0; // Set as digital I/O
  ANSELbits.ANS3 = 0; // Set as digital I/O

  ADCON0 = 0x01; // ADC enabled, channel 0 selected
  __delay_ms(100);
}

unsigned int ADC_Get_Sample(unsigned char channel) {
  ADCON0bits.CHS = channel; // Select ADC channel
  __delay_us(5);            // Acquisition time
  ADCON0bits.GO_nDONE = 1;  // Start conversion
  while (ADCON0bits.GO_nDONE)
    ;                              // Wait for conversion to complete
  return ((ADRESH << 8) + ADRESL); // Return result
}

void getMode() {
  wIn = ADC_Get_Sample(0);
  if (wIn < 200) {
    iMode = cModeFreqVar;
  } else if (wIn < 400) {
    iMode = cModeFreq10K;
  } else if (wIn < 600) {
    iMode = cModeFreq5K;
  } else if (wIn < 800) {
    iMode = cModeFreq2K;
  } else {
    iMode = cModeFreq1K;
  }
}

int main() {
  setup();
  while (1) {
    getMode();
    switch (iMode) {
    case cModeFreq1K:
      Osc_1KHz();
      break;
    case cModeFreq2K:
      Osc_2KHz();
      break;
    case cModeFreq5K:
      Osc_5KHz();
      break;
    case cModeFreq10K:
      Osc_10KHz();
      break;
    case cModeFreqVar:
      Osc_Var();
      break;
    }
  }
};

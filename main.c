/* 10-30-13
 * ADC Conversion code via EC 440 code sample (Prof. Giles)
*/

#include "msp430g2553.h"
#include "LM335_tools.h"

// Define bit masks for ADC pin and channel used as P1.4
#define ADC_INPUT_BIT_MASK 0x10
#define ADC_INCH INCH_4

 /* declarations of functions defined later */
 void init_adc(void);
 void init_wdt(void);

// =======ADC Initialization and Interrupt Handler========

// Global variables that store the results (read from the debugger)
volatile int latest_result;   // most recent result is stored in latest_result
volatile unsigned long conversion_count=0; //total number of conversions done
volatile float fahrenheit;
/*
 * The ADC handler is invoked when a conversion is complete.
 * Its action here is to store the result in memory.
 * and possibly print.
 */
 void interrupt adc_handler(){
	 latest_result=ADC10MEM;   // store the answer
	 ++conversion_count;       // increment the total conversion count

	 fahrenheit = getFahrenheit(latest_result, 3.55);
 }
 ISR_VECTOR(adc_handler, ".int05")

// Initialization of the ADC
void init_adc(){
  ADC10CTL1= ADC_INCH	//input channel 4
 			  +SHS_0 //use ADC10SC bit to trigger sampling
 			  +ADC10DIV_4 // ADC10 clock/5
 			  +ADC10SSEL_0 // Clock Source=ADC10OSC
 			  +CONSEQ_0; // single channel, single conversion
 			  ;
  ADC10AE0=ADC_INPUT_BIT_MASK; // enable A4 analog input
  ADC10CTL0= SREF_0	//reference voltages are Vss and Vcc
 	          +ADC10SHT_3 //64 ADC10 Clocks for sample and hold time (slowest)
 	          +ADC10ON	//turn on ADC10
 	          +ENC		//enable (but not yet start) conversions
 	          +ADC10IE  //enable interrupts
 	          ;
}



// ===== Watchdog Timer Interrupt Handler =====
interrupt void WDT_interval_handler(){
		ADC10CTL0 |= ADC10SC;  // trigger a conversion
}
ISR_VECTOR(WDT_interval_handler, ".int10")

 void init_wdt_no_interrupt(){
	// setup the watchdog timer as an interval timer
	// INTERRUPT NOT YET ENABLED!
  	WDTCTL =(WDTPW +		// (bits 15-8) password
     	                   	// bit 7=0 => watchdog timer on
       	                 	// bit 6=0 => NMI on rising edge (not used here)
                        	// bit 5=0 => RST/NMI pin does a reset (not used here)
           	 WDTTMSEL +     // (bit 4) select interval timer mode
  		     WDTCNTCL  		// (bit 3) clear watchdog timer counter
  		                	// bit 2=0 => SMCLK is the source
  		                	// bits 1-0 = 00 => source/32K
 			 );
 }

/*
 * The main program just initializes everything and leaves the action to
 * the interrupt handlers!
 */

void main(){

	WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer
	BCSCTL1 = CALBC1_8MHZ;			// 8Mhz calibration for clock
  	DCOCTL  = CALDCO_8MHZ;

  	init_adc();
  	init_wdt_no_interrupt(); // does NOT enable the wdt interrupt yet
  	_bis_SR_register(GIE); //enable interrupts so we can print.

    IE1 |= WDTIE; // enable WDT interrupt so that conversions can start

  	_bis_SR_register(LPM0_bits); //powerdown CPU

}

// fuse settings:
// avrdude -c dragon_hvsp -p attiny85 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -P usb


#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


#define LED PORTB1

#define set(x) |= (1<<x)
#define clr(x) &=~(1<<x) 
#define inv(x) ^=(1<<x)
#define chk(var,pos) ((var) & (1<<(pos)))

uint8_t sleepCount = 0;

int main() {
  // do nothing for a little while...
  _delay_ms(5000);
  
  
  // disable interrupts...
  cli();
  
  // reset the 'why the cpu was reset' flag...
  MCUSR = 0;

  // disable and clear watchdog settings a la ATtiny25/45/85 [DATASHEET] p. 46
	WDTCR = (1<<WDCE) | (1<<WDE);
	WDTCR = 0;
  
  // set interrupt time to about 1 second and enable...
	WDTCR = (1<<WDIE) | (1<<WDP2) | (1<<WDP1);
  
  // enable sleep mode for power down...
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  // enable interrupts...
  sei();
  
  // setup port b and turn off led...
  DDRB set(LED);
  PORTB clr(LED);
  
  for(;;) {
    // loop here...
    
    // go to sleep - sleep_cpu() requires more code to enable and disable sleep...
    sleep_mode();
  }
  
}


ISR(WDT_vect) {
  // blink led...
  PORTB inv(LED);
}


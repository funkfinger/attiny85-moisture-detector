// fuse settings:
// avrdude -c dragon_hvsp -p attiny85 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -P usb


#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


#define LED PORTB1
#define SPEAKER PORTB3

#define set(x) |= (1<<x)
#define clr(x) &=~(1<<x) 
#define inv(x) ^=(1<<x)
#define chk(var,pos) ((var) & (1<<(pos)))

uint8_t awakeCounter = 0;

// _delay_ms can't take a var? so this is here...
void delay_ms(uint16_t count) {
  while(count--) {
    _delay_ms(1);
  }
}

// reduce power requirements and go to sleep...
void nightyNight() {
  // disable adc...
  power_adc_disable();
  
  // go to sleep - sleep_cpu() requires more code to enable and disable sleep...
  sleep_mode();
  
}

void beepOn(uint8_t length) {
  PORTB set(SPEAKER);
  delay_ms(length);
  PORTB clr(SPEAKER);
}

void turnItUp() {
  // wait a sec before doing this...
  _delay_ms(1000);
  // four short beeps just to tell you I'm on...
  for(int i = 0 ; i<4 ; i++ ) {
    beepOn(50);
    _delay_ms(100);
  }
}

int main() {
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

  // setup port b for speaker and turn off...
  DDRB set(SPEAKER);
  PORTB clr(SPEAKER);

  
  // play turn on sound...
  turnItUp();
  
  for(;;) {
    // loop here...
    nightyNight();
  }
  
}


ISR(WDT_vect) {
  awakeCounter++;
  if(awakeCounter > 10) {
    awakeCounter = 1;
    PORTB inv(LED);
  }
}


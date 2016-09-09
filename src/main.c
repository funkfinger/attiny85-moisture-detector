// fuse settings:
// avrdude -c dragon_hvsp -p attiny85 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -P usb


#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


#define SPEAKER PORTB3
#define MOIST_CHK PORTB1

#define set(x) |= (1<<x)
#define clr(x) &=~(1<<x) 
#define inv(x) ^=(1<<x)

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

void checkMoist() {
  if(bit_is_clear(PINB, MOIST_CHK)) {
    awakeCounter = 6; // force to beep again...
    beepOn(50);
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
    
  // moisture sensor as an input...
  DDRB clr(MOIST_CHK);
  PORTB set(MOIST_CHK);

  // setup port b for speaker and turn off...
  DDRB set(SPEAKER);
  PORTB clr(SPEAKER);

  
  // play turn on sound...
  turnItUp();
  
  for(;;) {
    // loop here...
    nightyNight();
    
    if(awakeCounter > 5) {
      checkMoist();
    }

  }
  
}


ISR(WDT_vect) {
  awakeCounter++;
}


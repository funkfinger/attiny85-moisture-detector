# attiny85-moisture-detector
got a leak, need an alarm....

#### ATTiny85 fuse settings:
    avrdude -c dragon_hvsp -p attiny85 -U lfuse:w:0x62:m -U hfuse:w:0xcf:m -U efuse:w:0xff:m -P usb
    
This is basically what I'm doing - [https://learn.sparkfun.com/tutorials/h2ohno/](https://learn.sparkfun.com/tutorials/h2ohno/) - but why not re-invent a wheel...


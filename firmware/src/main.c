/*
 * main.c
 *
 *  Created on: 10 de mai de 2020
 *      Author: raffaello
 */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "usbdrv/usbdrv.h"
#include "simple_spi.h"
#include "rgb_led_driver.h"

/*
 * ATTINY85 @ 16MHz
 * */

/* USB report descriptor, size must match usbconfig.h USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH */
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x09, 0x05,                    // USAGE (Game Pad)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x09, 0x01,                    //   USAGE (Pointer)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x30,                    //     USAGE (X)
	0x09, 0x31,                    //     USAGE (Y)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x95, 0x02,                    //   REPORT_COUNT (2)
	0x81, 0x02,                    //   INPUT (Data, Var, Abs)
	0xc0,                          // END_COLLECTION
	0x05, 0x09,                    // USAGE_PAGE (Button)
	0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
	0x29, 0x08,                    //   USAGE_MAXIMUM (Button 8)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    // REPORT_SIZE (1)
	0x95, 0x08,                    // REPORT_COUNT (8)
	0x81, 0x02,                    // INPUT (Data, Var, Abs)
	0xc0                           // END_COLLECTION
};

static uchar report_buffer[3] = {0x80, 0x80, 0x00}; //initial positions
static uchar idleRate;   /* repeat rate for keyboards, never used for mice */
static volatile uint16_t last_signal = 0x0000;
static volatile uchar timer_trigger_counter = 0;
static volatile uchar has_changed = 0;

static void buildReport() {
	uchar directions = (uchar) (0x000f & last_signal);
	report_buffer[0] = 0x80; 										//center x
	report_buffer[1] = 0x80; 										//center y
	report_buffer[2] = (uchar) ((0x0ff0 & last_signal) >> 4); 		// buttons
	if(directions & 0x04) { report_buffer[1] = 0x00; }				// up
	if(directions & 0x08) { report_buffer[0] = 0xff; }				// right
	if(directions & 0x02) { report_buffer[1] = 0xff; }				// down
	if(directions & 0x01) { report_buffer[0] = 0x00; }				// left
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t    *rq = (void *)data;

    /* The following requests are never used. But since they are required by
     * the specification, we implement them in this example.
     */
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* class request type */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* we only have one report type, so don't look at wValue */
            usbMsgPtr = (void *)&report_buffer;
            return sizeof(report_buffer);
        }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
            usbMsgPtr = &idleRate;
            return 1;
        }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
            idleRate = rq->wValue.bytes[1];
        }
    }else{
        /* no vendor specific requests implemented */
    }
    return 0;   /* default for not implemented requests: return no data back to host */
}

void configure_hardware() {
	simple_spi_init();
	SPI_PORT &= ~(1 << DI_PIN); //pull down

	//reset both shift registers
	simple_spi_send(0x00); //reset button scanner
	simple_spi_send(0x00); //reset button scanner
}

int main(void) {
	uint16_t mask = 0x0001;
	uint16_t signal = 0x0000;
	uchar   b1, b2;
	configure_hardware();
	rgb_led_init();

	usbDeviceDisconnect();
	for(b1 = 0 ; b1 < 20 ; b1++) {  /* 300 ms disconnect */
		_delay_ms(15);
	}
	usbDeviceConnect();
	wdt_enable(WDTO_1S);
	usbInit();
	sei();

	has_changed = 1; //build the first report

	for(;;) {
		wdt_reset();
		usbPoll();
		if(usbInterruptIsReady()) { //send data when the usb is ready to receive
			if(has_changed) { //and when any change occurred
				buildReport(); //build the report according descriptor
				has_changed = 0; //clean change flag
			}
			usbSetInterrupt(report_buffer, sizeof(report_buffer)); //send
		}

		while(!(mask & 0x1000)) {
			// placing bit on right place to shift into shifters
			b2 = (uchar)((mask & 0xff00) >> 8);
			b1 = (uchar)(mask & 0x00ff);

			b2 |= (rgb_led_state() << 5); //next rgb led state

			/*
			 * the folowing 3 lines are needed to activate the clock line
			 * the pin should be pulled up for at least 1us
			 * this will charge the 100nf capacitor and it will activate the transistor
			 * this time was selected by trial and error and it is dependent of the circuit conditions
			 * */
			SPI_PORT |= (1 << USCK_PIN); //enable clock (and charge latch cap)
			_delay_us(1); //waiting for charge
			SPI_PORT &= ~(1 << USCK_PIN); //disable clock

			simple_spi_send(b2);
			simple_spi_send(b1);
			_delay_us(666); //waiting for button settle down (debouncing)

			if(PINB & 0x01) { //reading if the button is pressed
				signal |= mask; //add the state of each button to the buffer
			}
			mask = (mask << 1); //set the next button to be read
		}
		if(last_signal != signal) {
			last_signal = signal;
			has_changed = 1;
		}
		mask = 0x0001; //reset mask state
		signal = 0x0000; //reset signal report state
	}
	return 0;
}

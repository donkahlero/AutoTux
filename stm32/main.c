/**
 * Main file for the stm32 part of AutoTux.
 * Some of the files such as usbcfg.*, chconf, mcuconf and halconf are from the ChibiOS
 * examples for stm32. The config files are tailored for our particular needs.
 * Also note this from the ChibiOS example readme:
 *
 * "Some files used by the demo are not part of ChibiOS/RT but are copyright of
 * ST Microelectronics and are licensed under a different license.
 * Also note that not all the files present in the ST library are distributed
 * with ChibiOS/RT, you can find the whole library on the ST web site."
 */


// General includes
#include <stdio.h>
#include <string.h>
// ChibiOS includes
#include <ch.h>
#include <chprintf.h>
#include <hal.h>
#include "usbcfg.h"

// Local includes
#include "autotuxconfig.h"
#include "sensorInput.h"
#include "packet.h"
#include "controlOutput.h"


//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------


#define DEBUG_OUTPUT 1


// Initializes sensor thread, drivers etc.
void initialize(void);


//-----------------------------------------------------------------------------
// Implementation - main loop.
//-----------------------------------------------------------------------------


int main(void) {
	initialize();

	// Buffer for received byte
	msg_t charbuf;

	// Last valid control data bytes
	// TODO: MOVE TO OUTPUT HARDWARE FILE
	unsigned char controlData[CONTROL_DATA_BYTES];
	int iterationsWithoutReceive = 0;
	int bytesReceived = 0;

	unsigned char sensorData[SENSOR_DATA_BYTES];

	// TODO: MOVE TO OUTPUT HARDWARE FILE
	// Car initial state: stopped, wheels centered
	controlOutputStopCenter();


	// Main loop. Iteration counter for activity LED
	while(true) {

		//---------------------------------------------------------------------
		// Reset all LEDS
		//---------------------------------------------------------------------
		palClearPad(GPIOD, GPIOD_LED4);
		palClearPad(GPIOD, GPIOD_LED5);

		//---------------------------------------------------------------------
		// Receiving part
		//---------------------------------------------------------------------

		// Read bytes until we get a timeout, meaning we have caught up with
		// whatever is sent from the high-level board. Abort if we reach byte limit.
		charbuf = chnGetTimeout(&SDU1, TIME_IMMEDIATE);
		bytesReceived = 0;
		while (charbuf != Q_TIMEOUT && charbuf != Q_RESET &&
				bytesReceived < MAX_RECEIVE_BYTES_IN_ITERATION) {
			// Received another byte
			bytesReceived++;

			// Add the byte to the packet buffer
			appendToBuffer((unsigned char)charbuf);

			// Read the next byte - but first wait a bit, the USB-serial driver
			// tends to hang if we read to soon.
			chThdSleepMicroseconds(50);
			charbuf = chnGetTimeout(&SDU1, TIME_IMMEDIATE);
		}

		// Received all bytes available from serial. Time to try to instantiate
		// a packet, provided we did receive something this iteration
		bool receivedValidPacket = FALSE;
		if (getPacketBufferSize() >= CONTROL_DATA_PACKET_SIZE &&
				bytesReceived > 0) {
			// TODO: Light up red LED if failed to instantiate packet
			if (readPacketFromBuffer(controlData) == PACKET_OK) {
				// Valid packet - green LED
				palSetPad(GPIOD, GPIOD_LED4);
				receivedValidPacket = TRUE;
				iterationsWithoutReceive = 0;
			} else {
				// Broken packet or garbage - red LED
				palSetPad(GPIOD, GPIOD_LED5);
			}
		}

		// Increase counter if we didn't receive a valid packet
		if (!receivedValidPacket &&
				iterationsWithoutReceive <= MAX_ITERATIONS_WITHOUT_RECEIVE) {
			iterationsWithoutReceive++;
		}


		//---------------------------------------------------------------------
		// Output to hardware
		//---------------------------------------------------------------------

		// Unless we received too much data at once or no data for a number of iterations,
		// controlData contains the latest valid instructions. Output them to hardware.
		if (bytesReceived <= MAX_RECEIVE_BYTES_IN_ITERATION &&
				iterationsWithoutReceive < MAX_ITERATIONS_WITHOUT_RECEIVE) {
			// Forward control data to hardware
			controlOutput(controlData);
		} else {
			// Serial connection rules violated. Stop the car and center wheels!
			controlOutputStopCenter();
		}

		//---------------------------------------------------------------------
		// Sending part
		//---------------------------------------------------------------------

		if (DEBUG_OUTPUT) {
			// Send debug output to serial instead of a normal packet.
			sensorDebugOutput((BaseSequentialStream*) &SDU1);
		} else {
			// Send a sensor data packet. Fill data array with sensor values.
			getSensorData(sensorData);

			// Send to SDU1
			sendPacket(sensorData, SENSOR_DATA_BYTES, (BaseSequentialStream*) &SDU1);
		}

		chThdSleepMilliseconds(100);
	}
	return 0;
}


/**
 * Initializes sensor thread, drivers etc
 */
void initialize() {
	// Initialize drivers etc
	halInit();
	chSysInit();

	// Initialize sensor settings
	sensorSetup();
	controlOutputSetup();

 	// Initialize serial over USB
	sduObjectInit(&SDU1);
 	sduStart(&SDU1, &serusbcfg);

  	// Activate USB driver. The delay means that if the device is reset, it will
 	// be unavailable to the host for a while, and then reattached.
	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1500);
	usbStart(serusbcfg.usbp, &usbcfg);
  	usbConnectBus(serusbcfg.usbp);
}

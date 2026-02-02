#include <hp165x.h>
#include <stdio.h>
#include <stddef.h>

#define INPUT_BUFFER_SIZE 256

static volatile char inputBuffer[INPUT_BUFFER_SIZE];
static volatile uint16_t inputBufferHead=0;
static volatile uint16_t inputBufferTail=0;
static volatile uint16_t inputCount=0;
static volatile const char* outputData=NULL;
static volatile uint32_t outputCount=0;

int16_t simple_serial_getchar(void) {
	if (inputCount) {
		char c = inputBuffer[inputBufferHead];
		inputBufferHead = (inputBufferHead+1) % INPUT_BUFFER_SIZE;
		inputCount--;
		return (uint16_t)(uint8_t)c;
	}
	return -1;
}

uint16_t simple_serial_available(void) {
	return inputCount;
}

int16_t simple_serial_peek(void) {
	if (inputCount) {
		return (uint16_t)(uint8_t)inputBuffer[inputBufferHead];
	}
	else {
		return -1;
	}
}

void _myHandleSerialInterrupt(void) {
	if (SERIAL_STATUS_RECEIVE_READY & *SERIAL_STATUS) {
		char c = *SERIAL_DATA;
		if (inputCount<INPUT_BUFFER_SIZE-1) {
			inputBuffer[inputBufferTail] = c;
			inputBufferTail = (inputBufferTail+1) % INPUT_BUFFER_SIZE;
			inputCount++;
		}
		else {
			inputBufferHead = (inputBufferHead+1) % INPUT_BUFFER_SIZE;
			inputBuffer[inputBufferTail] = c;
			inputBufferTail = (inputBufferTail+1) % INPUT_BUFFER_SIZE;
		}
	}
	if (SERIAL_STATUS_TRANSMIT_READY & *SERIAL_STATUS) {
		if (outputCount != 0) {
			if (outputCount != 0) {
				*SERIAL_DATA = *outputData;
				outputCount--;
				outputData++;
			}
		}
		else {
			*SERIAL_COMMAND &= ~SERIAL_COMMAND_TRANSMIT;
		}
	}
}

uint16_t simple_serial_is_send_done(void) {
	return outputCount == 0;
}

void simple_serial_send(uint32_t size, const void* data) {
	outputCount = 0;
	outputData = (const char*)data;
	outputCount = size; 
	*SERIAL_COMMAND |= SERIAL_COMMAND_TRANSMIT;
}

void myHandleSerialInterrupt(void);
asm("  .globl myHandleSerialInterrupt\n"
	"myHandleSerialInterrupt:\n"
	"  movem.l %d0-%d1/%a0-%a1,-(%sp)\n"
	"  jsr _myHandleSerialInterrupt\n"
	"  movem.l (%sp)+,%d0-%d1/%a0-%a1\n"
	"  rte");
	
void simple_serial_close() {
	unpatchInt(INT_SERIAL);
	inputCount = 0;
	inputBufferHead = 0;
	inputBufferTail = 0;
	outputCount = 0;
}

void simple_serial_init(uint16_t baud) {
	simple_serial_close();
	serialSetup(baud, PARITY_NONE, STOP_BITS_1, DATA_BITS_8, PROTOCOL_NONE);
	patchInt(INT_SERIAL,myHandleSerialInterrupt);
}

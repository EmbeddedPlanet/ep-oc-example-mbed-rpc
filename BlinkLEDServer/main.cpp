#include "drivers/DigitalOut.h"

#include "erpc_simple_server.h"
#include "erpc_basic_codec.h"
#include "erpc_crc16.h"
#include "UARTTransport.h"
#include "DynamicMessageBufferFactory.h"
#include "blink_led_server.h"

/**
 * Macros for setting console flow control.
 */
#define CONSOLE_FLOWCONTROL_RTS     1
#define CONSOLE_FLOWCONTROL_CTS     2
#define CONSOLE_FLOWCONTROL_RTSCTS  3
#define mbed_console_concat_(x) CONSOLE_FLOWCONTROL_##x
#define mbed_console_concat(x) mbed_console_concat_(x)
#define CONSOLE_FLOWCONTROL mbed_console_concat(MBED_CONF_TARGET_CONSOLE_UART_FLOW_CONTROL)

mbed::DigitalOut led1(LED1, 1);
mbed::DigitalOut led2(LED2, 1);
mbed::DigitalOut led3(LED3, 1);
mbed::DigitalOut led4(LED4, 1);
mbed::DigitalOut* leds[] = { &led1, &led2, &led3, &led4 };

/****** erpc declarations *******/

void led_on(uint8_t led) {
	if(0 < led && led <= 4) {
		*leds[led - 1] = 0;
	}
}

void led_off(uint8_t led) {
	if(0 < led && led <= 4) {
		*leds[led - 1] = 1;
	}
}

/** erpc infrastructure */
ep::UARTTransport uart_transport(STDIO_UART_TX, STDIO_UART_RX, 9600);
ep::DynamicMessageBufferFactory dynamic_mbf;
erpc::BasicCodecFactory basic_cf;
erpc::Crc16 crc16;
erpc::SimpleServer rpc_server;

/** LED service */
LEDBlinkService_service led_service;

int main(void) {

	// Initialize the rpc server
	uart_transport.setCrc16(&crc16);

	// Set up hardware flow control, if needed
#if CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTS
	uart_transport.set_flow_control(mbed::SerialBase::RTS, STDIO_UART_RTS, NC);
#elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_CTS
	uart_transport.set_flow_control(mbed::SerialBase::CTS, NC, STDIO_UART_CTS);
#elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTSCTS
	uart_transport.set_flow_control(mbed::SerialBase::RTSCTS, STDIO_UART_RTS, STDIO_UART_CTS);
#endif
	
	rpc_server.setTransport(&uart_transport);
	rpc_server.setCodecFactory(&basic_cf);
	rpc_server.setMessageBufferFactory(&dynamic_mbf);

	// Add the led service to the server
	rpc_server.addService(&led_service);

	// Run the server. This should never exit
	rpc_server.run();

  
}

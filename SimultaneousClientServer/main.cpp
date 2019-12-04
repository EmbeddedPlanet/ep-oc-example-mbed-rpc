#include "drivers/DigitalOut.h"
#include "drivers/DigitalButton.h"
#include "events/EventQueue.h"
#include "rtos/rtos.h"
#include "platform/mbed_wait_api.h"

/****** erpc includes *********/
#include "erpc_simple_server.h"
#include "erpc_basic_codec.h"
#include "erpc_crc16.h"
#include "UARTTransport.h"
#include "DynamicMessageBufferFactory.h"
#include "erpc_arbitrated_client_manager.h"
#include "erpc_transport_arbitrator.h"
#include "blink_led_server.h"
#include "button_input.h"

// Retarget pins for NRF52840_DK
#ifndef PIN_NAME_BUTTON_1
#define PIN_NAME_BUTTON_1 BUTTON1
#define PIN_NAME_BUTTON_2 BUTTON2
#define PIN_NAME_BUTTON_3 BUTTON3
#define PIN_NAME_BUTTON_4 BUTTON4
#endif

mbed::DigitalOut led1(LED1, 1);
mbed::DigitalOut led2(LED2, 1);
mbed::DigitalOut led3(LED3, 1);
mbed::DigitalOut led4(LED4, 1);
mbed::DigitalOut* leds[] = { &led1, &led2, &led3, &led4 };

// Forward declaration of button handlers
void long_press_handler(ep::ButtonIn* button_ptr);
void short_press_handler(ep::ButtonIn* button_ptr);

// Declare buttons
ep::DigitalButton button1(PIN_NAME_BUTTON_1, true, short_press_handler, long_press_handler);
ep::DigitalButton button2(PIN_NAME_BUTTON_2, true, short_press_handler, long_press_handler);
ep::DigitalButton button3(PIN_NAME_BUTTON_3, true, short_press_handler, long_press_handler);
ep::DigitalButton button4(PIN_NAME_BUTTON_4, true, short_press_handler, long_press_handler);
ep::DigitalButton* buttons[] = {
		&button1,
		&button2,
		&button3,
		&button4
};

const int num_buttons = (sizeof(buttons)/sizeof(ep::DigitalButton*));

events::EventQueue main_queue;

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
erpc::ArbitratedClientManager client_mgr;
erpc::BasicCodec codec;
erpc::TransportArbitrator xport_arbitrator;
erpc::SimpleServer rpc_server;
erpc::ClientManager* g_client; // extern required by the RPC client shim code

/** LED service */
LEDBlinkService_service led_service;

/** Button press handlers **/

void long_press_handler(ep::ButtonIn* button_ptr) {
	for(int i = 0; i < num_buttons; i++) {
		if(button_ptr == buttons[i]) {
			// Call the RPC button handler
			main_queue.call(button_handler, i+1, false);
		}
	}
}

void short_press_handler(ep::ButtonIn* button_ptr) {
	for(int i = 0; i < num_buttons; i++) {
		if(button_ptr == buttons[i]) {
			// Call the RPC button handler
			main_queue.call(button_handler, i+1, true);
		}
	}
}

void server_thread_main(void) {
	rpc_server.run();
}

int main(void) {

	// Populate the extern used by the generated erpc client code
	g_client = &client_mgr;

	// Initialize the shared rpc transport
	uart_transport.setCrc16(&crc16);
	uart_transport.set_flow_control(mbed::SerialBase::RTSCTS, STDIO_UART_RTS, STDIO_UART_CTS);
	xport_arbitrator.setSharedTransport(&uart_transport);
	xport_arbitrator.setCodec(&codec);

	// Initialize the rpc server
	rpc_server.setTransport(&xport_arbitrator);
	rpc_server.setCodecFactory(&basic_cf);
	rpc_server.setMessageBufferFactory(&dynamic_mbf);

	// Add the led service to the server
	rpc_server.addService(&led_service);

	// Initialize the rpc client
	client_mgr.setArbitrator(&xport_arbitrator);
	client_mgr.setCodecFactory(&basic_cf);
	client_mgr.setMessageBufferFactory(&dynamic_mbf);

	// Calls to the client API are ready to be handled

	// Spin off the server into a separate thread
	rtos::Thread server_thread;
	server_thread.start(server_thread_main);

	// Run the event queue dispatch to handle button press events.
	// This should never exit
	main_queue.dispatch_forever();

}

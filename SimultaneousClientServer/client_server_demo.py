# led_test_client.py
# Test client for erpc led server example
# Author: becksteing
# Date: 5/13/2019
# Blinks LEDs on a connected Mbed-enabled board running the erpc LED server example

from time import sleep
import erpc
from client_server_example import blink_led
from client_server_example import button_input
import sys
import os
import threading

class ButtonInputHandler:

    def __init__(self, fn):
        self.button_handler = fn # Function pointer to handler function

def button_handler(button, short_press):
    # Print out the button press event information
    print("{} press on button {}!".format(("short" if short_press else "long"), button))

def client_thread_main():
     # Blink LEDs on the connected erpc server
    turning_on = True
    while True:
        for i in range(1, 5):
            if(turning_on):
                client.led_on(i)
            else:
                client.led_off(i)
            sleep(0.1)

        turning_on = not turning_on


if __name__ == "__main__":

    if len(sys.argv) != 2:
        print("Usage: python led_test_client.py <serial port to use>")
        exit()

    # Initialize the shared rpc transport
    xport = erpc.transport.SerialTransport(sys.argv[1], 9600)
    codec = erpc.basic_codec.BasicCodec()
    xport_arbitrator = erpc.arbitrator.TransportArbitrator(xport, codec)

    # Initialize the rpc server
    server_thread = erpc.simple_server.ServerThread(xport_arbitrator, erpc.basic_codec.BasicCodec)
    event_handler = ButtonInputHandler(button_handler)
    button_service = button_input.server.ButtonInputService(event_handler)
    server_thread.add_service(button_service)
    
    client_mgr = erpc.client.ClientManager(xport, erpc.basic_codec.BasicCodec)
    client_mgr.arbitrator = xport_arbitrator
    client = blink_led.client.LEDBlinkServiceClient(client_mgr)

    # Spin off the server thread
    server_thread.start()

    client_thread = threading.Thread(target=client_thread_main)
    client_thread.start()

    # Listen to keyboard exceptions here so we can kill all threads and exit
    while(True):
        try:
            sleep(0.2)
        except:
            print("")
            print("Exiting client server demo...")
            os._exit(1)

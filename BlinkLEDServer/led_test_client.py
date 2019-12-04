# led_test_client.py
# Test client for erpc led server example
# Author: becksteing
# Date: 5/13/2019
# Blinks LEDs on a connected Mbed-enabled board running the erpc LED server example

from time import sleep
import erpc
from client_python import blink_led
import sys


if __name__ == "__main__":

    if len(sys.argv) != 2:
        print("Usage: python led_test_client.py <serial port to use>")
        exit()

    # Initialize all erpc infrastructure
    xport = erpc.transport.SerialTransport(sys.argv[1], 9600)
    client_mgr = erpc.client.ClientManager(xport, erpc.basic_codec.BasicCodec)
    client = blink_led.client.LEDBlinkServiceClient(client_mgr)

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

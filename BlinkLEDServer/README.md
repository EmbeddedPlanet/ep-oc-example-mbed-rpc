# BlinkLEDServer

This is a simple example project showing the basics of how to set up an mbed-rpc project. Under the hood, mbed-rpc relies on erpc from Freescale/NXP.

## Prerequisites

This example assumes you have followed the basic `Getting Started` section of the `mbed-rpc` library README. Once you have done that, your system will have the necessary tools to generate erpc service code along with the python library required to run this example.

## Running the Example

### Overview of BlinkLEDServer

This project consists of two different programs: a Blink LED RPC Server running on a connected Mbed-enabled board and a Python client script running on your machine.

This example was originally tested with the following hardware configuration:

- Server running on nRF52840_DK
- Python client running on an Ubuntu 18.04 VM with a Windows 10 Pro Host OS

### Building the Server

Most of the code is already in place to build the server application. 

First, you will need to execute the following to make sure all the associated dependencies are cloned fully:

```
mbed deploy
cd mbed-rpc/erpc
git checkout add-mbed-os-support
```

Now all that's left is to generate the erpc server files using the `erpcgen` tool.

To generate the erpc server files, execute the following command:

`erpcgen -g c -o erpc-files led-service.erpc`

The tool will use the RPC specification file, `led-service.erpc`, to generate "shim code" for both the client and server in C/C++. The generated files will be placed in a new folder called `erpc-files`. Since we are building the server side of the RPC example, you **must** either **delete or ignore the client implementation file(s). If you don't do this, the build will have duplicate symbols (since the client code implements the RPC function call stubs). In this case, simply execute the following:

`rm ./erpc-files/blink_led_client.cpp`

You can then build the example by running `mbed compile`. Upload the code to your target and proceed to "Running the Client".

**Note:** Since this library is built on Mbed, all supported Mbed targets should be able to run this code. You should double check whether your Mbed board interface chip requires hardware flow control on the debug UART. If it **does not**, you must comment out the line in main.cpp that sets up hardware flow control the UART. The original hardware (nRF52840_DK) requires this to function correctly.

### Running the Client

For simplicity, this example uses a Python client to interact with the RPC server that should be running on your Mbed board. Before running the script, you must again generate the required erpc files, this time for Python. Also make sure you have installed the erpc library for Python by following the main `mbed-rpc` README.

To generate the Python files, execute the following command:

`erpcgen -g py -o client_python led-service.erpc`

This will put the generated files in a new `client_python` subdirectory. You can then run the provided client example by executing the following:

`python led_test_client.py /dev/ttyACM0`

**Note:** Make sure to replace `/dev/ttyACM0` in the above command with the serial port handle actually connected to your Mbed board!

If the example runs successfully, you should see the LEDs on your board blinking in sequence!

# SimultaneousClientServer

This is a slightly more complex example project than `BlinkLEDServer`. Each endpoint in this RPC project simultaneously runs a server and client using the same transport. This is accomplished using multithreading concepts and arbitrated access to the transport.

## Prerequisites

This example assumes to have followed the basic `Getting Started` section of the `mbed-rpc` library README. You should also read the [erpc documentation](https://github.com/EmbeddedRPC/erpc/wiki). You may want to start with the simpler `BlinkLEDServer` example if you have not used the `mbed-rpc` library before.

## Running the Example

### Overview of SimultaneousClientServer

This project consists of two different programs and two different RPC service specifications. The first service is the same Blink LED service used in the `BlinkLEDServer` example. The additional service in this example is a Button Input service that exposes callbacks to be executed over RPC when a button press is detected. An Mbed-enabled board hosts the Blink LED server and the host laptop, running a python script, hosts the Button Input server. Both the Mbed-enabled board and the host laptop execute client calls to each other. 

This example was originally tested with the following hardware configuration:

- Server running on nRF52840_DK
- Python client running on an Ubuntu 18.04 VM with a Windows 10 Pro Host OS

### Building the Mbed Application

Most of the code is already in place to build the Mbed application. All that's left is to generate the erpc files using the `erpcgen` tool. You may need to initialize the `ep-core` library first, see the main README for information on how to do that.

Since this example involves two RPC services, you must generate the LED Blink _server_ files **and** the Button Input _client_ files to build the Mbed application. (todo: I think it's possible to combine both of these service specifications into the same `.erpc` file)

To generate the erpc sfiles, execute the following commands:

`erpcgen -g c -o erpc-files led-service.erpc`
`erpcgen -g c -o erpc-files button-service.erpc`

The tool will use the RPC specification files to generate "shim code" for both the client and server in C/C++. The generated files will be placed in a new folder called `erpc-files`. Since we are building the LED server and Button client side of the RPC example, you **must** either **delete or ignore the unneeded server/client implementation file(s). If you don't do this, the build will have duplicate symbols (since the client code implements the RPC function call stubs). In this case, simply execute the following:

`rm ./erpc-files/blink_led_client.cpp && rm ./erpc-files/button_input_server.*`

You can then build the example by running `mbed compile`. Upload the code to your target and proceed to "Running the Client".

**Note:** Since this library is built on Mbed, all supported Mbed targets should be able to run this code. You should double check whether your Mbed board interface chip requires hardware flow control on the debug UART. If it **does not**, you must comment out the line in main.cpp that sets up hardware flow control the UART. The original hardware (nRF52840_DK) requires this to function correctly.

### Running the Client

For simplicity, this example uses a Python client to interact with the RPC server that should be running on your Mbed board. Before running the script, you must again generate the required erpc files, this time for Python. Also make sure you have installed the erpc library for Python by following the main `mbed-rpc` README.

To generate the Python files, execute the following commands:

`erpcgen -g py -o client_server_example led-service.erpc`
`erpcgen -g py -o client_server_example button-service.erpc`

This will put the generated files in a new `client_server_example` subdirectory. You can then run the provided python client/server example by executing the following:

`python client_server_demo.py /dev/ttyACM0`

**Note:** Make sure to replace `/dev/ttyACM0` in the above command with the serial port handle actually connected to your Mbed board!

If the example runs successfully, you should see the LEDs on your board blinking in sequence. When you press a button on the Mbed board you should see a message appear in the terminal running the python program.

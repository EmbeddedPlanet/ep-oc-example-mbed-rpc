# ep-oc-example-mbed-rpc
Examples for the mbed-rpc library

This repository contains examples of using the [mbed-rpc library](https://github.com/EmbeddedPlanet/mbed-rpc) with Mbed-OS. See each subdirectory for directions on building the examples

## Building Examples - General
Generally, the examples just need to be initialized and all dependencies pulled in. The examples use [Mbed's CLI tools](https://github.com/ARMmbed/mbed-cli) for this.

You will need to install mbed-cli and associated tools before building these examples.

Once mbed-cli is installed, simply navigate to the example's directory, follow the directions in that example's specific README for generating any additional sources, and then execute the following:

```
mbed new .
mbed deploy
```

This will pull in all dependencies automatically. Make sure to set up your target and toolchain before executing `mbed compile`.

## Configuration and Memory Optimization

While erpc is very lightweight already, hosting an erpc server and acting as an erpc client currently requires pulling in Mbed's RTOS libraries.

This can make it impossible to run on more constrained targets -- especially in terms of RAM consumption.

It is possible to reduce static RAM usage by changing the configuration using Mbed's JSON-based build configuration system.

Some settings that can decrease RAM usage:

### Memory Optimization -- Decrease Main Thread Stack Size

Depending on your application's RAM requirements, you can reduce the default size of the main thread stack. An example `mbed_app.json` file accomplishing this is shown below:

```
{
   "target_overrides": {
      "*": {
         "rtos.main-thread-stack-size": 1024,
         }
    }
}
```

By default, the main thread stack size is set to 4kB! Changing this can cut RAM usage significantly if your application isn't doing much other than RPC calls and lightweight processing. Make sure not to decrease the stack size too much or you'll encounter stack overflow errors.

### Memory Optimization -- Decrease Default Thread Stack Size

Similar to above, any additional threads created at runtime are given a default stack size of 4kB! You can either change this in code (by passing a non-zero size parameter to the `Thread` object's constructor) or change the default stack size allocated to new threads.

To do this, add the following line to your `mbed_app.json` configuration file:

```	    
"rtos.thread-stack-size": 1024
```

### Memory Optimization -- Adjusting Buffer Sizes

If you are using the `UARTTransport` class included in `mbed-rpc` you should note that it is a buffered UART implementation. Due to the design of erpc, this is required to ensure incoming messages are received completely.

By default, the RX and TX buffers are 256 bytes **each** in size. You can adjust this by adding the following lines to your `mbed_app.json` file:

```
"drivers.uart-serial-txbuf-size": 128,
"drivers.uart-serial-rxbuf-size": 128
```

Additionally, erpc allocates buffers messages internally with a default size of 256 bytes **each**. You can change the default buffer size allocated by erpc by each `MessageBufferFactory` by adding the following line to your `mbed_app.json`:

```
"mbed-rpc.erpc-default-buffer-size": 128
```

### Flash/RAM Usage Optimization -- Release Build Profile

Mbed's build system uses **profiles** to adjust optimization and compiler flags. The most optimized build profile included with Mbed is the `release` profile. It can sometimes shave off a few extra kB of flash/RAM usage.

To build using the release profile, add the `--profile` flag to your `mbed compile` command as shown below:

```
mbed compile --profile ./mbed-os/tools/profiles/release.json
```

### Real-World Results

With all the above optimizations taken into account, the `SimultaneousClientServer` demo application was able to successfully run on an STM32L073RZ Nucleo board (128kB flash / 20kB RAM)

Note that even with the debug configuration it is possible to run `SimultaneousClientServer` on this target but it uses a majority of the RAM (~15kB).

In entirety, the `mbed_app.json` used for this build is shown below:

```
{
   "target_overrides": {
      "NUCLEO_L073RZ": {
         "rtos.main-thread-stack-size": 1024,
	       "rtos.thread-stack-size": 1024,
	       "drivers.uart-serial-txbuf-size": 128,
	       "drivers.uart-serial-rxbuf-size": 128,
	       "mbed-rpc.erpc-default-buffer-size": 128
      }
   }
}
```

and the build command was `mbed compile --profile ./mbed-os/tools/profiles/release.json`.

The build output is shown below, with a reduced Flash/RAM footprint of ~65kB/8.7kB:

```
Building project SimultaneousClientServer (NUCLEO_L073RZ, GCC_ARM)
Scan: SimultaneousClientServer
Link: SimultaneousClientServer
Elf2Bin: SimultaneousClientServer
| Module                           |     .text |    .data |     .bss |
|----------------------------------|-----------|----------|----------|
| [fill]                           |   100(+0) |    8(+0) |   23(+0) |
| [lib]/c.a                        | 17260(+0) | 2472(+0) |   89(+0) |
| [lib]/gcc.a                      |  8568(+0) |    0(+0) |    0(+0) |
| [lib]/misc                       |   216(+0) |    4(+0) |   28(+0) |
| [lib]/stdc++.a                   |     0(+0) |    0(+0) |    0(+0) |
| ep-oc-mcu/devices                |    56(+0) |    0(+0) |   36(+0) |
| erpc-files/blink_led_server.o    |   214(+0) |    0(+0) |    0(+0) |
| erpc-files/button_input_client.o |   112(+0) |    0(+0) |    0(+0) |
| main.o                           |  2752(+0) |   20(+0) | 1136(+0) |
| mbed-os/components               |    36(+0) |    0(+0) |    0(+0) |
| mbed-os/drivers                  |  3440(+0) |    0(+0) |    0(+0) |
| mbed-os/events                   |  1308(+0) |    0(+0) |    0(+0) |
| mbed-os/hal                      |  1610(+0) |    8(+0) |  130(+0) |
| mbed-os/platform                 |  3554(+0) |  260(+0) |  284(+0) |
| mbed-os/rtos                     |  9912(+0) |  168(+0) | 3164(+0) |
| mbed-os/targets                  |  9832(+0) |    4(+0) |  806(+0) |
| mbed-rpc/erpc                    |  2970(+0) |    0(+0) |    0(+0) |
| Subtotals                        | 61940(+0) | 2944(+0) | 5696(+0) |
Total Static RAM memory (data + bss): 8640(+0) bytes
Total Flash memory (text + data): 64884(+0) bytes

Image: ./BUILD/NUCLEO_L073RZ/GCC_ARM-RELEASE/SimultaneousClientServer.bin
```

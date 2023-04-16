![CI status](https://github.com/lains/ticdecodecpp/actions/workflows/main.yml/badge.svg)

# C++ TIC decoding library

French meters use a serial communication protocol called TeleInfo for customers (or TIC).

This library is a C++ implementation of decoding algorithms to parse this TIC serial data into interpreted data.
It does so with the minimum overhead, without any dynamic allocation, and is thus also well fitted for embedded systems as the Arduino.

[LibTeleinfo](https://github.com/hallard/LibTeleinfo) is another library that does the same job for Arduino.

There are serveral C or C++ implementations available but most of them dynamically allocate buffers and fully parse the TIC data in structures.
In contrast, this library uses callbacks to user functions whenever new data is available, offering a view on a statically allocated buffer representing the data.
In its most efficient form (when compiler directive `__TIC_UNFRAMER_FORWARD_FRAME_BYTES_ON_THE_FLY__` is defined), there is only one static buffer allocated for storing TIC data, and its size (in bytes) is set by the [MAX_DATASET_SIZE constant in TIC/DatasetExtractor.h](include/TIC/DatasetExtractor.h).

This library can parse both standard TIC (9600 bauds) or historical TIC (1200 bauds) streams, and supports decoding horodates.

## Using the library

In order for it to decode in real-time, you will have to regularly invoke [TIC::Unframer::pushBytes()](include/TIC/Unframer.h) with all TIC bytes received by the serial port of your device .

## Running unit tests

To execute unit tests, run the following command from the sources top directory:
```
make check
```

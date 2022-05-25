## Uart bridge sample 
This sample show the extension feature of the bus.

It uses the RISCV hifive1_revb board inside the Renode environment and shows the bus exchange data information with a Python script that uses the Pyserial module. To run that, you'll need to have the Renode framework, a Python version 3.6 or newer, and the pyserial module properly installed.

### Running the sample
Run the command `make run` in one terminal to start the Renode execution. After that, run the command `make serial_monitor` in another terminal to enable the serial decoder.

From Renode console you would see something like this:
```
D: [Bridge] send data 0
D: [Core] sending start measurement with status 1
D: [ZBUS] pub start_measurement at CMAKE_SOURCE_DIR/src/core.c:24
D: [ZBUS] notify!
D: [Bridge] send data 0
D: [Peripheral] sending sensor data
D: [ZBUS] pub sensor_data at CMAKE_SOURCE_DIR/src/peripheral.c:26
D: [ZBUS] notify!
D: [Bridge] send data 0
D: [Core] sending start measurement with status 0
D: [ZBUS] pub start_measurement at CMAKE_SOURCE_DIR/src/core.c:24
D: [ZBUS] notify!
D: [Bridge] send data 0
D: [Peripheral] sending sensor data
D: [ZBUS] pub sensor_data at CMAKE_SOURCE_DIR/src/peripheral.c:26
D: [ZBUS] notify!
D: [Bridge] send data 0
D: [Core] sending start measurement with status 1
D: [ZBUS] pub start_measurement at CMAKE_SOURCE_DIR/src/core.c:24
D: [ZBUS] notify!
D: [Bridge] send data 0
D: [Peripheral] sending sensor data
D: [ZBUS] pub sensor_data at CMAKE_SOURCE_DIR/src/peripheral.c:26
D: [ZBUS] notify!
D: [Bridge] send data 0
D: [Core] sending start measurement with status 0
D: [ZBUS] pub start_measurement at CMAKE_SOURCE_DIR/src/core.c:24
D: [ZBUS] notify!
D: [Bridge] send data 0
D: [Peripheral] sending sensor data
D: [ZBUS] pub sensor_data at CMAKE_SOURCE_DIR/src/peripheral.c:26
D: [ZBUS] notify!
D: [Bridge] send data 0
D: [Core] sending s
```

From the serial decoder (python script) you would see something like this:
```
PUB [sensor_data] -> b'\xc5\x01\x00\x00\xb2\x11\x00\x00'
PUB [start_measurement] -> b'\x00'
PUB [sensor_data] -> b'\xc6\x01\x00\x00\xbc\x11\x00\x00'
PUB [start_measurement] -> b'\x01'
PUB [sensor_data] -> b'\xc7\x01\x00\x00\xc6\x11\x00\x00'
PUB [start_measurement] -> b'\x00'
PUB [sensor_data] -> b'\xc8\x01\x00\x00\xd0\x11\x00\x00'
PUB [start_measurement] -> b'\x01'
PUB [sensor_data] -> b'\xc9\x01\x00\x00\xda\x11\x00\x00'
PUB [start_measurement] -> b'\x00'
PUB [sensor_data] -> b'\xca\x01\x00\x00\xe4\x11\x00\x00'
PUB [start_measurement] -> b'\x01'
PUB [sensor_data] -> b'\xcb\x01\x00\x00\xee\x11\x00\x00'
PUB [start_measurement] -> b'\x00'
PUB [sensor_data] -> b'\xcc\x01\x00\x00\xf8\x11\x00\x00'
PUB [start_measurement] -> b'\x01'
PUB [sensor_data] -> b'\xcd\x01\x00\x00\x02\x12\x00\x00'
PUB [start_measurement] -> b'\x00'
PUB [sensor_data] -> b'\xce\x01\x00\x00\x0c\x12\x00\x00'
PUB [start_measurement] -> b'\x01'
PUB [sensor_data] -> b'\xcf\x01\x00\x00\x16\x12\x00\x00'
PUB [start_measurement] -> b'\x00'
```

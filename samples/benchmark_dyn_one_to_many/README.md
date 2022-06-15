### Benchmark 256KB
This is an attempt to measure time for sending 256KB from the producer to the consumers. You can choose from 1 to 16 consumers and the message size. 

To run the benchmark individually, you must execute the following command: `make benchmark MSG_SIZE=32 ONE_TO=8 ASYNC=0 BOARD=nrf52840dk_nrf52840`

Notice we have the following parameters:

* **MSG_SIZE**: the size of the message to be transferred;
* **ONE_TO**: number of consumers to send;
* **ASYNC**: if the execution must be asynchronous or synchronous. Use 1 to async e 0 to sync;
* **BOARD**: the board name you want to use.

The result would be something like:
```
*** Booting Zephyr OS build zephyr-v3.0.0  ***
 Benchmark 1 to 8: SYNC transmission and message size 32
 - Bytes sent = 262144, received = 262144 
 - Average data rate: 560136.75B/s
 - Duration: 468ms

@468
```

### Running the benchmark automatically

There is a Robot script called `benchmark_256KB.robot` which runs all the interesting combinations of inputs for the benchmark.
The result file called `zbus_dyn_benchmark_256KB.csv` is generated in the project root folder. At the CSV file we have:

| Style | Number of consumers | Message size (bytes) | Duration (ms) | RAM (bytes) | ROM (bytes) |
| ------------------ | ------------------- | ------------ | -------- | ----- | ----- |
| SYNC/ASYNC | 1,2,4,8 | 1,2,4,8,16,32,64,128,256 | float | int   | int |

To execute the script, you must run the following command: `robot -d build benchmark_256KB.robot`.

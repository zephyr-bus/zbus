import serial

ser = serial.Serial('/tmp/uart')
channel_name = ["Version", "Sensor data", "Start measurement", "Finish"]
while(1):
    d = ser.readline()
    print(f"PUB [{channel_name[d[1]]}] -> {d[2:-2:]}")

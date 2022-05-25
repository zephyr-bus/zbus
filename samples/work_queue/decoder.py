import serial
import json
j = """
[
    {"name":"version","on_changed": false, "read_only": true, "message_size": 4},
    {"name":"sensor_data","on_changed": true, "read_only": false, "message_size":8},
    {"name":"start_measurement","on_changed": false, "read_only": false, "message_size": 1},
    {"name":"finish","on_changed": false, "read_only": false, "message_size": 1}
]
"""

channels = json.loads(j)


def fetch_sentence():
    channel_id = int.from_bytes(ser.read(), "little")
    channel_name = channels[channel_id]['name']
    msg_size = channels[channel_id]['message_size']
    msg = ser.read(msg_size)
    return (channel_name, msg_size, msg)


ser = serial.Serial('/tmp/uart')
while(1):
    d = ser.read()
    if d == b'$':
        channel_name, msg_size, msg = fetch_sentence()
        print(f"PUB [{channel_name}] -> {msg}")

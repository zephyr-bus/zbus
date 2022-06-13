#!/usr/bin/python3
import subprocess
import os
import traceback
import serial


def filter_at(ser, char):
    while True:
        line = ser.readline()
        print(line)
        if line.startswith(char):
            return int(line[1:-2:])


ser = serial.Serial('/dev/ttyACM0', 115200)
results = []


def sh(args, timeout=None, check_result=True, cd=False):
    print(f" >>> {args}")
    res = subprocess.run(args, shell=True, timeout=timeout)
    if check_result:
        assert res.returncode == 0, res.returncode
    if cd:
        directory = args.split()
        os.chdir(directory[1])
    return res


def exec_benchmark_for(one_to, msg_size, async_):
    sh(f"make benchmark MSG_SIZE={msg_size} ONE_TO={one_to} ASYNC={async_}")
    duration = 0
    duration += filter_at(ser, b'@')
    for i in range(2):
        sh("west flash")
        duration += filter_at(ser, b'@')
    results.append(
        f"{'ASYNC' if async_ else 'SYNC'}, {one_to}, {msg_size}, {duration/3}\n")


def cleaning_workdir():
    sh("cd ../", cd=True)
    sh("rm -rf test-sample/")


if __name__ == "__main__":
    code = 0
    try:
        for sync in [0, 1]:
            for consumers in [1, 2, 4, 8]:
                for msg_size in [1, 2, 4, 8, 16, 32, 64, 128, 256]:
                    exec_benchmark_for(consumers, msg_size, sync)
            with open(os.path.join('benchmark.csv'), 'a') as f:
                f.writelines(results)
                results = []
    except:
        print(traceback.print_exc())
        code = 1
    finally:
        exit(code)

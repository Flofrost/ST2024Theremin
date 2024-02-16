from serial import Serial
from sys import stdin

samples = stdin.read()
samples = samples[1:-2].split(",")
samples = [int(s.strip()) for s in samples]
samples = [256 + s if s < 0 else s for s in samples]

port = Serial("/dev/ttyUSB0", 115200)

port.write(samples)

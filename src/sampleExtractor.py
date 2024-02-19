#!/bin/python3

import argparse

# Wave file needs to be 16bits mono
# Also need to specify the number of periods in the file

parser = argparse.ArgumentParser(
    prog="Sample Extractor",
    description="Extracts and enhances a sample from a wav file, given a number of periods",
)

parser.add_argument(
    "filename",
    type=str,
    help = "Path to file to extract sample from"
)
parser.add_argument("-p", "--periods",
    type=int,
    required=True,
    help="Number of periods in file to extract sample from"
)

args = parser.parse_args()
print(args)

periods = args.periods
f = open(args.filename, "rb")
header = f.read(0x28)
# f.seek(0x28)
length = int.from_bytes(f.read(4), byteorder="little")
# f.seek(0x2C)
# data = list(f.read())
data = []
while True:
    chunk = f.read(2)
    if not chunk:
        break

    data.append(int.from_bytes(chunk, byteorder="little"))
f.close()

data = data[:length // 2]
# data = [data[int((i/256) * len(data))] - 128 for i in range(256)]
# maximum = max(max(data), -min(data))
# gain = 127 / maximum
print(data)

sampleLength = (length //2) / periods
for i in range(periods):
    f = open(f"{i}.wav", "wb")
    f.write(header)
    f.write(int(sampleLength).to_bytes(4, "little"))
    for a in [b.to_bytes(2, "little") for b in data[int(sampleLength * i ): int(sampleLength * (i+1))]]:
        f.write(a)
    f.close

print(f"Periods: {periods}, File: {args.filename}, Length: {length}")
# print([int(data[i] * gain) for i in range(256)])


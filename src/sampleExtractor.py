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

numberOfPeriods = args.periods
f = open(args.filename, "rb")

header = f.read(0x28)
fileLength = int.from_bytes(f.read(4), byteorder="little")

data = []
while True:
    sample = f.read(2)
    if not sample:
        break

    data.append(int.from_bytes(sample, byteorder="little"))
f.close()

dataLength = len(data)

sampleLength = (dataLength) / numberOfPeriods

# Only for testing
for i in range(numberOfPeriods):
    f = open(f"{i}.wav", "wb")
    f.write(header)
    f.write(int(sampleLength * 2).to_bytes(4, "little"))
    print(int(sampleLength * i), int(sampleLength * (i+1)))
    for a in [b.to_bytes(2, "little") for b in data[int(sampleLength * i): int(sampleLength * (i+1))]]:
        f.write(a)
    f.close

print(f"FileLength: {fileLength}, DataLength: {dataLength}, SampleLength: {sampleLength}, Periods: {numberOfPeriods}")


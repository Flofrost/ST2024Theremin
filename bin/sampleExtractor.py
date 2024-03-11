from sys import argv


f = open(argv[1], "rb")
f.seek(0x28)
length = int.from_bytes(f.read(4), byteorder="little")
f.seek(0x2C)
data = list(f.read())
data = data[:length]
data = [data[int((i/8192) * len(data))] - 4096 for i in range(8192)]
maximum = max(max(data), -min(data))
gain = 4096 / maximum

print([int(data[i] * gain) for i in range(8192)])

# for(uint8_t i = 0 ; i < N_VOICES ; i++)
#         if(voices[i].stage != off){
#             voiceSample = loadedInstrument.samples[voices[i].phase >> 8] >> 2;
#             voiceSample = (voiceSample * (voices[i].amplitude + (tremolo << 4))) >> 7;
#             sample += voiceSample;

#             voices[i].phase += voices[i].frequency + (vibrato << 4);
#         }

sample_len = 8192
freqEch = 44100


sample = []
for phase in range(8192):
    voiceSample = data[phase >> 8] >> 2
    sample.append(voiceSample)

f.close()
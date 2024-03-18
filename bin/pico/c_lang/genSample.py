from math import pi, sin

print([int(512 + 511.999 * sin(2 * pi * x / 8192)) for x in range(8192)])

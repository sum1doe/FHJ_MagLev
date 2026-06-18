data = []

# source:
"""
double shunt2current(int16 input) {
    return 0.136 * pow((double) input, 1.5);
}
"""

def func(x):
    return 0.136 * (x**1.5)

for i in range(0, 1024):
    data.append(func(i))

data = [str(int(i)) for i in data]

print(",".join(data))

file = open("ShuntToCurrent.dat", "w")
file.write(",".join(data))
file.write("\n")
file.close()
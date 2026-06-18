data = []

# Source
"""
TODO: Figure out direction based thing.
"""

def func(x):
    return x

for i in range(0, 1024):
    data.append(func(i))

data = [str(int(i)) for i in data]

print(",".join(data))

file = open("AccelToCurrent.dat", "w")
file.write(",".join(data))
#because ccs throws a fit if I don't
file.write("\n")
file.close()
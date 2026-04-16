output = ""

def curve(x):
    # 166.606/x^0.401582
    return 166.606 * (x ** -0.401582)


for i in range(4096):
    # 166.606/x^0.401582

    if i < 2412:
        word = str(int(curve(2412-i)*10))
    elif i > 2412:
        word = str(int(curve(i-2412)*10))
    else:
        word = "9999"

    print(i, word)
    output += word
    output += ","

file = open("SensorDistanceCurve.dat", "w")
file.write(output[:-1])
file.close()

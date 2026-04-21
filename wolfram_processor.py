x_points = """5
120
255
455
650
938
1142
1485
1720

2045"""
y_points = """0
4
8
15
20
26
30
36
43
47"""

x = [i for i in x_points.split("\n") if len(i) > 0]
y = [i for i in y_points.split("\n") if len(i) > 0]

output = "https://www.wolframalpha.com/input?i2d=true&i={"
for i in range(min(len(x), len(y))):
    output += "{" + f"{x[i]}%2C{y[i]}" + "}%2C"

output = output[:-3] + "}+power+fit"
print(output)

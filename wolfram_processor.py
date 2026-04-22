x_points = """0
3
3
5
5
8
8
10
13
15
18
20
23
26
29"""
y_points = """42
155
155
280
285
429
433
554
733
889
1144
1358
1660
1889
2248"""

x = [i for i in x_points.split("\n") if len(i) > 0]
y = [i for i in y_points.split("\n") if len(i) > 0]

output = "https://www.wolframalpha.com/input?i2d=true&i={"
for i in range(min(len(x), len(y))):
    output += "{" + f"{x[i]}%2C{y[i]}" + "}%2C"

output = output[:-3] + "}+power+fit"
print(output)

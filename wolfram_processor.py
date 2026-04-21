x_points = """5
106
203
405
606
860
1015
1216
1421
1631
1919
2150
2269"""
y_points = """0
8
12
17
23
28
32
36
40
44
48
52
54"""

x = [i for i in x_points.split("\n") if len(i) > 0]
y = [i for i in y_points.split("\n") if len(i) > 0]

output = "{"
for i in range(min(len(x), len(y))):
    output += "{" + f"{x[i]},{y[i]}" + "},"

output = output[:-1] + "}"
print(output)

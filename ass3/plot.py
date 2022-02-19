import matplotlib.pyplot as plt
import sys

y = []
x = []
sum = 0
f = open(sys.argv[1], "r")
for line in f:
    lines = [i for i in line.split()]
    y.append(float(lines[0])/(float(lines[1])))
    x.append(float(lines[1]))


print(sum)
for i in range(1, len(x)):
    x[i] = x[i-1]+x[i]

if(sys.argv[1] == "points1.txt"):
    plt.title("Server Data Transmission")
else:
    plt.title("Client Data Transmission")
plt.xlabel('seconds')
plt.ylabel('10^8 b/s')

plt.plot(x, y, c='g')

plt.show()

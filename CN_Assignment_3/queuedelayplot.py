import matplotlib.pyplot as plt
import numpy as np


def get_file_data():
    file = open("tcp-example.tr", "r")
    data = file.readlines()
    return data

def get_data():
    data = get_file_data()
    xcoordinates = []
    ycoordinates = []
    queue = {}

    delay_t = 0
    packets_t = 0
    counter = 0

    for i in data:
        arr = i.strip().split()

        seq = int(arr[36][4:])
        clock = float(arr[1])
        enq = arr[0] == "+"
        deq = arr[0] == "-"

        if enq:
            queue[seq] = clock
        elif deq:
            if seq not in q.keys():
                continue
            delay_t += clock - q[seq]
            packets_t += 1

            tt = clock - queue[seq]
            if len(xcoordinates) > 0 and xcoordinates[-1] == clock:
                ycoordinates[-1] = (ycoordinates[-1] * counter + tt) / (counter + 1)
                counter += 1
            else:
                counter = 1
                xcoordinates.append(clock)
                ycoordinates.append(tt)
            queue.pop(seq)

    xpoints = np.array(xcoordinates)
    ypoints = np.array(ycoordinates)

    return xpoints, ypoints

def main():
    xpoints, ypoints = get_data()
    plt.plot(xpoints, ypoints, "black")
    plt.xlabel("time")
    plt.ylabel("queue delay")
    plt.show()    

if __main__ == "__name__":
    main()
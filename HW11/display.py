# draw a 40 x 30 image from serial data
import serial
ser = serial.Serial('/dev/tty.usbmodem144301',timeout=0) # use your port name
#print(ser.name)

import numpy as np # for arrays

# size of the window, each pixel will be 10 x 10
WIDTH = 400
HEIGHT = 300

# arrays to store data
num_rows = 3
red = np.linspace(0,0,40*num_rows)
green = np.linspace(0,0,40*num_rows)
blue = np.linspace(0,0,40*num_rows)

# send a newline to ask for a new image
ser.write(('\n').encode())

def draw():
    ind = 0
    for y in range(0,num_rows,1):
        ind = ind + 1 # fudge
        for x in range(0,39,1):
            BOX = Rect((x*10, y*10+70*(y+1)), (10, 10))
            COLOR = red[ind],green[ind],blue[ind]
            screen.draw.filled_rect(BOX, COLOR)
            ind = ind + 1

def update():
    data_read = ser.readline() # read a line into a bytearray
    data_text = str(data_read,'utf-8') # turn the line into a string
    data = list(map(int,data_text.split())) # turn the string into a list of ints

    if(len(data)==4):
        red[data[0]] = data[1]
        green[data[0]] = data[2]
        blue[data[0]] = data[3]
        #print(data[0])
        # after you get an image, ask for another
        if data[0] == 119:
            ser.write(('\n').encode())

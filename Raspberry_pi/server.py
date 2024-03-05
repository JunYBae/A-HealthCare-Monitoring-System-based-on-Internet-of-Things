from bluetooth import *

import threading

from matplotlib import pyplot as plt

from matplotlib import animation

from mpl_toolkits.axes_grid1 import make_axes_locatable

import numpy as np

import time

import matplotlib

from matplotlib import font_manager

# 폰트 경로 설정
font_path = '/usr/share/fonts/truetype/msttcorefonts/times.ttf'
prop = font_manager.FontProperties(fname=font_path)




# full size display

matplotlib.use("Qt5Agg")


global foot_total

global foot_left

global foot_right

global client_sock1, client_sock2


max_retries = 10 # connecting count

retry_count = 0



def connect1():

    global retry_count, max_retries, client_sock1

    while retry_count < max_retries:

        try:

            client_sock1 = BluetoothSocket(RFCOMM)

            client_sock1.connect(("98:DA:60:04:CA:0C", 1))

            print("bluetooth connect device1")

            return

       

        except BluetoothError as e:

            print("Bluetooth Error: {}".format(e))

            retry_count += 1

            print("Retry connection.. (Attempt:{})".format(retry_count))

            time.sleep(1) # wait 1sec

       

    print("Time out")

    sys.exit() # program exit


def connect2():

    global retry_count, max_retries, client_sock2

    retry_count = 0

    while retry_count < max_retries:

        try:

            client_sock2 = BluetoothSocket(RFCOMM)

            client_sock2.connect(("98:DA:60:07:D8:33", 1))

            print("bluetooth connect device2")

            return

       

        except BluetoothError as e:

            print("Bluetooth Error: {}".format(e))

            retry_count += 1

            print("Retry connection.. (Attempt:{})".format(retry_count))

            time.sleep(1) # wait 1sec

       

    print("Time out")

    sys.exit() # program exit

           

       

def receive_data_device1():

    while True:

        global foot_left, client_sock1

        decoded_data = client_sock1.recv(1024).decode('utf-8')

        result_data = ''.join(char for char in decoded_data if char.isdigit() or char == '@')

        foot_press = result_data.split('@') # @ split

        #foot_press = [int(x) for x in foot_press] # string -> int

        foot_press = filter(None, foot_press)

        foot_press = list(map(int, foot_press))

        foot_left = foot_press[0:4] # left_foot

        #print(foot_left)


def receive_data_device2():

    while True:

        global foot_right, client_sock2

        decoded_data = client_sock2.recv(1024).decode('utf-8')

        result_data = ''.join(char for char in decoded_data if char.isdigit() or char == '@')

        foot_press = result_data.split('@') # @ split

        #foot_press = [int(x) for x in foot_press] # string -> int

        foot_press = filter(None, foot_press)

        foot_press = list(map(int, foot_press))

        foot_right = foot_press[0:4] # right_foot

        #print(foot_right)


connect1()

connect2()

device1 = threading.Thread(target=receive_data_device1, args=())

device2 = threading.Thread(target=receive_data_device2, args=())

device1.start()

device2.start()


############################################################

#fig = plt.figure(figsize=(5, 3))

fig = plt.figure()

plt.rc("font", family="times new roman")

ax = plt.subplot(221)

#plt.title("left_hitmap")

divider = make_axes_locatable(ax)

cax = divider.append_axes('right', size='5%', pad=0.05)

line  = ax.matshow(np.zeros([2,2]), cmap='jet', vmin=0, vmax=30000)

ax.axis('off') # x, y lable off

cbar1 = plt.colorbar(line, cax=cax) # colorbar set

cbar1.set_ticks(np.arange(0, 30001, step=10000))

for label in cbar1.ax.get_yticklabels():
   label.set_fontproperties(prop)
   label.set_fontsize(14)

ax_2 = plt.subplot(222)

#plt.title("right_hitmap",  fontproperties=prop)

divider = make_axes_locatable(ax_2)

cax_2 = divider.append_axes('right', size='5%', pad=0.05)

line_2  = ax_2.matshow(np.zeros([2,2]), cmap='jet', vmin=0, vmax=30000)

ax_2.axis('off')

cbar2 = plt.colorbar(line_2, cax_2)

cbar2.set_ticks(np.arange(0, 30001, step=10000))

for label in cbar2.ax.get_yticklabels():
   label.set_fontproperties(prop)
   label.set_fontsize(14)

ax_3 = plt.subplot(223, xlim=(0,20), ylim=(0,120000))

plt.ylabel('1/gram', fontproperties=prop, fontsize=14)

plt.xticks(np.arange(0, 20, 2), [])

plt.yticks(np.arange(0, 120000, 30000),
fontproperties=prop, fontsize=14)

plt.title("left_total", fontproperties=prop, fontsize=14)

ax_4 = plt.subplot(224, xlim=(0,20), ylim=(0,120000))

plt.ylabel('1/gram', fontproperties=prop, fontsize=14)

plt.xticks(np.arange(0, 20, 2), [])

plt.yticks(np.arange(0, 120000, 30000), fontproperties=prop, fontsize=14)

plt.title("right_total", fontproperties=prop, fontsize=14)

plt.tight_layout()


max_points = 20


line_3, = ax_3.plot(np.arange(max_points),

                np.ones(max_points, dtype=float)*np.nan, lw=1, c='green',ms=1)

line_4, = ax_4.plot(np.arange(max_points),

                np.ones(max_points, dtype=float)*np.nan, lw=1, c='purple',ms=1)


def init():

    return line

def init_2():

    return line_2

def init_3():

    return line_3

def init_4():

    return line_4

########################################################3



def animate(i):

    global foot_left

    list1 = np.array([[foot_left[0], foot_left[1]], [foot_left[2], foot_left[3]]])

    line.set_array(list1)

    return line


def animate_2(i):

    global foot_right

    list2 = np.array([[foot_right[0], foot_right[1]], [foot_right[2], foot_right[3]]])

    line_2.set_array(list2)

    return line_2


def animate_3(i):

    global foot_left

    y_3 = sum(foot_left)

    old_y_3 = np.nan_to_num(line_3.get_ydata())

   

    temp = np.sum(old_y_3[18:])

    y_3 = (temp + y_3) / 3

   

    new_y_3 = np.r_[old_y_3[1:], y_3]

    line_3.set_ydata(new_y_3)

    return line_3


def animate_4(i):

    global foot_right

    y_4 = sum(foot_right)

    old_y_4 = np.nan_to_num(line_4.get_ydata())

   

    temp = np.sum(old_y_4[18:])

    y_4 = (temp + y_4) / 3


    new_y_4 = np.r_[old_y_4[1:], y_4]

    line_4.set_ydata(new_y_4)

    return line_4


anim = animation.FuncAnimation(fig, animate  , init_func= init ,frames=1, interval=1)

anim_2 = animation.FuncAnimation(fig, animate_2  , init_func= init_2 ,frames=1, interval=1)

anim_3 = animation.FuncAnimation(fig, animate_3  , init_func= init_3 ,frames=1, interval=1)

anim_4 = animation.FuncAnimation(fig, animate_4  , init_func= init_4 ,frames=1, interval=1)


   


mng = plt.get_current_fig_manager() # zoomed (full_size display)

mng.window.showMaximized()

fig = plt.gcf()

fig.canvas.manager.toolbar.setVisible(False)


plt.show()


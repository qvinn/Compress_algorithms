#This Python file uses the following encoding: utf-8
#import sys
#from PySide2.QtWidgets import QApplication
#import matplotlib.pyplot as plt
#import csv

#def csv_reader(file_obj):
#    """
#    Read a csv file
#    """
#    reader = csv.reader(file_obj)
#    for row in reader:
#        print(" ".join(row))

#if __name__ == "__main__":
#    csv_path = "I:\Quantum_Radiolocation\Work_3\Analizing\Analizing\My_base.csv"
#    with open(csv_path, "r") as f_obj:
#        csv_reader(f_obj)
#-----------------------------------------------------------------------------------------------------------------------
#def csv_dict_reader(file_obj):
#    """
#    Read a CSV file using csv.DictReader
#    """
#    reader = csv.DictReader(file_obj, delimiter=',')
#    for line in reader:
#        print("first_name: " + line["first_name"]),
#        print("last_name: " + line["last_name"]),
#        print("address: " + line["address"]),
#        print("city: " + line["city"]),
#        print("state: " + line["state"]),
#        print("zip_code: " + line["zip_code"]),
#        print("\n")


#if __name__ == "__main__":
#    with open("I:\Quantum_Radiolocation\Work_3\Analizing\Analizing\My_test_base.csv") as f_obj:
#        csv_dict_reader(f_obj)
#-----------------------------------------------------------------------------------------------------------------------

#start_size = []
#final_size = []
#percent_compression = []

## x-coordinates of left sides of bars
#left = [1, 2, 3, 4, 5, 6, 7, 8]

## labels for bars
#tick_label = ['Adaptiva_Huffman', 'Arithmetic_adptive', 'Huffman', 'LZ77', 'LZARI', 'LZSS', 'LZW', 'RLE']


#def csv_dict_reader(file_obj):
#    """
#    Read a CSV file using csv.DictReader
#    """
#    reader = csv.DictReader(file_obj, delimiter='|')
#    for line in reader:
#        print("start_size: " + line["start_size"])
#        print("final_size: " + line["final_size"])
#        print("percent_compression: " + line["percent_compression"])
#        start_size.append(int(line["start_size"]))
#        final_size.append(int(line["final_size"]))
#        percent_compression.append(float(line["percent_compression"]))
#        print("\n")


#if __name__ == "__main__":
#    with open("D:\C++ Projects\Compress_algorithms\Analizing\My_base.csv") as f_obj:
#        csv_dict_reader(f_obj)
#    print(start_size)
#    print(final_size)
#    print(percent_compression)

#    # plotting a bar chart
#    plt.bar(left, percent_compression, tick_label = tick_label, width = 0.5, color = ['green'])

#    # naming the x-axis
#    plt.xlabel('x - axis')
#    # naming the y-axis
#    plt.ylabel('y - axis')
#    # plot title
#    plt.title('My bar chart!')

#    # function to show the plot
#    plt.show()

import sys
from PySide2.QtWidgets import QApplication
import csv
import tkinter
import tkinter.ttk as ttk
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
from matplotlib.backend_bases import key_press_handler
from matplotlib.figure import Figure
import numpy as np

root = tkinter.Tk()
root.wm_title("Algoritms Results")

nb = ttk.Notebook(root)
nb.pack(fill='both', expand='yes')

algo_name = []
start_size = []
final_size = []
percent_compression = []
time_encode = []
time_decode = []
bytes_loss = []
percent_bytes_loss = []

# x-coordinates of left sides of bars
left = [1, 2, 3, 4, 5, 6, 7, 8]

# labels for bars
# tick_label = ['Adaptiva_Huffman', 'Arithmetic_adptive', 'Huffman', 'LZ77', 'LZARI', 'LZSS', 'LZW', 'RLE']

def csv_dict_reader(file_obj):
    """
    Read a CSV file using csv.DictReader
    """
    reader = csv.DictReader(file_obj, delimiter='|')
    for line in reader:
        print("algo_name: " + line["algo_name"])
        print("start_size: " + line["start_size"])
        print("final_size: " + line["final_size"])
        print("percent_compression: " + line["percent_compression"])
        print("time_encode: " + line["time_encode"])
        print("time_decode: " + line["time_decode"])
        print("bytes_loss: " + line["bytes_loss"])
        print("percent_bytes_loss: " + line["percent_bytes_loss"])
        algo_name.append(line["algo_name"])
        start_size.append(int(line["start_size"]))
        final_size.append(int(line["final_size"]))
        percent_compression.append(float(line["percent_compression"]))
        time_encode.append(int(line["time_encode"]))
        time_decode.append(int(line["time_decode"]))
        bytes_loss.append(int(line["bytes_loss"]))
        percent_bytes_loss.append(float(line["percent_bytes_loss"]))
        print("\n")

with open("D:\C++ Projects\Compress_algorithms\Analizing\log1.csv") as f_obj:
    csv_dict_reader(f_obj)

#print(algo_name)
#print(start_size)
#print(final_size)
#print(percent_compression)
#print(time_encode)
#print(time_decode)
#print(bytes_loss)
#print(percent_bytes_loss)

#-------------------------------------TAB_1-------------------------------------#
tab1 = ttk.Frame(nb)

fig1 = Figure(figsize=(5, 4), dpi=100)
fig1.add_subplot(111).bar(left, percent_compression, tick_label = algo_name, width = 0.5, color = ['green'])

canvas1 = FigureCanvasTkAgg(fig1, master=tab1)
canvas1.draw()
canvas1.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

toolbar1 = NavigationToolbar2Tk(canvas1, tab1)
toolbar1.update()
canvas1.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

nb.add(tab1, text='Compression From Start Size(in %)')

#-------------------------------------TAB_2-------------------------------------#
tab2 = ttk.Frame(nb)

fig2 = Figure(figsize=(5, 4), dpi=100)
fig2.add_subplot(111).bar(left, time_decode, tick_label = algo_name, width = 0.5, color = ['green'])

canvas2 = FigureCanvasTkAgg(fig2, master=tab2)
canvas2.draw()
canvas2.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

toolbar2 = NavigationToolbar2Tk(canvas2, tab2)
toolbar2.update()
canvas2.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

nb.add(tab2, text='Time Encode(in ms)')

#-------------------------------------TAB_3-------------------------------------#
tab3 = ttk.Frame(nb)

fig3 = Figure(figsize=(5, 4), dpi=100)
fig3.add_subplot(111).bar(left, percent_compression, tick_label = algo_name, width = 0.5, color = ['green'])

canvas3 = FigureCanvasTkAgg(fig3, master=tab3)
canvas3.draw()
canvas3.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

toolbar3 = NavigationToolbar2Tk(canvas3, tab3)
toolbar3.update()
canvas3.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

nb.add(tab3, text='Time Decode(in ms)')

#-------------------------------------TAB_4-------------------------------------#
tab4 = ttk.Frame(nb)

fig4 = Figure(figsize=(5, 4), dpi=100)
fig4.add_subplot(111).bar(left, percent_bytes_loss, tick_label = algo_name, width = 0.5, color = ['green'])

canvas4 = FigureCanvasTkAgg(fig4, master=tab4)
canvas4.draw()
canvas4.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

toolbar4 = NavigationToolbar2Tk(canvas4, tab4)
toolbar4.update()
canvas4.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

nb.add(tab4, text='Bytes Lost(in %)')

#def on_key_press(event):
#    print("you pressed {}".format(event.key))
#    key_press_handler(event, canvas, toolbar)


#canvas.mpl_connect("key_press_event", on_key_press)


#def _quit():
#    root.quit()
#    root.destroy()

#button = tkinter.Button(master=root, text="Quit", command=_quit)
#button.pack(side=tkinter.BOTTOM)

tkinter.mainloop()

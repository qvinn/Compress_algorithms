import sys
from PySide2.QtWidgets import QApplication
import csv
import tkinter
import tkinter.ttk as ttk
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
from matplotlib.backend_bases import key_press_handler
from matplotlib.figure import Figure
import numpy as np
import glob
from tkinter import filedialog

root = tkinter.Tk()
root.wm_title("Algoritms Results")
root.geometry("500x500")

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
algo_name_final = []
start_size_final = []
final_size_final = []
percent_compression_final = []
time_encode_final = []
time_decode_final = []
bytes_loss_final = []
percent_bytes_loss_final = []
left = []                       # x-coordinates of left sides of bars

 #----------Read a CSV file using csv.DictReader----------#
def csv_dict_reader(file_obj):
    reader = csv.DictReader(file_obj, delimiter='|')
    algo_name1 = []
    start_size1 = []
    final_size1 = []
    percent_compression1 = []
    time_encode1 = []
    time_decode1 = []
    bytes_loss1 = []
    percent_bytes_loss1 = []
    for line in reader:
        algo_name1.append(line["algo_name"])
        start_size1.append(int(line["start_size"]))
        final_size1.append(int(line["final_size"]))
        percent_compression1.append(float(line["percent_compression"]))
        time_encode1.append(int(line["time_encode"]))
        time_decode1.append(int(line["time_decode"]))
        bytes_loss1.append(int(line["bytes_loss"]))
        percent_bytes_loss1.append(float(line["percent_bytes_loss"]))
    algo_name.append(algo_name1)
    start_size.append(start_size1)
    final_size.append(final_size1)
    percent_compression.append(percent_compression1)
    time_encode.append(time_encode1)
    time_decode.append(time_decode1)
    bytes_loss.append(bytes_loss1)
    percent_bytes_loss.append(percent_bytes_loss1)

def show_stat(paths):
    del algo_name[:], start_size[:], final_size[:], percent_compression[:], time_encode[:], time_decode[:], bytes_loss[:], percent_bytes_loss[:], algo_name_final[:], start_size_final[:], final_size_final[:]
    del percent_compression_final[:], time_encode_final[:], time_decode_final[:], bytes_loss_final[:], percent_bytes_loss_final[:]
    for path in paths:
        files = glob.glob(path)
        for name in files:
            with open(name) as f_obj:
                csv_dict_reader(f_obj)

    for y in range(0, len(start_size[0])):
        q = 0
        w = 0
        e = 0
        r = 0
        t = 0
        u = 0
        i = 0
        for x in range(0, len(start_size)):
            q = q + start_size[x][y]
            w = w + final_size[x][y]
            e = e + percent_compression[x][y]
            r = r + time_encode[x][y]
            t = t + time_decode[x][y]
            u = u + bytes_loss[x][y]
            i = i + percent_bytes_loss[x][y]
        algo_name_final.append(algo_name[0][y])
        start_size_final.append(int(q/len(start_size)))
        final_size_final.append(int(w/len(start_size)))
        percent_compression_final.append(float(e/len(start_size)))
        time_encode_final.append(int(r/len(start_size)))
        time_decode_final.append(int(t/len(start_size)))
        bytes_loss_final.append(int(u/len(start_size)))
        percent_bytes_loss_final.append(float(i/len(start_size)))
        left.append(int (y + 1))

    #-------------------------------------TAB_1-------------------------------------#
    tab1 = ttk.Frame(nb)

    fig1 = Figure(figsize=(5, 4), dpi=100)
    fig1.add_subplot(111).bar(left, percent_compression_final, tick_label = algo_name_final, width = 0.5, color = ['green'])

    canvas1 = FigureCanvasTkAgg(fig1, master=tab1)
    canvas1.draw()
    canvas1.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

    toolbar1 = NavigationToolbar2Tk(canvas1, tab1)
    toolbar1.update()
    canvas1.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

    #-------------------------------------TAB_2-------------------------------------#
    tab2 = ttk.Frame(nb)

    fig2 = Figure(figsize=(5, 4), dpi=100)
    fig2.add_subplot(111).bar(left, time_encode_final, tick_label = algo_name_final, width = 0.5, color = ['green'])

    canvas2 = FigureCanvasTkAgg(fig2, master=tab2)
    canvas2.draw()
    canvas2.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

    toolbar2 = NavigationToolbar2Tk(canvas2, tab2)
    toolbar2.update()
    canvas2.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

    #-------------------------------------TAB_3-------------------------------------#
    tab3 = ttk.Frame(nb)

    fig3 = Figure(figsize=(5, 4), dpi=100)
    fig3.add_subplot(111).bar(left, time_decode_final, tick_label = algo_name_final, width = 0.5, color = ['green'])

    canvas3 = FigureCanvasTkAgg(fig3, master=tab3)
    canvas3.draw()
    canvas3.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

    toolbar3 = NavigationToolbar2Tk(canvas3, tab3)
    toolbar3.update()
    canvas3.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

    #-------------------------------------TAB_4-------------------------------------#
    tab4 = ttk.Frame(nb)

    fig4 = Figure(figsize=(5, 4), dpi=100)
    fig4.add_subplot(111).bar(left, percent_bytes_loss_final, tick_label = algo_name_final, width = 0.5, color = ['green'])

    canvas4 = FigureCanvasTkAgg(fig4, master=tab4)
    canvas4.draw()
    canvas4.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

    toolbar4 = NavigationToolbar2Tk(canvas4, tab4)
    toolbar4.update()
    canvas4.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

    for tab in nb.tabs():
        nb.forget(tab)

    nb.add(tab1, text='Compression From Start Size(in %)')
    nb.add(tab2, text='Time Encode(in ms)')
    nb.add(tab3, text='Time Decode(in ms)')
    nb.add(tab4, text='Bytes Lost(in %)')

def _quit():
    root.quit()
    root.destroy()

def _chose_file():
    lst = filedialog.askopenfilenames()
    paths = list(lst)
    show_stat(paths)

button = tkinter.Button(master=root, text="Quit", command=_quit)
button.pack(side=tkinter.RIGHT, padx=5, pady=5)

button1 = tkinter.Button(master=root, text="Choose File", command=_chose_file)
button1.pack(side=tkinter.RIGHT)

tkinter.mainloop()

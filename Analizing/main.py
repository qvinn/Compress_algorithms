# This Python file uses the following encoding: utf-8
import sys
from PySide2.QtWidgets import QApplication
import matplotlib.pyplot as plt
import csv

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
start_size = []
final_size = []
percent_compression = []

# x-coordinates of left sides of bars
left = [1, 2, 3, 4, 5, 6, 7, 8]

# labels for bars
tick_label = ['Adaptiva_Huffman', 'Arithmetic_adptive', 'Huffman', 'LZ77', 'LZARI', 'LZSS', 'LZW', 'RLE']


def csv_dict_reader(file_obj):
    """
    Read a CSV file using csv.DictReader
    """
    reader = csv.DictReader(file_obj, delimiter=',')
    for line in reader:
        print("start_size: " + line["start_size"])
        print("final_size: " + line["final_size"])
        print("percent_compression: " + line["percent_compression"])
        start_size.append(int(line["start_size"]))
        final_size.append(int(line["final_size"]))
        percent_compression.append(float(line["percent_compression"]))
        print("\n")


if __name__ == "__main__":
    with open("I:\Quantum_Radiolocation\Work_3\Analizing\Analizing\My_base.csv") as f_obj:
        csv_dict_reader(f_obj)
    print(start_size)
    print(final_size)
    print(percent_compression)

    # plotting a bar chart
    plt.bar(left, percent_compression, tick_label = tick_label, width = 0.5, color = ['green'])

    # naming the x-axis
    plt.xlabel('x - axis')
    # naming the y-axis
    plt.ylabel('y - axis')
    # plot title
    plt.title('My bar chart!')

    # function to show the plot
    plt.show()

import os
import numpy as np
import re 
import math
from docplex.mp.model import Model
import time
import pandas as pd

files_name = []
folder_path = 'D:\\CPLEX-ORLAB\\Instance Knapsack'
#extension = ".txt"
files = os.listdir(folder_path)
for file_name in files:
    files_name.append(file_name)

def solvee(file):
    #mở và đọc file 
    with open(os.path.join(folder_path,file) ,'r',encoding = 'utf-8') as f :
        text = f.read()

    # Xử lí file
    text = text.split('\n')
    for x in range(len(text)):
        text[x] = text[x].split()
    capacity = float(text[0][1])
    text = text[1:-2]
    n = len(text)

    #Khởi tạo model
    opt_mod = Model(name = 'Knapsack')
    opt_mod.set_time_limit(60)
    #Khởi tạo các biến
    x = [opt_mod.binary_var(name = f'x{i}') for i in range(n)]
    #Add objective function
    opt_mod.set_objective('max' , sum([x[i] * float(text[i][0]) for i in range(n)]))
    #Add constraint
    opt_mod.add_constraint( sum([x[i] * float(text[i][1]) for i in range(n)]) <= capacity)
    #Solve
    opt_mod.solve(log_output = False)
    result = opt_mod.solution.get_objective_value()

    return file , result , opt_mod.solve_details.time , opt_mod.solve_details.gap

if __name__ == '__main__':
    for i in range(len(files_name)):
        print(solvee(files_name[i]))




    
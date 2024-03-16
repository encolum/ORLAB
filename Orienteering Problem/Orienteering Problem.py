import os
import numpy as np
import re 
import math
from docplex.mp.model import Model
import time
import pandas as pd

#Đọc file 
files_name = []
folder_path = 'D:\CPLEX-ORLAB\Instance OP'
extension = ".txt"
files = os.listdir(folder_path)
for file_name in files:
    if os.path.isfile(os.path.join(folder_path, file_name)) and file_name.endswith(extension):
        files_name.append(file_name)

# Xử lí dữ liệu:
def solvee(file):
    #mở và đọc file 
    with open(os.path.join(folder_path,file) ,'r',encoding = 'utf-8') as f :
        lines = f.readlines()
    # Xử lí dữ liệu
    lines = [line.split() for line in lines]
    tmax = lines[0][0]
    vehicle = lines[0][1]
    lines = lines[1::]
    n = len(lines)
    lines[1] , lines[n-1] = lines[n-1] , lines[1]
    t = np.zeros((n, n))
    for i in range(n):
        for j in range(n):
            t[i][j] = (math.sqrt((float(lines[i][0]) - float(lines[j][0]))**2 + (float(lines[i][1]) - float(lines[j][1]))**2))

    #Khởi tạo model và biến
    opt_mod = Model('OP')
    x = [[opt_mod.binary_var(name=f'x_{i}_{j}') for j in range(n)] for i in range(n)]
    value = [line[2] for line in lines]

    #Set objective function
    ob_func = opt_mod.set_objective('max', sum([x[i][j] * float(value[i]) for i in range(1, n-1) for j in range(1, n)]))

    #Add constraint
    opt_mod.add_constraint(sum([x[0][j] for j in range(1, n)]) == 1)
    opt_mod.add_constraint(sum([x[i][n-1] for i in range(n-1)]) == 1)
    # Add constraint
    for k in range(1, n-1):
        opt_mod.add_constraint(sum([x[i][k] for i in range(n-1)]) <= 1)
        opt_mod.add_constraint(sum([x[k][j] for j in range(1, n)]) <= 1)
        a = sum([x[k][j] for j in range(1, n)])
        b = sum([x[i][k] for i in range(n-1)])
        opt_mod.add_constraint(a == b)
    opt_mod.add_constraint(sum([t[i][j] * x[i][j]  for j in range(1, n) for i in range(0,n-1)]) <= int(tmax))

    u = [0]
    for i in range(1, n):
        u += [opt_mod.integer_var(name=f'u{i}', lb=2, ub=n)]

    for i in range(1, n):
        for j in range(1, n):
            opt_mod.add_constraint(u[i] - u[j] + 1 <= (n-1) * (1 - x[i][j]))
    opt_mod.solve(log_output = False)
    result = opt_mod.solution.get_objective_value()

    return file , result , opt_mod.solve_details.time , opt_mod.solve_details.gap
if __name__ == '__main__':
    for i in range(len(files_name)):
        print(solvee(files_name[i]))
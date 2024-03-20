import os
import numpy as np
import re 
import math
from docplex.mp.model import Model
import time
import pandas as pd
files_name = []
folder_path = 'D:\CPLEX-ORLAB\Instance TOP'
extension = ".txt"
files = os.listdir(folder_path)
for file_name in files:
    if os.path.isfile(os.path.join(folder_path, file_name)) and file_name.endswith(extension):
        files_name.append(file_name)

def solvee(file):
    #Đọc file
    with open(os.path.join(folder_path,file) ,'r',encoding = 'utf-8') as f :
        text = f.read()

    #Xử lí file
    text = text.split('\n')
    vari = text[0:3]
    for i in range(len(vari)):
        vari[i] = vari[i].split()
    n = int(vari[0][1])
    P = int(vari[1][1])
    t = float(vari[2][1])
    matches = text[3:-1]
    for i in range(len(matches)):
        matches[i] = matches[i].split('\t')
    c = np.zeros((n,n))
    for i in range(n):
        for j in range(n):
            c[i][j] = math.ceil(math.sqrt( (float(matches[i][0]) - float(matches[j][0]))**2 + (float(matches[i][1]) - float(matches[j][1]))**2 ))
    S = [int(match[2]) for match in matches]
    #Build model : 
    opt_mod = Model(name = 'TOP')
    opt_mod.set_time_limit(60)
    ## Add variable 
    x = [[[opt_mod.binary_var(name = f'x_{i}_{j}_{p}') for p in range(P)] for j in range(n)] for i in range(n)]
    y = [[opt_mod.binary_var(name = f'y_{i}_{j}') for j in range(n)] for i in range(n)]
    ## Add Objective
    obj_func = opt_mod.set_objective('max' , sum( [S[i] * y[i][p] for i in range(1,n-1) for p in range(0,P) ]) ) 

    ## Add constraint:
    #Constraint 1
    opt_mod.add_constraint(sum([x[0][j][p] for j in range(1,n) for p in range(P)]) == P )
    opt_mod.add_constraint(sum([x[i][n-1][p] for i in range(n-1) for p in range(P)]) == P)
    #Constraint 2
    for k in range(1,n-1):
        opt_mod.add_constraint(sum([y[k][p] for p in range(P)]) <= 1)
    #Constraint 3
    for k in range(1,n-1): 
        for p in range(P):
            opt_mod.add_constraint(sum([x[i][k][p] for i in range(n-1)]) == y[k][p])
            opt_mod.add_constraint(sum([x[k][j][p] for j in range(1,n)]) == y[k][p])
    #constraint 4
    for p in range(P):
        opt_mod.add_constraint(sum([c[i][j] * x[i][j][p] for j in range(1,n) for i in range(n-1)]) <= t)
    #Constraint 5 and 6 
    u =[[0]] 
    for i in range(1,n):
        u += [[opt_mod.integer_var(name = f'u_{i}_{p}' , lb = 1 , ub = n) for p in range(P)]] 
    for p in range(P):  
        for j in range(1,n):
            for i in range(1,n):
                opt_mod.add_constraint(u[i][p] - u[j][p] + 1 <= (n-1) * (1-x[i][j][p]))
    opt_mod.solve(log_output = False)
    if opt_mod.solution == None:
        result = 10**9
        opt_mod_time = 0
        opt_mod_gap = 10**6
    else:
        result = opt_mod.solution.get_objective_value()
        opt_mod_time = opt_mod.solve_details.time
        opt_mod_gap = opt_mod.solve_details.gap

    return file , result , opt_mod_time , opt_mod_gap
if __name__ == '__main__':
    for i in range(len(files_name)):
        print(solvee(files_name[i]))
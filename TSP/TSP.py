import os
import numpy as np
import re 
import math
from docplex.mp.model import Model
import time
import pandas as pd


files_name = []
folder_path = 'D:\CPLEX-ORLAB\instances'
extension = ".sop"
files = os.listdir(folder_path)
for file_name in files:
    if os.path.isfile(os.path.join(folder_path, file_name)) and file_name.endswith(extension):
        files_name.append(file_name)


def solvee(file):
    #mở và đọc file 
    with open(os.path.join(folder_path,file) ,'r',encoding = 'utf-8') as f :
        text = f.read()
    #tìm ra toạ độ các đỉnh bằng regex
    pattern = r'(\d+)\s+([\d]+\.?[\d]+)\s+([\d]+\.?[\d]+)'
    matches = re.findall(pattern,text)
    matchs = []
    for x in matches:
        match = tuple(item for item in x if item != '')
        matchs.append(match)
    #tìm ra số đỉnh bằng regex
    pattern1 =  r"DIMENSION:\s*(\d+)"
    n = (re.findall(pattern1 , text))
    n = int(n[0])

    #tạo ra cost 
    c = np.zeros((n,n))
    for i in range(n):
        for j in range(n):
            # print(i , j )
            c[i][j] = math.ceil(math.sqrt( (float(matchs[i][1]) - float(matchs[j][1]))**2 + (float(matchs[i][2]) - float(matchs[j][2]))**2 ))

# Khởi tạo mô hình
    opt_mod = Model(name='TSP')
    opt_mod.set_time_limit(60)
    x = [[opt_mod.binary_var(name=f'x_{i}_{j}') for j in range(n)] for i in range(n)]


### Add constraint
    #Tong cua x[i][j] == 1; i =1,n (Voi moi gia tri i thi ta them 1 rang buoc)
    for i in range(n):
        opt_mod.add_constraint(sum([x[i][j] for j in range(n) if i != j] ) == 1 )
    #tong cua x[i][j] == 1 ; j=1,n (voi moi gia trị j thì ta thêm 1 ràng buộc)
    for j in range(n):
        opt_mod.add_constraint(sum([x[i][j] for i in range(n) if i != j] ) == 1 )
    #u denotes cho thứ tự đến 
    #u bang bao nhieu 0 quan trong , Vi du u2 = 2 < u7 = 3 thi den 2 truoc 7
    #vi du u2 = 2 , u7 = 3 thi di tu 2-->7
    #u=2 thi den thanh pho day' dau` tien xong cu tiep tuc
    u = [0]
    for i in range(1,n):
        u += [opt_mod.integer_var(name = f'u{i}', lb =1 , ub = n)]
    for i in range(1,n):
        for j in range(1,n):
            if i != j :
                opt_mod.add_constraint(u[i] - u[j] + 1 <= (n-1)*(1-x[i][j]))

    ob_func = opt_mod.set_objective('min' , sum( [x[i][j] * c[i][j] for i in range(n) for j in range(n) if i != j]))
    
    #start_time = time.time()
    opt_mod.solve(log_output = False)
    if opt_mod.solution == None:
        result = 10**9
    else:
        result = opt_mod.solution.get_objective_value()
    #end_time = time.time()

    return file , result ,  opt_mod.solve_details.time , opt_mod.solve_details.gap

if __name__ == '__main__':
    for i in range(len(files_name)):
        print(solvee(files_name[i]))
        

    

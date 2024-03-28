import os
import numpy as np
import re 
import math
from docplex.mp.model import Model
import time
import pandas as pd
files_name = []
folder_path = 'D:\\CPLEX-ORLAB\\Instance STOP'
sub_folders = os.listdir(folder_path)
for sub_folder in sub_folders:
    sub_folder_path = os.path.join(folder_path, sub_folder)
    files = os.listdir(sub_folder_path)
print(files)
# print(files_name)
# def solvee(file):
#     # Xử lí file
#     with open(os.path.join(sub_folder_path,file) ,'r',encoding = 'utf-8') as f :
#         text = f.read()
#         f.seek(0)
#         lines = f.readlines()
#     mat = lines[64:]
#     for i in range(len(mat)):
#         mat[i] = mat[i].split()
#     #Get profit 
#     profit = [int(match[1]) for match in mat]
#     m = [(int(match[0]),(match[2:])) for match in mat]
#     S = dict()
#     for key,value in m:
#         S[key] = value
#     for key,value in S.items():
#         S[key] = [int(x) - 1  for x in value]
#     K = len(S)
#     pattern = r'(\d+)\s+([\d]+\.?[\d]+)\s+([\d]+\.?[\d]+)'
#     matches = re.findall(pattern,text)
#     matchs = []
#     for x in matches:
#         match = tuple(item for item in x if item != '')
#         matchs.append(match)
#     #tìm ra số đỉnh bằng regex
#     pattern1 =  r"DIMENSION:\s*(\d+)"
#     n = (re.findall(pattern1 , text))
#     n = int(n[0])

#     #tạo ra cost 
#     c = np.zeros((n,n))
#     for i in range(n):
#         for j in range(n):
#             # print(i , j )
#             c[i][j] = math.ceil(math.sqrt( (float(matchs[i][1]) - float(matchs[j][1]))**2 + (float(matchs[i][2]) - float(matchs[j][2]))**2 ))
#     pattern2 = r'TMAX: \s*(\d+)'
#     tmax = (re.findall(pattern2,text))
#     tmax = int(tmax[0])
#     pattern3 = r'VEHICLES: \s*(\d+)'
#     vehicle = re.findall(pattern3, text)
#     vehicle = int(vehicle[0])

#     #Tao model 
#     opt_mod = Model( name = 'STOP')
#     opt_mod.set_time_limit(600)
#     #Add variables:
#     x = [[opt_mod.binary_var(name = f'x_{i}_{j}') for j in range(n)] for i in range(n)]
#     y = [opt_mod.binary_var(name = f'y_{k}') for k in range(K)]
#     s = [opt_mod.continuous_var(name = f's_{i}') for i in range(n)]
#     #Objective:
#     ob_func = opt_mod.set_objective('max' , sum( [profit[k] * y[k] for k in range(K)]))
#     #Constraint 1 :
#     for j in range(n):
#         opt_mod.add_constraint(sum([x[i][j] for i in range(n) if i != j]) == sum([x[j][i] for i in range(n) if i != j]) )
#     #Constraint 2 : 
#     for k in range(K):
#         opt_mod.add_constraint(sum([x[i][j] for j in S[k] for i in range(n) if i not in S[k]]) == y[k] )
#     #Constraint 3: 
#     for i in range(n):
#         for j in range(1,n):
#             opt_mod.add_constraint(s[j] >= s[i] + c[i][j]*x[i][j] - tmax*(1-x[i][j]))
#     #Constraint 4 : 
#     for i in range(n):
#         opt_mod.add_constraint(s[i] + c[i][0]*x[i][0] <= tmax)
#     #Constraint 5 : 
#     for i in range(1,n):
#         opt_mod.add_constraint(sum([x[i][0]]) <= vehicle)
#     #Constraint 6 : 
#     for i in range(n):
#         opt_mod.add_constraint(s[i] <= tmax)
#         opt_mod.add_constraint(s[i] >= 0 )
#     opt_mod.solve(log_output = False)
#     result = opt_mod.solution.get_objective_value()
#     return  file, result , opt_mod.solve_details.time , opt_mod.solve_details.gap , opt_mod.get_solve_details().best_bound

# if __name__ == '__main__':
#     for i in range(len(files_name)):
#         print(solvee(files_name[i]))




    

from run_experiments import *






# iccad_2020_benchmark_list = ["case1","case2"]

#iccad_2020_benchmark_list = ["case1","case2","case3"]

iccad_2020_benchmark_list = ["case2"]
  
#iccad_2020_benchmark_list = ["case1","case2",\
    #"case3","case4","case5","case6"]    


with open("experiments.json") as outfile: 
    experiments = json.load(outfile)
    for exp in experiments:
        print("Experiment: ", exp)
        exp_name = exp
        run_experiments(experiments[exp],exp_name,iccad_2020_benchmark_list)
        
   





# 

        




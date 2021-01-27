import json 
import os
import shutil
import subprocess

def run_experiments(experiment,exp_name,iccad_2020_benchmark_list):
    for benchmark in iccad_2020_benchmark_list:  
        with open("file_name.json", "w") as outfile: 
            print(benchmark)
            file_dict ={ 
                "file_name" : benchmark+".json", 
                "file_name_report" : benchmark+"_report.json"  
            } 

            file_name_json = json.dumps(file_dict, indent = 4) 
            outfile.write(file_name_json) 

            new_dir = "../build/experiments/file_name.json"
            shutil.move("file_name.json", new_dir)
            # rm -rf log_4.txt
            outfile.close()

            with open(benchmark+".json", "w") as outfile_params: 
                params_dict = experiment
                # {
                #     "construct_net_boxes_rtree"        : "true",
                #     "cluster_based_on_panel" : "true",
                #     "run_ilp_on_panels_parallel_loop" : "false",
                #     "run_astar_on_panels_parallel_loop" : "true", 
                #     "run_astar_on_panels_parallel_section" : "false",
                #     "run_astar_on_panels_sequential" : "false",
                #     "cell_movement_serial": "true",
                #     "cell_movement_time_out": "false",
                #     "move_cell_multi": "true"
                # }


                params_json = json.dumps(params_dict, indent = 4) 
                outfile_params.write(params_json) 
                new_dir = "../build/experiments/"+benchmark+".json"
                shutil.move(benchmark+".json", new_dir)
                outfile_params.close()

            benchmark_input = "./input_files/iccad2020/cases/" + benchmark+".txt"
            benchmark_output = benchmark+"_out.txt"
            benchmark_log = benchmark+"_log.txt"
            cmd = "./run.sh "+ benchmark_input + " "+ benchmark_output + " " + benchmark_log
    if False:
            os.system(cmd)
    # 
    try:
        dir_name = exp_name
        cmd = "mkdir" + " " + dir_name
        os.system(cmd)
    except:
        print("Could not make the ",dir_name," file exist")

    for benchmark in iccad_2020_benchmark_list:  
        # get in file
        # try:
        #     src_dir = "../build/experiments/input_files/iccad2020/cases/"+benchmark+".txt"
        #     # target_dir = "/" + dir_name +"/"+ benchmark+"_out.txt"
        #     # print(target_dir)
        #     shutil.copy(src_dir, benchmark+".txt")
        # except:
        #     print("not move the input file ",benchmark)
        
        # move reports to target folder 
        for extension in ["_out.txt", ".json", "_log.txt", "_report.json"]:

            src_dir = "../build/experiments/"
            target_dir = "./"+ exp_name +"/"

            # get out file
            try:
                file_name = benchmark + extension
                shutil.move(src_dir + file_name, target_dir + file_name)
            except:
                print("not move the " + extension + " file ",benchmark)
            

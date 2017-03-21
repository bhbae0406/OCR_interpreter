import os

input_dir = os.getcwd() + "/input"
input_files = os.listdir(input_dir)

for input_file in input_files: 
    os.system("./threshTest " + "./input/" + input_file)

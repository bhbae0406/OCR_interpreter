import os

input_dir = os.getcwd() + "/input"
input_files = os.listdir(input_dir)

jpg_files = []
xml_files = []
   
for input_file in input_files:
   extension = output_file.rsplit('.', 1)[1]   
   if extension == "jpg":
      jpg_files.append(input_file)
   else if extension == "xml":
      xml_files.append(input_file)
      
for input_file in xml_files: 
   file_name = input_file.rsplit('.', 1)[0] 
   print("./threshTest " + "./input/" + input_file + " " + file_name)
   os.system("./threshTest " + "./input/" + input_file + " " + file_name)

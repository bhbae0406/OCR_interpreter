import os
from PIL import Image

input_dir = os.getcwd() + "/input"
input_files = os.listdir(input_dir)
jpg_files = []
xml_files = []
   
for input_file in input_files:
   extension = input_file.rsplit('.', 1)
   if len(extension) == 2:
      extension = extension[1]
      if extension == "jpg":
         jpg_files.append(input_file)
      elif extension == "xml":
         xml_files.append(input_file)

for image in jpg_files:
   image_name = image.rsplit('.', 1)[0] 
   print("generating invalid zones for " + image_name)
   os.system("python ./image_processing/mser_filter.py ./input/ ./input/image_region/ " + image_name)
for input_file in xml_files:
   file_name = input_file.rsplit('.', 1)[0]
   image_path = "./input/" + file_name + ".jpg"
   if os.path.exists(image_path):
       im = Image.open(image_path)
       dim = im.size
       print("Dimensions for " + file_name + ": ")
       print(dim)
       os.system("./threshTest " + "./input/" + input_file + " " + file_name + " " + str(dim[0]) + " " + str(dim[1]))

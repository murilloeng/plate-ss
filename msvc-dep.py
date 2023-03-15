#imports
import os
import sys

#data
src_path = sys.argv[1]
obj_path = sys.argv[2]
dep_path = obj_path.replace(".o", ".d")

#misc
def dir_path(inc_path):
	dirs = [
		"inc/",
		os.path.dirname(src_path).replace("\\", "/") + "/"
	]
	for dir in dirs:
		if os.path.isfile(dir + inc_path):
			return dir + inc_path

def file_dependencies(path):
	#data
	dependencies = []
	#open
	file = open(path, "r", encoding="utf-8")
	#loop
	for line in file:
		if line.startswith("#include \""):
			dependencies.append(dir_path(line.strip("\n").strip("#include ").strip("\"")))
	for dependency in dependencies:
		dependencies += file_dependencies(dependency)
	#close
	file.close()
	#return
	return list(set(dependencies))

#main
file = open(dep_path, "w")
file.write(obj_path + " : ")
dependencies = file_dependencies(src_path)
for dependency in dependencies:
	file.write(dependency + " ")
file.close()
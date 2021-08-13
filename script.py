import os

path = os.getcwd()
source = os.path.join(path, "drivers/source")
source_dirs = os.listdir(source)
test_dir = os.path.join(path, "drivers/tests/UNITTESTS")

cmake_file = os.path.join(test_dir, "AnalogIn/CMakeLists.txt")
test_file = os.path.join(test_dir, "AnalogIn/test_analogin.cpp")

for filename in source_dirs:
    try:
        # Create target Directory
        dirName = os.path.join(test_dir, os.path.splitext(filename)[0])
        os.mkdir(dirName)
        os.popen(f"cp {cmake_file} {dirName}/CMakeLists.txt")
        os.popen(f"cp {test_file} {dirName}/test_{os.path.splitext(filename)[0].lower()}.cpp")
        #print("Directory " , dirName ,  " Created ")
        #print(f"cp {test_file} {dirName}/test_{os.path.splitext(filename)[0].lower()}.cpp")
    except FileExistsError:
        print("Directory " , dirName ,  " already exists")


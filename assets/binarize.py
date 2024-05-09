import subprocess
import os.path

# Love the obfuscation
files = bytes.decode(subprocess.check_output(["ls", "--hide=*.py"]), encoding='utf-8').strip().split('\n')
os.mkdir("build/")

for file in files:
    name = os.path.splitext(file)[0]
    new_file = "build/" + name + ".h"
    f = open(new_file, "w")
    subprocess.call(["xxd", "-i", file], stdout=f)
    f.close()

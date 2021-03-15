import os
import subprocess
import random as rd

def test_functions(cmd_stream):
    command_list = " ".join([str(i) for i in cmd_stream])
    cmd_stream = bytes(command_list + "\x04", 'ascii')

    p1 = subprocess.Popen(my_path, cwd=slide_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    p1.stdin.write(cmd_stream)
    p1.stdin.flush()
    res1 = p1.stdout.read()

    p2 = subprocess.Popen(ref, cwd=slide_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    p2.stdin.write(cmd_stream)
    p2.stdin.flush()
    res2 = p2.stdout.read()

    if res1 != res2:
        print("Failure with: " + command_list)

try:
    os.environ.pop('PYTHONIOENCODING')
except KeyError:
    pass

slide_path = "/import/adams/5/z5368211/assignments/slide"
my_path = [slide_path + "/slide_ext_both"]
ref = ['1511', 'slide_ext_both']

cmd_stream = [2, 1, 1, 1, 1, 1, -1, 3, 3, 3, 3, 3, 3, 3]

test_functions(cmd_stream)

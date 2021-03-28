#!usr/bin/python3

import os
import subprocess
import random as rd

def test_functions(cmd_stream):
    my_path = ["./slide_ext"]
    ref = ['1511', 'slide_ext_both']
    slide_path = "/import/adams/5/z5368211/assignments/slide"

    command_list = " ".join([str(i) for i in cmd_stream])
    cmd_stream = bytes(command_list + "\x04", 'ascii')

    res1 = subprocess.Popen(my_path, cwd=slide_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE).communicate(input=cmd_stream)[0]

    res2 = subprocess.Popen(ref, cwd=slide_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE).communicate(input=cmd_stream)[0]

    if res1 != res2:
        print("Failure with: " + command_list + "\n")

def random_block():
    rand_num = rd.random()
    num = 0
    if rand_num < 0.75:
        num = 1
    elif rand_num < 0.875:
        num = 2
    else:
        num =  rd.randint(4,9)

    rand_num = rd.random()
    if rand_num < 0.3:
        return num
    else:
        return -num

def random_command():
    rand_num = rd.random()
    if rand_num < 0.333:
        rand_num = rd.random()
        lst = [1]
        if rand_num < 0.5:
            lst.append(1)
        else:
            lst.append(-1)
        return lst
    elif rand_num < 0.666:
        return [3]
    else:
        rand_num = rd.random()
        lst = [4]
        if rand_num < 0.5:
            lst.append(2)
        else:
            lst.append(1)
        return lst

def valid(y, x):
    if 0 <= y and y <= 14:
        if 0 <= x and x <= 14:
            return True
    return False

def cmd_gen():
    block_count = rd.randint(50,500)
    cmd_list = [block_count]
    for i in range(block_count):
        y = rd.randint(0,14)
        x = rd.randint(0,14)
        cmd_list.append(y)
        cmd_list.append(x)
        cmd_list.append(random_block())
    no_pos = True
    no_valid = True
    i = 3
    while i < len(cmd_list):
        if cmd_list[i] > 0:
            no_pos = False
        if valid(cmd_list[i-2], cmd_list[i-1]):
            no_valid = False
        if not (no_pos or no_valid):
            break
        i += 3

    if no_pos:
        cmd_list[3] = -cmd_list[3]
        if not valid(cmd_list[1], cmd_list[2]):
            cmd_list[1] = 1
            cmd_list[2] = 1
    if no_valid:
        cmd_list[1] = 1
        cmd_list[2] = 1
        if cmd_list[3] < 0:
            cmd_list[3] = -cmd_list[3]
        
    command_count = rd.randint(50,500)
    for _ in range(command_count):
        for i in random_command():
            cmd_list.append(i)

    return cmd_list

if __name__ == "__main__":
    try:
        os.environ.pop('PYTHONIOENCODING')
    except KeyError:
        pass

    count = 0
    while True:
        cmd_list = cmd_gen()
        test_functions(cmd_list)
        count += 1
        if count % 1000 == 0:
            print(count)

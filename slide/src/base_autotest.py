import os
import subprocess
import random as rd

def test_functions(cmd_stream):
    my_path = ["./slide"]
    ref = ['1511', 'slide_reference']
    slide_path = "/import/adams/5/z5368211/assignments/slide"

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
        print("Failure with: " + command_list + "\n")

def random_block():
    rand_num = rd.random()
    if rand_num < 0.75:
        return 1
    else:
        return rd.randint(4,9)


def random_command():
    rand_num = rd.random()
    if rand_num < 0.333:
        rand_num = rd.random()
        if rand_num < 0.75:
            lst = [1]
            if rand_num < 0.375:
                lst.append(1)
            else:
                lst.append(-1)
            return lst
        else:
            return [1,rd.randint(-10,10)]
    elif rand_num < 0.666:
        return [3]
    else:
        rand_num = rd.random()
        if rand_num < 0.75:
            lst = [4]
            if rand_num < 0.375:
                lst.append(2)
            else:
                lst.append(1)
            return lst
        else:
            return [4,rd.randint(-10,10)]

def valid(y, x):
    if 0 <= y and y <= 14:
        if 0 <= x and x <= 14:
            return True
    return False

def cmd_gen():
    block_count = rd.randint(1,200)
    cmd_list = [block_count]
    for i in range(block_count):
        y = rd.randint(-2,16)
        x = rd.randint(-2,16)
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
    if no_valid:
        cmd_list[1] = 1
        cmd_list[2] = 1
        
    command_count = rd.randint(1,50)
    for _ in range(command_count):
        for i in random_command():
            cmd_list.append(i)

    return cmd_list

if __name__ == "__main__":
    try:
        os.environ.pop('PYTHONIOENCODING')
    except KeyError:
        pass

    while True:
        test_functions(cmd_gen())

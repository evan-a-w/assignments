#!usr/bin/python3

import os
import subprocess
import random as rd

def test_functions(command_list, fails):
    my_path = ["./ext_beats"]
    ref = ['1511', 'cs_beats_ext']
    slide_path = "/import/adams/5/z5368211/assignments/beats"

    cmd_stream = bytes(command_list + "\x04", 'ascii')

    r1 = subprocess.Popen(my_path, cwd=slide_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE).communicate(input=cmd_stream)[0]
    r2 = subprocess.Popen(ref, cwd=slide_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE).communicate(input=cmd_stream)[0]

    if r1 != r2:
        print("Failure with fail%d.in\n" % fails)
        f = open("fail%d.in" % fails, "w")
        f.write(command_list)
        f.close()
        return 1
    else:
        return 0

def cmd_gen():
    cmd_list = ""

    no_beats = rd.randint(30,200)
    for beat in range(no_beats):
        omax = (0,0)
        for note in range(rd.randint(5,200)):
            octave = rd.randint(-2,11)
            key = rd.randint(-2,13)
            cmd_list += "a %d %d\n" % (octave, key)
            if omax[0] < octave:
                break
            elif omax[0] == octave and key <= omax[1]:
                break

            if octave == omax[0] and key > omax[1]:
                omax = (octave,key)
            elif octave > omax[0]:
                omax = (octave,key)

        cmd_list += "A\n"
    cmd_list += "P\n"
    cmd_list += ">\n"
    cmd_list += "S test\n"

    beats_to_merge = rd.randint(-2,no_beats)
    merged_beats = rd.randint(-2,beats_to_merge)
    cmd_list += "M %d %d\n" % (beats_to_merge, merged_beats)
    cmd_list += "P\n"

    cmd_list += "L test\n"
    cmd_list += "P\n"

    return cmd_list

if __name__ == "__main__":
    try:
        os.environ.pop('PYTHONIOENCODING')
    except KeyError:
        pass
    
    count = 0
    f = open("last_test.in", "w")
    f.write(cmd_gen())
    f.close()
    fails = 0
    while True:
        fails += test_functions(cmd_gen(), fails)
        count += 1
        if count % 100 == 0:
            print(count)

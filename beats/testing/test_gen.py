s = ""
for i in range(10):
    for y in range(12):
        s += "a %d %d\n" % (i,y)
s += "A\n"
s += "P\n"
s += "S gen_test\n"
s += ">\n"
s += "M 8 2\n"
s += "L gen_test\n"
s += "P\n"

f = open("gen_test.in", "w")
f.write(s)
f.close()

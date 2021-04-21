s = "S empty\n"
for i in range(100000):
    s += "A\n"
s += "S full\n"
s += "L empty\n"
s += "P\n"
s += "L full\n"
s += "P\n"

f = open("gen_test.in", "w")
f.write(s)
f.close()

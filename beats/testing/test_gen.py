s = "S empty\n"
for i in range(45000):
    s += "A\n"
s += "S full\n"
s += "L empty\n"
s += "P\n"
s += "L full\n"
s += "P\n"
print(len(s))

f = open("gen_test.in", "w")
f.write(s)
f.close()

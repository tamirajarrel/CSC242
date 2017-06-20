import random

SYMBOL_SIZE = 90
COMPOUND_SIZE = 100
#m/n = 4.3 by textbook

with open("BigKB.txt", 'w') as f:
    f.write("Symbols:\n")
    for i in range(0, SYMBOL_SIZE):
        f.write(str(i))
        f.write("\n")
    f.write("Compounds:\n")
    for i in range(SYMBOL_SIZE, SYMBOL_SIZE+COMPOUND_SIZE):
        f.write(str(i))
        f.write(" ")
        if random.random() < .1:
            f.write("null ")
            f.write( random.choice(["null ", "not "]) )
        else:
            f.write(str(random.randrange(0,SYMBOL_SIZE)))#i-1
            f.write(" ")
            f.write(random.choice( ["and ", "or ", "imp ", "bicond "]) )


        f.write(str(random.randrange(0, SYMBOL_SIZE)))#i-1
        f.write(" true\n")




    f.write("Target:\n")
    f.write(str(random.randrange(0,SYMBOL_SIZE)))

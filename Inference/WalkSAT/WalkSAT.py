#!/usr/bin/python3.5

# Operator Specification:
# not = not
# and = and
# or = or
# imp = implies
# bicond = biconditional


from abc import ABCMeta, abstractmethod
import copy
import random
import sys

DEPTH_LIMIT = 100000  #If we dont find it after this there probably isnt one


class KnowledgeBase: #the knowledge bsae with a Dict of Symbols and Compound Sentences
    def __init__(self):
        self.Sentences = {}
        self.SymbolTable = []
        self.model = Model()
        self.SatList = {}

    def buildRandomModel(self, symbolList, model):

        if len(symbolList) == 0:
            return
        sym = symbolList[0]
        model.setEntry(sym.name, random.choice([True, False]))
        self.buildRandomModel(symbolList[1:], model)


    def printModel(self):
        self.model.printEntries(self)
        print( "|\t", end="")
        self.printKB(self.model)
        print("")

    def printProblem(self):
        self.printSymbols()
        print("|\t", end="")
        self.printKB()
        print("")

    def printKB(self, model = None):
        isSat = True
        for key in sorted(self.Sentences.keys()):
            if type(self.Sentences[key]) is CompoundSentence:
                if(self.Sentences[key].inKB):
                    if(model == None):
                        print(self.Sentences[key].name, "\t", end="")
                    else:
                        print(self.Sentences[key].isSatisfied(model), "\t", end="")
                        if not(self.Sentences[key].isSatisfied(model)):
                            isSat = False
        if isSat and model != None:
            print("Satisfied!", end="")





    def printSymbols(self):
        for i in self.SymbolTable:
            print(i.name[0:min(len(i.name),6)], "\t", end="")

    def updateSatisfaction(self):
        for key in (self.Sentences.keys()):
            if type(self.Sentences[key]) is CompoundSentence and (self.Sentences[key].inKB):
                self.SatList[self.Sentences[key].name] = self.Sentences[key].isSatisfied(self.model)

    def checkSatisfaction(self):
        for key in self.SatList:
            if not self.SatList[key]:
                return False
        return True

    def countSatisfaction(self):
        count = 0
        for key in self.SatList:
            if self.SatList[key]:
                count+=1
        return count

    def pickRandomUnsatisfied(self):
        slist = (list(self.SatList.keys()))
        r = random.choice(slist)
        while self.SatList[r]:
            r = random.choice(slist)

        sent = r
        return self.goDeeper(self.Sentences[sent])

    def pickBestUnsatisfied(self):
        max = -10000
        mod = self.model.symbolMap
        flipMax = None#default value
        oldSat = self.countSatisfaction()

        for flip in mod:
            mod[flip] = not mod[flip]
            self.updateSatisfaction()
            count = self.countSatisfaction() - oldSat
            if count >= max:
                max = count - oldSat
                flipMax = flip
            mod[flip] = not mod[flip]
        #if flipMax == None:
        #    flipMax = random.choice(mod.keys())
        return self.Sentences[flipMax]





    def pickRandom(self):
        slist = (list(self.SatList.keys()))
        sent = random.choice(slist)
        return self.goDeeper(self.Sentences[sent])

    def goDeeper(self, sent):

        if type(sent) is Symbol:
            return sent
        else:
            if(sent.lhs == None):
                return self.goDeeper(sent.rhs)
            return self.goDeeper(random.choice([sent.lhs,sent.rhs]))







class Sentence:
    __metaclass__ = ABCMeta

    @abstractmethod #this is pythons abstract class implementation if you werent aware
    def isSatisfied(self, model):
        pass


class CompoundSentence(Sentence):
    def __init__(self, name, left, right, op, KB, inKB):
        if left != None:
            self.lhs = KB.Sentences[left]
        else:
            self.lhs = None
        self.rhs = KB.Sentences[right]
        self.op = op #the connective
        self.name = name
        self.KB = KB

        self.inKB = inKB
        KB.Sentences[name] = self


    def isSatisfied(self, model):#switch case over the possible operators
        #print(self.name,self.lhs,self.rhs.name,self.op,self.inKB)


        if(self.lhs is None):#This is the format of a unary sentence
            if(self.op == "not"):



                return not (self.rhs.isSatisfied(model)) #notice that isSatisfied will evaluate to a True or False if rhs is a Symbol, but it will be recursive if rhs is a Sentnce. This works because both classes have the isSatisfied method from the abstract class Sentence
            else:
                return self.rhs.isSatisfied(model)
        elif(self.op == "and"):
            return self.lhs.isSatisfied(model) and self.rhs.isSatisfied(model)
        elif(self.op == "or"):
            return self.lhs.isSatisfied(model) or self.rhs.isSatisfied(model)
        elif(self.op == "imp"):
            return (not self.lhs.isSatisfied(model)) or self.rhs.isSatisfied(model)
        elif(self.op == "bicond"):
            return (self.lhs.isSatisfied(model) == self.rhs.isSatisfied(model))





class Model:    #assigns symbols truth or false values
    def __init__(self):
        self.symbolMap = {}#this maps all symbols to booleans
    def setEntry(self, name, b):
        self.symbolMap[name] = b
    def printEntries(self, KB):
        for i in KB.SymbolTable:
            print(self.symbolMap[i.name], "\t", end="")



class Symbol(Sentence):#Symbols are just atomic Truesies or Falsies
    def __init__(self, name, KB):
        self.KB = KB
        self.name = name
        KB.Sentences[name] = self
        KB.SymbolTable.append(self)
    def isSatisfied(self, model):
        return model.symbolMap[self.name]








def parseKB(fileName, KB):
    with open(fileName) as f:
        s = f.readline()
        #print(s)
        if(s != "Symbols:\n"):
            raise(IOError("Bad Format"))
        s = f.readline()
        while(s != "Compounds:\n"):
            s=s.split()#cutoff the newline
            #print(s)
            sym = Symbol(s[0], KB)
            s=f.readline()
        s= f.readline()
        while(s != "Target:\n"):
            print(s)
            s = s.split()
            name = s[0]#a string name for the compound
            left = s[1]#a string name for the left symbol/compound
            op = s[2]
            right = s[3]#a string name for the right symbol/compound
            inKB = s[4]
            if left == "null":
                left = None
            if op == "null":
                op = None
            newCompound = CompoundSentence(name, left, right, op, KB, (inKB == "true"))
            s=f.readline()

        s=f.readline()

        s = s.split()
        name = s[0]#a string name for the compound
        left = s[1]#a string name for the left symbol/compound
        op = s[2]
        right = s[3]#a string name for the right symbol/compound
        if left == "null":
            left = None
        if op == "null":
            op = None
        KB.Target = CompoundSentence(name, left, right, op, KB, False)


def walkSat(KB, depth, ret):

    while depth > 0:
        KB.updateSatisfaction()
        b = KB.checkSatisfaction()
        KB.printModel()
        if b:
            ret.value = depth
            return True
        else:


            #A 50% probability of random or greedy pick
            flip = KB.pickRandomUnsatisfied().name if random.choice([True,False]) else KB.pickBestUnsatisfied().name

            KB.model.symbolMap[flip] = not KB.model.symbolMap[flip]
            depth -= 1
    ret.value = 0
    return False



class intWrapper:
    def __init__(self):
        self.value = 0

def checkEntailment(KB):
    maxDepth = DEPTH_LIMIT
    x = intWrapper()
    KB.buildRandomModel(KB.SymbolTable, KB.model)

    success = walkSat(KB, maxDepth,x)
    print("\n")
    if not success:
        print("No satisfying model found for given Knowledge Base")
    else:

        maxDepth = ( maxDepth - x.value+2)*30  #really rough guess, if it takes x tries to find the satisfying solutions, we should be able to find the contradiction if there is one in 100*x moves
        print(maxDepth)

        NotTarget = CompoundSentence("NotTarget", None, KB.Target.name, "not", KB, True)
        KB.buildRandomModel(KB.SymbolTable, KB.model)
        success = walkSat(KB, maxDepth,x)
        propName = KB.Target.name
        if(success):
            print("Model found which satisfies NOT ", propName, ", ", propName, " is not entailed.")
        else:
            print("No model found which satisfies NOT ", propName, ", ", propName, " is likely entailed.")





KB = KnowledgeBase()
parseKB(sys.argv[1], KB)


KB.printProblem()


checkEntailment(KB)

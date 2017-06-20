#!/usr/bin/env python3

# Operator Specification:
# not = not
# and = and
# or = or
# imp = implies
# bicond = biconditional


from abc import ABCMeta, abstractmethod
import copy
import sys

class KnowledgeBase: #the knowledge bsae with a Dict of Symbols and Compound Sentences
    def __init__(self):
        self.Sentences = {}
        self.SymbolTable = []
        self.modelList = []
        self.Target = ""

    def buildModels(self, symbolList, model):

        if len(symbolList) == 0:
            self.modelList.append(model)
            return


        sym = symbolList[0]

        modelA = copy.deepcopy(model)

        model.setEntry(sym.name, False)
        modelA.setEntry(sym.name, True)

        self.buildModels(symbolList[1:], model)
        self.buildModels(symbolList[1:], modelA)



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

    def checkEntailment(self, propName):
        entailed = True
        for model in self.modelList:
            kbSat = True
            for key in sorted(self.Sentences.keys()):
                if ( type(self.Sentences[key]) is CompoundSentence ) and (self.Sentences[key].inKB) and (not self.Sentences[key].isSatisfied(model)):
                    kbSat = False
                    break
            if  (kbSat and not model.symbolMap[propName]):
                entailed = False

        return entailed



    def printSymbols(self):
        for i in self.SymbolTable:
            print(i.name[0:min(len(i.name)-1,6)], "\t", end="")


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
        self.inKB = inKB
        self.KB = KB
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
        s=s.split()[0]
        KB.Target = s












KB = KnowledgeBase()
parseKB(sys.argv[1], KB)

model = Model()
KB.buildModels(KB.SymbolTable, model)
KB.printSymbols()
print("|\t", end="")
KB.printKB()
print("")
for i in KB.modelList:
    i.printEntries(KB)
    print("|\t", end="")
    KB.printKB(i)
    print("")

if(KB.checkEntailment(KB.Target)):
    print(KB.Target,"is entailed")
else:
    print(KB.Target,"is not entailed")

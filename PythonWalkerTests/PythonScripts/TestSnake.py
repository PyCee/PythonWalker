 
from math import sqrt
from TestAnimal import TestAnimal

gloVar = 0

class Position:
    def __init__(self):
        self.X = 0.0
        self.Y = 0.0
    def Pythag(self):
        return sqrt(self.X * self.X + self.Y * self.Y)

class TestSnake (TestAnimal):
    def __init__(self):
        super().__init__()
        self.Name = "Snakey"
        self.Length = 1
        self.birthplace = ""
        self.TerrariumWidth = 1
        self.TerrariumDepth = 2.0
        self.TerrariumHeight = 2.0
        self.position = Position()
        self.Friends = []
    def ReturnIntSeven(self):
        return 7
    def SetName(self,newName):
        self.Name = newName
    def returnSound(self):
        return self.sound
    def returnLength(self):
        return self.length
    def GetGlobalVar(self):
        return gloVar
    def SetGlobalVar(self, value):
        global gloVar
        gloVar = value
    def calculateTerrariumArea(self):
        return self.TerrariumHeight * self.TerrariumDepth * self.TerrariumWidth
    def AddThreeParametersTogether(self, a, b, c):
        return a+b+c
    def AddNumbersTogether(self, numbers):
        return sum(numbers)
    def AddNumbersTogetherList(self, numbers):
        return self.AddNumbersTogether(numbers)
    def getFriends(self):
        return self.friends
    def CountToFive(self):
        return [1, 2, 3, 4, 5]
    def NoParamFunction(self):
        pass
    def HitPythonTypeError(self):
        self.age = "bad" + 7.0 + False
    def PythonPrintMessage(self, msg):
        print(msg)
    def GetPositionXIndirectly(self):
        return self.position.X
    def MethodWithPass(self):
        pass
    



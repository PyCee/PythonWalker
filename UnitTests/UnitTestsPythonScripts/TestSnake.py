 
from math import sqrt
from TestAnimal import TestAnimal

gloVar = 0

class Position:
    def __init__(self):
        self.X = 0.0
        self.Y = 0.0
    def pythag(self):
        return sqrt(self.X * self.X + self.Y * self.Y)

class TestSnake (TestAnimal):
    def __init__(self):
        super().__init__()
        self.name = "Snakey"
        self.birthplace = ""
        self.terrariumWidth = 1
        self.terrariumDepth = 2.0
        self.terrariumHeight = 2.0
        self.position = Position()
        self.friends = []
    def returnSound(self):
        return self.sound
    def returnLength(self):
        return self.length
    def getGlobalVar(self):
        return gloVar
    def setGlobalVar(self, value):
        global gloVar
        gloVar = value
    def calculateTerrariumArea(self):
        return self.terrariumHeight * self.terrariumDepth * self.terrariumWidth
    def AddThreeParametersTogether(self, a, b, c):
        return a+b+c
    def AddNumbersTogether(self, numbers):
        return sum(numbers)
    def getFriends(self):
        return self.friends
    def countToFive(self):
        return (1, 2, 3, 4, 5)
    def noParamFunction(self):
        pass
    def issueFunction(self):
        self.age = "bad" + 7.0 + False
    def printMessage(self, msg):
        print(msg)
    



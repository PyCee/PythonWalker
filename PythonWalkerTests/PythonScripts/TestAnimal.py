 
class TestAnimal(object):
    def __init__(self):
        self.name = "Animal"
        self.length = 1
        self.Age = 1
        self.IsCool = False
    def IncreaseAgeByOne(self):
        self.Age += 1
    def changeName(self, newName):
        self.name = newName



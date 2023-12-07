 
class TestAnimal(object):
    def __init__(self):
        self.name = "Animal"
        self.length = 1
        self.age = 1
        self.isCool = False
    def increaseAgeByOne(self):
        self.age += 1
    def changeName(self, newName):
        self.name = newName



 
class TestPlant(object):
    def __init__(self):
        self.name = "Plant"
        self.length = 1
        self.age = 1
        self.isAnnual = False
    def increaseAgeByOne(self):
        self.age += 1
    def changeName(self, newName):
        self.name = newName



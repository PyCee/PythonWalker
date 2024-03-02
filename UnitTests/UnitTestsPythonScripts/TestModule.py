
helloWorld = "HelloWorld"
gloVar = 1

def moduleFunctionMultplyGlobalByTwo():
    global gloVar
    print("gloVar before " + str(gloVar))
    gloVar = gloVar * 2
    print("gloVar after " + str(gloVar))

def moduleFunctionReturnHelloWorld():
    return helloWorld

def moduleFunctionMultply(num1, num2):
    return num1 * num2
    



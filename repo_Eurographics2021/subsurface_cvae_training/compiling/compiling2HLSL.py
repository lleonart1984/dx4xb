import math
from torch import nn

def flattenLayers (model):
    # case is a sequential
    if isinstance(model, nn.Sequential):
        for m in model: # iterate over submodules
            for l in flattenLayers(m):
                yield l
        return
    yield model

def compileLayerExecution(layer : nn.Linear, activations, inputVariables, outputVariables, outputVariableTypes):
    M = layer.weight.detach().numpy() # dense weights
    W = layer.bias.detach().numpy() # bias weights
    input_size = layer.in_features
    output_size = layer.out_features

    # Get matrices values
    m = [] # matrices values
    col = 0
    remain_cols = input_size
    while remain_cols > 0:
        row = 0
        remain_rows = output_size
        cols = min(4, remain_cols)
        while remain_rows > 0:
            rows = min(4, remain_rows)
            m.append("float"+str(cols)+"x"+str(rows)+"("+','.join([str(v)+"f" for v in [M[i][j] for j in range(col*4, col*4 + cols) for i in range(row*4, row*4+rows)]])+")")
            remain_rows -= rows
            row += 1
        remain_cols -= cols
        col += 1

    # Get bias vectors
    b = [] # bias vectors
    index = 0
    remain = output_size
    while remain > 0:
        varSize = min(4, remain)
        b.append("float"+str(varSize)+"("+','.join([str(v)+"f" for v in [W[i] for i in range(index*4, index*4 + varSize)]])+")")
        remain -= varSize
        index += 1

    activating = ""
    for a in activations:
        activating = a+"("+activating

    # produce output assignaments
    code = ""
    index = 0
    for oVar, oType in zip(outputVariables, outputVariableTypes):
        code += oType +" "+oVar + " = " + activating +'+'.join(["mul("+inputVariables[x]+","+m[x * math.ceil(output_size/4)+index]+")" for x in range(0, len(inputVariables))])+" + "+b[index]+ (')'*len(activations))+ "; \n\r"
        index += 1
    return code

def compileSigmoidActivation(vecType):
    return " "+vecType+" sigmoidActivation("+vecType+" x){ return 1.0f / (1.0f + exp(-x)); }\n"

def compileSoftPlusActivation(vecType):
    return " "+vecType+" softplusActivation("+vecType+" x){ return log(1 + exp(x)); }\n"

def compileTanHActivation(vecType):
    return " "+vecType+" tanhActivation("+vecType+" x){ return tanh(x); }\n"

def compileRELUActivation(vecType):
    return " "+vecType+" reluActivation("+vecType+" x){ return max(0, x); }\n"

def compileActivation(activation):
    compileFunction = None
    functionName = "unknown"

    if isinstance(activation, nn.Sigmoid):
        compileFunction = compileSigmoidActivation
        functionName = "sigmoidActivation"

    if isinstance(activation, nn.Softplus):
        compileFunction = compileSoftPlusActivation
        functionName = "softplusActivation"

    if isinstance(activation, nn.Tanh):
        compileFunction = compileTanHActivation
        functionName = "tanhActivation"
    
    if isinstance(activation, nn.ReLU):
        compileFunction = compileRELUActivation
        functionName = "reluActivation"

    code = ""
    # compile all functions possible overloads
    for i in range(1, 5):
        code += compileFunction("float"+str(i))+"\n"
    return (code, functionName)

def compileModelToHLSL(models):
    code = ""

    activations = {}

    for modelName, model in models.items():
        layers = list(flattenLayers(model))
        index = 0
        linearLayerToCompile = None
        activationsToApply = []

        modelInputSize = layers[0].in_features

        bodyCode = ""

        def declareVars(layerIndex, nodes):
            remain = nodes
            n = []
            nt = []
            varIndex = 0
            while remain > 0:
                varSize = min(4, remain)
                n.append("n_"+str(layerIndex)+"_"+str(varIndex))
                nt.append("float"+str(varSize))
                remain -= varSize
                varIndex += 1
            return n, nt

        # Declare input nodes and assign the parameter array
        currentLayerNodes, currentLayerNodeTypes = declareVars(0, modelInputSize)
        remain = modelInputSize
        startIndex = 0
        for ivar, itype in zip(currentLayerNodes, currentLayerNodeTypes):
            bodyCode += itype+" "+ivar+" = "+itype+"("+",".join(['_input['+str(startIndex + x)+']' for x in range(min(4, remain))])+");\n"
            startIndex += 4
            remain -= 4

        def compileCurrentLayer():
            nonlocal index
            nonlocal bodyCode
            nonlocal currentLayerNodes
            nonlocal currentLayerNodeTypes
            if linearLayerToCompile is None:
                return
            outputVariables, outputVariableTypes = declareVars(index + 1, linearLayerToCompile.out_features)
            # add layer function
            bodyCode += compileLayerExecution(linearLayerToCompile, activationsToApply, currentLayerNodes, outputVariables, outputVariableTypes)
            currentLayerNodes = outputVariables
            currentLayerNodeTypes = outputVariableTypes
            index += 1


        for l in layers:
            if isinstance(l, nn.Linear): # New dense layer compile last
                compileCurrentLayer()
                linearLayerToCompile = l
                activationsToApply = []
            else:
                if not ( type(l) in activations ):
                    activationCode, name = compileActivation(l)
                    activations[type(l)] = name
                    code += activationCode
                activationName = activations[type(l)] # get the name of the activation
                activationsToApply.append(activationName) # append for future layer compilation
        compileCurrentLayer()

        # append model main function to code
        # function signature
        modelOutputSize = linearLayerToCompile.out_features # last layer output size
        code += "void "+modelName+"(float _input["+str(modelInputSize)+"], out float _output["+str(modelOutputSize)+"]) { \n"

        code += bodyCode # append input copying and layer executions

        # set values to output
        for i in range(0, modelOutputSize):
            outputIndex = int(i/4)
            outputComponent = i % 4
            code += "_output["+str(i)+"] = "+currentLayerNodes[outputIndex]+"["+str(outputComponent)+"];\n"

        code += "}\n"

    return code
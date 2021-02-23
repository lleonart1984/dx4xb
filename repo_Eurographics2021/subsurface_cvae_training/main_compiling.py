import torch
from torch import nn
from torch.nn import functional as F
from torch import Tensor
from typing import *
import numpy as np

import hashlib

from cvae.modeling import CVAEModel
import cvae.training

import hashlib

import compiling.compiling2HLSL

folder = '.\\Running'

# Load lenGenModel

def lenGenFactory(depth, width, activation, latent):
    return CVAEModel(2, 1, latent, width, depth, activation)

lenGenModel = cvae.training.get_last_state(
    model_name='lenGen', 
    model_type='cvae', 
    model_factory=lenGenFactory,
    config = {
        'depth': 2,
        'width': 8,
        'activation': nn.Softplus,
        'latent': 2
    },
    folder='.\\Running',
    top_state=None
)

# Load pathGenModel

def pathGenFactory(depth, width, activation, latent):
    return CVAEModel(3, 3, latent, width, depth, activation)

pathGenModel = cvae.training.get_last_state(
    model_name='pathGen', 
    model_type='cvae', 
    model_factory=pathGenFactory,
    config = {
        'depth': 3,
        'width': 16,
        'activation': nn.Softplus,
        'latent': 5
    },
    folder='.\\Running',
    top_state=None
)

# Load scatGenModel

def scatGenFactory(depth, width, activation, latent):
    return CVAEModel(7, 6, latent, width, depth, activation)

scatGenModel = cvae.training.get_last_state(
    model_name='scatGen', 
    model_type='cvae', 
    model_factory=scatGenFactory,
    config = {
        'depth': 3,
        'width': 12,
        'activation': nn.Softplus,
        'latent': 5
    },
    folder='.\\Running',
    top_state=None
)

code = compiling.compiling2HLSL.compileModelToHLSL({
    'lenModel' : lenGenModel.model.cpu().decoder.model.model,
    'pathModel' : pathGenModel.model.cpu().decoder.model.model,
    'scatModel' : scatGenModel.model.cpu().decoder.model.model
})

file = open('compiledModels.h','w+')
file.write(code)
file.close()
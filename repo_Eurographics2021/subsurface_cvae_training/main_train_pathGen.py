#%% [markdown]
'''
# Generating the PathGen Model.

This model learns how to generate the outgoing position and direction given by cos theta, beta and alpha.
'''
# %%

import torch
from torch import nn
from torch.nn import functional as F
from torch import Tensor
import numpy as np

import hashlib

from cvae.dataman import DataManager
from cvae.modeling import CVAEModel
from cvae.training import train_models

# %% [markdown]
'''
Loading the data set. Selecting from every row the values of sigma (density) and g (anisotropy factor), and 
value of n (number of scatters log scale) as conditioning. Result z, and tangent beta and alpha values.
'''
# %%
# Loading the data set for training the LenGen model
pathGen_ds = DataManager('.\\DataSets\\ScattersDataSet.npz', 
            conditions = {
                'sigma' : None,
                'g'     : None,
                'n'     : lambda n: np.log(n)
            },
            targets = {
                'z'             : None,
                'tangent_beta'  : None,
                'tangent_alpha' : None
            }
)
pathGen_ds.set_device(torch.device('cuda:0')) # move the data set to gpu memory.
# %%

# factory function for the CVAE model in case of the len
def pathGenFactory(depth, width, activation, latent):
    return CVAEModel(3, 3, latent, width, depth, activation)

# %% [markdown]
'''
Training different configuration is possible. The idea is to choose a compact model but without sacrifying accuracy.
Testings should vary widths (making the network wider) and depth.
'''
# %%
pathGenConfigs = [
    {
        'depth': 2,
        'width': 12,
        'activation': nn.Softplus,
        'latent': 5
    },
    {
        'depth': 3,
        'width': 16,
        'activation': nn.Softplus,
        'latent': 5
    }
]
# %%

train_models(
    model_name='pathGen', model_type='cvae', model_factory=pathGenFactory,
    configs = pathGenConfigs,
    data = pathGen_ds,
    output_folder='.\\Running',
    batch_size = 16*1024,
    epochs = 16000
)

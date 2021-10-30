#%% [markdown]
'''
# Generating the ScatGen Model.

This model learns how to generate the representative position and direction given 
.
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
Loading the data set. 
'''
# %%

# Loading the data set for training the LenGen model
scatGen_ds = DataManager('.\\DataSets\\ScattersDataSet.npz', 
            conditions = {
                'sigma'         : None,
                'g'             : None,
                'albedo'        : lambda a: np.power(1 - a, 1./6.),
                'n'             : lambda n: np.log(n),
                'z'             : None,
                'tangent_beta'  : None,
                'tangent_alpha' : None
            },
            targets = {
                'representative_Xx'            : None,
                'representative_Xy'            : None,
                'representative_Xz'            : None,
                'representative_Wx'            : None,
                'representative_Wy'            : None,
                'representative_Wz'            : None
            }
)
scatGen_ds.set_device(torch.device('cuda:0')) # move the data set to gpu memory.

# %%

# factory function for the CVAE model in case of the scatGen model
def scatGenFactory(depth, width, activation, latent):
    return CVAEModel(7, 6, latent, width, depth, activation)
# %%
scatGenConfigs = [
    {
        'depth' : 3,
        'width' : 12,
        'activation' : nn.Softplus,
        'latent' : 5
    },
    {
        'depth' : 3,
        'width' : 16,
        'activation' : nn.Softplus,
        'latent' : 5
    }
]
# %%
train_models(
    model_name='scatGen', model_type='cvae', model_factory=scatGenFactory,
    configs = scatGenConfigs,
    data = scatGen_ds,
    output_folder='.\\Running',
    batch_size = 16*1024,
    epochs = 16000
)
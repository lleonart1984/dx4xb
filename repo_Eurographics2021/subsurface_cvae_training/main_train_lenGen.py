#%% [markdown]
'''
# Generating the LenGen Model.

This model learns how to generate the number of scatters event (more specifically the log value of it).
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
Loading the data set. Selecting from every row the values of sigma (density) and g (anisotropy factor) as conditioning.
The value of n (number of scatters) is scaled to logarithm.
'''
# %%
# Loading the data set for training the LenGen model
lenGen_ds = DataManager('.\\DataSets\\ScattersDataSet.npz', 
            conditions = {
                'sigma' : None,
                'g'     : None
            },
            targets = {
                'n'     : lambda n: np.log(n)
            }
)
lenGen_ds.set_device(torch.device('cuda:0')) # move the data set to gpu memory.
# %%

# factory function for the CVAE model in case of the len
def lenGenFactory(depth, width, activation, latent):
    return CVAEModel(2, 1, latent, width, depth, activation)

# %% [markdown]
'''
Training different configuration is possible. The idea is to choose a compact model but without sacrifying accuracy.
Testings should vary widths (making the network wider) and depth.
'''
# %%
lenGenConfigs = [
    {
        'depth': 3,
        'width': 8,
        'activation': nn.Softplus,
        'latent': 2
    }
]
# %%

train_models(
    model_name='lenGen', model_type='cvae', model_factory=lenGenFactory,
    configs = lenGenConfigs,
    data = lenGen_ds,
    output_folder='.\\Running',
    batch_size = 16*1024,
    epochs = 16000
)

# %%

# %%

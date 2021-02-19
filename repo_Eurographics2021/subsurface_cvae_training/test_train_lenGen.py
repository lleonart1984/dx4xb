#%% [markdown]
'''
# Visualization of the LenGen model

This notebook allows to explore the distribution of the LenGen generated values and compare with the true distribution.
The main visualization is histograms with log scale for the number of scatters.
'''
# %%
import torch
from torch import nn
from torch.nn import functional as F
from torch import Tensor
from typing import *
import numpy as np

import hashlib

from cvae.modeling import CVAEModel
import cvae.training

from generating.scatters import Generator

import matplotlib.pyplot as plt
# if using a Jupyter notebook, includue:
# %matplotlib inline
# %%

# factory function for the CVAE model in case of the lenGen
def lenGenFactory(depth, width, activation, latent):
    return CVAEModel(2, 1, latent, width, depth, activation)

# %%
lenConfigs = [
    {
        'depth': 3,
        'width': 8,
        'activation': nn.Softplus,
        'latent': 2
    }
]
# %%

states = cvae.training.get_last_states(
    model_name='lenGen', model_type='cvae', model_factory=lenGenFactory,
    configs = lenConfigs,
    output_folder='.\\Running',
    top_state=None
)

# %%

def genLogNbyMC (sigma, g, samples = 10000):
    gen = Generator(samples)
    N, x, w, X, W = gen.generate_batch(sigma, g, 1.0)
    return torch.log(N)

def genNbyMC (sigma, g, samples = 10000):
    gen = Generator(samples)
    N, x, w, X, W = gen.generate_batch(sigma, g, 1.0)
    return N

def genLogNbyModel (sigma, g, model : CVAEModel, samples = 10000):
    sigma = torch.full(size = (samples, 1), fill_value=sigma, dtype=torch.float32).to(torch.device('cuda:0'))
    g = torch.full(size = (samples, 1), fill_value=g, dtype=torch.float32).to(torch.device('cuda:0'))
    logN = torch.clamp_min((model.sampleDecoder (torch.cat([sigma, g], dim=1))), 0)
    return torch.log(torch.round(torch.exp(logN)));

def genNbyModel (sigma, g, model : CVAEModel, samples = 10000):
    sigma = torch.full(size = (samples, 1), fill_value=sigma, dtype=torch.float32).to(torch.device('cuda:0'))
    g = torch.full(size = (samples, 1), fill_value=g, dtype=torch.float32).to(torch.device('cuda:0'))
    logN = torch.clamp_min((model.sampleDecoder (torch.cat([sigma, g], dim=1))), 0)
    return torch.round(torch.exp(logN)+0.49)

# %%

def viewKDComparison(sigma, g, model):
    SAMPLES = 10000
    realLogN = genNbyMC(sigma, g, SAMPLES)
    modelLogN = genNbyModel(sigma, g, model, 10*SAMPLES)
    # modelN = genMuByModel(sigma, g, model, 10*SAMPLES)
    logbins = np.logspace(0,10,100, base=2.718)
    plt.xscale('log')
    # logbins = np.linspace(0,10,100)
    _ = plt.hist(torch.clamp_min(realLogN + torch.randn_like(realLogN)*0.5, 1.0).cpu().numpy(), bins=logbins, density = True)
    _ = plt.hist(torch.clamp_min(modelLogN + torch.randn_like(modelLogN)*0.5, 1.0).cpu().detach().numpy(), bins=logbins, density = True, histtype='step')

# %%

def viewState(state):
    _ = plt.plot(state.loss_history[-400 : -1])
    _ = plt.plot(state.kl_history[-400 : -1])
    _ = plt.figure()
    # gs = [-0.7, 0.0]
    gs = [-0.7, 0.0, 0.4, 0.9]
    # densities = [1, 2]
    densities = [1, 5, 25, 125]
    plt.tight_layout(pad=0.0, h_pad=0.0, w_pad=0.0)
    fig, slots = plt.subplots(len(densities), len(gs), figsize=(len(gs)*4,len(densities)*1.5), sharex=True)
    plt.yticks([])
    row = 0
    for density in densities:
        figure = 0
        for g in gs:
            rowstr = "$\sigma_t="+str(density)+"$"
            slot = slots[row][figure]
            plt.tight_layout(pad=0.0, h_pad=0.0, w_pad=0.0)
            plt.subplot(slot)
            plt.yticks([])
            plt.ylabel("")
            plt.margins(x = 0, y = 0)
            plt.tight_layout(pad=0.0, h_pad=0.0, w_pad=0.0)
             # plt.xscale('log')
            if row == len(densities) - 1:
                plt.xlabel("$g="+str(g)+"$")
            if figure == 0:
                plt.ylabel(rowstr)
            viewKDComparison(density, g, state.model)
            figure += 1
        row += 1        
    plt.margins(0,0)
    plt.savefig('tight_logn_pdf_2_8.pdf')

# %%

for s in states:
    _ = plt.plot(s.lrs[:-1])

# %%

for s in states:
    viewState(s)


# %%

# %%

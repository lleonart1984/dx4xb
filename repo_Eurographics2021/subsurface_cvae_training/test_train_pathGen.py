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

# factory function for the CVAE model in case of the path
def pathGenFactory(depth, width, activation, latent):
    return CVAEModel(3, 3, latent, width, depth, activation)

# %%
pathGenConfigs = [
    {
        'depth': 3,
        'width': 16,
        'activation': nn.Softplus,
        'latent': 5
    }
    # ,
    #{
    #    'depth': 3,
    #    'width': 16,
    #    'activation': nn.Softplus,
    #    'latent': 5
    #}
]

# %%

states = cvae.training.get_last_states(
    model_name='pathGen', model_type='cvae', model_factory=pathGenFactory,
    configs = pathGenConfigs,
    output_folder='.\\Running',
    top_state=None
)

# %%

def genXzbyMC (sigma, g, samples = 10000):
    gen = Generator(samples)
    N, x, w, X, W = gen.generate_batch(sigma, g, 1.0)
    return N, x[:,2]

def genXzbyModel (sigma, g, N, model : CVAEModel, samples = 10000):
    sigma = torch.full(size = (samples, 1), fill_value=sigma, dtype=torch.float32).to(torch.device('cuda:0'))
    g = torch.full(size = (samples, 1), fill_value=g, dtype=torch.float32).to(torch.device('cuda:0'))
    return torch.clamp(
        model.sampleDecoder (
            torch.cat([sigma, g, torch.log(N)], dim=1), onlyMu=False
        )[:,0], -1, 1)

def viewKDComparison(sigma, g, model):
    SAMPLES = 10000
    realN, realXz = genXzbyMC(sigma, g, SAMPLES)
    modelXz = genXzbyModel(sigma, g, realN, model, SAMPLES)
    plt.xscale('linear')
    _ = plt.hist(realXz.cpu().numpy(), bins=50, density = True)
    _ = plt.hist(modelXz.cpu().detach().numpy(), bins=50, density = True, histtype='step')
# %%
def viewState(state):
    #_ = plt.plot(state.lossHistory[20 : -1])
    #_ = plt.plot(state.klDivHistory[20 : -1])
    _ = plt.figure()
    # gs = [-0.7, 0.0]
    gs = [-0.7, 0.0, 0.4, 0.9]
    # densities = [1, 2]
    densities = [1, 5, 25, 125]
    fig, slots = plt.subplots(len(densities), len(gs), figsize=(len(gs)*4,len(densities)*1.5), sharex=True)
    plt.yticks([])
    row = 0
    for density in densities:
        figure = 0
        for g in gs:
            rowstr = "$\sigma_t="+str(density)+"$"
            slot = slots[row][figure]
            plt.tight_layout(pad=0.0, h_pad=1.2, w_pad=1.4)
            plt.subplot(slot)
            plt.xticks(rotation=45)
            #plt.tight_layout(pad=0.0, h_pad=0.0, w_pad=0.0)
            plt.yticks([])
            plt.ylabel("")
            plt.xscale('log')
            if row == len(densities) - 1:
                plt.xlabel("$g="+str(g)+"$")
            if figure == 0:
                plt.ylabel(rowstr)
            viewKDComparison(density, g, state.model)
            figure += 1
        row += 1        
    plt.margins(0,0)
    plt.savefig('tight_theta_pdf_3_16.pdf')
# %%
for s in states:
    _ = plt.plot(s.lrs[-1000:])
# %%
for s in states:
    viewState(s)
# %%
def getBetaAndAlpha(x : Tensor, w : Tensor):
    N = x
    B = torch.cross(
        N,
        torch.Tensor([[0.000017,0.00000013,1.0]]).repeat(len(x), 1).to(x.device)
    )
    B = B / torch.sqrt(torch.sum(B**2, dim=1, keepdim=True))
    T = torch.cross(N, B)
    return torch.sum(T * w, dim = -1), torch.sum(B * w, dim = -1)

def genZBAbyMC (sigma, g, samples = 10000):
    gen = Generator(samples)
    N, x, w, X, W = gen.generate_batch(sigma, g, 1.0)
    B, A = getBetaAndAlpha(x, w)
    return N, x[:,2], B, A

def genZBAbyModel (sigma, g, N, model : CVAEModel, samples = 10000):
    sigma = torch.full(size = (samples, 1), fill_value=sigma, dtype=torch.float32).to(torch.device('cuda:0'))
    g = torch.full(size = (samples, 1), fill_value=g, dtype=torch.float32).to(torch.device('cuda:0'))
    XBA = model.sampleDecoder (torch.cat([sigma, g, torch.log(N)], dim=1), onlyMu=False)
    return torch.clamp(XBA[:,0],-1,1), torch.clamp(XBA[:,1],-1,1), torch.clamp( XBA[:,2],-1,1)
# %%
import scipy.ndimage as ndimage
from matplotlib import ticker, cm, colors

def viewZBAComparison(sigma, g, model, showA = False):
    SAMPLES = 10000
    realN, realZ, realB, realA = genZBAbyMC(sigma, g, SAMPLES)
    modelZ, modelB, modelA = genZBAbyModel(sigma, g, realN, model, SAMPLES)
    realCos = realA if showA else realB # torch.sqrt(torch.clamp_min(1 - realB**2 - realA**2, 0))
    modelCos = modelA if showA else modelB # torch.sqrt(torch.clamp_min(1 - modelB**2 - modelA**2, 0))
    BINS = 40
    realHist, _, _ = np.histogram2d(x = (realZ).cpu().numpy(), y = realCos.cpu().numpy(), range=[[-1.5, 1.5], [-1.5,1.5]], bins = BINS, density = True)
    modelHist, _, _ = np.histogram2d(x = modelZ.detach().cpu().numpy(), y = modelCos.detach().cpu().numpy(), range=[[-1.5, 1.5], [-1.5,1.5]], bins = BINS, density = True)
    realZ = ndimage.gaussian_filter(realHist.T, sigma=2, order=0)
    modelZ = ndimage.gaussian_filter(modelHist.T, sigma=2, order=0)
    X = np.linspace(-1.5,1.5,BINS)
    Y = np.linspace(-1.5,1.5,BINS)
    levels = [0.001, 0.01, 0.1, 0.25, 1, 4, 12.8, 25.6, 51.2]
    cs = plt.contourf(X, Y, realZ, levels = levels, interpolation='bicubic', norm = colors.LogNorm(), cmap=cm.Blues_r)
    ct = plt.contour(X,Y, modelZ, levels = levels, interpolation='bicubic', norm = colors.LogNorm(), cmap=cm.Oranges_r) 
    #plt.clabel(ct,fmt='%.1f')
    #plt.clabel(cs,fmt='%.1f')
# %%
def viewState(state):
    # gs = [-0.7, 0.0]
    gs = [-0.7, 0.0, 0.4, 0.9]
    # densities = [1, 2]
    densities = [1, 5, 25, 125]
    fig, slots = plt.subplots(len(densities), len(gs), figsize=(len(gs)*2,len(densities)*2), sharex=True)
    plt.yticks([])
    row = 0
    for density in densities:
        figure = 0
        for g in gs:
            rowstr = "$\sigma_t="+str(density)+"$"
            slot = slots[row][figure]
            plt.tight_layout(pad=0.0, h_pad=0.2, w_pad=0.4)
            plt.subplot(slot)
            plt.yticks([])
            plt.ylabel("")
            #plt.xscale('log')
            if row == len(densities) - 1:
                plt.xlabel("$g="+str(g)+"$")
            if figure == 0:
                plt.ylabel(rowstr)
            viewZBAComparison(density, g, state.model, showA=False)
            figure += 1
        row += 1        
    plt.margins(0,0)
    plt.savefig('tight_beta_pdf_3_16.pdf')
# %%
for s in states:
    viewState(s)
# %%

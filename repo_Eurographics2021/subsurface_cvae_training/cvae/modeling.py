import torch
from torch import nn
from torch.nn import functional as F
from torch import Tensor
from typing import List
import numpy as np

# Block Model. Represents a simple model with depth layers of width nodes and input_dim, output_dim as first and last layer size.
class BlockModel (nn.Module):
    
    @staticmethod
    def dense(a, b):
        '''
        Tool method to create a layer of b nodes fully connected with previous a nodes.
        The initialization is xavier uniform
        '''
        d = nn.Linear(a, b)
        torch.nn.init.xavier_uniform_(d.weight)
        return d

    def __init__(self, input_dim, output_dim, width, depth, activation=nn.ReLU, activationAtLast = False):
        '''
        Creates a NN with <depth> hidden layers of <width> nodes each.
        Input layer has size <input_dim> and output layer <output_dim>.
        All dense layers are activated with <activation> except last one that uses linear. 
        '''
        super(BlockModel, self).__init__()
        modules = []
        # first hidden layer
        modules.append(nn.Sequential(BlockModel.dense(input_dim, width if depth > 0 else output_dim), activation() if depth > 0 else None)) 
        # next depths - 1 hidden layers
        if depth > 0:
            for _ in range(depth - 1): 
                modules.append(nn.Sequential(BlockModel.dense(width, width), activation()))
            # last layer
            if activationAtLast:
                modules.append(nn.Sequential(BlockModel.dense(width, output_dim), activation()))
            else:
                modules.append(BlockModel.dense(width, output_dim)) # last layer has linear activation
        self.model = nn.Sequential(*modules)

    def forward(self, x : Tensor) -> Tensor:
        return self.model(x)

# Represents a customized conditional probabilistic model with gaussians generation y~N(mu(x | c), e^logVar(x | c))
class GaussianModel (nn.Module):
    def __init__(self, 
            condition_dim, target_dim, output_dim, 
            width, depth, activation=nn.ReLU, forced_logVar = None
        ):
        '''
        Creates the conditional model using block sequence of layers.
        if forced_logVar is not None the model generates mu and a constant logVar equals to that value.
        if forced_logVar is None the model generates both mu and logVar.
        '''
        super(GaussianModel, self).__init__()
        self.output_dim = output_dim
        self.forced_logVar = forced_logVar
        self.model = BlockModel(
            condition_dim + target_dim, 
            output_dim * 2 if forced_logVar is None else output_dim, 
            width, depth, activation
        )

    def forward(self, c : Tensor, x : Tensor) -> List[Tensor]:
        '''
        Evaluates the model and return two tensors, one with mu and another with log variance
        '''
        o = self.model(torch.cat([c,x], dim=-1))
        if self.forced_logVar is None:
            mu, logVar = o.chunk(2, dim=-1)
            logVar = torch.clamp(logVar, -16, 16)
        else:
            mu, logVar = o, torch.full_like(o, self.forced_logVar)
        return [mu, logVar]

    @staticmethod
    def sample(mu : Tensor, logVar : Tensor) -> Tensor:
        return mu + torch.exp(logVar * 0.5) * torch.randn_like(mu)

# CVAE network. Represents a trainable CVAE with two Gaussian models as encoder and decoder.
class CVAEModel (nn.Module):
    def __init__(
            self, condition_dim, target_dim, latent_dim, 
            width, depth, activation=nn.ReLU, forced_logVar = None
        ):
        super(CVAEModel, self).__init__()
        self.latent_dim = latent_dim
        self.encoder = GaussianModel(
            condition_dim, target_dim, latent_dim, 
            width * 3, depth + 1, 
            # The encoder triples the width nodes and increase the depth to have more expresivity.
            # More complexity of the encoder doesnt affect the performance of the decoder.
            activation=nn.Softplus
        )
        self.decoder = GaussianModel(
            condition_dim, latent_dim, target_dim,
            width, depth,
            activation=activation, forced_logVar=forced_logVar
        )

    def forceLogVar (self, newLogVar):
        self.decoder.forced_logVar = newLogVar

    def forward(self, c : Tensor, x : Tensor) -> List[Tensor]:
        latentMu, latentLogVar = self.encoder(c, x)
        z = GaussianModel.sample(latentMu, latentLogVar)
        xMu, xLogVar = self.decoder(c, z)
        return [latentMu, latentLogVar, z, xMu, xLogVar]

    def sampleDecoder(self, c : Tensor, onlyMu = False) -> Tensor:
        z = torch.randn ((len(c), self.latent_dim)).to(c.device)
        xMu, xLogVar = self.decoder(c, z)
        if onlyMu :
            return xMu
        else:
            return GaussianModel.sample(xMu, xLogVar)

    @staticmethod
    def KL_divergence(mu, logVar):
        '''
        Compute KL Divergence between prior distribution and the standard normal distribution
        '''
        # return 0.5 * torch.sum(
        #     torch.exp(logVar) + mu ** 2 - 1 - logVar,
        #     dim = -1
        # ) 
        return torch.sum(
            torch.exp(logVar) + mu ** 2 - logVar,
            dim = -1
        ) 

    @staticmethod
    def posterior_LogLikelihood(x, xMu, xLogVar):
        '''
        Compute the posterior log-likelihood of x assuming N(xMu, xVar)
        1.837877066 == log (2 * pi)
        Norm(x) = exp(-0.5 (x - xMu)**2 / s**2 ) / (s**2 * 2 * pi)^0.5
        log-Norm = -0.5 ((x - xMu)**2 / s**2 + log(s**2) + log(2 * pi))
        '''
        # return -0.5 * torch.sum((x - xMu) ** 2 / torch.exp(xLogVar) + xLogVar + 1.837877066, dim = -1)
        return -torch.sum((x - xMu) ** 2 / torch.exp(xLogVar) + xLogVar, dim = -1)
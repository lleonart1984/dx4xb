import torch
import numpy as np
from torch import nn
from torch.nn import functional as F
from torch import Tensor

class Generator:
    '''
    Represents a generator of scatter events inside an homogeneous unitary sphere volume.
    '''
    def __init__(self, batch_size):
        self.batch_size = batch_size
        self.device = torch.device('cuda:0')
        self.PI = np.pi

    # DSL for torch

    def fixed(self, *cmp):
        return torch.Tensor([list(cmp)]).to(self.device).repeat(self.batch_size, 1)

    def vec(self, *cmp):
        return torch.cat(list(cmp), dim=-1)

    def random(self):
        return torch.rand(self.batch_size, 1).to(self.device)

    def gauss(self):
        return torch.randn(self.batch_size, 1).to(self.device)

    def bfixed(self, *cmp):
        return torch.BoolTensor([list(cmp)]).to(self.device).repeat(self.batch_size, 1)

    def _x(self, v):
        return v[:,0:1]

    def _y(self, v):
        return v[:,1:2]

    def _z(self, v):
        return v[:,2:3]

    def ifthen(self, c, x, y):
        z = torch.zeros_like(x)
        c = c.repeat(1, x.shape[1])
        z[c] = x[c]
        z[~c] = y[~c]
        return z # c*x + (~c)*y

    def sampleIsotropic(self, w):
        theta = 2 * self.PI * self.random()
        phi = torch.acos(torch.clamp(1 - 2 * self.random(), -1, 1))
        return self.vec(
            torch.sin(phi)*torch.cos(theta),
            torch.sin(phi)*torch.sin(theta),
            torch.cos(phi)
        )

    def cross(self, pto1, pto2):
        return self.vec(
            self._y(pto1) * self._z(pto2) - self._z(pto1) * self._y(pto2),
            self._z(pto1) * self._x(pto2) - self._x(pto1) * self._z(pto2),
            self._x(pto1) * self._y(pto2) - self._y(pto1) * self._x(pto2)
        )

    def dot(self, pto1, pto2):
        return torch.sum(pto1*pto2, dim=1, keepdim=True)

    def normalize(self, v):
        return v / torch.sqrt(self.dot(v,v))

    def orthonormal(self, n):
        other = self.ifthen(torch.abs(self._z(n)) < 0.999, self.fixed(0,0,1), self.fixed(1,0,0)) 
        B = self.normalize(self.cross(other, n))
        T = self.normalize(self.cross(n, B))
        return B, T

    def sampleAnisotropic(self, g, w):
        one_minus_g2 = 1 - g**2
        one_plus_g2 = 1 + g**2
        one_over_2g = 1 / (2*g)
        phi = 2*self.PI*self.random()
        t = (one_minus_g2) / (1.0 - g + 2.0 * g * self.random())
        cosTheta = one_over_2g * (one_plus_g2 - t * t)
        sinTheta = torch.sqrt(torch.clamp_min(1.0 - cosTheta**2, 0.0))
        t0, t1 = self.orthonormal(w)
        return sinTheta * torch.sin(phi) * t0 + sinTheta * torch.cos(phi) * t1 + cosTheta * w

    def distanceToSphere(self, x, w):
        '''
        x is assumed inside the sphere
        '''
        b = 2 * self.dot(x, w)
        c = self.dot(x, x) - 1
        Disc = b * b - 4 * c
        return 0.5*(-b + torch.sqrt(Disc))

    def generate_batch(self, sigma, g, varphi):

        fileName = f'cachedSamples{str(self.batch_size)}x{str(sigma)},{str(g)},{str(varphi)}.npz'

        try:
            data = np.load("Cache\\"+fileName)
            return \
                torch.from_numpy(data['N']).to(self.device), \
                torch.from_numpy(data['x']).to(self.device), \
                torch.from_numpy(data['w']).to(self.device), \
                torch.from_numpy(data['X']).to(self.device), \
                torch.from_numpy(data['W']).to(self.device) 
        except:
            pass
        phaseFunction = self.sampleIsotropic if abs(g) < 0.0001 else lambda w : self.sampleAnisotropic(g, w)

        x = self.fixed(0,0,0)
        w = self.fixed(0,0,1)
        X = x
        W = w
        A = 0
        G = 1
        N = self.fixed(0)
        running = self.bfixed(True)

        while running.any():
            G = G * varphi
            A = A + G
            takenInnerSample = self.random() < (G / A)
            X = self.ifthen(running & takenInnerSample, x, X)
            W = self.ifthen(running & takenInnerSample, w, W)
            w = self.ifthen(running, phaseFunction(w), w)
            N = self.ifthen(running, N + 1, N)
            t = -torch.log(1 - self.random())/sigma
            d = self.distanceToSphere(x, w)
            x = self.ifthen(running, x + w * torch.min(t, d), x)
            running = running & (t < d)
        
        np.savez("Cache\\"+fileName, 
            N = N.cpu().numpy(),
            x = x.cpu().numpy(),
            w = w.cpu().numpy(),
            X = X.cpu().numpy(),
            W = W.cpu().numpy())

        return N, x, w, X, W
import torch
from torch import nn
from torch.nn import functional as F
from torch import Tensor
from typing import List
import numpy as np

from cvae.dataman import DataManager
from cvae.modeling import CVAEModel

from tools import printProgressBar
from tools import get_config_code_and_summary

import pickle
import os, shutil

# Represents a state of a training process. Used by the training function to advance gradually the optimization search.
class TrainerState:
    def __init__(self, model, optimizer, scheduler):
        self.model = model
        self.optimizer = optimizer
        self.loss_history = []
        self.kl_history = []
        self.lrs = []
        self.scheduler = scheduler

    def getCurrentEpoch(self):
        return len(self.loss_history)

    def advance(self, data : DataManager, batch_size = 1024):
        '''
        Executes one epoch in the training
        '''
        self.model.train() # set in train mode

        loss_accum, KL_accum, batches = 0.0, 0.0, 0

        for c, x in data.get_batches(batch_size): # get data shuffled in batches
            self.optimizer.zero_grad()
            latent_mu, latent_logVar, z, x_mu, x_logVar = self.model(c, x)

            posterior_ll = torch.mean( CVAEModel.posterior_LogLikelihood(x, x_mu, x_logVar), dim = 0 ) # accumulate batch
            kl_div = torch.mean( CVAEModel.KL_divergence(latent_mu, latent_logVar), dim = 0 ) # accumulate batch
            elbo = posterior_ll - kl_div
            loss = -elbo
            
            loss_accum += loss.item()
            KL_accum += kl_div.item()
            batches += 1

            # perform optimization
            loss.backward()

            self.optimizer.step()

        self.lrs.append(self.optimizer.param_groups[0]['lr'])

        # self.scheduler.step(accLoss / numberOfBatches)
        self.scheduler.step()

        self.loss_history.append(loss_accum / batches)
        self.kl_history.append(KL_accum / batches)

        return loss_accum / batches # loss evaluation average

    def save(self, folder_name):
        '''
        Saves the current state of the training process at specific folder.
        Notice: use a different folder for each state.
        '''
        os.makedirs(folder_name, exist_ok=True)
        torch.save(self.model.state_dict(), folder_name + "\\model.pt")
        torch.save(self.optimizer.state_dict(), folder_name+"\\optimizer.pt")
        with open(folder_name+"\\loss.bin", 'wb') as fp:
            pickle.dump(self.loss_history, fp)
        with open(folder_name+"\\klDiv.bin", 'wb') as fp:
            pickle.dump(self.kl_history, fp)
        with open(folder_name+"\\lr.bin", 'wb') as fp:
            pickle.dump(self.lrs, fp)

    def load(self, folder_name):
        '''
        Loads the state of a training process from a specific folder.
        '''
        self.model.load_state_dict(torch.load(folder_name + "\\model.pt"))
        self.optimizer.load_state_dict(torch.load(folder_name+"\\optimizer.pt"))
        with open(folder_name+"\\loss.bin",'rb') as fp:
            self.loss_history = pickle.load(fp)
        with open(folder_name + "\\klDiv.bin", 'rb') as fp:
            self.kl_history = pickle.load(fp)
        with open(folder_name + "\\lr.bin", 'rb') as fp:
            self.lrs = pickle.load(fp)

class WavingScheduler:
    '''
    Defines a custom scheduler with a periodic wave and exponential reduction
    '''
    def __init__(self, optimizer, period, gamma):
        self.optimizer = optimizer
        self.period = period
        self.gamma = gamma
        self.initial_lr = self.optimizer.param_groups[0]['lr']
        self.epoch = 0
    
    def step(self):
        #self.optimizer.param_groups[0]['lr'] = self.initial_lr * (-np.cos(2*self.epoch * 3.14159 / self.period) + 2) * np.power(self.gamma, self.epoch) + 0.000001
        self.optimizer.param_groups[0]['lr'] = self.initial_lr * np.power(self.gamma, self.epoch) + 0.000001
        self.epoch += 1


def create_initial_state(model : CVAEModel, epochs = 400):
    '''
    Creates the initial training state for a specific model.
    The optimizer used is Adamax with starting lr of 0.001.
    The scheduler used is a custom waving scheduler that fall exponentially to a factor of 0.001 depending on the number of epochs.
    '''
    optimizer = torch.optim.Adamax(model.parameters(), lr = 0.01)
    gamma = np.exp(np.log(0.005)/epochs)
    scheduler = WavingScheduler(optimizer, 400, gamma=gamma)
    return TrainerState(model, optimizer, scheduler)

def train(model : CVAEModel, data : DataManager, folder_name, batch_size = 1024*16, epochs = 400, restart = True):
    '''
    Training of a specific model using data and specific batch size.
    The training process saves every 10 epochs in specified folder, within folder names state0, state1 ....
    If there is states already saved the training process will be resumed on the last saved state unless restart is True.
    '''

    state = create_initial_state(model, epochs)

    stateID = 0
    while os.path.exists(folder_name+"\\state"+str(stateID+1)):
        stateID += 1
        if restart:
            shutil.rmtree(folder_name+"\\state"+str(stateID))

    if restart:
        stateID = 0
    
    if stateID > 0:
        state.load(folder_name+"\\state"+str(stateID))
    else:
        state.save(folder_name+"\\state0")

    for e in range(epochs):
        loss = state.advance(data, batch_size=batch_size)
        if (state.getCurrentEpoch() % 10 == 0):
            printProgressBar((e+1)/epochs, prefix = 'Epoch '+str(state.getCurrentEpoch()), suffix = 'Loss: '+str(loss))
            stateID += 1
            state.save(folder_name+"\\state"+str(stateID))
    print()

def train_models (model_name, model_type, model_factory, configs, data : DataManager, output_folder, batch_size, epochs):
    '''
    Train different configurations for a model.
    model_factory is a method that will receive all named parametes from a specific setting.
    setting is a dictionary of each model parameter, the different values it can take.
    Model name refers to the specific model, e.g. lenGen, model type refers to the type of vae used, e.g. cvae.
    '''
    print("[INFO] Training configs: " + str(len(configs)))

    for config in configs:
        code, summary = get_config_code_and_summary(config)
        print("[INFO] Training: "+summary)
        model = model_factory(**config).to(torch.device('cuda:0'))
        train(
            model,
            data, 
            output_folder+"\\"+model_name+"\\"+model_type+"\\"+code, 
            batch_size = batch_size, 
            epochs=epochs, 
            restart=True
        )
        torch.cuda.empty_cache()

    print("[INFO] All models trained")

def get_last_state(model_name, model_type, model_factory, config, folder, top_state = None):
    '''
    Returns a the final state of a trained configuration.
    If top_state is not None, this value tops the state returned. i.e. get the state 3 even if there are 100
    '''
    code, summary = get_config_code_and_summary(config)
    print("[INFO] Loading: "+summary)
    folder_name = folder+"\\"+model_name+"\\"+model_type+"\\"+code
    model = model_factory(**config).to(torch.device('cuda:0'))
    state = create_initial_state(model)
    
    stateID = 0
    while os.path.exists(folder_name+"\\state"+str(stateID+1)):
        stateID += 1

    if top_state is not None:
        stateID = min(stateID, top_state)

    if stateID > 0:
        state.load(folder_name+"\\state"+str(stateID))

    return state

def get_last_states (model_name, model_type, model_factory, configs, output_folder, top_state = None):
    '''
    Returns a list with final states for different trained configurations.
    If top_state is not None, this value tops the state returned. i.e. get the state 3 even if there are 100
    '''
    states = []
    for config in configs:
        states.append(get_last_state(model_name, model_type, model_factory, config, output_folder, top_state))

    return states
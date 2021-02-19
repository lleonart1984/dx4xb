import torch
from torch import nn
from torch.nn import functional as F
from torch import Tensor
import numpy as np


class DataManager:
    '''
    Gets the data from a dataset file and keeps the columns representing the conditions and the targets for a specific model training.
    It can be specified a transformation mapping for a specific value in order to train.
    '''
    def __init__(self, path, conditions, targets, data_count = None):
        '''
        Constructor.
        path: path to the dataset file
        conditions: dictionary with the name of the tensor for the conditions and the mapping function. 
        targets: dictionary with the name of the tensor for the targets and the mapping function.
        Maps function must be None if no map is needed.
        '''
        data = np.load(path)
        def get_data_and_map (dataID, map): return data[dataID].astype(np.float32) if map is None else map(data[dataID].astype(np.float32))
        # Build the conditional part and the target part
        self.conditions = torch.tensor(np.array([get_data_and_map(d, map) for d, map in conditions.items()]).T, dtype=torch.float)
        self.target = torch.tensor(np.array([get_data_and_map(d, map) for d, map in targets.items()]).T, dtype=torch.float)
        self.condition_mapping = conditions # save mappings for filtering
        self.target_mapping = targets
        self.set_device(None)
        self.data_count = len(self.conditions) if data_count is None else data_count
        self.condition_dimension = len(conditions)
        self.target_dimension = len(targets)

    def data_in_range(self, **ranges):
        '''
        Gets the subset of the data refering to a specific range of conditioning parameters.
        ranges represents a dictionary with the component -> (min, max) association.
        '''
        print("creating mask ", self.data_count)
        mask = torch.full((self.data_count,), fill_value=True, dtype=bool).to(self.device)
        for c, (cmp, map) in enumerate(self.condition_mapping.items()):
            if cmp in ranges.keys():
                a = ranges[cmp][0] if map is None else map(ranges[cmp][0])
                b = ranges[cmp][1] if map is None else map(ranges[cmp][1])
                mask &= (self.conditions[:, c] >= a) & (self.conditions[:, c] <= b)
        return self.conditions[mask], self.target[mask]

    def get_batches(self, batch_size):
        ''' 
        Gets the data randomly placed in batches of batch_size length.
        All batches are granted to have the same size.
        If batch_size is greater than dataset size, then a single batch with all the data shuffled is return.
        ''' 
        batch_size = min (batch_size, self.data_count)
        total_size = batch_size * int(self.data_count / batch_size)
        indices = torch.randperm(total_size)
        return zip(torch.split(self.conditions[0:total_size, :][indices], batch_size), torch.split(self.target[0:total_size, :][indices], batch_size))

    def set_device(self, device):
        '''
        Moves the data to a specific device.
        If device is None, then the data is placed on CPU memory.
        ''' 
        self.device = device if device is not None else torch.device('cpu')
        self.target = self.target.to(device)
        self.conditions = self.conditions.to(device)
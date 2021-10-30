#%% [markdown]
'''
# Testing DataSet

This notebook allows to visualize the generated data set with histograms.
'''
#%%

from cvae.dataman import DataManager
import numpy as np
import matplotlib.pyplot as plt
%matplotlib inline

# %%

dataset = DataManager('.\\DataSets\\ScattersDataSet.npz',
    conditions={
        'sigma' : None,
        'g' : None
    },
    targets= {
        'n' : lambda n : np.log(n)
    }
)

print('[INFO] Loaded '+str(dataset.data_count))

# %%

conditions, targets = dataset.data_in_range(sigma=(1.0, 1.4), g=(-0.2, 0.2))
logbins = np.logspace(0,8,100, base=2.718)
plt.xscale('log')
_ = plt.hist(((np.exp(targets[:,0]).numpy())), bins = logbins, density=True)

# %%

conditions, targets = dataset.data_in_range(sigma=(15, 16), g=(-0.1, 0.1))
plt.xscale('log')
logbins = np.logspace(0, 6, 400)
_ = plt.hist(np.exp(targets[:,0]).numpy() + np.random.randn(len(targets))*0.5, bins=logbins, density = True)

# %%

# %%

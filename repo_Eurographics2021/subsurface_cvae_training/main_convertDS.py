
# %% [markdown]
'''
# Converting Data

This notebook converts the data from a csv format in the .ds files to the npz format in numpy.
'''
# %%
import numpy as np

def convertFile(folderName, name):
    '''
    Takes the file <name>.ds and convert into the file <name>.npz
    '''
    print('[INFO] Reading file: '+name+'.ds')
    file = open(folderName+'\\'+name+'.ds', 'r')
    contents = file.readlines()
    file.close()

    num_lines = len(contents)
    num_vars = len(contents[0].split(','))
    print('[INFO] Shape of data: {}x{}'.format(num_lines, num_vars))

    data = np.zeros((num_lines, num_vars))
    p_old = 0

    for i, line in enumerate(contents):
        # read data
        new_data = [float(x) for x in line.split(',')]
        data[i] = new_data
        # show progress
        p = int(np.round(100 * i / num_lines))
        if (p % 5 == 0) and (p != p_old):
            print('[INFO] {}% completed.'.format(p))
            p_old = p

    print('[INFO] Selecting valid data.')
    valid = ~np.any(np.isnan(data), axis=1)
    data = data[valid]

    print('[INFO] Storing data in *.npz format.')
    np.savez(
        folderName+'\\'+name+'.npz',
        sigma = data[:,0],
        g = data[:, 1],
        albedo = data[:,2],
        n = data[:, 3],
        z = data[:, 4],
        tangent_beta = data[:, 5],
        tangent_alpha = data[:, 6],
        representative_Xx = data[:, 7],
        representative_Xy = data[:, 8],
        representative_Xz = data[:, 9],
        representative_Wx = data[:, 10],
        representative_Wy = data[:, 11],
        representative_Wz = data[:, 12]
    )
# %% [markdown]
'''
Choosing the files to convert...
'''
# %%
fileNames = ['ScattersDataSet']

for f in fileNames:
    convertFile('.\\DataSets', f)
# %%

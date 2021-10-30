# Learning Multiple-Scattering Solutions for Sphere-Tracing of Volumetric Subsurface Effects (Eurographics 2021)

L. Leonard, K. Höhlein, R. Westermann



This step-by-step guide is intended for those who wants to reproduce the technique presented in the paper. 
We demonstrate that speed-ups in volumetric light-transport simulation can be achieved through data-driven learning of scattering process statistics inside spherical geometries with the help of conditional variational autoencoders (CVAE), which can be translated into shader functions and integrated efficiently into volumetric path-tracing pipelines. 
We provide code for generating the required training data and realizing the CVAE-based path-tracer. Additionally, we provide code for a comparison method based on tabulated scattering statistics as described in Mueller et al. (2016).

To generate scene renderings with the CVAE-based rendering solution or the tabulation-based method, the following steps have to be executed:

- Generation of the data
- Training of the model
- Compilation of the model to HLSL code
- Scene-rendering using DX12 with the CVAE shader

## Generating the data

The models are trained with ground-truth light-path statistics of photons being scattered inside spherical regions of homogeneous, translucent materials. The code for the generation procedures is stored in the folder `subsurface_cvae_generating`. 
In the `SphereScattering` `Program.cs` code you may uncomment the desired method to generate the required data. 
The generation process might take 12-24 hours, depending on the machine.  

```c#
static void Main(string[] args)
{
    //// Used to generate 4 million samples for CVAE training
    //GenerateDatasetForTrainingCVAE();

    //// Used to Generate Mueller data tables
    //GenerateDatasetForTabulationMethod();

    // Used to generate samples for extended table
    // Generates 8 files of 16 million samples each...
    GenerateDatasetForTabulationXMethod();
}
```

The data for the CVAE training will be stored in the file `ScattersDataSet.ds`, containing comma-separated values named. 
The other two tables are readily available and stored in `.bin` files. For proceeding, they must be copied to the output folder of the rendering project.

## Training the CVAE

Model training is conducted in Python, using Numpy (numpy 1.18.5) and PyTorch (torch 1.6.0) modules. Code for model training is stored in the folder `subsurface_cvae_training`. 

The first step is to convert the `.ds` file generated with the scattering process as a `.npz` file. 
Copy the generated `ScattersDataSet.ds` file to a created `DataSets` folder in the project folder. Then run the `main_convertDS.py` python module, which will generate a data file `ScattersDataSet.npz` to enable faster loading of the data in further Python scripts.

To train the three CVAE models (len, path and scat) it is necessary to run the following modules: `main_train_lenGen.py`, `main_train_pathGen.py` and `main_train_scatGen.py` respectively.

We can specify in the script different settings to train.

```python
lenGenConfigs = [
    {
        'depth': 3,
        'width': 8,
        'activation': nn.Softplus,
        'latent': 2
    }
]
```

An id is generated for each training settings and saved in a Running folder.

It is possible to examine the learned distributions using the scripts `main_test_lenGen.py`.

## Compiling the models to HLSL

In order to compile the desired models into HLSL the script `main_compiling.py` can be used. 
Note here that in the code a single configuration will be finally compiled for each model type. 
The compiled model is stored in a single file `compiledModels.h` where each model is translated to an HLSL function that receives the values for the decoder in an array, and returns in an out array the values of mu and sigma for the variables (first all mu and then all sigma).

```c++
void lenModel(float _input[4], out float _output[2]) { ... }
```

This code is HLSL and thus can be included in any shader.

## Rendering subsurface effects with DX12

The rendering project is stored in `subsurface_cvae_rendering` folder. 
The RTX-implementation of the CVAE technique and other methods can be found in the project `dx4xb.Techniques`. This project requires a PC with RTX technology support.

The models are loaded from the user desktop folder in a Models folder. All models used to tests are provided in this repository.

The settings used in the paper can be executed via `Eurographics.CVAE_ST` project. In that project, a series of include statements can be commented and uncommented to define the desired settings.

```c++
#include "Teaser.h"
//#include "Comparisons.h"
//#include "ComparisonsLucy.h"
//#include "ComplexityBunny.h"
//#include "ComplexityLucy.h"
//#include "Absorptions.h"
//#include "NEETests.h"
```

Each of these files encodes a particular scene setup and a rendering technique (or set of techniques) being used. Also, there are `define` values, which can be used to toggle among different settings.


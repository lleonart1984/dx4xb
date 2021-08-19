#ifndef PARAMETERS_H
#define PARAMETERS_H

// Use narrow light source to show real bias and variance in algorithms
//#define USE_NARROW_LIGHTSOURCE

//#define USE_CVAE_X

#define NO_DRAW_LIGHT_SOURCE

// Use skybox to show fancy scenes
#define USE_SKYBOX 


// Max number of outside bounces allowed in a Pathtracer
//#define MAX_PATHTRACING_BOUNCES 5
#define MAX_PATHTRACING_BOUNCES 4

//#define SHOW_COMPLEXITY

#define SV_SIZE 16

#define VOXEL_LOD 0

#define VOXEL_SIZE (1 << VOXEL_LOD)

#endif
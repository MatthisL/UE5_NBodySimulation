`By Matthis Le Texier, Nov. 2023` 

---
<br />

## `N-Body Simulation`

N-Body simulation refers to a computational technique used in physics and astrophysics to model the gravitational interaction between a large number of particles, often celestial bodies like stars and galaxies. The "N" in N-Body represents the number of particles involved.


## `Context`

I created this project as part of a technical test during a job application process for a game studio.
I found the subject very interesting and so, looked for documentation about it. There are multiple ways to optimize the simulation. I chose the "programming" way, for now at least, instead of the algorithmic way.

The reason is quite simple, I wanted, since a long time, to get a step in programming shaders with HLSL and render them on Unreal. I chose the hard way with only C++ and `.usf` files.

For this project, the particles (or bodies) are instanced on the Game Thread with `UInstancedStaticMeshComponent`. The simulation computations, however, are made on the GPU with a compute shader for the obvious advantage of parallelism. 

But damn, Unreal is such a pain and boiler plate code to setup and dispatch a simple compute shader compare to Unity ! That was a real challenge.

### `Constraints`

The main goal was to simulate the most possible bodies with a decent framerate on Unreal Engine 5.1.  
- 2D Simulation
- No user input intended and only a camera fixing the simulation going on
- When bodies go beyond the screen bounds, they should wrap and continue their movements on the opposite of the screen
- Project can be shipped without errors and executed on Windows


## `Project Overview`


### Unreal Project Settings
| <!-- --> | <!-- --> |
| -------- | -------- | 
| Unreal Version  | 5.1 |
| Target RHI  | DX11  | 

*TODO: I have to figure out why it's not working with DX12. May need to use the RDG (Rendering Dependency Graph) interface.*

### Folders overview

- Plugins/NBodySimShader 

`Compute shaders need to live inside a module loading in PostConfigInit in order to be compiled and be visible inside the engine. The simplest way is to put it inside a plugin with a custom C++ interface to dispatch it from the GameThread`

- Source/NBodySimulation/Engine

`The main class to run the simulation is an AActor, ASimulationEngine, that use the plugin interface to setup and run the compute shader. It also read the simulation config from the data asset, explained below.`

- Source/NBodySimulation/Config

`Here we have the SimulationConfig.h/.cpp which defines a UDataAsset where simulation parameters and settings are stored such as the number of bodies to spawn.`


### How to run the simulation

1. Open the project with Unreal Engine 5.1 (could work with 5.x versions, but not guaranted).
2. Open the `Maps/L_Simulation` level.
3. Start and enjoy !\
--
4. You can edit the simulation settings (or create a new `SimulationConfig` data asset and set it in the `BP_SimulationEngine`) in the `DA_SimulationConfig` asset.



## `Further Possible Optimizations (Algorithmic)`

### Barnes Hut Algorithm

Read [this](https://patterns.eecs.berkeley.edu/?page_id=193#Barnes_Hut)

### Fast Multipole Method (FMM)

> The FMM algorithm was designed to compute pair-wise interactions between N particles, which belong to the class of n-body problems. It reduces the complexity from a quadratic (N elements interact with N elements) to a quasi-linear complexity. The central idea of the FMM is to avoid computing the interactions between all the elements by approximating the interactions between elements that are far enough. ([source](https://www.researchgate.net/publication/346980118_TBFMM_A_C_generic_and_parallel_fast_multipole_method_library))

In this algorithm, we are using the [QuadTree](https://en.wikipedia.org/wiki/Quadtree) data structure, allowing to split the 2D screen of the simulation into multiple cells.

| ![Quadtree Visual](Doc/QuadTree.png) | 
|:--:| 
| *The [QuadTree](https://en.wikipedia.org/wiki/Quadtree) data structure* |

<br />

>The FMM algorithm is based on six operators with names that respect the format X2Y, where X represents the source of the operator and Y the destination. The operators are P2M,M2M,M2L,L2L,L2P and P2P, where P means particle, M multipole and L local. The term particle is used for a legacy reason, but it represents the basic interaction elements that interact and for which we want to approximate the interactions. The multipole part represents the aggregation of potential, i.e. it represents what is emitted by a sub-part of the simulation box, whereas the local part represents the outside that is emitted onto a sub-part of the simulation box. The different operators are schematized in Figure 2. ([source](https://www.researchgate.net/publication/346980118_TBFMM_A_C_generic_and_parallel_fast_multipole_method_library))

| ![FMM Visual](Doc/FMM_Visual_2.png) |
| ![FMM Visual 2](Doc/FMM_Visual.png) |
|:--:|
| *Figure 2 : Illustration of the FMM algorithm. (a,b,c) The building of the octree. (d,e,f,g) The FMMalgorithm and its operators.* |


## `Resources`

- https://patterns.eecs.berkeley.edu/?page_id=193#Naive_algorithm  
- https://en.wikipedia.org/wiki/N-body_simulation  
- http://www.scholarpedia.org/article/N-body_simulations_(gravitational)  
- https://docplayer.net/236402722-Parallel-fmm-algorithm-based-on-space-decomposition.html
- https://www.researchgate.net/publication/346980118_TBFMM_A_C_generic_and_parallel_fast_multipole_method_library
- https://andyljones.com/posts/multipole-methods.html


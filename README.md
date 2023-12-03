`By Matthis Le Texier, Nov. 2023` 

---
<br />

## `N-Body Simulation`

N-Body simulation refers to a computational technique used in physics and astrophysics to model the gravitational interaction between a large number of particles, often celestial bodies like stars and galaxies. The "N" in N-Body represents the number of particles involved.


## `Context`

I created this project as part of a technical test during a job application process for a game studio.


## `Constraints`

The main goal was to simulate the most possible bodies with a decent framerate on Unreal Engine 5.1.  
- 2D Simulation
- No user input intended and only a camera fixing the simulation going on
- When bodies go beyond the screen bounds, they should wrap and continue their movements on the opposite of the screen
- Project can be shipped without errors and executed on Windows



## `Optimization Method`


### Fast Multipole Method (FMM)

> The FMM algorithm was designed to compute pair-wise interactions between N particles, which belong to the class of n-body problems. It reduces the complexity from a quadratic (N elements interact with N elements) to a quasi-linear complexity. The central idea of the FMM is to avoid computing the interactions between all the elements by approximating the interactions between elements that are far enough. ([source](https://www.researchgate.net/publication/346980118_TBFMM_A_C_generic_and_parallel_fast_multipole_method_library))

In this algorithm, we are using the [QuadTree](https://en.wikipedia.org/wiki/Quadtree) data structure, allowing to split the 2D screen of the simulation into multiple cells.

| ![Quadtree Visual](https://www.researchgate.net/profile/Rio-Yokota/publication/51944732/figure/fig1/AS:277577370488833@1443191010060/Illustration-of-the-complete-FMM-algorithm-with-the-upward-sweep-and-the-downward-sweep.png) | 
|:--:| 
| *The [QuadTree](https://en.wikipedia.org/wiki/Quadtree) data structure* |

<br />

>The FMM algorithm is based on six operators with names that respect the format X2Y, where X represents the source of the operator and Y the destination. The operators are P2M,M2M,M2L,L2L,L2P and P2P, where P means particle, M multipole and L local. The term particle is used for a legacy reason, but it represents the basic interaction elements that interact and for which we want to approximate the interactions. The multipole part represents the aggregation of potential, i.e. it represents what is emitted by a sub-part of the simulation box, whereas the local part represents the outside that is emitted onto a sub-part of the simulation box. The different operators are schematized in Figure 2. ([source](https://www.researchgate.net/publication/346980118_TBFMM_A_C_generic_and_parallel_fast_multipole_method_library))

| ![FMM Visual](https://www.researchgate.net/profile/Berenger-Bramas/publication/302564087/figure/fig10/AS:648972192468995@1531738436560/Fast-multipole-method-algorithm.png) |
| ![FMM Visual 2](FMM_Visual.png) |
|:--:|
| *Figure 2 : Illustration of the FMM algorithm. (a,b,c) The building of the octree. (d,e,f,g) The FMMalgorithm and its operators.* |


## `Resources`

- https://patterns.eecs.berkeley.edu/?page_id=193#Naive_algorithm  
- https://en.wikipedia.org/wiki/N-body_simulation  
- http://www.scholarpedia.org/article/N-body_simulations_(gravitational)  
- https://docplayer.net/236402722-Parallel-fmm-algorithm-based-on-space-decomposition.html
- https://www.researchgate.net/publication/346980118_TBFMM_A_C_generic_and_parallel_fast_multipole_method_library
- https://andyljones.com/posts/multipole-methods.html


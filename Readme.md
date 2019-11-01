# M3DA

Advanced 3D Modelisation - Lille's university coursework  
This repository includes OpenGL exercises from my university coursework Advanced 3D Modelisation.
Lille University, France - 2019

--------------------------------------------
## _TP01 - Cylinder_

_[Warning] In order to make it work on Windows, please comment "LIBS+=-lglew32s" in ther user.pro file and replace uint by unsigned int in "VertexArray.h"._

In this exercise, I have done generalized cylinders which allow us to build a surface from a section "swiping"/following a path. This is known as 3D Sweep Objects.  
First, I took care of orientations and tangent path lines.  
Then, I added spline-shaped paths, so we have a more natural curve.  
Finally, I correctly computed normals in order to have persuasive lighting.

--------------------------------------------
## _TP02 - Nurbs_

_[Warning] In order to make it work on Windows, please comment "LIBS+=-lglew32s" in ther user.pro file and replace uint by unsigned int in "VertexArray.h"._

In this exercise, I have seen B-splines, which are functions represented by several polynomials connected to each other so we have a continuous curve. This allows us to have local control. As you can see on the GIF below, if we move one control point, it will not impact the entire curve.

Another feature of B-splines is basis functions. Their numbers depends on the number of control points and on the curve's degree. They allow us to control the weight of the curve. As you can see on the GIF below, if basis functions are either 0 or 1, we have a Bezier curve !

--------------------------------------------
## _TP03 - Subdivision_

_[Warning] In order to make it work on Windows, please replace uint by unsigned int in "VertexArray.h"._

In this exercise, I have seen subdivision techniques such as Chaikin approximation (not verifying control points), Dyn-Levin interpolation (verifying control points) :  


But also Catmull-Clark subdivision, which create new faces by computing new points (middle of a face, middle of an edge, weighted old points) :  

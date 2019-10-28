# M3DA

Advanced 3D Modelisation - Lille's university coursework  
This repository includes OpenGL exercises from my university coursework Advanced 3D Modelisation.
Lille University, France - 2019

--------------------------------------------
## _TP01 - Cylinder_

_[Warning] In order to make it work on Windows, please comment "LIBS+=-lglew32s" in ther user.pro file and replace uint by unsigned int in "VertexArray.h"._

In this exercise, I have done generalized cylinders which allow us to build a surface from a section "swiping"/following a path. Here, I had to take care of orientations and tangent path lines.
Then I added spline-shaped paths, so we have a more natural curve.
Finally, I had to correctly compute normals in order to have persuasive lighting.

--------------------------------------------
## _TP02 - Nurbs_

_[Warning] In order to make it work on Windows, please comment "LIBS+=-lglew32s" in ther user.pro file and replace uint by unsigned int in "VertexArray.h"._

In this exercise, I have done evenly distributed B-splines as well as specifically spread curves like Bézier before moving in to Nurbs and surface Nurbs.

--------------------------------------------
## _TP03 - Subdivision_

_[Warning] In order to make it work on Windows, please replace uint by unsigned int in "VertexArray.h"._

In this exercise, I have seen Chaikin approximation, Dyn-Levin interpolation, and Catmull-Clark subdivision.

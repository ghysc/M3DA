# Context

This repository includes OpenGL exercises from my university coursework Advanced 3D Modelisation.
Lille University, France - 2019

***TP01 - Cylinder***

[Warning] In order to make it work on Windows, please comment "LIBS+=-lglew32s" in ther user.pro file and replace uint by unsigned int in "VertexArray.h".

In this exercise, I have done generalized cylinders which allow us to build a surface from a section "swiping"/following a path. Here, I had to take care of orientations and tangent path lines.
Then I added spline-shaped paths, so we have a more natural curve.
Finally, I had to correctly compute normals in order to have persuasive lighting.

***TP02 - Nurbs***

[Warning] In order to make it work on Windows, please comment "LIBS+=-lglew32s" in ther user.pro file and replace uint by unsigned int in "VertexArray.h".

In this exercise, I have done evenly distributed B-splines as well as specifically spread curves like Bézier before moving in to Nurbs and surface Nurbs.
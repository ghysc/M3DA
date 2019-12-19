#version 150

// @author F. Aubert (Univ. Lille)

const int iMax=150; // max loop
const int nbNodeMax=20; // max node

// uniforms
uniform int nbNode;
uniform int nbLeaf;
uniform int tree[nbNodeMax*2];
uniform mat4 leafEyeMatrix[nbNodeMax];
uniform vec4 colorLeaf[nbNodeMax];

// in out
out vec4 fragColor;
in vec2 fVertex;

// globals
float distanceNode[nbNodeMax];
bool isSet[nbNodeMax];
vec4 colorNode[nbNodeMax];

// sphere distance
float dSphere(vec3 p) {
    vec4 s = vec4(0., 0., -2., 1.);
    return length(p-s.xyz)-s.w;
}

// box distance
float dBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

// cone distance
float dCone(vec3 p, vec2 c)
{
  // c is the sin/cos of the angle
  float q = length(p.xy);
  return dot(c,vec2(q,p.z));
}

// cylinder distance
float dCappedCylinder( vec3 p, float h, float r )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - vec2(h,r);
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

// octahedron distance
float dOctahedron( vec3 p, float s)
{
  p = abs(p);
  float m = p.x+p.y+p.z-s;
  vec3 q;
       if( 3.0*p.x < m ) q = p.xyz;
  else if( 3.0*p.y < m ) q = p.yzx;
  else if( 3.0*p.z < m ) q = p.zxy;
  else return m*0.57735027;

  float k = clamp(0.5*(q.z-q.y+s),0.0,s);
  return length(vec3(q.x,q.y-s+k,q.z-k));
}

// distance from p to an union of primitives
float dList(vec3 p) {
    float minD = 999.;
    for(int i=0; i<leafEyeMatrix.length(); i++) {
        vec4 newP = leafEyeMatrix[i]*vec4(p, 1.);
        minD = min(dSphere(newP.xyz), minD);
    }
  return minD;
}


// set distanceNode[indexNode] if indexNode is a primitive
void setDistanceLeaf(vec3 p,int indexNode,int primitive) {
    float d=0.;
    //Pour l'instant, comme il n'y a que des sphères, on utilise que dSphere()
    if (primitive == 0) {
        vec4 newP=leafEyeMatrix[indexNode]*vec4(p, 1.);
        d=dSphere(newP.xyz);
    } else if (primitive == 1) {
        vec4 newP=leafEyeMatrix[indexNode]*vec4(p, 1.);
        d=dBox(newP.xyz, vec3(1, 0.4, 0.5));
    } else if (primitive == 2) {
        vec4 newP=leafEyeMatrix[indexNode]*vec4(p, 1.);
        float angle = 340;
        d=dCone(newP.xyz, vec2(cos(angle), sin(angle)));
    } else if (primitive == 3) {
        vec4 newP=leafEyeMatrix[indexNode]*vec4(p, 1.);
        d=dCappedCylinder(newP.xyz, 1, 3);
    } else if (primitive == 4) {
        vec4 newP=leafEyeMatrix[indexNode]*vec4(p, 1.);
        d=dOctahedron(newP.xyz, 5.);
    }
    distanceNode[indexNode]=d; // TODO : set distanceNode[indexNode]
}


void updateParent(int indexNode) {
  int parent = tree[indexNode*2+1];

  float d=0.;
  if (isSet[parent]) {
      int operation=tree[parent*2];
      if(operation == 0) {
          // Réunion
          d=min(distanceNode[parent], distanceNode[indexNode]);
          colorNode[parent] = distanceNode[parent] < distanceNode[indexNode] ? colorNode[parent] : colorNode[indexNode];
      } else if(operation == 1) {
          // Intersection
          d=max(distanceNode[parent], distanceNode[indexNode]);
          colorNode[parent] = distanceNode[parent] < distanceNode[indexNode] ? colorNode[parent] : colorNode[indexNode];
      } else if(operation == 2) {
          d=max(distanceNode[parent], -distanceNode[indexNode]);
          colorNode[parent] = distanceNode[parent] < distanceNode[indexNode] ? colorNode[parent] : colorNode[indexNode];
      }
  } else {
      //Si un seul enfant est set
      d=distanceNode[indexNode];
      colorNode[parent] = colorNode[indexNode];
      isSet[parent]=true;
  }

  distanceNode[parent]=d; // TODO : set distanceNode[parent]
}

float distanceTree(vec3 p) {
  bool done=false;

  int operation;
  int parent;

  int i;

  for(i=0;i<nbNode;++i) {
    isSet[i]=false; // a child already updated the node i ?
  }

  for(i=0;i<nbNode-1;++i) {
    operation=tree[i*2];
    parent=tree[i*2+1];
    if (operation>=4) {
      setDistanceLeaf(p,i,operation-4);
    }
    updateParent(i);
  }

  return distanceNode[nbNode-1];
}

void main() {
  vec3 p=vec3(0,0,2); // ray origin
  vec3 u=normalize(vec3(fVertex,1.)-p); // ray direction

  fragColor = vec4(0.2,0.2,0.2,1); // default color of the pixel

  // init color of the nodes for leaves
  for(int i=0;i<nbLeaf;++i) {
    colorNode[i]=colorLeaf[i];
  }

  // init of the main loop for sphere tracing
  bool done=false;
  bool far=false;
  float d;
  int iter=iMax;


  // main loop of the sphere tracing
  while(!done) {
    d=distanceTree(p);
    p=p+u*d;

    --iter;
    if (d<0.00001) done=true;
    else if (d>10.0) {done=true;far=true;}
    else if (iter==0) {far=true;done=true;}
  }

  float epsilon = 0.00001;

  // set the fragment color
  if (!far) {
      // Light
      vec3 l = vec3(0.,0.,0.8);
      // Derivates for normal
      float dpx = (distanceTree(vec3(p.x+epsilon,p.y,p.z)) - distanceTree(vec3(p.x-epsilon,p.y,p.z))) / 2*epsilon;
      float dpy = (distanceTree(vec3(p.x,p.y+epsilon,p.z)) - distanceTree(vec3(p.x,p.y-epsilon,p.z))) / 2*epsilon;
      float dpz = (distanceTree(vec3(p.x,p.y,p.z+epsilon)) - distanceTree(vec3(p.x,p.y,p.z-epsilon))) / 2*epsilon;
      // Normal
      vec3 n = normalize(vec3(dpx, dpy, dpz));
      // Diffuse color
      vec3 col = colorNode[nbNode-1].xyz;//vec3(1.,0.,0.);
      fragColor=vec4(col * dot(l,n) ,1.0);
  }
}

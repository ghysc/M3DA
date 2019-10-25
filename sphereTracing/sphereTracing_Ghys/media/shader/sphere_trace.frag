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
    return 0.0;
}


// distance from p to an union of primitives
float dList(vec3 p) {
  return 0.0;
}


// set distanceNode[indexNode] if indexNode is a primitive
void setDistanceLeaf(vec3 p,int indexNode,int primitive) {
  // TODO

  distanceNode[indexNode]=0; // TODO : set distanceNode[indexNode]
}


void updateParent(int indexNode) {
  int parent = tree[indexNode*2+1];

  // TODO

  distanceNode[parent]=0; // TODO : set distanceNode[parent]
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
  vec3 p=vec3(0,0,0); // ray origin
  vec3 u=vec3(0,0,0); // ray direction

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
    d=dSphere(p);


    --iter;
    if (d<0.00001) done=true;
    else if (d>10.0) {done=true;far=true;}
    else if (iter==0) {far=true;done=true;}
  }

  // set the fragment color
  if (!far) {
    fragColor=vec4(1.0,0,0,1.0);
    //fragColor=vec4(1,0,0,1);
  }
}

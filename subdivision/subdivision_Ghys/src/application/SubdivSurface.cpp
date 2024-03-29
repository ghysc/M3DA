#include "SubdivSurface.h"
#include "GLTool.h"

using namespace p3d;
using namespace std;

SubdivSurface::~SubdivSurface() {

}


SubdivSurface::SubdivSurface() {

}


void SubdivSurface::input(p3d::Mesh *m) {
  _input=m;
}

void SubdivSurface::source(p3d::Mesh *m) {
  _source=m;
}


void SubdivSurface::source(const std::string &filename) {
  delete _source;
  _source=new Mesh();
  _source->readInit(filename,false);
  _source->requestInitDraw();
}


/** returns the edge index of [v1,v2] (-1 if not found)
 *
 * @pre : vector _edgeForVertex must be intialized before
 */
int SubdivSurface::findEdge(int v1,int v2) {
  ///
  vector<int> &edgeV1=_edgeOfVertex[v1];
  for(unsigned int i=0;i<edgeV1.size();++i) {
    int b=_edge[edgeV1[i]]._b;
    if (b==v2) return edgeV1[i];
  }
  vector<int> &edgeV2=_edgeOfVertex[v2];
  for(unsigned int i=0;i<edgeV2.size();++i) {
    int b=_edge[edgeV2[i]]._b;
    if (b==v1) return edgeV2[i];
  }
  return -1;
}



void SubdivSurface::prepare() {
  if (!_input) return;
  /// compute edge :
  ///
  int v1,v2;
  _edgeOfVertex.clear();
  _edge.clear();
  _edgeOfVertex.resize(_input->nbPosition());
  for(unsigned int i=0;i<_input->nbFace();++i) { // i is the face (index)
    v1=_input->indexPositionVertexFace(i,-1); // the vertex before the first one in face i (i.e. the last one)
    for(unsigned int j=0;j<_input->nbVertexFace(i);++j) { // j is the j-th vertex in the face i
      v2=_input->indexPositionVertexFace(i,j);
      int foundEdge=findEdge(v1,v2);
      if (foundEdge!=-1) { // the edge already exists
        _edge[foundEdge]._right=i; // if already exists, the _left face is already set
      }
      else { // create the edge [v1,v2]
        Edge e;
        e._a=v1;
        e._b=v2;
        e._left=i; // the current face is put on _left (preserve the orientation of the mesh)
        e._right=-1; // will be set if the same edge is encountered after
        _edge.push_back(e);
        _edgeOfVertex[v1].push_back(_edge.size()-1);
        _edgeOfVertex[v2].push_back(_edge.size()-1);
      }
      v1=v2; // next
    }
  }
}



void SubdivSurface::computePointFace() {
  /* TODO : compute all point face (set the vector _pointFace : _pointFace[i] for the i-th face of _input).
   * - input = Mesh *_input
   * - _input->nbFace()
   * - _input->nbVertex(i) : number of vertices of i-th face
   * - _input->positionMesh(i,j) : the position of the j-th vertex of the i-th face
   */
  _pointFace.clear();

  for(int i=0; i<_input->nbFace(); i++) {
      Vector3 centroid;
      for(int j=0; j<_input->nbVertexFace(i); j++) {
        centroid += _input->positionMesh(_input->indexPositionVertexFace(i,j));
      }
      centroid /= _input->nbVertexFace(i);
      _pointFace.push_back(centroid);
  }

}

void SubdivSurface::computePointEdge() {
  /* TODO : compute all point face (set the vector _pointEdge : _pointEdge[i] for the i-th edge).
   * input = Mesh *_input
   * _edge[i]._a, _edge[i]._b : give the index of the two positions of i-th edge (._a and ._b are indexes for _input)
   * _input->position(i) : the i-th position of input (as referred by ._a and ._b).
   * _edge[i]._left, _edge[i]._right : indexes of the two faces incident to i-th face
   * - _pointFace[i] : should give the point Face of the i-th face.
   */
  _pointEdge.clear();

  for(int i=0; i<_edge.size(); i++) {
    Vector3 centroid;
    centroid += _input->positionMesh(_edge[i]._a);
    centroid += _input->positionMesh(_edge[i]._b);
    centroid += _pointFace[_edge[i]._left];
    centroid += _pointFace[_edge[i]._right];
    centroid /= 4;
    _pointEdge.push_back(centroid);
  }

}


void SubdivSurface::computePointVertex() {
  /* TODO : compute all point vertex (set the vector _pointVertex : _pointVertex[i] for the i-th position).
   * input = Mesh *_input
   * - _edgeOfVertex[i][j] : gives the index (for the vector _edge) of the j-th edge of the i-th vertex
   */
  _pointVertex.clear();

  for(int i=0; i<_edgeOfVertex.size(); i++) {
    double n = _edgeOfVertex[i].size();
    Vector3 newV;
    Vector3 sumEdges;
    Vector3 sumFaces;

    for(int j=0; j<_edgeOfVertex[i].size(); j++) {
        // Sum of edges
        int ie = _edgeOfVertex[i][j];
        sumEdges += _pointEdge[ie];
        // Sum of faces
        int iface;
        if(_edge[ie]._a == i) {
            iface = _edge[ie]._left;
        } else {
            iface = _edge[ie]._right;
        }
        sumFaces += _pointFace[iface];
    }

    newV = ((n-2) / n * _input->positionMesh(i)) + sumEdges / pow(n, 2) + sumFaces / pow(n, 2);
    _pointVertex.push_back(newV);
  }

}

int SubdivSurface::findNextEdge(int i,int j) {

  return -1; // happens for a boundary edge
}

void SubdivSurface::buildMesh() {
  Mesh *m=new Mesh();
  /* TODO : build the new mesh
   * - m->addPositionMesh(aVector3) to add a vertex
   * - m->addFaceMesh({v1,v2,v3,...}) to add a face : caution : v1,v2,v3,... are indexes (int) of the positions of m
   * - caution with the indexes (indexes of m are not the same that the ones for _input : track them).
   *
   */

  for(int i=0; i<_pointFace.size(); i++) {
      m->addPositionMesh(_pointFace[i]);
  }
  for(int i=0; i<_pointEdge.size(); i++) {
      m->addPositionMesh(_pointEdge[i]);
  }
  for(int i=0; i<_pointVertex.size(); i++) {
      m->addPositionMesh(_pointVertex[i]);
  }

  for(int i=0; i<_pointVertex.size(); i++) {
    int ip = _pointFace.size() + _pointEdge.size() + i;

    for(int j=0; j<_edgeOfVertex[i].size(); j++) {
        // _edgeOfVertex[i][j] : gives the index (for the vector _edge) of the j-th edge of the i-th vertex
        int ie1 = _edgeOfVertex[i][j];
        int iface;
        if(_edge[ie1]._a == i) {
            iface = _edge[ie1]._left;
        } else {
            iface = _edge[ie1]._right;
        }
        int ie2 = 0;
        for(int k=0; k<_edgeOfVertex[i].size(); k++) {
            // We don't want the same edge
            if(_edgeOfVertex[i][j] == _edgeOfVertex[i][k]) continue;
            // We are looking for another edge with the same face index
            if(_edge[_edgeOfVertex[i][k]]._left == iface || _edge[_edgeOfVertex[i][k]]._right == iface) {
                ie2 = _edgeOfVertex[i][k];

                ie1 += _pointFace.size();
                ie2 += _pointFace.size();
                m->addFaceMesh({ip, ie1, iface, ie2});
            }
        }
    }
  }

  /* end TODO */


  _result=m;
  _result->computeNormal();
  _result->computeTexCoord();
}


void SubdivSurface::catmullClarkIter() {
  prepare();
  computePointFace();
  computePointEdge();
  computePointVertex();
  buildMesh();
}

void SubdivSurface::catmullClark() {
  delete _result;
  _pointVertex.clear();
  _pointEdge.clear();
  _pointFace.clear();
  _result=_source->clone();
  for(int i=0;i<_nbIteration;++i) {
    _input=_result;
    catmullClarkIter();
    delete _input;
  }
}




void SubdivSurface::drawTest() {
  glPointSize(10);


  p3d::ambientColor=Vector4(1,0,0,1);
  p3d::shaderVertexAmbient();
  if (_pointVertex.size()>0) p3d::drawPoints(_pointVertex);


  p3d::ambientColor=Vector4(0,1,0,1);
  p3d::shaderVertexAmbient();
  if (_pointEdge.size()>0) p3d::drawPoints(_pointEdge);


  p3d::ambientColor=Vector4(0,0,1,1);
  p3d::shaderVertexAmbient();
  if (_pointFace.size()>0) p3d::drawPoints(_pointFace);
}






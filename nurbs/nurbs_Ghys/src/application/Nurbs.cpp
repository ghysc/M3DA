#include "Nurbs.h"
#include <cmath>
#include <iostream>

#include "Vector3.h"
#include "Matrix4.h"

using namespace std;
using namespace p3d;

Nurbs::~Nurbs() {
}

Nurbs::Nurbs() {
  _control.clear();
  _nbControl[D_U]=0;
  _nbControl[D_V]=0;
  _knot[D_U].clear();
  _knot[D_V].clear();
  _degree[D_U]=0;
  _degree[D_V]=0;

}

double Nurbs::startInterval(EDirection direction) {
  return _knot[direction][_degree[direction]];
}

double Nurbs::endInterval(EDirection direction) {
  return _knot[direction][_nbControl[direction]]-0.00001;
}


bool Nurbs::inInterval(EDirection direction, double u) {
  return (u>=startInterval(direction) && u<=endInterval(direction));
}


void Nurbs::knotUniform(EDirection direction,int nb) {
  _knot[direction].resize(nb);
  /* TODO : set uniform knots
   * _knot[direction][i] is the knot u_i for the given direction
   */
  double step = (double) 1/(nb-1);
  for(int i=0; i<nb; i++) {
      _knot[direction][i] = step*i;
  }
}


/** Eval the basis function Nkp(t) for the knot vector knot **/
double Nurbs::evalNkp(int k,int p,double u,const std::vector<double> &knot) {
  double result=0.0;
  /* TODO : compute Nkp(u)
   * - knot[i] : the knot i
   * - p : degree
   * - k : indice of the basis function.
   */
  if(p == 0) {
      if(u >= knot[k] && u < knot[k+1]) {
          result = 1;
      } else {
          result = 0;
      }
  } else {
      // Faire ces 2 vérifications if permet d'autoriser le fait que deux noeuds se confondent
      if((knot[k+p]-knot[k]) != 0.)
        result += ((u-knot[k])/(knot[k+p]-knot[k])) * evalNkp(k, p-1, u, knot);
      if((knot[k+p+1]-knot[k+1]) != 0.)
        result += ((knot[k+p+1]-u)/(knot[k+p+1]-knot[k+1])) * evalNkp(k+1, p-1, u, knot);
  }

  return result;
}


double Nurbs::evalNkp(EDirection direction,int k,int p,double t) {
  return evalNkp(k,p,t,_knot[direction]);
}


void Nurbs::clearControl() {
  _nbControl[D_U]=0;
  _nbControl[D_V]=0;
  _control.clear();
}

void Nurbs::initControlGrid() {
  _nbControl[D_U]=5;
  _nbControl[D_V]=4;
  _control.clear();
  double u=-1;
  double v=-1;
  double stepU=2.0/(_nbControl[D_U]-1);
  double stepV=2.0/(_nbControl[D_V]-1);
  for(int i=0;i<_nbControl[D_V];++i) {
    u=-1;
    for(int j=0;j<_nbControl[D_U];++j) {
      _control.push_back(Vector4(u,v,double(rand())/RAND_MAX-0.5,1));
      u+=stepU;
    }
    v+=stepV;
  }
  knotRemap(D_U);
  knotRemap(D_V);
}


void Nurbs::addControlU(const Vector4 &p) {
  _control.push_back(p);
  _nbControl[D_U]++;
  knotRemap(D_U);
}


Vector3 Nurbs::pointCurve(double u) {
  Vector4 result(0,0,0,0);
/* TODO :
 * - compute P(t) in result. Use the direction D_U only (curve)
 * - control(i) : control points
 * - nbControl(D_U) : number of control points
 * - evalNkp(k,p,u,_knot[D_U]) to eval basis function
 */
  // Comme "nbre de noeuds = nbre de points + degré + 1", p = nbre de noeuds - nbre de points - 1
  int p = _degree[D_U]; //_knot[direction].size()-nbControl(D_U)-1;
  double nkp;
  for(int k=0; k<nbControl(D_U); k++) {
      // Pour respecter u ∈ [up,un+1[
      if(u >= _knot[D_U][p] && u < _knot[D_U][nbControl(D_U)+1]) {
          nkp = evalNkp(k, p, u, _knot[D_U]);
          result += nkp * control(k);
      }
  }

  return Vector3(result.x(),result.y(),result.z());
}


Vector3 Nurbs::pointSurface(double u,double v) {
  Vector4 result(0,0,0,0);
  /* TODO :
   * - compute P(u,v) in result.
   * - control(i,j) : control points (i= indice in direction D_U, j=indice in direction D_V)
   * - nbControl(D_U), nbControl(D_V) to know the number of control points in each direction.
   * - degree(D_U), degree(D_V) to get the degree in each direction.
   * - evalNkp(k,p,t,_knot[<D_U or D_V>]) to eval basis function in each direction
   */
  double nkpV;
  for(int i=0; i<nbControl(D_V); i++) {
      if(v >= _knot[D_V][_degree[D_V]] && v < _knot[D_V][nbControl(D_V)+1]) {
          Vector4 resultCurve(0,0,0,0);
          double nkpU;
          for(int k=0; k<nbControl(D_U); k++) {
              // Pour respecter u ∈ [up,un+1[
              if(u >= _knot[D_U][_degree[D_U]] && u < _knot[D_U][nbControl(D_U)+1]) {
                  nkpU = evalNkp(k, _degree[D_U], u, _knot[D_U]);
                  resultCurve += nkpU * control(k, i);
              }
          }
          nkpV = evalNkp(i, _degree[D_V], v, _knot[D_V]);
          result += nkpV * resultCurve;
      }
  }

  return result.project(); // divide by w
}



void Nurbs::knotRemap(EDirection direction) {
  while (!checkNbKnot(direction)) {
    int nb=nbKnot(direction);
    _knot[direction].push_back(_knot[direction][nbKnot(direction)-1]);
    for(unsigned int i=nb-1;i>0;--i) {
      _knot[direction][i]=_knot[direction][i+1]-(_knot[direction][i]-_knot[direction][i-1])*(nb-1)/nb;
    }
  }
}


bool Nurbs::checkNbKnot(EDirection direction) {
  return (nbKnot(direction)>=nbControl(direction)+degree(direction)+1);
}


void Nurbs::knotOpenUniform(EDirection direction) {
  _knot[direction].resize(nbControl(direction)+degree(direction)+1);

  /* TODO : the first and the last knots have a degree+1 multiplicity
   */
  int multiplicity = _degree[direction]+1;
  int n = _knot[direction].size()-1;
  if(direction == D_V) {
      std::cout << "multiplicity : " << multiplicity << "\n";
      std::cout << "n : " << n << "\n";
  }
  for(int i=0; i<multiplicity; i++) {
      // First
      _knot[direction][i] = 0;
      // End
      _knot[direction][n-i] = 1;
      if(direction == D_V)
        std::cout << "n-i : " << n-i << " = " << _knot[direction][n-i] <<"\n";
  }

  // Nombre entre 0 et 1 exclus
  int n01Points = _knot[direction].size()-2*multiplicity;
  double step = (double) 1/(n01Points+1);
  for(int i=0; i<n01Points; i++) {
      _knot[direction][i+multiplicity] = (i*step)+step;
  }
}


void Nurbs::knotBezier(EDirection direction) {

  /* TODO : define a bezier curve : degree = nbControl-1,
   */
    degree(direction, nbControl(direction)-1);
    /*
     *  La multiplicité = nombre de noeuds/2,
     *  or nombre de noeuds = nbControl+degree+1 et degree = nbControl-1,
     *  donc nombre de noeuds = nbControl*2 et multiplicité = nbControl
     */
    int multiplicity = nbControl(direction);
    int n = _knot[direction].size()-1;
    for(int i=0; i<multiplicity; i++) {
        // First
        _knot[direction][i] = 0;
        // End
        _knot[direction][n-i] = 1;
    }

}

void Nurbs::setCircle() {
  /* Have to set : _control, _degree[D_U], _knot[D_U], _nbControl[D_U]
   *
   */
  _control.clear();
  _control.resize(7);

  // cos 60 = 0.5
  double cos60 = 0.5;
  double radius = 1;
  double stepTheta = 360/3;

  double p3x = cos((90+stepTheta*0)*M_PI/180) * radius;
  double p3y = sin((90+stepTheta*0)*M_PI/180) * radius;
  _control[3] = Vector4(p3x * cos60, p3y * cos60, 0., cos60);
  double p1x = cos((90+stepTheta*1)*M_PI/180) * radius;
  double p1y = sin((90+stepTheta*1)*M_PI/180) * radius;
  _control[1] = Vector4(p1x * cos60, p1y * cos60, 0., cos60);
  double p5x = cos((90+stepTheta*2)*M_PI/180) * radius;
  double p5y = sin((90+stepTheta*2)*M_PI/180) * radius;
  _control[5] = Vector4(p5x * cos60, p5y * cos60, 0., cos60);

  _control[0] = Vector4((p5x+p1x)/2., (p5y+p1y)/2., 0., 1.);
  _control[2] = Vector4((p1x+p3x)/2., (p1y+p3y)/2., 0., 1.);
  _control[4] = Vector4((p3x+p5x)/2., (p3y+p5y)/2., 0., 1.);
  _control[6] = _control[0];

  degree(D_U, 2);

  _knot[D_U] = {0,0,0,1,1,2,2,3,3,3};
  // Normalize values
  double max = 3;
  for(int i=0; i<_knot[D_U].size(); i++) {
      _knot[D_U].at(i) = _knot[D_U].at(i)/max;
  }

  _nbControl[D_U] = _control.size();

}


void Nurbs::setRevolution(int nbV) {
  if (nbV<2) return;
  _nbControl[D_V]=nbV;
  _degree[D_V]=_degree[D_U];
  _control.resize(_nbControl[D_U]*_nbControl[D_V]);
  knotRemap(D_V);
  knotOpenUniform(D_V);

  double stepTheta=360.0/(nbV-1);
  double theta=stepTheta;
  Matrix4 rotate;
  for(int slice=nbControl(D_U);slice<nbControl(D_U)*nbControl(D_V);slice+=nbControl(D_U)) {
    rotate.setRotation(theta,0,1,0);
    for(int istack=0;istack<nbControl(D_U);++istack) {
      _control[slice+istack]=rotate*_control[istack];
    }
    theta+=stepTheta;
  }
}




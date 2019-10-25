#include "SubdivCurve.h"
#include <cmath>
#include <iostream>

#include "Vector3.h"
#include "Matrix4.h"

using namespace std;
using namespace p3d;

SubdivCurve::~SubdivCurve() {
}

SubdivCurve::SubdivCurve() {
  _nbIteration=1;
  _source.clear();
  _result.clear();

}


void SubdivCurve::addPoint(const p3d::Vector3 &p) {
  _source.push_back(p);
}

void SubdivCurve::point(int i,const p3d::Vector3 &p) {
  _source[i]=p;
}

void SubdivCurve::chaikinIter(const vector<Vector3> &p) {
  /* TODO : one iteration of Chaikin : input = p, output = you must set the vector _result (vector of Vector3)
   */
  _result.clear();

  for(int i=0; i<p.size()-1; i++) {
      Vector3 q0 = 3*p[i]/4 + p[i+1]/4;
      Vector3 q1 = p[i]/4 + 3*p[i+1]/4;
      _result.push_back(q0);
      _result.push_back(q1);
  }
  if(isClosed()) {
      Vector3 qFin = 3*p[p.size()-1]/4 + p[0]/4;
      Vector3 qDebut = p[p.size()-1]/4 + 3*p[0]/4;
      _result.push_back(qFin);
      _result.push_back(qDebut);
  }

}

void SubdivCurve::dynLevinIter(const vector<Vector3> &p) {
  /* TODO : one iteration of DynLevin : input = p, output = you must set the vector _result (vector of Vector3)
   */
  _result.clear();

  Vector3 qPremier = p[0];
  Vector3 qDeuxieme = -(p[2]+p[p.size()-1])/16 + 9*(p[1]+p[0])/16;
  _result.push_back(qPremier);
  _result.push_back(qDeuxieme);
  for(int i=1; i<p.size()-2; i++) {
      Vector3 q0 = p[i];
      Vector3 q1 = -(p[i+2]+p[i-1])/16 + 9*(p[i+1]+p[i])/16;
      _result.push_back(q0);
      _result.push_back(q1);
  }
  Vector3 qFin = p[p.size()-2];
  Vector3 qDebut = -(p[0]+p[p.size()-3])/16 + 9*(p[p.size()-1]+p[p.size()-2])/16;
  _result.push_back(qFin);
  _result.push_back(qDebut);
  qFin = p[p.size()-1];
  qDebut = -(p[1]+p[p.size()-2])/16 + 9*(p[0]+p[p.size()-1])/16;
  _result.push_back(qFin);
  _result.push_back(qDebut);

}


void SubdivCurve::chaikin() {
  if (_source.size()<2) return;
  vector<Vector3> current;
  _result=_source;
  for(int i=0;i<_nbIteration;++i) {
    current=_result;
    chaikinIter(current);
  }
}

void SubdivCurve::dynLevin() {
  if (_source.size()<2) return;
  if (!isClosed()) return;
  vector<Vector3> current;
  _result=_source;
  for(int i=0;i<_nbIteration;++i) {
    current=_result;
    dynLevinIter(current);
  }
}



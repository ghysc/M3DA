#include "GLApplication.h"
#include "GLTool.h"

#include "Vector3.h"
#include "Vector2.h"


#include <iostream>

/*!
*
* @file
*
* @brief
* @author F. Aubert
*
*/


using namespace std;
using namespace p3d;

GLApplication::~GLApplication() {
}

enum EMenu {M_Draw_Square, M_Set_Section_Square, M_Set_Section_Circle,M_Draw_Section,M_Draw_Path,M_Spline_Line,M_Build_Extrusion,M_Build_Revolution};
enum EDraw {D_Square,D_Path,D_Section,D_Extrusion};
enum EPath {Path_Line,Path_Spline};
EMenu _activeMenu;
EDraw _activeDraw;
EPath _activePath;

GLApplication::GLApplication() {
  //
  _leftPanelMenu << "Draw square (drawing example)" << "Input section (= square)" << "Input section (= circle)" << "Input section" << "Input Path" << "Switch Linear/Spline Path";
  _leftPanelMenu  << "Draw extrusion" << "Draw revolution (Path = circle)";
  _activeMenu=M_Draw_Square;
  _activeDraw=D_Square;
  _activePath=Path_Line;

  pathDefault();
  sectionCircle();

  _cameraSection.ortho(-2.5,2.5,-2.5,2.5,0,2);
  _cameraPath.ortho(-2.5,2.5,-2.5,2.5,0,2);
  _cameraPath.position(0,1,0);
  _cameraPath.lookAt(Vector3(0,0,0));
  _cameraExtrusion.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
  _cameraExtrusion.position(0,0,10);
  _cameraExtrusion.lookAt(Vector3(0,0,0));

}


/** ********************************************************************** **/
void GLApplication::initialize() {
  // appelée 1 seule fois à l'initialisation du contexte
  // => initialisations OpenGL
  glClearColor(1,1,1,1);

  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearDepth(1);

  p3d::initGLTool();

  // ...
}

void GLApplication::resize(int width,int height) {
  // appelée à chaque dimensionnement du widget OpenGL
  // (inclus l'ouverture de la fenêtre)
  // => réglages liés à la taille de la fenêtre
  _cameraPath.viewport(0,0,width,height);
  _cameraSection.viewport(0,0,width,height);
  _cameraExtrusion.viewport(0,0,width,height);
  // ...
}

void GLApplication::update() {
  // appelée toutes les 20ms (60Hz)
  // => mettre à jour les données de l'application
  // avant l'affichage de la prochaine image (animation)
  // ...

  if (_activeDraw==D_Path) {
    if (mouseLeftPressed()) {
      _path.push_back(_cameraPath.windowToWorld(mouseX(),mouseY()));
    }
    if (keyPressed(Qt::Key_X)) _path.clear();
  }
  if (_activeDraw==D_Section) {
    if (mouseLeft()) {
      _section.push_back(_cameraSection.windowToWorld(mouseX(),mouseY()).xy());
    }
    if (keyPressed(Qt::Key_X)) _section.clear();
  }
  if (_activeDraw==D_Extrusion) {
    updateCameraExtrusion();
  }
}

/* ************************************************************ */

void GLApplication::updateCameraExtrusion() {
  if (mouseLeft()) {
    Vector3 center=_cameraExtrusion.pointTo(Coordinate_Local,Vector3(0,0,0));
    Vector3 vertical=Vector3(0,1,0);
    _cameraExtrusion.translate(center,Coordinate_Local);
    _cameraExtrusion.rotate(-deltaMouseX()/2.0,vertical,Coordinate_Local);
    _cameraExtrusion.rotate(deltaMouseY()/2.0,Vector3(1,0,0),Coordinate_Local);
    _cameraExtrusion.translate(-center,Coordinate_Local);
  }
  if (left()) _cameraExtrusion.left(0.3);
  if (right()) _cameraExtrusion.right(0.3);
  if (forward()) _cameraExtrusion.forward(0.3);
  if (backward()) _cameraExtrusion.backward(0.3);
  if (accelerateWheel()) {
    _frustum*=1.05;
    _cameraExtrusion.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
  }
  if (decelerateWheel()) {
    _frustum/=1.05;
    _cameraExtrusion.frustum(-_frustum,_frustum,-_frustum,_frustum,0.03,1000);
  }
}


void GLApplication::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  switch(_activeDraw) {
  case D_Square: p3d::apply(_cameraSection);drawSquare();break;
  case D_Section: p3d::apply(_cameraSection);drawSection();break;
  case D_Extrusion: p3d::apply(_cameraExtrusion);drawExtrusion();break;
  case D_Path:
    apply(_cameraPath);    if (_activePath==Path_Line) {
      drawPathLine();
    }
    else if (_activePath==Path_Spline) {
      drawPathSpline();
    }
  default:break;
  }
}


/** ************************************************************************ **/

/**
 * @brief GLApplication::drawSquare
 * Example of drawing primitives : p3d::drawPoints and p3d::drawLineStrip and p3d:draw for text. Notice shader activation before the p3d::draw... calls
 */
void GLApplication::drawSquare() {
  // compute points of the square in array of Vector2
  vector<Vector2> pts;
  pts.resize(5);
  pts[0]=Vector2(-0.5,-0.5);
  pts[1]=Vector2(0.5,-0.5);
  pts[2]=Vector2(0.5,0.5);
  pts[3]=Vector2(-0.5,0.5);
  pts[4]=pts[0];

  // draw square (line strip)
  p3d::ambientColor=Vector4(1,0,0,1); // the drawing color for the shader : must be set before the call p3d::shaderVertexAmbient
  p3d::shaderVertexAmbient(); // enable a simple shader.
  p3d::drawLineStrip(pts,5); // draw the array of Vector2D with the actual shader (you must set the number of points to draw : 5 here for the square).

  // draw vertices (points)
  glPointSize(10);
  p3d::ambientColor=Vector4(0,0,1,1);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(pts);

  // draw text
  p3d::ambientColor=Vector4(1,0,1,1);
  p3d::draw("V0",Vector3(pts[0],0));
}


/** ************************************************************************ **/

/**
 * @brief GLApplication::sectionSquare
 * Example of cross section setup (here a square : notice the repeat of the first vertex to close the cross section).
 */

void GLApplication::sectionSquare() {
  _section.clear();
  _section.push_back(Vector2(-1,-1));
  _section.push_back(Vector2(1,-1));
  _section.push_back(Vector2(1,1));
  _section.push_back(Vector2(-1,1));
  _section.push_back(Vector2(-1,-1));
}


/**
 * @brief GLApplication::crossSectionCircle
 * set the cross section (i.e. set _inputCrossSection) as a circle
 */
void GLApplication::sectionCircle() {
  _section.clear();

  float radius = 1.f;
  float nbPoints = 10.f;
  float PI = 3.14159f / nbPoints;
  float step = 2.0 * PI;

  for(float i=0; i <= nbPoints; i++) {
      float x = radius * cos(step * i);
      float y = radius * sin(step * i);
      _section.push_back(Vector2(x,y));
  }
}


/** ************************************************************************ **/

void GLApplication::pathDefault() {
    _path.clear();
    _path.push_back(Vector3(-2,0,-2));
    _path.push_back(Vector3(0,0,2));
    _path.push_back(Vector3(2,0,-1));
}

void GLApplication::pathCircle() {
    _path.clear();

    float radius = 1.f;
    float nbPoints = 10.f;
    float PI = 3.14159f / nbPoints;
    float step = 2.0 * PI;

    for(float i=0; i <= nbPoints; i++) {
        float x = radius * cos(step * i);
        float y = radius * sin(step * i);
        _path.push_back(Vector3(x,0,y));
    }
}

/** ************************************************************************ **/

void GLApplication::drawSection() {
  p3d::ambientColor=Vector4(1,0,0,1);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(_section);
  p3d::drawLineStrip(_section);
}

void GLApplication::drawPathLine() {
  if (_path.size()<1) return;

  p3d::ambientColor=Vector4(0,0,1,1);
  p3d::shaderVertexAmbient();
  p3d::drawPoints(_path);
  p3d::drawLineStrip(_path);
}


void GLApplication::drawExtrusion() {
  if (_extrusion.size()<4) return;
  int nbSlice=_section.size();

  p3d::ambientColor=Vector4(1,0,0,1);
  p3d::shaderVertexAmbient();

  //drawGrid(_extrusion,nbSlice); // comment this once last question done

  p3d::lightPosition[0]=Vector4(0,0,10,1);
  p3d::lightIntensity[0]=1.0;
  p3d::material(Vector4(0,0,0.3,1),Vector3(0,0.2,0.8),Vector3(0,0.8,0.3),100);
  p3d::diffuseBackColor=Vector3(0.8,0,0);
  p3d::shaderLightPhong();
  fillGrid(_extrusion,_normalExtrusion,nbSlice);

  drawPath();
}



void GLApplication::drawPathSpline() {
  if (_path.size()>=2) {
    vector<Vector3> toDraw;
    toDraw.clear();
    int nbPts=100;
    double step=1.0/(nbPts-1);
    double t=0;
    for(int i=0;i<nbPts;++i) {
      toDraw.push_back(pointSpline(t));
      t+=step;
    }

    p3d::ambientColor=Vector4(0,0,1,1);
    p3d::shaderVertexAmbient();
    drawLineStrip(toDraw,toDraw.size());
  }
  if (_path.size()>0) {
    p3d::ambientColor=Vector4(0,0,1,1);
    p3d::shaderVertexAmbient();
    p3d::drawPoints(_path);
  }
}

/** ************************************************************************* **/

/**
 * @brief transform the point p in the plane (x,y,0) to the point in the plane with the normal n (i.e. rotation of the plane (x,y,0))
 * @param p : the point expressed in the plane (x,y,0)
 * @param n : the normal of the plane
 * @return the transformation of the point p
 */

Vector3 GLApplication::rotatePlane(const Vector3 &p,const Vector3 &n) {
    Quaternion q = Quaternion();
    q.setRotation(Vector3(0,0,1), n);
    return q * p;
}



Vector3 GLApplication::pointSpline(double tNormalized) {
  Vector3 result;

  int i = tNormalized * (_path.size()-1);
  double t = tNormalized * (_path.size()-1) - i;
  Vector3 point0 = _path[i];
  Vector3 point1 = _path[i+1];
  Vector3 tangent0 = tangentPathLine(i);
  Vector3 tangent1 = tangentPathLine(i+1);

    //t3(2P0−2P1+T0+T1)+t2*(−3P0+3P1−2T0−T1)+tT0+P0

  result = (t*t*t) * (2*point0-2*point1+tangent0+tangent1) + (t*t) *  (-3*point0+3*point1-2*tangent0-tangent1) + t * tangent0 + point0;
  /*
  double h00 = 2*(tNormalized*tNormalized*tNormalized) - 3*(tNormalized*tNormalized) + 1;
  double h10 = (tNormalized*tNormalized*tNormalized) - 2*(tNormalized*tNormalized) + tNormalized;
  double h01 = -2*(tNormalized*tNormalized*tNormalized) + 3*(tNormalized*tNormalized);
  double h11 = (tNormalized*tNormalized*tNormalized) - (tNormalized*tNormalized);

  result = h00*_path[point0] + h10*tangent0 + h01*_path[point1] + h11*tangent1;
  */

  return result;
}


Vector3 GLApplication::tangentPathSpline(double tNormalized, bool isRevolution) {
  Vector3 result;

  int i = tNormalized * (_path.size()-1);
  double t = tNormalized * (_path.size()-1) - i;
  Vector3 point0 = _path[i];
  Vector3 point1 = _path[i+1];
  Vector3 tangent0;
  if(isRevolution)
      tangent0 = tangentPathCircle(i);
  else
      tangent0 = tangentPathLine(i);
  Vector3 tangent1;
  if(isRevolution)
      tangent1 = tangentPathCircle(i+1);
  else
      tangent1 = tangentPathLine(i+1);

  //P 0 ( t ) = ( 3t 2 2t 1 0 ) M h G
  result = (3*t*t) * (2*point0-2*point1+tangent0+tangent1) + (2*t) *  (-3*point0+3*point1-2*tangent0-tangent1) + tangent0;

  return result;
}



Vector3 GLApplication::tangentPathLine(unsigned int i) {
  Vector3 result;

  if(i==0)
      result = _path[1]-_path[0];
  else if(i==_path.size()-1)
      result = _path[_path.size()-1]-_path[_path.size()-2];
  else
      result = _path[i+1]-_path[i-1];

  return result;
}

Vector3 GLApplication::tangentPathCircle(unsigned int i) {
  Vector3 result;

  if(i==0)
      result = _path[1]-_path[_path.size()-2]; // au lieu de _path[1]-_path[0];
  else if(i==_path.size()-1)
      result = _path[1]-_path[_path.size()-2]; // au lieu de _path[_path.size()-1]-_path[_path.size()-2];
  else
      result = _path[i+1]-_path[i-1];

  return result;
}

/** ************************************************************************* **/

void GLApplication::normalSection() {
  _normalSection.clear();

  for(int j=0; j<_section.size(); j++) {
      //p2 - p1 puis y en x et x en y pour trouver la normale du segment adjoint (p2 = le point actif)
      Vector2 v1 = _section[(j+1)%_section.size()] - _section[j];
      double tmp = v1.x();
      v1.x(-v1.y());
      v1.y(tmp);
      Vector2 v2 = _section[j] - _section[(j-1) < 0 ? _section.size()-1 : (j-1)];
      double tmp2 = v2.x();
      v2.x(-v2.y());
      v2.y(tmp2);
      _normalSection.push_back((v1+v2)/2);
  }
}


void GLApplication::extrudeLine(bool isRevolution) {
  if (_path.size()<1 || _section.size()<1) return;

  _extrusion.clear();
  _normalExtrusion.clear(); // for lighting (last question)

  normalSection();

  for(int i=0; i<_path.size(); i++) {
      for(int j=0; j<_section.size(); j++) {
          if(isRevolution) {
              Vector3 rotated = _path[i] + rotatePlane(Vector3(_section[j], 0.), tangentPathCircle(i));
              _extrusion.push_back(Vector3(rotated.x(), rotated.y(), rotated.z()));
          } else {
              Vector3 rotated = _path[i] + rotatePlane(Vector3(_section[j], 0.), tangentPathLine(i));
              _extrusion.push_back(Vector3(rotated.x(), rotated.y(), rotated.z()));
          }
          Vector3 normalRot = rotatePlane(Vector3(_normalSection[j], 0.), tangentPathLine(i));
          _normalExtrusion.push_back(Vector3(normalRot.x(), normalRot.y(), normalRot.z()));
      }
  }

}

void GLApplication::extrudeSpline(bool isRevolution) {
  if (_path.size()<1 || _section.size()<1) return;

  _extrusion.clear();
  _normalExtrusion.clear(); // for lighting (last question)

  normalSection();

  int nbPoints = 100;
  double step=1.0/(nbPoints-1);
  double t=0;
  for(int i=0; i<nbPoints; i++) {
      for(int j=0; j<_section.size(); j++) {
          Vector3 rotated = pointSpline(t) + rotatePlane(Vector3(_section[j], 0.), tangentPathSpline(t, isRevolution));
          _extrusion.push_back(Vector3(rotated.x(), rotated.y(), rotated.z()));
          Vector3 normalRot = rotatePlane(Vector3(_normalSection[j], 0.), tangentPathSpline(t, isRevolution));
          _normalExtrusion.push_back(Vector3(normalRot.x(), normalRot.y(), normalRot.z()));
      }
      t+=step;
  }

}


/** ************************************************************************* **/


double GLApplication::scale(double tNormalized) {
  return 1.0;

}

void GLApplication::buildExtrusion(bool isRevolution) {
  if (_activePath==Path_Line)
    extrudeLine(isRevolution);
  else if (_activePath==Path_Spline) {
    extrudeSpline(isRevolution);
  }
}

void GLApplication::drawPath() {
  if (_activePath==Path_Line) {
    drawPathLine();
  }
  else if (_activePath==Path_Spline) {
    drawPathSpline();
  }
}


/** ********************************************************************** **/
/** i = button number, s = button text
 */
void GLApplication::leftPanel(int i,const std::string &) {
  _activeMenu=static_cast<EMenu>(i);

  switch (_activeMenu) {
  case M_Draw_Square:
    _activeDraw=D_Square;
    break;
  case M_Set_Section_Square:
    sectionSquare();
    _activeDraw=D_Section;
    break;
  case M_Set_Section_Circle:
    sectionCircle();
    _activeDraw=D_Section;
    break;
  case M_Build_Extrusion:
    buildExtrusion(false);
    _activeDraw=D_Extrusion;
    break;
  case M_Build_Revolution:
    pathCircle();
    buildExtrusion(true);
    _activeDraw=D_Extrusion;
    break;
  case M_Draw_Path:
    _activeDraw=D_Path;
    break;
  case M_Draw_Section:
    _activeDraw=D_Section;
    break;
  case M_Spline_Line:
    if (_activePath==Path_Line) _activePath=Path_Spline;
    else _activePath=Path_Line;
    if (_activeDraw==D_Extrusion) {
      buildExtrusion(false);
    }
    break;
  default:break;
  }

  /*
  switch (i) {
    case 0:...;break;
    case 1:...;break;
    ...
  }
  */
}
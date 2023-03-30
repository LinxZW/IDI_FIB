// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

void MyGLWidget::carregaShaders() {
    BL2GLWidget::carregaShaders();
    projLoc = glGetUniformLocation(program->programId(), "proj");
    viewLoc = glGetUniformLocation(program->programId(), "view");
}

void MyGLWidget::projectTransform() {
    // glm::perspective (FOV en radians, ra window, znear, zfar)
    glm::mat4 Proj = glm::perspective(cam3persona.fov, cam3persona.raw, cam3persona.zNear, cam3persona.zFar);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform () {
    // glm::lookAt (OBS, VRP, UP) on posar l'observador
    glm::mat4 View = glm::lookAt (cam3persona.OBS, cam3persona.VRP, cam3persona.UP);
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::paintGL() {
    
    glViewport (0, 0, width(), height()); // Aquesta crida no caldria perquè Qt la fa de forma automàtica amb aquests paràmetres

	// Esborrem el frame-buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Carreguem la transformació de model
	modelTransform (true);
	// Activem el VAO per a pintar el homer 
	glBindVertexArray (VAO_Homer);
	// pintem
	glDrawArrays(GL_TRIANGLES, 0, m.faces().size()*3);


    modelTransform (false);
    glBindVertexArray (VAO_Terra);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glBindVertexArray (0);
}

void MyGLWidget::ini_camera() {
    //La distacia pot ser un valor qualsevol que permeti veure tota l'escena
    float distancia = escena.radi * 2.0;
    cam3persona.zNear = distancia - escena.radi;
    cam3persona.zFar = distancia + escena.radi;
    cam3persona.raw = 1.0f;
    //fov = 2*angle
    cam3persona.fov = 2.0 * asin(escena.radi/distancia);
    cam3persona.OBS = glm::vec3(0,0,distancia);
    cam3persona.VRP = glm::vec3(0,0,0);
    cam3persona.UP = glm::vec3(0,1,0);

    projectTransform();
    viewTransform();
}

MyGLWidget::~MyGLWidget() {
}

void MyGLWidget::initializeGL() {
    initializeOpenGLFunctions();  
    glEnable (GL_DEPTH_TEST);
    glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
    carregaShaders();
    creaBuffers();
    escala = 1.0f;

    calculaCapsa_Homer();
    calculaCapsa_Escena();
    ini_camera();
}

void MyGLWidget::creaBuffers() {

    m.load ("../models/HomerProves.obj");

    // Creació del Vertex Array Object per pintar
    glGenVertexArrays(1, &VAO_Homer);
    glBindVertexArray(VAO_Homer);

    GLuint VBO_Homer[2];
    //Fent 2 buffers un per guardar els vertex i un el color
    glGenBuffers(2, VBO_Homer);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[0]);
    glBufferData (GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces ().size () * 3 * 3, m.VBO_vertices (), GL_STATIC_DRAW); 
    
    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces ().size () * 3 * 3, m.VBO_matdiff (), GL_STATIC_DRAW);

    // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);


    //<<creació del buffer del terra>> 
    glm::vec3 posterra[6];
    posterra[0] = glm::vec3(-2.0, -1.0, -2.0);
    posterra[1] = glm::vec3(-2.0, -1.0, 2.0);
    posterra[2] = glm::vec3(2.0, -1.0, -2.0);
    posterra[3] = glm::vec3(-2.0, -1.0, 2.0);
    posterra[4] = glm::vec3(2.0, -1.0, -2.0);
    posterra[5] = glm::vec3(2.0, -1.0, 2.0);
    
    //un altre manera de declarar els components dels vectors 
    glm::vec3 colterra[6] = {
        glm::vec3(1,0,1),
        glm::vec3(1,0,1),
        glm::vec3(1,0,1),
        glm::vec3(1,0,1),
        glm::vec3(1,0,1),
        glm::vec3(1,0,1)
    };

    // Creació del Vertex Array Object per pintar
    glGenVertexArrays(1, &VAO_Terra);
    glBindVertexArray(VAO_Terra);

    GLuint VBO_TerraPos;
    glGenBuffers(1, &VBO_TerraPos);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);


    GLuint VBO_TerraCol;
    glGenBuffers(1, &VBO_TerraCol);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerraCol);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colterra), colterra, GL_STATIC_DRAW);

    // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    glBindVertexArray (0);  
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_S: { // escalar a més gran
      escala += 0.05;
      break;
    }
    case Qt::Key_D: { // escalar a més petit
      escala -= 0.05;
      break;
    }

    case Qt::Key_R: { // escalar a més gran
      angle += 0.05;
      break;
      default: event->ignore(); break;
    }
  }
  update();
}

void MyGLWidget::modelTransform (bool gir) 
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  if (gir) transform = glm::rotate(transform, angle, glm::vec3(0,1,0));
  transform = glm::scale(transform, glm::vec3(homer.escala));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::calculaCapsa_Homer() {
    float maxx, maxy, maxz, minx, miny, minz;
    //els 3 primers vertexs de la taula de vertexs del model homer
    //per inicialitzar els continguts dels variables 
    minx = maxx = m.vertices()[0];
    miny= maxy = m.vertices()[1];
    minz = maxz = m.vertices()[2];
    for (unsigned int i = 3; i < m.vertices().size(); i+=3) {
        if (m.vertices()[i] < minx) minx = m.vertices()[i];
        if (m.vertices()[i] > maxx) maxx = m.vertices()[i];

        if (m.vertices()[i+1] < miny) miny = m.vertices()[i+1];
        if (m.vertices()[i+1] > maxy) maxy = m.vertices()[i+1];

        if (m.vertices()[i+2] < minz) minz = m.vertices()[i+2];       
        if (m.vertices()[i+2] > maxz) maxz = m.vertices()[i+2];
    }

    //posem en un struct els valors de max, min trobats 
    homer.minim = glm::vec3(minx,miny,minz);
    homer.maxim = glm::vec3(minx,miny,minz);
    homer.centre = (homer.minim + homer.maxim)/2.f;
    homer.centreBase = glm::vec3((minx + maxx)/2.f, miny, (minz+maxz)/2.f);
    homer.escala = 1;
    std::cout << std::endl << 
  "MIN(x,y,z) = (" << minx << "," << miny << "," << minz << ")" << std::endl <<
  "MAX(x,y,z) = (" << maxx << "," << maxy << "," << maxz << ")" << std::endl;
}

void MyGLWidget::calculaCapsa_Escena() {
    escena.maxim = glm::vec3(2,1,2);
    //sabem que el mionim del terra esta al -1,-1,-1
    //punts extrems del terra + al ̧cada del HomerProves es 2
    escena.minim = glm::vec3(-2,-1,-2);  
    escena.centre = (escena.maxim + escena.minim)/2.0f;
    escena.radi = glm::distance(escena.centre, escena.maxim);
}

//s'activa automaticament quan canviem la mida del view
void MyGLWidget::resizeGL(int w, int h) {
    float rav = float(w)/float(h); 
    //fer que raw = rav 
    if (rav > 1) cam3persona.raw = rav;
    //canviar l'angle de obertura
    else cam3persona.fov = 2 * atan(tan(cam3persona.fov)/rav);
    projectTransform();
}

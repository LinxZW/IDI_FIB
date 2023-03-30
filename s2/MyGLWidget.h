// MyGLWidget.h
#include "BL2GLWidget.h"
#include "../Model/model.h"

class MyGLWidget : public BL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : BL2GLWidget(parent) {}
    ~MyGLWidget();

  private:

    int printOglError(const char file[], int line, const char func[]);
  
  protected:

    //posem virtual si la funcio ja existia previament en el BL2GLWidget 
    virtual void carregaShaders();

    virtual void paintGL();

    virtual void creaBuffers();

    virtual void initializeGL();

    virtual void keyPressEvent (QKeyEvent *event);
   
    virtual void modelTransform (bool gir); 

    void viewTransform();

    void projectTransform();

    void ini_esccena();

    void ini_camera();

    void calculaCapsa_Homer();

    void calculaCapsa_Escena();

    void resizeGL(int w, int h);

    GLuint VAO_Terra;

    //Variable de gluniform locatin 
    GLuint projLoc, viewLoc;

    //VAO del homer
    GLuint VAO_Homer;

    float angle = 0;

    //Declaracio de models

    Model m; //un únic model
    //Declaracio de 3 models, però un de std i un de array 
    //Model vectorModels[3];
    //vector<Model> models;
    struct Figura {
      Model model;
      glm::vec3 minim;
      glm::vec3 maxim;
      glm::vec3 centre;
      glm::vec3 centreBase;
      float escala;
    };

    Figura homer;

    struct Escena {
      glm::vec3 minim;
      glm::vec3 maxim;
      glm::vec3 centre;
      float radi;
    };

    Escena escena;

    struct Camera {
      float fov;
      float fovini;
      float raw;
      float zNear;
      float zFar;
      float left;
      float right;
      float botom;
      float top;
      glm::vec3 OBS, VRP, UP;
    };

    Camera cam3persona;
    Camera cam1persona;
};
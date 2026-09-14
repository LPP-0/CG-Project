#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <IL/il.h>
#include "include/engine.h"
#include "include/parser.h" 
#include "structs.h" 

Scene myScene;
float alpha, beta , radius;

void changeSize(int w, int h) {
    if (h == 0)
        h = 1;
    float ratio = 1.0 * w / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);

    gluPerspective(myScene.camera.fov, ratio, myScene.camera.nearPlane, myScene.camera.farPlane);
    glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(myScene.camera.position.x, myScene.camera.position.y, myScene.camera.position.z,
              myScene.camera.lookAt.x, myScene.camera.lookAt.y, myScene.camera.lookAt.z,
              myScene.camera.up.x, myScene.camera.up.y, myScene.camera.up.z);

    updateLightPositions(myScene);

    glDisable(GL_LIGHTING);
    drawAxes();
    glEnable(GL_LIGHTING);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    renderGroup(myScene.root);

    glutSwapBuffers();
    glutPostRedisplay();
}


void processKeys(unsigned char key, int x, int y) {
    switch (key) {
        case 'w': radius -= 0.5f; break; 
        case 's': radius += 0.5f; break;

        case 'z': radius -= 5.0f; break;
        case 'x': radius += 5.0f; break;
    }

    updateCamera(alpha, beta, radius, myScene.camera);
    glutPostRedisplay();
}

void processSpecialKeys(int key, int x, int y) {
    float step = 0.1f;
    switch (key) {
        case GLUT_KEY_LEFT:  alpha -= step; break;
        case GLUT_KEY_RIGHT: alpha += step; break;
        case GLUT_KEY_UP:    beta += step; 
            if (beta > 1.5f) beta = 1.5f; 
            break;
        case GLUT_KEY_DOWN:  beta -= step; 
            if (beta < -1.5f) beta = -1.5f; 
            break;
    }
    
    myScene.camera.position.x = radius * cos(beta) * sin(alpha);
    myScene.camera.position.y = radius * sin(beta);
    myScene.camera.position.z = radius * cos(beta) * cos(alpha);

    glutPostRedisplay();
}

int main(int argc, char **argv) {

    std::string xmlFile = "scenes/test.xml";

    if (argc > 1){
        xmlFile = argv[1];
    }

    parseXML(xmlFile, myScene);
    syncCamera(alpha, beta, radius, myScene.camera);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(myScene.window.width, myScene.window.height);
    glutCreateWindow("CG@DI-UM");

    #ifndef __APPLE__
    glewInit();
    #endif

    ilInit();

    setupLighting(myScene);

    initSceneBuffers(myScene);

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene); 

    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    

    glutMainLoop();

    return 1;
}

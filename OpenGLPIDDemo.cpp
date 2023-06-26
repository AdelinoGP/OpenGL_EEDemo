#include <GL/glut.h>
#include <stdio.h>
#include <corecrt_math.h>
#include <corecrt_math_defines.h>
#include <iostream>

#define XDMAX 800
#define YDMAX 650

class Color {
public:
    float red;
    float green;
    float blue;
    float alpha;

    Color(float red, float green, float blue, float alpha) {
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->alpha = alpha;
    }
};

class Vector2 {
public:
    float x;
    float y;

    Vector2() {
        this->x = 0;
        this->y = 0;
    }

    Vector2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    Vector2 operator+(float addValue) {
        Vector2 result(this->x, this->y);
        result.x += addValue;
        result.y += addValue;
        return result;
    }
};

float lerpf(float a, float b, float step)
{
    return (a * (1.0 - step)) + (b * step);
}

void ChangeSize(GLsizei w, GLsizei h) {
    if (h == 0)
        h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        gluOrtho2D(0, XDMAX, 0, YDMAX * h / w);
    else
        gluOrtho2D(0, XDMAX * w / h, 0, YDMAX);
}

void SpecialKeyEvent(int tecla, int xmouse, int ymouse) {
    switch (tecla) {
    case GLUT_KEY_UP:
        break;

    case GLUT_KEY_DOWN:
        break;

    case GLUT_KEY_END:
        break;
    }
    glutPostRedisplay();
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);


    glColor3f(1, 1, 0);
    glLineWidth(3);

    glBegin(GL_LINE_STRIP);
        glVertex2f(0,200);
        glVertex2f(200, 100);
        glVertex2f(150, 200);
    glEnd();

    glutSwapBuffers();
}

int main(int argc, char** argv) {

    //Inicialização e abrir janela
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(480, 320);
    glutCreateWindow("PacmanAnimation-Adelino");

    //Callbacks
    glutDisplayFunc(RenderScene);
    glutIdleFunc(RenderScene);
    glutSpecialFunc(SpecialKeyEvent);
    glutReshapeFunc(ChangeSize);

    //Entrar no loop principal do Glut
    glutMainLoop();

    return 1;
}
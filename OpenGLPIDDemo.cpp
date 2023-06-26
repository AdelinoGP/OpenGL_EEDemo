#include <GL/glut.h>
#include <stdio.h>
#include <corecrt_math.h>
#include <corecrt_math_defines.h>
#include <iostream>
#include <list>
#include "OpenGLPIDDemo.h"
#include <functional>

using namespace std;

#define XDMAX 1000
#define YDMAX 660

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
    static Vector2 getFromList(list<Vector2> _list, int _i) {
        list<Vector2>::iterator it = _list.begin();
        for (int i = 0; i < _i; i++) ++it;
        return *it;
    }
};

class PIDController {
public:
    float initialValue = 0;
    float currentValue;
    float desiredValue = 30;

    float currentTick;

    float cumulative_error = 0;
    float previous_error = 0;

    float Kp = 1.5;
    float Ki = 0.05;
    float Kd = 0.1;

    list<Vector2> currentSimulatedValues;

    list<Vector2> SimulateTillDesired() {

        cout << "Valor Inicial: "<< initialValue << " Valor desejado: "<< desiredValue << endl;
        currentTick = 0;
        currentValue = initialValue;

        list<Vector2> simulatedValues = { Vector2(currentTick,currentValue) };

        int ticksWhereEqual = 0;
        while (ticksWhereEqual < 10 && currentTick < 1000) {
            SimulateTick();
            simulatedValues.emplace_back(Vector2(currentTick, currentValue));
            float difference = desiredValue - currentValue;
            if (difference > -0.005 && difference < 0.005)
                ticksWhereEqual++;
        }

        if (currentTick == 1000)
            cout << "Valor maximo de ticks excedido" << endl;

        return currentSimulatedValues = simulatedValues;
    }

    void SimulateTick() {
            currentValue += CalculatePID();
            currentTick++;
    }

    float CalculatePID() {
        float error = desiredValue - currentValue;
        float p_value = error * Kp;
        float i_value = cumulative_error * Ki;
        float d_value = (error - previous_error) * Kd;
        float pid_value = p_value + i_value + d_value;

        if (pid_value > 1)
            pid_value = 1;
        if( pid_value < -1)
            pid_value = -1;

        cumulative_error += error;
        previous_error = error;


        cout << "Valor PID"<< pid_value  << "Valor Atual:" << currentValue << endl;
        return pid_value;
    }
};

PIDController* controllerPtr;

float lerpf(float a, float b, float step){
    return (a * (1.0 - step)) + (b * step);
}

float normalizeFloat(float initialValue, float endValue, float current) {
    return  (current - initialValue) / (endValue - initialValue);
}

void RenderBackGround() {
    glColor3f(0.3 , 0.3, 0.3);
    glLineWidth(4);
    glRectf(0, 0, XDMAX / 3, YDMAX);

    glLineStipple(1, 10);
    glColor3f(1, 0.5f, 0);
    glLineWidth(3);

    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINE_STRIP);
        glVertex2f((XDMAX / 3) + 10, 10);
        glVertex2f((XDMAX / 3) + 10, YDMAX / 2);
        glVertex2f(XDMAX * 2 - XDMAX / 3, YDMAX / 2);
        glVertex2f((XDMAX / 3) + 10, YDMAX / 2);
        glVertex2f((XDMAX / 3) + 10, YDMAX - 10);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}

void RenderSimulatedValues(PIDController *pidController) {
    glColor3f(0, 0.5f, 1);
    glLineWidth(4);

    Vector2 initialPoint = Vector2((XDMAX / 3) + 11, 0);
    glBegin(GL_LINE_STRIP);
    int valuesSize = pidController->currentSimulatedValues.size();
    float initialValue = pidController->initialValue;
    float desiredValue = pidController->desiredValue * 2;

    for (Vector2 vec : pidController->currentSimulatedValues) {
        glVertex2f(initialPoint.x + (normalizeFloat(0, valuesSize, vec.x) * XDMAX), normalizeFloat(initialValue, desiredValue, vec.y) * YDMAX);

        cout << "Valor x: "<< initialPoint.x + (normalizeFloat(0, valuesSize, vec.x) * XDMAX) << " Valor Y: " << normalizeFloat(pidController->initialValue, pidController->desiredValue, vec.y) * YDMAX << endl;
    }
     
    glEnd();
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

void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

    RenderBackGround();

    RenderSimulatedValues(controllerPtr);

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    PIDController pidController;
    controllerPtr = &pidController;
    pidController.SimulateTillDesired();

    //Inicialização e abrir janela
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(XDMAX, YDMAX);
    glutCreateWindow("OpenGL_PID_Demo");

    //Callbacks
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeyEvent);
    glutReshapeFunc(ChangeSize);

    //Entrar no loop principal do Glut
    glutMainLoop();

    return 1;
}


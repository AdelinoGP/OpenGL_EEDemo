#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <corecrt_math.h>
#include <corecrt_math_defines.h>
#include <iostream>
#include <sstream>
#include <list>
#include <functional>

using namespace std;

#define XDMAX 1000
#define YDMAX 660

#define DEBUG_MODE false

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
    Color(float red, float green, float blue) {
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->alpha = 1;
    }
};

const Color white = Color(1, 1, 1);

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
    float zoomValue = 0;

    float cumulative_error = 0;
    float previous_error = 0;

    float Kp = 1.5;
    float Ki = 0.05;
    float Kd = 0.1;

    list<Vector2> currentSimulatedValues;

    list<Vector2> SimulateTillDesired() {
        if(DEBUG_MODE) cout << "Valor Inicial: "<< initialValue << " Valor desejado: "<< desiredValue << endl;

        cumulative_error = 0;
        previous_error = 0;
        currentTick = 0;
        currentValue = initialValue;

        list<Vector2> simulatedValues = { Vector2(currentTick,currentValue) };

        int ticksWhereEqual = 0;
        while (ticksWhereEqual < 10 && currentTick < 10000) {
            SimulateTick();
            simulatedValues.emplace_back(Vector2(currentTick, currentValue));
            float difference = desiredValue - currentValue;
            if (difference > -0.005 && difference < 0.005)
                ticksWhereEqual++;
        }

        if (currentTick == 10000)
            cout << "Numero maximo de ticks excedido" << endl;

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

        if (DEBUG_MODE) cout << "Valor PID"<< pid_value  << "Valor Atual:" << currentValue << endl;
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

void KeyEvent(unsigned char tecla, int xmouse, int ymouse) {
    if (tecla == 'q' || tecla == 'Q') controllerPtr->desiredValue++;

    if (tecla == 'w' || tecla == 'W') controllerPtr->desiredValue--;

    if (tecla == 'a' || tecla == 'A') controllerPtr->initialValue++;

    if (tecla == 's' || tecla == 'S') controllerPtr->initialValue--;

    if (tecla == 'r' || tecla == 'R') controllerPtr->Kp+= 0.05;

    if (tecla == 't' || tecla == 'T') controllerPtr->Kp-= 0.05;

    if (tecla == 'f' || tecla == 'F') controllerPtr->Ki+= 0.025;

    if (tecla == 'g' || tecla == 'G') controllerPtr->Ki-= 0.025;

    if (tecla == 'v' || tecla == 'V') controllerPtr->Kd+= 0.025;

    if (tecla == 'b' || tecla == 'B') controllerPtr->Kd-= 0.025;

    controllerPtr->SimulateTillDesired();

    glutPostRedisplay();
}
void MouseEvent(int button, int state, int x, int y)
{
    if (state == GLUT_UP) return;

    float minValue = controllerPtr->initialValue + controllerPtr->zoomValue;
    float maxValue = controllerPtr->desiredValue * 2 - controllerPtr->zoomValue;

            
    if (button == 3) { // Scroll Up
        if (maxValue - minValue < 2) return;
        controllerPtr->zoomValue += 0.5;
    }
    if (button == 4) { // Scroll Down
        controllerPtr->zoomValue -= 0.5;
    }
    glutPostRedisplay();
}

void RenderString(float x, float y, void* font, const char* string, Color const& rgb)
{
    glColor4f(rgb.red, rgb.green, rgb.blue, rgb.alpha);
    glRasterPos2f(x, y);
    const unsigned char* str = reinterpret_cast<const unsigned char*>(string);
    glutBitmapString(font, str);
}

void RenderBackGround() {
    glColor3f(0.3, 0.3, 0.3);
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

void RenderSimulatedValues(PIDController* pidController) {
    glColor3f(0, 0.5f, 1);
    glLineWidth(4);

    Vector2 initialPoint = Vector2((XDMAX / 3) + 11, 0);
    glBegin(GL_LINE_STRIP);
    int valuesSize = pidController->currentSimulatedValues.size();
    float initialValue = pidController->initialValue;
    float desiredValue = pidController->desiredValue * 2;

    for (Vector2 vec : pidController->currentSimulatedValues) {
        glVertex2f(initialPoint.x + (normalizeFloat(0, valuesSize, vec.x) * XDMAX), normalizeFloat(initialValue, desiredValue, vec.y) * YDMAX);

        if (DEBUG_MODE) cout << "Valor x: " << initialPoint.x + (normalizeFloat(0, valuesSize, vec.x) * XDMAX) << " Valor Y: " << normalizeFloat(pidController->initialValue, pidController->desiredValue, vec.y) * YDMAX << endl;
    }

    glEnd();

}

void RenderControlElement(Vector2 position, const char* titleString, const char* valueString) {
    RenderString(position.x, position.y, GLUT_BITMAP_TIMES_ROMAN_24, titleString, white);
    RenderString(position.x, position.y - 30, GLUT_BITMAP_TIMES_ROMAN_24, valueString, white);
}

void RenderControls(PIDController* pidController) {
    Vector2 elementPosition = Vector2(30, YDMAX - 30);
    ostringstream ss;
    ss.setf(ios::fixed);

    ss << setprecision(2) << "Q+  " << pidController->desiredValue << "  -W";
    RenderControlElement(elementPosition, "Valor Desejado:", ss.str().c_str());
    elementPosition.y -= 60;
    ss.str("");
    ss.clear();

    ss << setprecision(2) << "A+  " << pidController->initialValue << "  -S";
    RenderControlElement(elementPosition, "Valor Inicial:", ss.str().c_str());
    elementPosition.y -= 60;
    ss.str("");
    ss.clear();

    ss << setprecision(2) << "R+  " << pidController->Kp << "  -T";
    RenderControlElement(elementPosition, "Fator Proporcional:", ss.str().c_str());
    elementPosition.y -= 60;
    ss.str("");
    ss.clear();

    ss << setprecision(3) << "F+  " << pidController->Ki << "  -G";
    RenderControlElement(elementPosition, "Fator Integral:", ss.str().c_str());
    elementPosition.y -= 60;
    ss.str("");
    ss.clear();

    ss << setprecision(3) << "V+  " << pidController->Kd << "  -B";
    RenderControlElement(elementPosition, "Fator Derivativo:", ss.str().c_str());
    elementPosition.y -= 60;
    ss.str("");
    ss.clear();
}

void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

    RenderBackGround();

    RenderControls(controllerPtr);

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
    glutKeyboardFunc(KeyEvent);
    glutReshapeFunc(ChangeSize);
    glutMouseFunc(MouseEvent);

    //Entrar no loop principal do Glut
    glutMainLoop();

    return 1;
}


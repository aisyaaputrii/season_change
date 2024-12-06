#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>

enum Seasons { SUMMER, RAIN, AUTUMN, WINTER, SPRING };
Seasons currentSeason = SPRING;
std::vector<std::pair<float, float>> particles;

void initParticles() {
    particles.clear();
    for (int i = 0; i < 1000; i++) {
        particles.push_back({static_cast<float>(rand() % 200) / 100.0f - 1.0f,
                             static_cast<float>(rand() % 200) / 100.0f});
    }
}

void drawParticles() {
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (auto& particle : particles) {
        if (currentSeason == RAIN) glColor3f(0.0f, 0.0f, 1.0f);
        else if (currentSeason == WINTER) glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(particle.first, particle.second, -0.5f);
    }
    glEnd();

    for (auto& particle : particles) {
        particle.second -= 0.02f;
        if (particle.second < -1.0f) {
            particle.second = 1.0f;
            particle.first = static_cast<float>(rand() % 200) / 100.0f - 1.0f;
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    switch (currentSeason) {
        case SUMMER: glClearColor(0.5f, 0.8f, 1.0f, 1.0f); break;
        case RAIN: glClearColor(0.3f, 0.3f, 0.5f, 1.0f); break;
        case AUTUMN: glClearColor(0.9f, 0.6f, 0.3f, 1.0f); break;
        case WINTER: glClearColor(0.8f, 0.9f, 1.0f, 1.0f); break;
        case SPRING: glClearColor(0.5f, 1.0f, 0.5f, 1.0f); break;
    }

    if (currentSeason == RAIN || currentSeason == WINTER) drawParticles();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 's': currentSeason = SUMMER; break;
        case 'r': currentSeason = RAIN; initParticles(); break;
        case 'a': currentSeason = AUTUMN; break;
        case 'w': currentSeason = WINTER; initParticles(); break;
        case 'p': currentSeason = SPRING; break;
    }
    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    srand(static_cast<unsigned>(time(0)));
    initParticles();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Season Change with Rain");

    glewInit();
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}

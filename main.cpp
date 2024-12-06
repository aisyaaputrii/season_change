#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <string>

enum Seasons { SUMMER, RAIN, AUTUMN, WINTER, SPRING };
Seasons currentSeason = SPRING;
std::vector<std::pair<float, float>> particles;
GLuint autumnTexture, winterTexture, springTexture, summerTexture;

// Initialize particles
void initParticles() {
    particles.clear();
    for (int i = 0; i < 1000; i++) {
        particles.push_back({static_cast<float>(rand() % 200) / 100.0f - 1.0f,
                             static_cast<float>(rand() % 200) / 100.0f});
    }
}

// Load texture from file
GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Loaded texture: " << filename << std::endl;
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

// Draw particles
void drawParticles() {
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (auto& particle : particles) {
        if (currentSeason == RAIN) glColor3f(0.0f, 0.0f, 1.0f);  // Blue rain
        else if (currentSeason == WINTER) glColor3f(1.0f, 1.0f, 1.0f);  // White snow
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

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLuint texture;
    switch (currentSeason) {
        case AUTUMN: texture = autumnTexture; break;
        case WINTER: texture = winterTexture; break;
        case SPRING: texture = springTexture; break;
        case SUMMER: texture = summerTexture; break;
        default: texture = 0;
    }

    // Bind and draw the background texture
    if (texture != 0) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -0.9f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -0.9f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -0.9f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -0.9f);
        glEnd();
    }

    if (currentSeason == RAIN || currentSeason == WINTER) {
        drawParticles();
    }

    glutSwapBuffers();
}

// Keyboard input handler
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 's': currentSeason = SUMMER; break;
        case 'r': currentSeason = RAIN; initParticles(); break;
        case 'a': currentSeason = AUTUMN; break;
        case 'w': currentSeason = WINTER; initParticles(); break;
        case 'p': currentSeason = SPRING; break;
        default: return;
    }
    glutPostRedisplay();
}

// Update window title based on current season
void updateTitle() {
    const char* titles[] = {"Summer", "Rain", "Autumn", "Winter", "Spring"};
    std::string title = "Season Change - " + std::string(titles[currentSeason]);
    glutSetWindowTitle(title.c_str());
}

// Initialization function
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    srand(static_cast<unsigned>(time(0)));
    initParticles();

    autumnTexture = loadTexture("autumn_sky.jpg");
    winterTexture = loadTexture("winter_sky.jpg");
    springTexture = loadTexture("spring_sky.jpg");
    summerTexture = loadTexture("summer_sky.jpg");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

// Timer function
void timer(int value) {
    updateTitle();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Season Change");

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}


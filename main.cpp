#include <GL/glew.h>
#include <GL/freeglut.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>

enum Seasons { SUMMER, RAIN, AUTUMN, WINTER, SPRING };
Seasons currentSeason = SPRING;

// Texture management class
class TextureManager {
private:
    std::vector<GLuint> seasonTextures;
    int width, height, channels;

public:
    TextureManager() {
        seasonTextures.resize(5, 0);
    }

    bool loadTexture(Seasons season, const std::string& filename) {
        // Flip image vertically for OpenGL compatibility
        stbi_set_flip_vertically_on_load(true);

        // Load image
        unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &channels, 0);
        if (!imageData) {
            std::cerr << "Failed to load texture: " << filename 
                      << " (Error: " << stbi_failure_reason() << ")" << std::endl;
            return false;
        }

        // Generate OpenGL texture
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Determine format based on channels
        GLenum format = GL_RGB;
        if (channels == 4) format = GL_RGBA;
        else if (channels == 1) format = GL_LUMINANCE;

        // Upload texture data
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);

        // Free image data
        stbi_image_free(imageData);

        // Store texture
        seasonTextures[season] = textureID;
        return true;
    }

    void drawBackground(Seasons season) {
        if (seasonTextures[season] == 0) return;

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, seasonTextures[season]);

        glColor3f(1.0f, 1.0f, 1.0f);  // White to preserve texture colors
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
};

// Particle class
class Particle {
public:
    float x, y;
    float speed;
    float color[3];
    float size;

    Particle(Seasons season) {
        reset(season);
    }

    void reset(Seasons season) {
        // Randomize starting position more broadly
        x = static_cast<float>(rand() % 200) / 100.0f - 1.0f;
        y = 1.5f + static_cast<float>(rand() % 100) / 100.0f;  // Start higher
        size = 3.0f + static_cast<float>(rand() % 3);  // Increased size variation

        switch (season) {
            case WINTER: // Snowflakes
                color[0] = color[1] = color[2] = 0.9f + static_cast<float>(rand() % 10) / 100.0f;
                speed = 0.005f + static_cast<float>(rand() % 10) / 1000.0f;
                break;
            case RAIN: // Raindrops
                color[0] = 0.5f + static_cast<float>(rand() % 50) / 100.0f;
                color[1] = 0.7f + static_cast<float>(rand() % 30) / 100.0f;
                color[2] = 1.0f;
                speed = 0.005f + static_cast<float>(rand() % 15) / 1000.0f;
                break;
            case AUTUMN: // Falling leaves
                color[0] = 0.8f + static_cast<float>(rand() % 20) / 100.0f; // Varied orange/brown
                color[1] = 0.4f + static_cast<float>(rand() % 20) / 100.0f;
                color[2] = 0.1f + static_cast<float>(rand() % 10) / 100.0f;
                speed = 0.005f + static_cast<float>(rand() % 10) / 1000.0f;
                break;
            case SPRING: // Cherry blossom petals
                color[0] = 1.0f;
                color[1] = 0.7f + static_cast<float>(rand() % 30) / 100.0f; // Varied pink
                color[2] = 0.8f + static_cast<float>(rand() % 20) / 100.0f;
                speed = 0.005f + static_cast<float>(rand() % 10) / 1000.0f;
                break;
            default:
                color[0] = color[1] = color[2] = 1.0f;
                speed = 0.01f;
                break;
        }
    }
};

// Global variables
std::vector<Particle> particles;
TextureManager textureManager;

// Function to initialize particles
void initParticles(Seasons season) {
    particles.clear();
    for (int i = 0; i < 500; i++) {  // Increased particle count
        particles.emplace_back(season);
    }
}

// Function to draw particles
void drawParticles() {
    glDisable(GL_DEPTH_TEST); 
    glPointSize(5.0f);  // Ensure points are visible
    glBegin(GL_POINTS);
    for (auto& particle : particles) {
        glColor3f(particle.color[0], particle.color[1], particle.color[2]);
        glVertex2f(particle.x, particle.y);  // Use 2D coordinates
    }
    glEnd();

    // Update particle positions
    for (auto& particle : particles) {
        particle.y -= particle.speed;

        // Add slight horizontal movement for some seasons
        switch(currentSeason) {
            case AUTUMN:
                particle.x += sin(particle.y * 10) * 0.005f;
                break;
            case SPRING:
                particle.x += cos(particle.y * 10) * 0.005f;
                break;
            default:
                break;
        }

        // More forgiving reset condition
        if (particle.y < -1.5f) {  // Extended reset range
            particle.reset(currentSeason);
        }
    }
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Draw background texture
    textureManager.drawBackground(currentSeason);

    // Draw particles for current season
    drawParticles();

    glutSwapBuffers();
}

// Keyboard input handler
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 's': 
            currentSeason = SUMMER; 
            break;
        case 'r': 
            currentSeason = RAIN; 
            initParticles(RAIN);
            break;
        case 'a': 
            initParticles(AUTUMN);
            currentSeason = AUTUMN; 
            break;
        case 'w': 
            currentSeason = WINTER; 
            initParticles(WINTER);
            break;
        case 'p': 
            currentSeason = SPRING; 
            initParticles(SPRING);
            break;
    }
    glutPostRedisplay();
}

// Initialization function
void init() {
    glEnable(GL_DEPTH_TEST);
    srand(static_cast<unsigned>(time(0)));

    // Load background textures
    // IMPORTANT: Replace these paths with actual paths to your image files
    if (!textureManager.loadTexture(SUMMER, "/Users/aisyahputri/Documents/season_change/data/summer_sky.jpg")) {
        std::cerr << "Failed to load summer background" << std::endl;
    }
    if (!textureManager.loadTexture(RAIN, "/Users/aisyahputri/Documents/season_change/data/rain_sky.jpg")) {
        std::cerr << "Failed to load rainy background" << std::endl;
    }
    if (!textureManager.loadTexture(AUTUMN, "/Users/aisyahputri/Documents/season_change/data/autumn_sky.jpg")) {
        std::cerr << "Failed to load autumn background" << std::endl;
    }
    if (!textureManager.loadTexture(WINTER, "/Users/aisyahputri/Documents/season_change/data/winter_sky.jpg")) {
        std::cerr << "Failed to load winter background" << std::endl;
    }
    if (!textureManager.loadTexture(SPRING, "/Users/aisyahputri/Documents/season_change/data/spring_sky.jpg")) {
        std::cerr << "Failed to load spring background" << std::endl;
    }

    // Initial particle setup
    initParticles(currentSeason);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

// Timer function for continuous rendering
void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Seasonal Graphics");

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Setup OpenGL
    init();

    // Register callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
//Autor: Milos Milutinovic RA 129/2020

#define _CRT_SECURE_NO_WARNINGS

#define CRES 100
#define DRONE_SIZE 0.03
#define ENEMY_SIZE 0.03
#define X_POSITION_BASE -0.45
#define Y_POSITION_BASE -0.3
#define DRONE_STATUS_SIZE 0.08
#define BASE_SIZE 0.05
#define TARGET_SIZE 0
#define X_TARGET 0
#define Y_TARGET 0.2
#define ENEMY_SPEED 0.017
#define MAX_ENEMY_NUMBER 5
#define MAX_DIVISION_NUMBER 7
#define DRONE_MOVEMENT_SPEED 0.001

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include <list>

struct Position
{
    float x;
    float y;
};
struct Enemy {
    float circle[CRES * 2 + 4];
    unsigned int shader;
    unsigned int uPosLoc;
    unsigned int VAO;
    unsigned int VBO;
    Position offset;
    double spawnTime;

    bool operator==(const Enemy& x) const {
        return shader == x.shader && VAO == x.VAO && VBO == x.VBO && offset.x == x.offset.x && offset.y == x.offset.y;
    }
};
unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);
bool checkCollision(float x1, float y1, float r1, float x2, float y2, float r2);
unsigned int initDrone(float* Drone, unsigned int* VAO, unsigned int* VBO, float aspectRatio, int arraySize);
unsigned int initDroneNumberStatus(unsigned int* VAO, unsigned int* VBO);
Enemy initEnemy(float aspectRatio);
unsigned int initBackground(unsigned int* VAO, unsigned int* VBO, unsigned int unifiedShader);
unsigned int initGlass(unsigned int* VAO, unsigned int* VBO);
unsigned int initCommandTable(unsigned int* VAO, unsigned int* VBO, unsigned int unifiedShader);
unsigned int initTarget(unsigned int* VAO, unsigned int* VBO, unsigned int unifiedShader);
void initDroneStatus(float* droneStatus, unsigned int* VAO, unsigned int* VBO, float aspectRatio, int arraySize);
void initBase(float* base, unsigned int* VAO, unsigned int* VBO, float aspectRatio, int arraySize);
void getEnemyDirection(Enemy* enemy, float deltaTime);
Position generateRandomPositions();
void drawEnemy(Enemy* enemy, double elapsedTime);

int main(void)
{
    if (!glfwInit()) {
        std::cout << "GLFW Library failed to initialize! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 950;
    unsigned int wHeight = 850;
    const char wTitle[] = "PVO Novi Sad";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    if (window == NULL) {
        std::cout << "Window creation failed! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW Library failed to load! :'(\n";
        return 3;
    }
    srand(static_cast<unsigned int>(time(0)));

    unsigned int textureShader = createShader("basic.vert", "basic.frag");
    unsigned int textureShader1 = createShader("basic.vert", "basic.frag");
    unsigned int textureShader2 = createShader("basic.vert", "basic.frag");
    unsigned int textureShader3 = createShader("basic.vert", "basic.frag");
    unsigned int circleShader = createShader("circle.vert", "circle.frag");
    unsigned int pvoShader = createShader("circle.vert", "circle.frag");
    unsigned int VAO[8];
    glGenVertexArrays(8, VAO);
    unsigned int VBO[8];
    glGenBuffers(8, VBO);
    float aspectRatio = static_cast<float>(wWidth) / wHeight;
    int enemyCount = MAX_ENEMY_NUMBER;
    std::list<Enemy> Enemies;

    //-------------------------------------Definicije za teksturu-------------------------------------
    bool displayTexture = true;
    unsigned int checkerTexture = initBackground(VAO, VBO, textureShader);

    //-------------------------------------Definicije za poluprovidni prozor preko mape-------------------------------------
    unsigned int glassShader = initGlass(VAO, VBO);

    //-------------------------------------Definicije za indikator kada je aktivan dron-------------------------------------
    float droneStatus[CRES * 2 + 4];
    initDroneStatus(droneStatus, VAO, VBO, aspectRatio, sizeof(droneStatus));

    //-------------------------------------Definicije za PVO bazu-------------------------------------
    float base[CRES * 2 + 4]; // +4 je za x i y koordinate centra kruga, i za x i y od nultog ugla
    initBase(base, VAO, VBO, aspectRatio, sizeof(base));

    //-------------------------------------Definicije za helikopter-------------------------------------
    int enemySpawnCount = 0;
    Enemy firstEnemy = initEnemy(aspectRatio);
    Enemies.push_back(firstEnemy);
    enemySpawnCount++;

    //-------------------------------------Definicije za podeoke koliko dronova postoji-------------------------------------
    unsigned int divisionShader = initDroneNumberStatus(VAO, VBO);

    //-------------------------------------Definicije za dron-------------------------------------
    float Drone[CRES * 2 + 4];
    float xDrone;
    float yDrone;
    unsigned int dronShader = initDrone(Drone, VAO, VBO, aspectRatio,sizeof(Drone));
    unsigned int uPosLocDrone = glGetUniformLocation(dronShader, "uPos");

    //-------------------------------------Definicije za komandnu tablu-------------------------------------
    unsigned int checkerTexture1 = initCommandTable(VAO, VBO, textureShader1);

    //-------------------------------------Definicije za metu-------------------------------------
    unsigned int checkerTexture2 = initTarget(VAO, VBO, textureShader2);

    //------------------------------------Finished init-----------------------------------------
    double lastTime = glfwGetTime();
    double helicopterSpawnTime = lastTime;
    firstEnemy.uPosLoc = glGetUniformLocation(firstEnemy.shader, "uPos");
    int divisionNumber = MAX_DIVISION_NUMBER;
    bool spaceKeyPressed = false;
    bool activeDron = false;
    GLint colorLoc = glGetUniformLocation(circleShader, "customColor");
    bool gameOver = false;

    while (!glfwWindowShouldClose(window)) {

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            displayTexture = false;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            displayTexture = true;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spaceKeyPressed && divisionNumber > 0) {
            spaceKeyPressed = true;
            glUseProgram(circleShader);
            if (activeDron == false)
            {
                divisionNumber = divisionNumber - 1;
                xDrone = X_POSITION_BASE;
                yDrone = Y_POSITION_BASE;
            }
            activeDron = true;
            glUniform3f(colorLoc, 12.0 / 255, 188.0 / 255, 223.0 / 255);
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spaceKeyPressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            if (yDrone + Drone[1] <= 1.04)
                yDrone += DRONE_MOVEMENT_SPEED;
            else
                activeDron = false;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            if (yDrone + Drone[1] > -0.74)
                yDrone -= DRONE_MOVEMENT_SPEED;
            else
                activeDron = false;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            if (xDrone + Drone[0] <= 1.04)
                xDrone += DRONE_MOVEMENT_SPEED;
            else
                activeDron = false;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            if (xDrone + Drone[0] >= -1.04)
                xDrone -= DRONE_MOVEMENT_SPEED;
            else
                activeDron = false;
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            activeDron = false;
        }

        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        //---------crtanje teksture---------
        if (displayTexture) {
            glUseProgram(textureShader);
            glBindVertexArray(VAO[0]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, checkerTexture);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        //---------crtanje poluprovidnog stakla---------
        glUseProgram(glassShader);
        glBindVertexArray(VAO[1]);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


        //---------crtanje PVO baze---------
        glUseProgram(pvoShader);
        GLint colorPvo = glGetUniformLocation(pvoShader, "customColor");
        glUniform3f(colorPvo, 0.0, 1.0, 0.0);
        glBindVertexArray(VAO[3]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(base) / (2 * sizeof(float)));

        //---------crtanje helikoptera(neprijateljskih snaga)---------
        double elapsedTime = currentTime - helicopterSpawnTime;
        if (enemyCount > 0) {
            // Kolizija izmedju mete i helikoptera
            Enemies.remove_if([&gameOver](Enemy enemy) {
                bool output = checkCollision(enemy.circle[0] + enemy.offset.x,enemy.circle[1] + enemy.offset.y,ENEMY_SIZE,X_TARGET,Y_TARGET,TARGET_SIZE);
                if (output)
                    gameOver = true;
                return output;
                }); 
            // Kolizija izmedju drona i helikoptera
            Enemies.remove_if([xDrone, yDrone, &activeDron](Enemy enemy) {
                bool output = checkCollision(enemy.circle[0] + enemy.offset.x,enemy.circle[1] + enemy.offset.y,ENEMY_SIZE, xDrone, yDrone,DRONE_SIZE);
                if (output) {
                    activeDron = false;
                }
				return output;
                });
            
            for (Enemy& currentEnemy : Enemies) {
                getEnemyDirection(&currentEnemy, deltaTime);
                drawEnemy(&currentEnemy, currentTime - currentEnemy.spawnTime);
            }
        }

        if (elapsedTime > 2 && enemySpawnCount < MAX_ENEMY_NUMBER) {
            Enemy newEnemy = initEnemy(aspectRatio);
            Enemies.push_back(newEnemy);
            helicopterSpawnTime = currentTime;
            enemySpawnCount++;
        }

        //---------crtanje drona---------
        if (activeDron == true) {
            glUseProgram(dronShader);
            GLint colorLoc1 = glGetUniformLocation(dronShader, "uColor");
            glUniform4f(colorLoc1, 0.0, 0.0, 1.0, 1.0);
            glUniform2f(uPosLocDrone, xDrone, yDrone);
            glBindVertexArray(VAO[2]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(Drone) / (2 * sizeof(float)));
        }

        //----------crtanje komandne table-----------
        glUseProgram(textureShader1);
        glBindVertexArray(VAO[4]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, checkerTexture1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
        glBindTexture(GL_TEXTURE_2D, 0);

        //---------crtanje podeoka---------
        for (int i = 0; i < divisionNumber; ++i) {
            float offsetX = i * 0.1;
            glUseProgram(divisionShader);
            int offsetLocation = glGetUniformLocation(divisionShader, "uOffset");
            glUniform1f(offsetLocation, offsetX);
            glBindVertexArray(VAO[5]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }

        //---------crtanje indikatora---------
        if (activeDron == true) {
            glUseProgram(circleShader);
            glBindVertexArray(VAO[6]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(droneStatus) / (2 * sizeof(float)));
        }
        else {
            glUseProgram(circleShader);
            glBindVertexArray(VAO[6]);
            glUniform3f(colorLoc, 11.0 / 255, 12.0 / 255, 99.0 / 255);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(droneStatus) / (2 * sizeof(float)));
        }

        //----------crtanje mete-----------
        glUseProgram(textureShader2);
        glBindVertexArray(VAO[7]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, checkerTexture2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
        glBindTexture(GL_TEXTURE_2D, 0);

        if (gameOver == true) {
            break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        lastTime = currentTime;
    }

    glDeleteTextures(1, &checkerTexture);
    glDeleteTextures(1, &checkerTexture1);
    glDeleteTextures(1, &checkerTexture2);
    glDeleteBuffers(8, VBO);
    glDeleteVertexArrays(8, VAO);
    glDeleteProgram(textureShader);
    glDeleteProgram(textureShader1);
    glDeleteProgram(textureShader2);
    glDeleteProgram(circleShader);
    glDeleteProgram(pvoShader);
    glDeleteProgram(dronShader);
    glDeleteProgram(glassShader);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Shader program has an error! Error: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break; 
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

bool checkCollision(float x1, float y1, float r1, float x2, float y2, float r2) {
    float distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    float sumRadii = r1 + r2;
    
    return distance <= sumRadii;
}

unsigned int initDrone(float* Drone, unsigned int* VAO, unsigned int* VBO, float aspectRatio, int arraySize) {
    Drone[0] = 0;
    Drone[1] = 0;
    for (int i = 0; i <= CRES; i++)
    {
        Drone[2 + 2 * i] = Drone[0] + DRONE_SIZE * cos((3.141592 / 180) * (i * 360 / CRES)) / aspectRatio;
        Drone[2 + 2 * i + 1] = Drone[1] + DRONE_SIZE * sin((3.141592 / 180) * (i * 360 / CRES));
    }

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, arraySize, Drone, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    unsigned int dronShader = createShader("dron.vert", "dron.frag");
    return dronShader;
}
unsigned int initDroneNumberStatus( unsigned int* VAO, unsigned int* VBO) {
    float divisionVertices[] = {
        -0.8, -0.9,     167.0 / 255.0, 25.0 / 255.0, 75.0 / 255.0, 1.0,
        -0.75, -0.9,      167.0 / 255.0, 25.0 / 255.0, 75.0 / 255.0, 1.0,
        -0.8, -0.8,       167.0 / 255.0, 25.0 / 255.0, 75.0 / 255.0, 1.0,
        -0.75, -0.8,      167.0 / 255.0, 25.0 / 255.0, 75.0 / 255.0, 1.0,
    };
    unsigned int stride3 = (2 + 4) * sizeof(float);

    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(divisionVertices), divisionVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride3, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride3, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int divisionShader = createShader("division.vert", "division.frag");
    return divisionShader;
}

Enemy initEnemy(float aspectRatio) {
    float r2 = ENEMY_SIZE;
    Enemy output{};
    Position position = generateRandomPositions();
    output.circle[0] = position.x;
    output.circle[1] = position.y;
    for (int i = 0; i <= CRES; i++)
    {
        output.circle[2 + 2 * i] = output.circle[0] + r2 * cos((3.141592 / 180) * (i * 360 / CRES)) / aspectRatio; //Xi
        output.circle[2 + 2 * i + 1] = output.circle[1] + r2 * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    }
    output.offset.x = 0;
    output.offset.y = 0;
    output.spawnTime = glfwGetTime();

    glGenVertexArrays(1, &output.VAO);
    glGenBuffers(1, &output.VBO);
    glBindVertexArray(output.VAO);
    glBindBuffer(GL_ARRAY_BUFFER,output.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(output.circle), output.circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    output.shader = createShader("helicopter.vert", "helicopter.frag");

    return output;
}

unsigned int initBackground(unsigned int* VAO,unsigned int* VBO,unsigned int unifiedShader) {
    float vertices[] = {
        -1.0, -0.7,     0.0, 0.0,
        1.0, -0.7,      1.0, 0.0,
        -1.0, 1.0,      0.0, 1.0,
        1.0, 1.0,       1.0, 1.0,
    };
    unsigned int stride = (2 + 2) * sizeof(float);

    unsigned int indices[] =
    {
        0, 1, 2,
        1, 2, 3,
    };

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int checkerTexture = loadImageToTexture("res/mapa.png");
    glBindTexture(GL_TEXTURE_2D, checkerTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc, 0);

    return checkerTexture;
}

unsigned int initCommandTable(unsigned int* VAO, unsigned int* VBO, unsigned int unifiedShader) {
    float vertices[] = {
        -1.0, -1.0,     0.0, 0.0,
        1.0, -1.0,      1.0, 0.0,
        -1.0, -0.7,      0.0, 1.0,
        1.0, -0.7,       1.0, 1.0,
    };
    unsigned int stride = (2 + 2) * sizeof(float);

    unsigned int indices[] =
    {
        0, 1, 2,
        1, 2, 3,
    };

    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int checkerTexture = loadImageToTexture("res/commandTable.png");
    glBindTexture(GL_TEXTURE_2D, checkerTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc, 0);

    return checkerTexture;
}

unsigned int initTarget(unsigned int* VAO, unsigned int* VBO, unsigned int unifiedShader) {
    float vertices[] = {
        -0.1, 0.1,     0.0, 0.0,
        0.1, 0.1,      1.0, 0.0,
        -0.1, 0.3,      0.0, 1.0,
        0.1, 0.3,       1.0, 1.0,
    };
    unsigned int stride = (2 + 2) * sizeof(float);

    unsigned int indices[] =
    {
        0, 1, 2,
        1, 2, 3,
    };

    glBindVertexArray(VAO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int checkerTexture = loadImageToTexture("res/target.png");
    glBindTexture(GL_TEXTURE_2D, checkerTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc, 0);

    return checkerTexture;
}

unsigned int initGlass(unsigned int* VAO, unsigned int* VBO) {
    float glassVertices[] = {
           -1.0, -0.7,     0.0, 0.0, 0.0, 0.3,
           1.0, -0.7,      0.0, 0.0, 0.0, 0.3,
           -1.0, 1.0,      0.0, 0.0, 0.0, 0.3,
           1.0, 1.0,       0.0, 0.0, 0.0, 0.3
    };
    unsigned int stride1 = (2 + 4) * sizeof(float);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glassVertices), glassVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride1, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride1, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int glassShader = createShader("glass.vert", "glass.frag");
    return glassShader;
}
void initDroneStatus(float* droneStatus,unsigned int* VAO, unsigned int* VBO, float aspectRatio, int arraySize) {
    droneStatus[0] = 0.3;
    droneStatus[1] = -0.85;
    for (int i = 0; i <= CRES; i++)
    {

        droneStatus[2 + 2 * i] = droneStatus[0] + DRONE_STATUS_SIZE * cos((3.141592 / 180) * (i * 360 / CRES)) / aspectRatio;
        droneStatus[2 + 2 * i + 1] = droneStatus[1] + DRONE_STATUS_SIZE * sin((3.141592 / 180) * (i * 360 / CRES));
    }

    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, arraySize, droneStatus, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

}
 void initBase(float* base, unsigned int* VAO, unsigned int* VBO, float aspectRatio, int arraySize) {

    base[0] = X_POSITION_BASE;
    base[1] = Y_POSITION_BASE;
    for (int i = 0; i <= CRES; i++)
    {

        base[2 + 2 * i] = base[0] + BASE_SIZE * cos((3.141592 / 180) * (i * 360 / CRES)) / aspectRatio;
        base[2 + 2 * i + 1] = base[1] + BASE_SIZE * sin((3.141592 / 180) * (i * 360 / CRES));
    }

    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, arraySize, base, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
 Position generateRandomPositions() {
    srand(static_cast<unsigned int>(time(NULL)));
    int random = rand() % 4 + 1;    //random broj koji koristim da odredim sa koje ivice krece helihopter
    float random1 = static_cast<float>(rand() % 1000) / 1000;
    Position position{};
    switch (random) {
    case 1:             //ivice posmatram od gornje i u smeru kazaljke na satu rastu brojevi od 1 do 4 (1 ivica gore, 2 ivica desno...)
        position.x = random1;
        position.y = 1.0;
        break;
    case 2:
        position.x = 1.0;
        position.y = random1;
        break;
    case 3:
        position.x = random1;
        position.y = -0.7;
        break;
    case 4:
        position.x = -1.0;
        position.y = -0.7 + random1;
        break;
    default:
        break;
    }
    return position;

}
 void getEnemyDirection(Enemy* enemy, float deltaTime) {
     float directionX = X_TARGET - enemy->circle[0];
     float directionY = Y_TARGET - enemy->circle[1];
     enemy->offset.x += (directionX * ENEMY_SPEED * deltaTime * 10);
     enemy->offset.y += (directionY * ENEMY_SPEED * deltaTime * 10);
 }
 void drawEnemy(Enemy* enemy,double elapsedTime) {
     glUseProgram(enemy->shader);
     float blinkValue = abs(sin(3 * (0.01 + abs(enemy->offset.x) + abs(enemy->offset.y)) * elapsedTime));
     enemy->uPosLoc = glGetUniformLocation(enemy->shader, "uPos");
     int blinkLoc = glGetUniformLocation(enemy->shader, "blink");
     glBindVertexArray(enemy->VAO);
     glUniform1f(blinkLoc, blinkValue);
     glUniform2f(enemy->uPosLoc, enemy->offset.x, enemy->offset.y);
     glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(enemy->circle) / (2 * sizeof(float)));
 }
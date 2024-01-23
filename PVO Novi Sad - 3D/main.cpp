//Opis: Primjer ucitavanja modela upotrebom ASSIMP biblioteke
//Preuzeto sa learnOpenGL

#define MAX_DIVISION_NUMBER 10
#define CRES 100
#define _CRT_SECURE_NO_WARNINGS
#define DRONE_STATUS_SIZE 0.08

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "model.hpp"

const unsigned int wWidth = 1000;
const unsigned int wHeight = 1000;

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Enemy {
    glm::vec3 position;
    glm::vec3 targetCenter = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 direction;
    bool isDrawn;
    int count;
    float blinkValue;
};

//glm::vec3 cameraPos = glm::vec3(8.0f, 8.0f, 17.0f);
glm::vec3 cameraPos = glm::vec3(-6.0f, 10.0f, 13.0f);
//glm::vec3 cameraPos = glm::vec3(-6.0f, 7.0f, 7.0f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f , 0.0f) - cameraPos);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 lightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);
float pointLightVisibility = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool isDrawDrone = false;
bool activeDrone = false;
int divisionNumber;
bool spaceKeyPressed = false;
int enemyNumber = 5;

glm::vec3 dronePosition = glm::vec3(-85.0f, 90.0f, 165.0f);

//bool firstMouse = true;
//float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
//float pitch = 0.0f;
//float lastX = 400.0f / 2.0;
//float lastY = 300.0 / 2.0;
//float fov = 45.0f;
float droneScale = 30.0f;

unsigned int loadTexture(char const* path);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
glm::vec2 calculate_minimap_position(glm::vec3 position3d);

void initializeRandomSeed() {
    std::srand(static_cast<unsigned int>(std::time(0)));
}

glm::vec3 generateRandomPositionOnBorder(float maxX, float maxZ) {
    bool onXBorder = std::rand() % 2 == 0;

    float randomX;
    float randomZ;
    float randomY;

    if (onXBorder) {
        randomX = (std::rand() % 2 == 0) ? maxX : -maxX;
        randomZ = static_cast<float>(std::rand()) / RAND_MAX * maxZ;
        randomY = static_cast<float>(std::rand()) / RAND_MAX * 3.0f;
    }
    else {
        randomX = static_cast<float>(std::rand()) / RAND_MAX * maxX;
        randomZ = (std::rand() % 2 == 0) ? maxZ : -maxZ;
        randomY = static_cast<float>(std::rand()) / RAND_MAX * 3.0f;
    }

    return glm::vec3(randomX, randomY, randomZ);
}

bool checkCollision(const glm::vec3& position1, const glm::vec3& position2, float radius1, float radius2) {
    float distanceSquared = glm::dot(position1 - position2, position1 - position2);

    return distanceSquared <= (radius1 * radius2);
}

int main()
{
    initializeRandomSeed();
    if(!glfwInit())
    {
        std::cout << "GLFW fail!\n" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Window fail!\n" << std::endl;
        glfwTerminate();
        return -2;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float aspectRatio = static_cast<float>(wWidth) / wHeight;
    if (glewInit() !=GLEW_OK)
    {
        std::cout << "GLEW fail! :(\n" << std::endl;
        return -3;
    }

    unsigned int VAO[9];
    glGenVertexArrays(9, VAO);
    unsigned int VBO[9];
    glGenBuffers(9, VBO);

    //------------------------------ POD ------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
         1.0f, 0.0f, -1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
        -1.0f, 0.0f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
        -1.0f, 0.0f,  1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,

        -1.0f, 0.0f,  1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
         1.0f, 0.0f,  1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
         1.0f, 0.0f, -1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
    };
    unsigned int stride = (3 + 3 + 2) * sizeof(float);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int diffuseMap = loadTexture("res/mapa.png");
    unsigned int specularMap = loadTexture("res/mapa_specular.png");

    //-------------------------------------- OBLACI ------------------------------------
    float verticesOblaci[] = {
        // positions          // normals           // texture coords
         1.0f, 0.1f, -1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
        -1.0f, 0.1f, -1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
        -1.0f, 0.1f,  1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
        
        -1.0f, 0.1f,  1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
         1.0f, 0.1f,  1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
         1.0f, 0.1f, -1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
    };

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesOblaci), verticesOblaci, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int oblaci = loadTexture("res/clouds.png");

    //-------------------------------------- IME I PREZIME ------------------------------------
    float verticesIme[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, //top right
        -1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, //bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, //top left
         1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f  //bottom right
    };
    unsigned int indicesIme[] = {
        0, 1, 3,
        0, 2, 1
    };
    unsigned int VBOime, VAOime, EBOime;
    glGenVertexArrays(1, &VAOime);
    glGenBuffers(1, &VBOime);
    glGenBuffers(1, &EBOime);

    glBindVertexArray(VAOime);

    glBindBuffer(GL_ARRAY_BUFFER, VBOime);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesIme), verticesIme, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOime);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesIme), indicesIme, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    unsigned int ime = loadTexture("res/ime.png");

    //-------------------------------------- MAPA ------------------------------------
    float verticesMapa[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, //top right
        -1.0f, -0.7f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, //bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, //top left
         1.0f, -0.7f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f  //bottom right
    };
    unsigned int indicesMapa[] = {
        0, 1, 3,
        0, 2, 1
    };
    unsigned int EBOmapa;
    glGenBuffers(1, &EBOmapa);

    glBindVertexArray(VAO[2]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesMapa), verticesMapa, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOmapa);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesMapa), indicesMapa, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //-------------------------------------- PODEOCI ------------------------------------
    float divisionVertices[] = {
        -0.8, -0.9,     255.0 / 255.0, 178.0 / 255.0, 25.0 / 255.0, 1.0,
        -0.75, -0.9,    255.0 / 255.0, 178.0 / 255.0, 25.0 / 255.0, 1.0,
        -0.8, -0.8,     255.0 / 255.0, 178.0 / 255.0, 25.0 / 255.0, 1.0,
        -0.75, -0.8,    255.0 / 255.0, 178.0 / 255.0, 25.0 / 255.0, 1.0,
    };
    unsigned int stride3 = (2 + 4) * sizeof(float);

    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(divisionVertices), divisionVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride3, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride3, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
     divisionNumber = MAX_DIVISION_NUMBER;

    //-------------------------------------- INDIKATOR ------------------------------------
    float droneStatus[CRES * 2 + 4];
    float rIndicator = 1;
    droneStatus[0] = 0.3;
    droneStatus[1] = -0.85;
    for (int i = 0; i <= CRES; i++)
    {

        droneStatus[2 + 2 * i] = rIndicator * cos((3.141592 / 180) * (i * 360 / CRES)); //Xi
        droneStatus[2 + 2 * i + 1] = rIndicator * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    }

    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(droneStatus), droneStatus, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    Model fruska("res/mountain.obj");
    Model drone("res/Spiderbot.obj");
    Model neprijatelj("res/Drone.obj");
    Model niskoletni("res/CYBER-hover.obj");

    Shader unifiedShader("basic.vert", "basic.frag");
    Shader textureShader("texture.vert", "texture.frag");
    Shader minimapShader("minimap.vert", "minimap.frag");
    Shader circleShader("circle.vert", "circle.frag");

    //Render petlja
    glm::mat4 projectionP = glm::perspective(glm::radians(90.0f), (float)wWidth / (float)wHeight, 0.0001f, 1000.0f);

    PointLight pointLight;
    pointLight.ambient = glm::vec3(0.0f, 0.7f, 0.0f);
    pointLight.diffuse = glm::vec3(0.0f, 0.7f, 0.0f);
    pointLight.specular = glm::vec3(0.0f, 0.7f, 0.0f);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.007f;
    pointLight.quadratic = 0.5f;

    const int numberOfEnemies = 5;

    Enemy enemies[numberOfEnemies];
    enemies[0].position = generateRandomPositionOnBorder(10.0f, 10.0f);
    enemies[0].direction = glm::normalize(enemies[0].targetCenter - enemies[0].position);
    enemies[0].isDrawn = false;
    enemies[0].blinkValue = 0.0f;
    enemies[1].position = generateRandomPositionOnBorder(10.0f, 10.0f);
    enemies[1].direction = glm::normalize(enemies[1].targetCenter - enemies[1].position);
    enemies[1].isDrawn = false;
    enemies[1].blinkValue = 0.0f;
    enemies[2].position = generateRandomPositionOnBorder(10.0f, 10.0f);
    enemies[2].direction = glm::normalize(enemies[2].targetCenter - enemies[2].position);
    enemies[2].isDrawn = false;
    enemies[2].blinkValue = 0.0f;
    enemies[3].position = generateRandomPositionOnBorder(10.0f, 10.0f);
    enemies[3].direction = glm::normalize(enemies[3].targetCenter - enemies[3].position);
    enemies[3].isDrawn = false;
    enemies[3].blinkValue = 0.0f;
    enemies[4].position = generateRandomPositionOnBorder(10.0f, 10.0f);
    enemies[4].direction = glm::normalize(enemies[4].targetCenter - enemies[4].position);
    enemies[4].isDrawn = false;
    enemies[4].blinkValue = 0.0f;
    enemies->count = 5;
    Enemy lowEnemies[numberOfEnemies];
    lowEnemies[0].position = generateRandomPositionOnBorder(10.0f, 10.0f);
    lowEnemies[0].position.y = 1.0;
    lowEnemies[0].direction = glm::normalize(lowEnemies[0].targetCenter - lowEnemies[0].position);
    lowEnemies[0].isDrawn = false;
    lowEnemies[1].position = generateRandomPositionOnBorder(10.0f, 10.0f);
    lowEnemies[1].position.y = 1.0;
    lowEnemies[1].direction = glm::normalize(lowEnemies[1].targetCenter - lowEnemies[1].position);
    lowEnemies[1].isDrawn = false;
    lowEnemies[2].position = generateRandomPositionOnBorder(10.0f, 10.0f);
    lowEnemies[2].position.y = 1.0;
    lowEnemies[2].direction = glm::normalize(lowEnemies[2].targetCenter - lowEnemies[2].position);
    lowEnemies[2].isDrawn = false;
    lowEnemies->count = 3;

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(window))
    {
        glViewport(0, 0, wWidth, wHeight);
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //------------------------------ PATOS ------------------------------
        unifiedShader.use();
        unifiedShader.setInt("material.diffuse", 0);
        unifiedShader.setInt("material.specular", 1);
        unifiedShader.setVec3("light.direction", lightDirection);
        unifiedShader.setVec3("viewPos", cameraPos);

        unifiedShader.setVec3("light.ambient", 0.7f, 0.7f, 0.7f);
        unifiedShader.setVec3("light.diffuse", 0.0f, 0.0f, 0.0f);
        unifiedShader.setVec3("light.specular", 0.4f, 0.4f, 0.4f);

        unifiedShader.setFloat("material.shininess", 32.0f);

        unifiedShader.setVec3("pointLight.position", dronePosition / droneScale);
        unifiedShader.setVec3("pointLight.ambient", pointLight.ambient);
        unifiedShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        unifiedShader.setVec3("pointLight.specular", pointLight.specular);
        unifiedShader.setFloat("pointLight.constant", pointLight.constant);
        unifiedShader.setFloat("pointLight.linear", pointLight.linear);
        unifiedShader.setFloat("pointLight.quadratic", pointLight.quadratic);
        unifiedShader.setFloat("pointLightVisibility", pointLightVisibility);
        unifiedShader.setFloat("transparency", 1.0f);

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        unifiedShader.setMat4("projection", projectionP);
        unifiedShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(10.0f, 0.0f, 10.0f));
        unifiedShader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //------------------------------ FRUSKA ------------------------------
         unifiedShader.use();
        glm::mat4 modelFruske = glm::mat4(1.0f);
        modelFruske = glm::scale(modelFruske, glm::vec3(0.18f, 0.02f, 0.1f));
        modelFruske = glm::translate(modelFruske, glm::vec3(-20.0, 0.5, 50.0));
        unifiedShader.setMat4("model", modelFruske);
        unifiedShader.setMat4("projection", projectionP);
        unifiedShader.setMat4("view", view);
        unifiedShader.setVec3("light.direction", lightDirection);
        unifiedShader.setVec3("viewPos", cameraPos);
        unifiedShader.setVec3("light.ambient", 0.5f, 0.5f, 0.5f);
        unifiedShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        unifiedShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        unifiedShader.setInt("material.diffuse", 0);
        unifiedShader.setInt("material.specular", 1);
        unifiedShader.setFloat("material.shininess", 64.0f);

        unifiedShader.setVec3("pointLight.position", dronePosition / droneScale);
        unifiedShader.setVec3("pointLight.ambient", pointLight.ambient);
        unifiedShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        unifiedShader.setVec3("pointLight.specular", pointLight.specular);
        unifiedShader.setFloat("pointLight.constant", pointLight.constant);
        unifiedShader.setFloat("pointLight.linear", pointLight.linear);
        unifiedShader.setFloat("pointLight.quadratic", pointLight.quadratic);
        unifiedShader.setFloat("pointLightVisibility", pointLightVisibility);
        unifiedShader.setFloat("transparency", 1.0f);

        fruska.Draw(unifiedShader);

        //------------------------------ DRON ------------------------------
        if (isDrawDrone)
        {
            pointLightVisibility = 1.0f;
            unifiedShader.use();
            glm::mat4 modelDrona = glm::mat4(1.0f);
            modelDrona = glm::scale(modelDrona, glm::vec3(1.0f / droneScale, 1.0f / droneScale, 1.0f / droneScale));
            modelDrona = glm::translate(modelDrona, dronePosition);
            unifiedShader.setMat4("model", modelDrona);
            unifiedShader.setMat4("projection", projectionP);
            unifiedShader.setMat4("view", view);
            unifiedShader.setVec3("light.direction", lightDirection);
            unifiedShader.setVec3("viewPos", cameraPos);
            unifiedShader.setVec3("light.ambient", 0.0f, 1.0f, 0.0f);
            unifiedShader.setVec3("light.diffuse", 0.0f, 1.0f, 0.0f);
            unifiedShader.setVec3("light.specular", 0.0f, 1.0f, 0.0f);
            unifiedShader.setInt("material.diffuse", 0);
            unifiedShader.setInt("material.specular", 1);
            unifiedShader.setFloat("material.shininess", 32.0f);
            unifiedShader.setFloat("pointLightVisibility", pointLightVisibility);
            unifiedShader.setFloat("transparency", 1.0f);
            drone.Draw(unifiedShader);
        }

        //------------------------------ NEPRIJATELJSKE SNAGE ------------------------------
        for (int i = 0; i < 5; i++) {
            glm::mat4 modelNeprijatelja = glm::mat4(1.0f);
            modelNeprijatelja = glm::scale(modelNeprijatelja, glm::vec3(1.0f / droneScale, 1.0f / droneScale, 1.0f / droneScale));
            if (checkCollision(enemies[i].position, glm::vec3(0.0, enemies[i].position.y, 0.0), 1.0, 0.1)) {
                //glfwSetWindowShouldClose(window, true);
                enemies[i].isDrawn = false;
            }
            if (enemies[i].isDrawn) {
                enemies[i].position.x += enemies[i].direction.x * deltaTime;
                enemies[i].position.z += enemies[i].direction.z * deltaTime;
            }
            if (currentFrame != 0.0 && (int)std::floor(currentFrame) % 3 == 0 && currentFrame <= 15.1) {
                    enemies[0 + (int)(std::floor(currentFrame) / 3 - 1)].isDrawn = true;
            }
            modelNeprijatelja = glm::translate(modelNeprijatelja, enemies[i].position * droneScale);
            unifiedShader.setMat4("model", modelNeprijatelja);

            unifiedShader.setMat4("projection", projectionP);
            unifiedShader.setMat4("view", view);
            unifiedShader.setVec3("light.direction", lightDirection);
            unifiedShader.setVec3("viewPos", cameraPos);
            unifiedShader.setVec3("light.ambient", 1.0f, 0.0f, 0.0f);
            unifiedShader.setVec3("light.diffuse", 1.0f, 0.0f, 0.0f);
            unifiedShader.setVec3("light.specular", 1.0f, 0.0f, 0.0f);
            unifiedShader.setInt("material.diffuse", 0);
            unifiedShader.setInt("material.specular", 1);
            unifiedShader.setFloat("material.shininess", 32.0f);
            unifiedShader.setFloat("transparency", 1.0f);
            
            if (checkCollision(enemies[i].position , dronePosition / droneScale, 0.9, 0.9) && enemies[i].isDrawn == true)
            {
                enemies[i].isDrawn = false;
                isDrawDrone = false;
                pointLightVisibility = 0.0f;
            }

            if (enemies[i].isDrawn == true) {
                neprijatelj.Draw(unifiedShader);
            }
        }

        //------------------------------ NISKOLETNI ------------------------------
        for (int i = 0; i < 3; i++) {
            glm::mat4 modelNiskoletnog = glm::mat4(1.0f);
            modelNiskoletnog = glm::scale(modelNiskoletnog, glm::vec3(0.7, 0.7, 0.7));
            if (checkCollision(lowEnemies[i].position, glm::vec3(0.0, lowEnemies[i].position.y, 0.0), 1.0, 0.1)) {
                //glfwSetWindowShouldClose(window, true);
                lowEnemies[i].isDrawn = false;
            }
            if (lowEnemies[i].isDrawn) {
                lowEnemies[i].position.x += lowEnemies[i].direction.x * (deltaTime / 3) ;
                lowEnemies[i].position.z += lowEnemies[i].direction.z * (deltaTime / 3);
            }
            if (currentFrame != 0.0 && (int)std::floor(currentFrame) % 3 == 0 && currentFrame <= 10.1) {
                lowEnemies[0 + (int)(std::floor(currentFrame) / 3) - 1].isDrawn = true;
            }
            modelNiskoletnog = glm::translate(modelNiskoletnog, lowEnemies[i].position * 1.0f / 0.7f);
            unifiedShader.setMat4("model", modelNiskoletnog);
            unifiedShader.setMat4("projection", projectionP);
            unifiedShader.setMat4("view", view);
            unifiedShader.setVec3("light.direction", lightDirection);
            unifiedShader.setVec3("viewPos", cameraPos);
            unifiedShader.setVec3("light.ambient", 0.0f, 0.0f, 1.0f);
            unifiedShader.setVec3("light.diffuse", 0.0f, 0.0f, 1.0f);
            unifiedShader.setVec3("light.specular", 0.0f, 0.0f, 1.0f);
            unifiedShader.setInt("material.diffuse", 0);
            unifiedShader.setInt("material.specular", 1);
            unifiedShader.setFloat("material.shininess", 32.0f);
            unifiedShader.setFloat("transparency", 1.0f);

            unifiedShader.setMat4("model", modelNiskoletnog);
            unifiedShader.setMat4("projection", projectionP);
            unifiedShader.setMat4("view", view);
            unifiedShader.setVec3("light.direction", lightDirection);
            unifiedShader.setVec3("viewPos", cameraPos);
            unifiedShader.setVec3("light.ambient", 0.0f, 0.0f, 1.0f);
            unifiedShader.setVec3("light.diffuse", 0.0f, 0.0f, 1.0f);
            unifiedShader.setVec3("light.specular", 0.0f, 0.0f, 1.0f);
            unifiedShader.setInt("material.diffuse", 0);
            unifiedShader.setInt("material.specular", 1);
            unifiedShader.setFloat("material.shininess", 32.0f);
            unifiedShader.setFloat("transparency", 1.0f);

            if (checkCollision(lowEnemies[i].position, dronePosition / droneScale, 0.8, 0.8) && lowEnemies[i].isDrawn == true)
            {
                lowEnemies[i].isDrawn = false;
                isDrawDrone = false;
            }

            if (lowEnemies[i].isDrawn == true) {
                niskoletni.Draw(unifiedShader);
            }
        }

        //------------------------------ OBLACI ------------------------------
        unifiedShader.use();
        unifiedShader.setInt("material.diffuse", 0);
        unifiedShader.setInt("material.specular", 1);
        unifiedShader.setVec3("light.direction", lightDirection);
        unifiedShader.setVec3("viewPos", cameraPos);

        unifiedShader.setVec3("light.ambient", 0.7f, 0.7f, 0.7f);
        unifiedShader.setVec3("light.diffuse", 0.0f, 0.0f, 0.0f);
        unifiedShader.setVec3("light.specular", 0.4f, 0.4f, 0.4f);

        unifiedShader.setFloat("material.shininess", 64.0f);

        if (isDrawDrone) {
            unifiedShader.setVec3("pointLights[0].position", dronePosition / droneScale);
            unifiedShader.setVec3("pointLights[0].ambient", pointLight.ambient);
            unifiedShader.setVec3("pointLights[0].diffuse", pointLight.diffuse);
            unifiedShader.setVec3("pointLights[0].specular", pointLight.specular);
            unifiedShader.setFloat("pointLights[0].constant", pointLight.constant);
            unifiedShader.setFloat("pointLights[0].linear", pointLight.linear);
            unifiedShader.setFloat("pointLights[0].quadratic", pointLight.quadratic);
            unifiedShader.setFloat("pointLightVisibility", pointLightVisibility);
            unifiedShader.setFloat("transparency", 0.3f);
        }
        else {
            unifiedShader.setVec3("pointLights[0].position", dronePosition.x / droneScale, 50, dronePosition.z / droneScale);
            unifiedShader.setVec3("pointLights[0].ambient", pointLight.ambient);
            unifiedShader.setVec3("pointLights[0].diffuse", pointLight.diffuse);
            unifiedShader.setVec3("pointLights[0].specular", pointLight.specular);
            unifiedShader.setFloat("pointLights[0].constant", pointLight.constant);
            unifiedShader.setFloat("pointLights[0].linear", pointLight.linear);
            unifiedShader.setFloat("pointLights[0].quadratic", pointLight.quadratic);
            unifiedShader.setFloat("pointLightVisibility", pointLightVisibility);
            unifiedShader.setFloat("transparency", 0.3f);
        }

        for (int i = 0; i < 5; i++) {
            if (enemies[i].isDrawn) {
                unifiedShader.setVec3("pointLights[" + std::to_string(i + 1) + "].position", enemies[i].position);
                float distance = glm::distance(enemies[i].position, glm::vec3(0.0, 0.0, 0.0));
                float blinkValue = abs(sin(10.0 / distance * 13));
                enemies[i].blinkValue = blinkValue;
            }
            if(!enemies[i].isDrawn) {
                unifiedShader.setVec3("pointLights[" + std::to_string(i + 1) + "].position", enemies[i].position.x, 50, enemies[i].position.z);
                enemies[i].blinkValue = 0.0;
            }
            unifiedShader.setVec3("pointLights[" + std::to_string(i + 1) + "].ambient", 1.0, 0.0, 0.0);
            unifiedShader.setVec3("pointLights[" + std::to_string(i + 1) + "].diffuse", 1.0, 0.0, 0.0);
            unifiedShader.setVec3("pointLights[" + std::to_string(i + 1) + "].specular", 1.0, 0.0, 0.0);
            unifiedShader.setFloat("pointLights[" + std::to_string(i + 1) + "].constant", pointLight.constant);
            unifiedShader.setFloat("pointLights[" + std::to_string(i + 1) + "].linear", pointLight.linear);
            unifiedShader.setFloat("pointLights[" + std::to_string(i + 1) + "].quadratic", pointLight.quadratic);
            unifiedShader.setFloat("pointLightVisibility", 1.0);
            unifiedShader.setFloat("transparency", 0.3f);
            unifiedShader.setFloat("blink", enemies[i].blinkValue);
        }
        float radius = 10.0f;
        float angularSpeed = 0.5f;

        float xPos = radius * cos(angularSpeed * currentFrame);
        float yPos = 10.0f;
        float zPos = radius * sin(angularSpeed * currentFrame);

        glm::vec3 spotlightPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 targetPoint = glm::vec3(xPos, yPos, zPos);
        glm::vec3 spotlightDirection = glm::normalize(targetPoint - spotlightPosition);

        unifiedShader.setVec3("spotLight.position", spotlightPosition);
        unifiedShader.setVec3("spotLight.direction", spotlightDirection);
        unifiedShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        unifiedShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        unifiedShader.setVec3("spotLight.ambient", 1.0f,1.0f,1.0f);
        unifiedShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        unifiedShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        unifiedShader.setFloat("spotLight.constant", 1.0f);
        unifiedShader.setFloat("spotLight.linear", 0.09f);
        unifiedShader.setFloat("spotLight.quadratic", 0.032f);
        unifiedShader.setMat4("projection", projectionP);
        unifiedShader.setMat4("view", view);

        glm::mat4 modelOblaka = glm::mat4(1.0f);
        modelOblaka = glm::scale(modelOblaka, glm::vec3(10.0f, 0.1f, 10.0f));
        modelOblaka = glm::translate(modelOblaka, glm::vec3(0.0, 20.5, 0.0));
        unifiedShader.setMat4("model", modelOblaka);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, oblaci);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //------------------------------ IME I PREZIME ------------------------------
        glViewport(0, 950, 500, 50);
        glBindTexture(GL_TEXTURE_2D, ime);
        textureShader.use();
        glBindVertexArray(VAOime);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //------------------------------ MINIMAPA ------------------------------
        glViewport(650, 588, 350, 412);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        textureShader.use();
        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //------------------------------ PODEOCI ------------------------------
        minimapShader.use();
        for (int i = 0; i < divisionNumber; ++i) {
            float offsetX = i * 0.1;
            minimapShader.setFloat("uOffset", offsetX);
            glBindVertexArray(VAO[3]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }

        //------------------------------ INDIKATOR ------------------------------
        circleShader.use();
        if (isDrawDrone == true) {
            glBindVertexArray(VAO[4]);
            circleShader.setVec3("uPos", 0.3, -0.85, 0.1);
            circleShader.setVec4("uCrcCol", 12.0 / 255, 188.0 / 255, 223.0 / 255, 1.0);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(droneStatus) / (2 * sizeof(float)));
        }
        else {
            glBindVertexArray(VAO[4]);
            circleShader.setVec3("uPos", 0.3, -0.85, 0.1);
            circleShader.setVec4("uCrcCol", 11.0 / 255, 12.0 / 255, 99.0 / 255, 1.0);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(droneStatus) / (2 * sizeof(float)));
        }

        glViewport(650, 650, 350, 350);
        //------------------------------ NEPRIJATELJI NA MINIMAPI ------------------------------
        circleShader.use();
        float minOriginal = 0.0f;
        float maxOriginal = 3.0f;
        float minScaled = 0.06f;
        float maxScaled = 0.09f;
        for (int i = 0; i < enemyNumber; i++) {
            float scaledValue = minScaled + ((enemies[i].position.y - minOriginal) / (maxOriginal - minOriginal)) * (maxScaled - minScaled);
            if (enemies[i].isDrawn == true) {
                glBindVertexArray(VAO[4]);
                glm::vec2 position = calculate_minimap_position(enemies[i].position);
                circleShader.setVec3("uPos", position.x, position.y, scaledValue);
                circleShader.setVec4("uCrcCol", 1.0, 0.0 + enemies[i].blinkValue, 0.0 + enemies[i].blinkValue, 1.0);
                glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(droneStatus) / (2 * sizeof(float)));
            }
        }
        //------------------------------ NISKOLETNI NEPRIJATELJI NA MINIMAPI ------------------------------
        circleShader.use();
        for (int i = 0; i < 3; i++) {
            float scaledValue = minScaled + ((lowEnemies[i].position.y - minOriginal) / (maxOriginal - minOriginal)) * (maxScaled - minScaled);
            if (lowEnemies[i].isDrawn == true) {
                glBindVertexArray(VAO[4]);
                glm::vec2 position = calculate_minimap_position(lowEnemies[i].position);
                circleShader.setVec3("uPos", position.x, position.y, scaledValue);
                circleShader.setVec4("uCrcCol", 0.0, 0.0 , 1.0, 1.0);
                glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(droneStatus) / (2 * sizeof(float)));
            }
        }
        //------------------------------ NISKOLETNI NEPRIJATELJI NA MINIMAPI ------------------------------
        circleShader.use();
        float scaledValue = minScaled + ((dronePosition.y / droneScale - minOriginal) / (maxOriginal - minOriginal)) * (maxScaled - minScaled);
        if (isDrawDrone == true) {
            glBindVertexArray(VAO[4]);
            glm::vec2 position = calculate_minimap_position(dronePosition / droneScale);
            circleShader.setVec3("uPos", position.x, position.y, scaledValue);
            circleShader.setVec4("uCrcCol", 0.0, 1.0, 0.0, 1.0);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(droneStatus) / (2 * sizeof(float)));
        }
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteBuffers(9, VBO);
    glDeleteBuffers(1, &VBOime);
    glDeleteBuffers(1, &EBOime);
    glDeleteBuffers(1, &EBOmapa);
    glDeleteVertexArrays(9, VAO);
    glDeleteVertexArrays(1, &VAOime);

    glfwTerminate();
    return 0;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spaceKeyPressed && divisionNumber > 0) {
        spaceKeyPressed = true;
        if (isDrawDrone == false)
        {
            dronePosition = glm::vec3(-85.0f, 90.0f, 165.0f);
            divisionNumber = divisionNumber - 1;
        }
        isDrawDrone = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        spaceKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        isDrawDrone = false;
        pointLightVisibility = 0.0f;
    }

    float cameraSpeed = static_cast<float>(3 * deltaTime);
    if ((dronePosition.x > 1.0 || dronePosition.x < -1.0) && (dronePosition.z > 1.0 || dronePosition.z < -1.0)) {

    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        //cameraPos += cameraSpeed * cameraFront;
        if (dronePosition.z / droneScale > -10.0f)
            dronePosition.z -= cameraSpeed * droneScale;
        else
            isDrawDrone = false;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        //cameraPos -= cameraSpeed * cameraFront;
        if (dronePosition.z / droneScale < 10.0f)
            dronePosition.z += cameraSpeed * droneScale;
        else
            isDrawDrone = false;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        //cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * 3.0f;
        if (dronePosition.x / droneScale > -10.0f)
            dronePosition.x -= cameraSpeed * droneScale;
        else
            isDrawDrone = false;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        //cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * 3.0f;
        if (dronePosition.x / droneScale < 10.0f)
            dronePosition.x += cameraSpeed * droneScale;
        else
            isDrawDrone = false;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        //cameraPos.y += cameraSpeed;
        dronePosition.y += cameraSpeed * droneScale;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        //if (dronePosition.y > 0.15) {
            //cameraPos.y -= cameraSpeed;
            dronePosition.y -= cameraSpeed * droneScale;
        //}
            if (checkCollision(dronePosition, glm::vec3(dronePosition.x, -1.0, dronePosition.z), 1.0, 1.0)) {
                isDrawDrone = false;
                activeDrone = false;
                dronePosition.y = 50.0;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

glm::vec2 calculate_minimap_position(glm::vec3 position3d) {
    glm::vec2 result;
    if(position3d.z < 0.0)
        result = glm::vec2(position3d.x / 10.0f, -position3d.z / 10.0f);
    else
        result = glm::vec2(position3d.x / 10.0f, -position3d.z / 10.0f);
    return result;
}

//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//    float xpos = static_cast<float>(xposIn);
//    float ypos = static_cast<float>(0);
//
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//    lastX = xpos;
//    lastY = ypos;
//
//    float sensitivity = 0.1f; // change this value to your liking
//    xoffset *= sensitivity;
//    yoffset *= sensitivity;
//
//    yaw += xoffset;
//    pitch += yoffset;
//
//    // make sure that when pitch is out of bounds, screen doesn't get flipped
//    if (pitch > 89.0f)
//        pitch = 89.0f;
//    if (pitch < -89.0f)
//        pitch = -89.0f;
//
//    glm::vec3 front;
//    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//    front.y = sin(glm::radians(pitch));
//    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//    cameraFront = glm::normalize(front);
//}
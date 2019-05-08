#include <iostream>
#include <fstream>
#include "glad/glad.h"
#include "OpenGLGraphicsManager.hpp"
#include "AssetLoader.hpp"
#include "IApplication.hpp"

const char VS_SHADER_SOURCE_FILE[] = "Shaders/color.vs";
const char PS_SHADER_SOURCE_FILE[] = "Shaders/color.ps";

using namespace Panda;

extern struct gladGLversionStruct GLVersion;

namespace Panda
{
    extern AssetLoader* g_pAssetLoader;

    static void OutputShaderErrorMessage(unsigned int shaderId, const char* shaderFilename)
    {
        int logSize, i;
        char* infoLog;
        std::ofstream fout;

        // Get the size of the string containing the information log for the failed shader compilation message.
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);

        // Increment the size by one to handle also the null terminator.
        logSize++;

        // Create a char buffer to hold the info log.
        infoLog = new char[logSize];
        if (!infoLog)
        {
            return;
        }

        // Now retrieve the info log.
        glGetShaderInfoLog(shaderId, logSize, NULL, infoLog);

        // Open a file to write the error message to.
        fout.open("shader-error.txt");

        // Write out the error message;
        for (i = 0; i < logSize; ++i)
        {
            fout << infoLog[i];
        }

        // Close the file.
        fout.close();

        // Pop a message up on the screen to notify the user to check the text file for compile errors.
        std::cerr << "Error compiling shader. Check shader-error.txt for message." << shaderFilename << std::endl;

        return;
    }

    static void OutputLinkerErrorMessage(unsigned int programId)
    {
        int logSize, i;
        char* infoLog;
        std::ofstream fout;

        // Get the size of the string containing the information log for the failed shader compilation message.
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);

        // Increment the size by one to handle also the null terminator.
        logSize++;

        // Create a char buffer to hold the info log.
        infoLog = new char[logSize];
        if (!infoLog)
        {
            return;
        }

        // Now retrieve the info log.
        glGetProgramInfoLog(programId, logSize, NULL, infoLog);

        // Open a file to write the error message to.
        fout.open("linker-error.txt");

        // Write out the error message.
        for (i = 0; i < logSize; ++i)
        {
            fout << infoLog[i];
        }

        // Close the file.
        fout.close();

        // Pop a message up on the screen to notify the user to check the text file for linker errors.
        std::cerr << "Error compiling linker. Check linker-error.txt for message." << std::endl;
    }

    int OpenGLGraphicsManager::Initialize()
    {
        int result;

        result = gladLoadGL();
        if (!result) {
            std::cerr << "OpenGL load failed!" << std::endl;
            result = -1;
        } else {
            result = 0;
            std::cout << "OpenGL Version " << GLVersion.major << "." << GLVersion.minor << " loaded" << std::endl;

            if (GLAD_GL_VERSION_3_0) {
                // Set the depth buffer to be entirely cleared to 1.0 values.
                glClearDepth(1.0f);

                // Enable depth testing.
                glEnable(GL_DEPTH_TEST);

                // Set the polygon winding to front facing for the left handed system.
                glFrontFace(GL_CW);

                // Enable back face culling.
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);

                // Initialize the world/model matrix to the identity matrix.
                m_WorldMatrix.SetIdentity();

                // Set the field of view and screen aspect ratio.
                float fieldOfView = PI / 4.0f;
                const GfxConfiguration& conf = g_pApp->GetConfiguration();

                float screenAspect = (float)conf.screenWidth / (float)conf.screenHeight;

                // Build the perspective projection matrix.
                BuildPerspectiveFovLHMatrix(m_ProjectionMatrix, fieldOfView, screenAspect, k_ScreenNear, k_ScreenFar);
            }

            InitializeShader(VS_SHADER_SOURCE_FILE, PS_SHADER_SOURCE_FILE);
            InitializeBuffers();
        }

        return result;
    }

    void OpenGLGraphicsManager::Finalize()
    {
        // Disable the two vertex array attributes.
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Release the vertex buffer.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &m_VertexBufferId);

        // Release the index buffer.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &m_IndexBufferId);

        // Release the vertex array object.
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &m_VertexArrayId);

        // Detach the vertex and fragment shaders from the program.
        glDetachShader(m_ShaderProgram, m_VertexShader);
        glDetachShader(m_ShaderProgram, m_FragmentShader);

        // Delete the vertex and fragment shaders.
        glDeleteShader(m_VertexShader);
        glDeleteShader(m_FragmentShader);

        // Delete the shader program.
        glDeleteProgram(m_ShaderProgram);
    }

    void OpenGLGraphicsManager::Tick()
    {
    }

    void OpenGLGraphicsManager::Clear()
    {
        // Set the color to clear the screen to.
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        // Clear the screen and depth buffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLGraphicsManager::Draw()
    {
        static float rotateAngle = 0.f;

        // Update world matrix to rotate the model.
        rotateAngle += PI / 120;
        Matrix4f rotationMatrixY;
        Matrix4f rotationMatrixZ;
        MatrixRotationY(rotationMatrixY, rotateAngle);
        MatrixRotationZ(rotationMatrixZ, rotateAngle);
        m_WorldMatrix = m_WorldMatrix * rotationMatrixY * rotationMatrixZ;

        // Generate the view matrix based on the camera's position.
        CalculateCameraPosition();

        // Set the color shader as the current shader program and set the matrices that it will use for rendering.
        glUseProgram(m_ShaderProgram);
        SetShaderParameters(m_WorldMatrix, m_ViewMatrix, m_ProjectionMatrix);

        // Render the model using the color shader.
        RenderBuffers();

        glFlush();
    }

    bool OpenGLGraphicsManager::SetShaderParameters(const Matrix4f& worldMatrix, const Matrix4f& viewMatrix, const Matrix4f& projectionMatrix)
    {
        unsigned int location;

        // Set the world matrix in the vertex shader.
        location = glGetUniformLocation(m_ShaderProgram, "worldMatrix");
        if (location == -1)
        {
            return false;
        }
        glUniformMatrix4fv(location, 1, false, worldMatrix.GetAddressOf());

        // Set the view matrix in the vertex shader.
        location = glGetUniformLocation(m_ShaderProgram, "viewMatrix");
        if(location == -1)
        {
            return false;
        }
        glUniformMatrix4fv(location, 1, false, viewMatrix.GetAddressOf());


        // Set the projection matrix in the vertex shader.
        location = glGetUniformLocation(m_ShaderProgram, "projectionMatrix");
        if (location == -1)
        {
            return false;
        }
        glUniformMatrix4fv(location, 1, false, projectionMatrix.GetAddressOf());

        return true;
    }

    bool OpenGLGraphicsManager::InitializeBuffers()
    {
        struct VertexType
        {
            Vector3Df position;
            Vector3Df color;
        };

        VertexType vertices[] = {
                {{  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f }},
                {{  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }},
                {{ -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }},
                {{ -1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f }},
                {{  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f }},
                {{  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f }},
                {{ -1.0f, -1.0f, -1.0f }, { 0.5f, 1.0f, 0.5f }},
                {{ -1.0f, -1.0f,  1.0f }, { 1.0f, 0.5f, 1.0f }},
        };
        uint16_t indices[] = { 1, 2, 3, 3, 2, 6, 6, 7, 3, 3, 0, 1, 0, 3, 7, 7, 6, 4, 4, 6, 5, 0, 7, 4, 1, 0, 4, 1, 4, 5, 2, 1, 5, 2, 5, 6 };

        // Set the number of vertices in the vertex array.
        m_VertexCount = sizeof(vertices) / sizeof (VertexType);

        // Set the number of indices in the index array.
        m_IndexCount = sizeof(indices) / sizeof(uint16_t);

        // Allocate an OpenGL vertex array object.
        glGenVertexArrays(1, &m_VertexArrayId);

        // Bind the vertex array object to store all the buffers and vertex attributes we create here
        glBindVertexArray(m_VertexArrayId);

        // Generate an ID for the vertex buffer.
        glGenBuffers(1, &m_VertexBufferId);

        // Bind the vertex buffer and load the vertex (position and color) data into the vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, m_VertexCount * sizeof(VertexType), vertices, GL_STATIC_DRAW);

        // Enable the twe vertex array attributes.
        glEnableVertexAttribArray(0);   // Vertex position.
        glEnableVertexAttribArray(1);   // Vertex color.

        // Specify the location and format of the position portion of the vertex buffer.
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexType), 0);

        // Specify the location and format of the color portion of the vertex buffer.
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(VertexType), (char*)NULL + (3 * sizeof(float)));

        // Generate an ID for the index buffer.
        glGenBuffers(1, &m_IndexBufferId);

        // Bind the index buffer and load the index data into it.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(uint16_t), indices, GL_STATIC_DRAW);

        return true;
    }

    void OpenGLGraphicsManager::RenderBuffers()
    {
        // Bind the vertex array object that stored all the information about the vertex and index buffers.
        glBindVertexArray(m_VertexArrayId);

        // Render the vertex buffer using the index buffer.
        glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_SHORT, 0);

        return;
    }

    void OpenGLGraphicsManager::CalculateCameraPosition()
    {
        Vector3Df up, position, lookAt;
        float yaw, pitch, roll;
        Matrix4f rotationMatrix;

        // Setup the vector that points upwards.
        up.x = 0.f;
        up.y = 1.f;
        up.z = 0.f;

        // Setup the position of the camera in the world.
        position.x = m_PositionX;
        position.y = m_PositionY;
        position.z = m_PositionZ;

        // Setup where the camera is looking by default.
        lookAt.x = 0.0f;
        lookAt.y = 0.0f;
        lookAt.z = 1.0f;

        // Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
        pitch = m_PositionX * 0.0174532925f;
        yaw   = m_PositionY * 0.0174532925f;
        roll  = m_PositionZ * 0.0174532925f;

        // Create the rotation matrix from the yaw, pitch, and roll values.
        MatrixRotationYawPitchRoll(rotationMatrix, yaw, pitch, roll);

        // Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
        TransformCoord(lookAt, rotationMatrix);
        TransformCoord(up, rotationMatrix);

        // Translate the rotated camera position to the location of the viewer
        lookAt.x = position.x + lookAt.x;
        lookAt.y = position.y + lookAt.y;
        lookAt.z = position.z + lookAt.z;

        // Finally create the view matrix from the three updated vectors.
        BuildViewMatrix(position, lookAt, up, m_ViewMatrix);
    }

    bool OpenGLGraphicsManager::InitializeShader(const char* vsFilename, const char* fsFilename)
    {
        std::string vertexShaderBuffer;
        std::string fragmentShaderBuffer;
        int status;

        // Load the vertex shader source file into a text buffer.
        vertexShaderBuffer = g_pAssetLoader->SyncOpenAndReadFileToString(vsFilename);
        if(vertexShaderBuffer.empty())
        {
            return false;
        }

        // Load the fragment shader source file into a text buffer.
        fragmentShaderBuffer = g_pAssetLoader->SyncOpenAndReadFileToString(fsFilename);
        if (fragmentShaderBuffer.empty())
        {
            return false;
        }

        // Create a vertex and fragment shader object.
        m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
        m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // Copy the shader source code string into the vertex and fragment shader objects.
        const char* _v_c_str = vertexShaderBuffer.c_str();
        glShaderSource(m_VertexShader, 1, &_v_c_str, NULL);
        const char* _f_c_str = fragmentShaderBuffer.c_str();
        glShaderSource(m_FragmentShader, 1, &_f_c_str, NULL);

        // Compile the shaders.
        glCompileShader(m_VertexShader);
        glCompileShader(m_FragmentShader);

        // Check to see if the vertex shader compiled successfully.
        glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &status);
        if(status != 1)
        {
            // If it did not compile then write the syntax error message out to a text file for review.
            OutputShaderErrorMessage(m_VertexShader, vsFilename);
            return false;
        }

        // Create a shader program object.
        m_ShaderProgram = glCreateProgram();

        // Attach the vertex and fragment shader to the program object.
        glAttachShader(m_ShaderProgram, m_VertexShader);
        glAttachShader(m_ShaderProgram, m_FragmentShader);

        // Bind the shader input variables.
        glBindAttribLocation(m_ShaderProgram, 0, "inputPosition");
        glBindAttribLocation(m_ShaderProgram, 1, "inputColor");

        // Link the shader program.
        glLinkProgram(m_ShaderProgram);

        // Check the status of the link.
        glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &status);
        if (status != 1)
        {
            // if it did not link then write the syntax error message out to a text fire for review
            OutputLinkerErrorMessage(m_ShaderProgram);
            return false;
        }

        return true;
    }
}

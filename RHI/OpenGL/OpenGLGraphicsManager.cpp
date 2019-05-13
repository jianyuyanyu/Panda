#include <iostream>
#include <fstream>
#include "OpenGLGraphicsManager.hpp"
#include "AssetLoader.hpp"
#include "IApplication.hpp"
#include "Utility.hpp"
#include "SceneManager.hpp"

const char VS_SHADER_SOURCE_FILE[] = "Shaders/basic.vs";
const char PS_SHADER_SOURCE_FILE[] = "Shaders/basic.ps";

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
                BuildPerspectiveFovMatrix(m_ProjectionMatrix, fieldOfView, screenAspect, k_ScreenNear, k_ScreenFar, g_ViewHandness);
            }

            InitializeShader(VS_SHADER_SOURCE_FILE, PS_SHADER_SOURCE_FILE);
            InitializeBuffers();
        }

        return result;
    }

    void OpenGLGraphicsManager::Finalize()
    {
        for (auto i = 0; i < m_Buffers.size() - 1; ++i)
            glDisableVertexAttribArray(i);

        for (auto buf : m_Buffers)
        {
            if (buf.first == "index")
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            else
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glDeleteBuffers(1, &buf.second);
        }

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
        m_WorldMatrix = rotationMatrixY * rotationMatrixZ;

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
		Matrix4f world = worldMatrix;
		Matrix4f view = viewMatrix;
		Matrix4f projection = projectionMatrix;

        unsigned int location;

        // Set the world matrix in the vertex shader.
        location = glGetUniformLocation(m_ShaderProgram, "worldMatrix");
        if (location == -1)
        {
            return false;
        }
        glUniformMatrix4fv(location, 1, false, world.GetAddressOf());

        // Set the view matrix in the vertex shader.
        location = glGetUniformLocation(m_ShaderProgram, "viewMatrix");
        if(location == -1)
        {
            return false;
        }
        glUniformMatrix4fv(location, 1, false, view.GetAddressOf());


        // Set the projection matrix in the vertex shader.
        location = glGetUniformLocation(m_ShaderProgram, "projectionMatrix");
        if (location == -1)
        {
            return false;
        }
        glUniformMatrix4fv(location, 1, false, projection.GetAddressOf());

        return true;
    }

    void OpenGLGraphicsManager::InitializeBuffers()
    {
        auto& scene = g_pSceneManager->GetScene();
        auto pGeometry = scene.GetFirstGeometry();
        while (pGeometry)
        {
            auto pMesh = pGeometry->GetMesh().lock();
            if (!pMesh) return;

            // Set the number of vertex properties.
            auto vertexPropertiesCount = pMesh->GetVertexPropertiesCount();

            // Set the number of vertices in the vertex array.
            auto vertexCount = pMesh->GetVertexCount();

            // Allocate an OpenGL vertex array object.
            GLuint vao;
            glGenVertexArrays(1, &vao);

            // Bind the vertex array object to store all the buffers and vertex attributes we create here.
            glBindVertexArray(vao);

            GLuint buffer_id;
            for (size_t i = 0; i < vertexPropertiesCount; ++i)
            {
                const SceneObjectVertexArray& v_property_array = pMesh->GetVertexPropertyArray(i);
                auto v_property_array_data_size = v_property_array.GetDataSize();
                auto v_property_array_data = v_property_array.GetData();

                // Generated an ID for the vertex buffer
                glGenBuffers(1, &buffer_id);

                // Bind the vertex buffer and load the vertex (position and color) data into the vertex buffer
                glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
                glBufferData(GL_ARRAY_BUFFER, v_property_array_data_size, v_property_array_data, GL_STATIC_DRAW);

                glEnableVertexAttribArray(i);

                glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
                switch(v_property_array.GetDataType())
                {
                    case VertexDataType::kVertexDataTypeFloat1:
                        glVertexAttribPointer(i, 1, GL_FLOAT, false, 0, 0);
                        break;
                    case VertexDataType::kVertexDataTypeFloat2:
                        glVertexAttribPointer(i, 2, GL_FLOAT, false, 0, 0);
                        break;
                    case VertexDataType::kVertexDataTypeFloat3:
                        glVertexAttribPointer(i, 3, GL_FLOAT, false, 0, 0);
                        break;
                    case VertexDataType::kVertexDataTypeFloat4:
                        glVertexAttribPointer(i, 4, GL_FLOAT, false, 0, 0);
                        break;
                    case VertexDataType::kVertexDataTypeDouble1:
                        glVertexAttribPointer(i, 1, GL_DOUBLE, false, 0, 0);
                        break;
                    case VertexDataType::kVertexDataTypeDouble2:
                        glVertexAttribPointer(i, 2, GL_DOUBLE, false, 0, 0);
                        break;
                    case VertexDataType::kVertexDataTypeDouble3:
                        glVertexAttribPointer(i, 3, GL_DOUBLE, false, 0, 0);
                        break;
                    case VertexDataType::kVertexDataTypeDouble4:
                        glVertexAttribPointer(i, 4, GL_DOUBLE, false, 0, 0);
                        break;
                    default:
                        assert(0);
                        break;
                }

                m_Buffers[v_property_array.GetAttributeName()] = buffer_id;
            }

            // Generate an ID for the index buffer.
            glGenBuffers(1, &buffer_id);

            const SceneObjectIndexArray& index_array = pMesh->GetIndexArray(0);
            auto index_array_size = index_array.GetDataSize();
            auto index_array_data = index_array.GetData();

            // Bind the index buffer and load the index data into it.
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_array_size, index_array_data, GL_STATIC_DRAW);

            // Set the number of indices in the index array.
            GLsizei indexCount = static_cast<GLsizei>(index_array.GetIndexCount());
            GLenum mode;
            switch(pMesh->GetPrimitiveType())
            {
                case PrimitiveType::kPrimitiveTypePointList:
                    mode = GL_POINTS;
                    break;
                case PrimitiveType::kPrimitiveTypeLineList:
                    mode = GL_LINES;
                    break;
                case PrimitiveType::kPrimitiveTypeLineStrip:
                    mode = GL_LINE_STRIP;
                    break;
                case PrimitiveType::kPrimitiveTypeTriList:
                    mode = GL_TRIANGLES;
                    break;
                case PrimitiveType::kPrimitiveTypeTriStrip:
                    mode = GL_TRIANGLE_STRIP;
                    break;
                case PrimitiveType::kPrimitiveTypeTriFan:
                    mode = GL_TRIANGLE_FAN;
                    break;
                default:
                    // ignore
                    continue;
            }

            GLenum type;
            switch(index_array.GetIndexType())
            {
                case IndexDataType::kIndexDataTypeInt8:
                    type = GL_UNSIGNED_BYTE;
                    break;
                case IndexDataType::kIndexDataTypeInt16:
                    type = GL_UNSIGNED_SHORT;
                    break;
                case IndexDataType::kIndexDataTypeInt32:
                    type = GL_UNSIGNED_INT;
                    break;
                default:
                    // not supported by OpenGL
                    std::cerr << "Error: unsupported index type " << index_array << std::endl;
                    std::cerr << "Mesh: " << *pMesh << std::endl;
                    std::cerr << "Geometry: " << *pGeometry << std::endl;
                    continue;
            }

            m_Buffers["index"] = buffer_id;

            DrawBatchContext dbc;
            dbc.vao = vao;
            dbc.mode = mode;
            dbc.type = type;
            dbc.count = indexCount;
            m_VAO.push_back(std::move(dbc));

            pGeometry = scene.GetNextGeometry();
        }

        return;
    }

    void OpenGLGraphicsManager::RenderBuffers()
    {
        for (auto dbc : m_VAO)
        {
            glBindVertexArray(dbc.vao);

            // Render the vertex buffer using the index buffer
            glDrawElements(dbc.mode, dbc.count, dbc.type, 0);
        }

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
        BuildViewMatrix(m_ViewMatrix, position, lookAt, up, g_ViewHandness);
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
		glBindAttribLocation(m_ShaderProgram, 1, "inputNormal");

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

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

                // Set the polygon winding to front facing for the right handed system.
                glFrontFace(GL_CCW);

                // Enable back face culling.
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);

                // Initialize the world/model matrix to the identity matrix.
                m_DrawFrameContext.WorldMatrix.SetIdentity();
            }

            InitializeShader(VS_SHADER_SOURCE_FILE, PS_SHADER_SOURCE_FILE);
            InitializeBuffers();
        }

        return result;
    }

    void OpenGLGraphicsManager::Finalize()
    {
        for (auto dbc : m_DrawBatchContext)
        {
            glDeleteVertexArrays(1, &dbc.vao);
        }

        m_DrawBatchContext.clear();
        for (auto i = 0; i < m_Buffers.size() - 1; ++i)
            glDisableVertexAttribArray(i);

        for (auto buf : m_Buffers)
        {
            glDeleteBuffers(1, &buf);
        }

        m_Buffers.clear();

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
        // Render the model using the color shader.
        RenderBuffers();

        glFlush();
    }

    bool OpenGLGraphicsManager::SetPerFrameShaderParameters()
    {
        unsigned int location;

        // Set the world matrix in the vertex shader.
        location = glGetUniformLocation(m_ShaderProgram, "worldMatrix");
        if(location == -1)
            return false;
        glUniformMatrix4fv(location, 1, false, m_DrawFrameContext.WorldMatrix.GetAddressOf());

        // Set the view matrix in the vertex shader.
        location = glGetUniformLocation(m_ShaderProgram, "viewMatrix");
        if (location == -1)
            return false;
        glUniformMatrix4fv(location, 1, false, m_DrawFrameContext.ViewMatrix.GetAddressOf());

        // Set the projection matrix in the vertex shader.
        location = glGetUniformLocation(m_ShaderProgram, "projectionMatrix");
        if (location == -1)
            return false;
        glUniformMatrix4fv(location, 1, false, m_DrawFrameContext.ProjectionMatrix.GetAddressOf());

        // Set lighting parameters for PS shader
        location = glGetUniformLocation(m_ShaderProgram, "lightPosition");
        if (location == -1)
            return false;
        glUniform3fv(location, 1, m_DrawFrameContext.LightPosition.GetAddressOf());

        location = glGetUniformLocation(m_ShaderProgram, "lightColor");
        if (location == -1)
            return false;
        glUniform4fv(location, 1, m_DrawFrameContext.LightColor.GetAddressOf());
    }

    bool OpenGLGraphicsManager::SetPerBatchShaderParameters(const char* paramName, float* param)
    {
        unsigned int location;

        location = glGetUniformLocation(m_ShaderProgram, paramName);
        if (location == -1)
            return false;
        glUniformMatrix4fv(location, 1, false, param);

        return true;
    }
    // bool OpenGLGraphicsManager::SetShaderParameters(const Matrix4f& worldMatrix, const Matrix4f& viewMatrix, const Matrix4f& projectionMatrix)
    // {
	// 	Matrix4f world = worldMatrix;
	// 	Matrix4f view = viewMatrix;
	// 	Matrix4f projection = projectionMatrix;

    //     unsigned int location;

    //     // Set the world matrix in the vertex shader.
    //     location = glGetUniformLocation(m_ShaderProgram, "worldMatrix");
    //     if (location == -1)
    //     {
    //         return false;
    //     }
    //     glUniformMatrix4fv(location, 1, false, world.GetAddressOf());

    //     // Set the view matrix in the vertex shader.
    //     location = glGetUniformLocation(m_ShaderProgram, "viewMatrix");
    //     if(location == -1)
    //     {
    //         return false;
    //     }
    //     glUniformMatrix4fv(location, 1, false, view.GetAddressOf());


    //     // Set the projection matrix in the vertex shader.
    //     location = glGetUniformLocation(m_ShaderProgram, "projectionMatrix");
    //     if (location == -1)
    //     {
    //         return false;
    //     }
    //     glUniformMatrix4fv(location, 1, false, projection.GetAddressOf());

    //     return true;
    // }

    void OpenGLGraphicsManager::InitializeBuffers()
    {
        auto& scene = g_pSceneManager->GetScene();
        auto pGeometryNode = scene.GetFirstGeometryNode();
        while (pGeometryNode)
        {
            auto pGeometry = scene.GetGeometry(pGeometryNode->GetSceneObjectRef());
            assert(pGeometry);
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

                m_Buffers.push_back(buffer_id);
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

            m_Buffers.push_back(buffer_id);

            DrawBatchContext dbc;
            dbc.vao = vao;
            dbc.mode = mode;
            dbc.type = type;
            dbc.count = indexCount;
            dbc.transform = pGeometryNode->GetCalclulatedTrasform();
            m_DrawBatchContext.push_back(std::move(dbc));

            pGeometryNode = scene.GetNextGeometryNode();
        }

        return;
    }

    void OpenGLGraphicsManager::RenderBuffers()
    {
        static float rotateAngle = 0.0f;

        // Update world matrix to rotate the model
        rotateAngle += PI / 120;
        Matrix4f rotationMatrixY;
        Matrix4f rotationMatrixZ;
        MatrixRotationZ(rotationMatrixZ, rotateAngle);
        m_DrawFrameContext.WorldMatrix = rotationMatrixZ;

        // Generate the view matrix based on the camera's position
        CalculateCameraMatrix();
        CalculateLights();

        SetPerFrameShaderParameters();

        for (auto dbc : m_DrawBatchContext)
        {
            // Set the color shader as the current shader program and set the matrices that it will use for rendering.
            glUseProgram(m_ShaderProgram);
            SetPerBatchShaderParameters("objectLocalMatrix", (*dbc.transform).GetAddressOf());

            glBindVertexArray(dbc.vao);

            // Render the vertex buffer using the index buffer
            glDrawElements(dbc.mode, dbc.count, dbc.type, 0);
        }

        return;
    }

    void OpenGLGraphicsManager::CalculateCameraMatrix()
    {
        auto& scene = g_pSceneManager->GetScene();
        auto pCameraNode = scene.GetFirstCameraNode();
        if (pCameraNode)
        {
            m_DrawFrameContext.ViewMatrix = *pCameraNode->GetCalclulatedTrasform();
            m_DrawFrameContext.ViewMatrix.SetInverse();
        }
        else
        {
            // use default build-in camera
            Vector3Df position = {0, 0, 5}, lookAt = {0, 0, 0}, up = {0, 1, 0};
            BuildViewMatrix(m_DrawFrameContext.ViewMatrix, position, lookAt, up, g_ViewHandness);
        }
        
        auto pCamera = scene.GetCamera(pCameraNode->GetSceneObjectRef());

        // Set the field of view and scene aspect ratio
        float fieldOfView = std::dynamic_pointer_cast<SceneObjectPerspectiveCamera>(pCamera)->GetFov();
        const GfxConfiguration& conf = g_pApp->GetConfiguration();

        float screenAspect = (float)conf.screenWidth / (float)conf.screenHeight;

        // Build the perspective projection matrix.
        BuildPerspectiveFovMatrix(m_DrawFrameContext.ProjectionMatrix, fieldOfView, screenAspect, pCamera->GetNearClipDistance(), pCamera->GetFarClipDistance());
    }

    void OpenGLGraphicsManager::CalculateLights()
    {
        auto& scene = g_pSceneManager->GetScene();
        auto pLightNode = scene.GetFirstLightNode();
        if (pLightNode)
        {
            m_DrawFrameContext.LightPosition = {0.0f, 0.0f, 0.0f};
            TransformCoord(m_DrawFrameContext.LightPosition, *pLightNode->GetCalclulatedTrasform());

            auto pLight = scene.GetLight(pLightNode->GetSceneObjectRef());
            if (pLight)
            {
                m_DrawFrameContext.LightColor = pLight->GetColor().Value;
            }
        }
        else
        {
            // use default build-in light
            m_DrawFrameContext.LightPosition = {10.0f, 10.0f, -10.0f};
            m_DrawFrameContext.LightColor = {1.0f, 1.0f, 1.0f, 1.0f};
        }
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

#include <iostream>
#include <fstream>
#include "OpenGLGraphicsManager.hpp"
#include "AssetLoader.hpp"
#include "IApplication.hpp"
#include "Utility.hpp"
#include "SceneManager.hpp"
#include "IPhysicsManager.hpp"

using namespace Panda;

extern struct gladGLversionStruct GLVersion;

const char VS_SHADER_SOURCE_FILE[] = "Shaders/basic_vs.glsl";
const char PS_SHADER_SOURCE_FILE[] = "Shaders/basic_ps.glsl";
#ifdef DEBUG
const char DEBUG_VS_SHADER_SOURCE_FILE[] = "Shaders/debug_vs.glsl";
const char DEBUG_PS_SHADER_SOURCE_FILE[] = "Shaders/debug_ps.glsl";
#endif

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

        result = GraphicsManager::Initialize();

        if (result)
            return result;

        result = gladLoadGL();
        if (!result) {
            std::cerr << "OpenGL load failed!" << std::endl;
            result = -1; 
        } else {
            result = 0;
            std::cout << "OpenGL Version " << GLVersion.major << "." << GLVersion.minor << " loaded" << std::endl;

            if (GLAD_GL_VERSION_3_3) {
                // Set the depth buffer to be entirely cleared to 1.0 values.
                glClearDepth(1.0f);

                // Enable depth testing.
                glEnable(GL_DEPTH_TEST);

                // Set the polygon winding to front facing for the right handed system.
                glFrontFace(GL_CCW);

                // Enable back face culling.
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }
        }

        return result;
    }

    void OpenGLGraphicsManager::Finalize()
    {
        // for (auto dbc : m_DrawBatchContext)
        // {
        //     glDeleteVertexArrays(1, &dbc.vao);
        // }

        // m_DrawBatchContext.clear();

        // for (auto buf : m_Buffers)
        // {
        //     glDeleteBuffers(1, &buf);
        // }

        // for(auto texture : m_Textures)
        // {
        //     glDeleteTextures(1, &texture);
        // }

        // m_Buffers.clear();
        // m_Textures.clear();
        ClearBuffers();

        if (m_ShaderProgram)
        {
            if (m_VertexShader)
            {
                glDetachShader(m_ShaderProgram, m_VertexShader);
                glDeleteShader(m_VertexShader);
            }

            if (m_FragmentShader)
            {
                glDetachShader(m_ShaderProgram, m_FragmentShader);
                glDeleteShader(m_FragmentShader);
            }

            // Delete the shader program.
            glDeleteProgram(m_ShaderProgram);
        }

        GraphicsManager::Finalize();
    }

    void OpenGLGraphicsManager::Clear()
    {
        GraphicsManager::Clear();

        // Set the color to clear the screen to.
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        // Clear the screen and depth buffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLGraphicsManager::Draw()
    {
        GraphicsManager::Draw();

        // Render the model using the color shader.
        RenderBuffers();

        glFlush();
    }

    bool OpenGLGraphicsManager::SetPerFrameShaderParameters(GLuint shader)
    {
        unsigned int location;

        // Set the world matrix in the vertex shader.
        location = glGetUniformLocation(shader, "worldMatrix");
        if(location == -1)
            return false;
        glUniformMatrix4fv(location, 1, false, m_DrawFrameContext.WorldMatrix.GetAddressOf());

        // Set the view matrix in the vertex shader.
        location = glGetUniformLocation(shader, "viewMatrix");
        if (location == -1)
            return false;
        glUniformMatrix4fv(location, 1, false, m_DrawFrameContext.ViewMatrix.GetAddressOf());

        // Set the projection matrix in the vertex shader.
        location = glGetUniformLocation(shader, "projectionMatrix");
        if (location == -1)
            return false;
        glUniformMatrix4fv(location, 1, false, m_DrawFrameContext.ProjectionMatrix.GetAddressOf());

        // Set lighting parameters for PS shader
        location = glGetUniformLocation(shader, "lightPosition");
        if (location == -1)
            return false;
        glUniform3fv(location, 1, m_DrawFrameContext.LightPosition.GetAddressOf());

        location = glGetUniformLocation(shader, "lightColor");
        if (location == -1)
            return false;
        glUniform4fv(location, 1, m_DrawFrameContext.LightColor.GetAddressOf());
    }

    bool OpenGLGraphicsManager::SetPerBatchShaderParameters(GLuint shader, const char* paramName, const Matrix4f& param)
    {
        unsigned int location;

        location = glGetUniformLocation(shader, paramName);
        if (location == -1)
            return false;
        glUniformMatrix4fv(location, 1, false, param.GetAddressOf());

        return true;
    }

    bool OpenGLGraphicsManager::SetPerBatchShaderParameters(GLuint shader, const char* paramName, const float param)
    {
        unsigned int location;

        location = glGetUniformLocation(shader, paramName);
        if(location == -1)
        {
            return false;
        }
        glUniform1f(location, param);

        return true;
    }

    bool OpenGLGraphicsManager::SetPerBatchShaderParameters(GLuint shader, const char* paramName, const Vector3Df& param)
    {
        unsigned int location;

        location = glGetUniformLocation(shader, paramName);
        if(location == -1)
        {
            return false;
        }
        glUniform3fv(location, 1, param.GetAddressOf());

        return true;   
    }

    bool OpenGLGraphicsManager::SetPerBatchShaderParameters(GLuint shader, const char* paramName, const int param)
    {
        unsigned int location;

        location = glGetUniformLocation(shader, paramName);
        if(location == -1)
        {
            return false;
        }
        
        glUniform1i(location, param);

        return true;   
    }

    void OpenGLGraphicsManager::InitializeBuffers(const Scene& scene)
    {
        // Geometries
        for (auto _it : scene.GeometryNodes)
        {
			auto pGeometryNode = _it.second;
            if (pGeometryNode->Visible())
            {
                std::string str = pGeometryNode->GetSceneObjectRef();
                auto pGeometry = scene.GetGeometry(pGeometryNode->GetSceneObjectRef());
                assert(pGeometry);
                auto pMesh = pGeometry->GetMesh().lock();
                if (!pMesh) continue;

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

                size_t indexGroupCount = pMesh->GetIndexGroupCount();
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

                for (size_t i = 0; i < indexGroupCount; ++i)
                {
                    // Generate an ID for the index buffer
                    glGenBuffers(1, &buffer_id);

                    const SceneObjectIndexArray& indexArray = pMesh->GetIndexArray(i);
                    size_t indexArraySize = indexArray.GetDataSize();
                    const void* indexArrayData = indexArray.GetData();

                    // Bind the index buffer and load the index data into it.
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArraySize, indexArrayData, GL_STATIC_DRAW);

                    // Set the number of indices in the index array
                    GLsizei indexCount = static_cast<GLsizei>(indexArray.GetIndexCount());

                    GLenum type;
                    switch(indexArray.GetIndexType())
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
                            std::cerr << "Error: unsupported index type " << indexArray << std::endl;
                            std::cerr << "Mesh: " << *pMesh << std::endl;
                            std::cerr << "Geometry: " << *pGeometry << std::endl;
                            continue;
                    }

                    m_Buffers.push_back(buffer_id);

                    size_t materialIndex = indexArray.GetMaterialIndex();
                    std::string materialKey = pGeometryNode->GetMaterialRef(materialIndex);
                    auto material = scene.GetMaterial(materialKey);
                    if (material)
                    {
                        auto color = material->GetBaseColor();
                        if (color.ValueMap)
                        {
                            Image texture = color.ValueMap->GetTextureImage();
                            auto it = m_TextureIndex.find(materialKey);
                            if (it == m_TextureIndex.end())
                            {
                                GLuint textureID;
                                glGenTextures(1, &textureID);
                                glActiveTexture(GL_TEXTURE0 + textureID);
                                glBindTexture(GL_TEXTURE_2D, textureID);
                                if (texture.BitCount == 24)
                                {
                                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.Width, texture.Height,
                                        0, GL_RGB, GL_UNSIGNED_BYTE, texture.Data);
                                }
                                else 
                                {
                                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.Width, texture.Height,
                                        0, GL_RGBA, GL_UNSIGNED_BYTE, texture.Data);
                                }
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                                m_TextureIndex[color.ValueMap->GetName()] = textureID;
                                m_Textures.push_back(textureID);
                            }
                        }
                    }

                    DrawBatchContext& dbc = *(new DrawBatchContext);
                    dbc.vao = vao;
                    dbc.mode = mode;
                    dbc.type = type;
                    dbc.count = indexCount;
                    dbc.node = pGeometryNode;
                    dbc.material = material;
                    //std::cout << dbc;
                    m_DrawBatchContext.push_back(std::move(dbc));
                }
            }
        }


        return;
    }

    void OpenGLGraphicsManager::ClearBuffers()
    {
#ifdef DEBUG
        ClearDebugBuffers();
#endif

        for (auto dbc : m_DrawBatchContext) {
            glDeleteVertexArrays(1, &dbc.vao);
        }

        m_DrawBatchContext.clear();

        for (auto buf : m_Buffers) {
            glDeleteBuffers(1, &buf);
        }

        for (auto texture : m_Textures) {
            glDeleteTextures(1, &texture);
        }

        m_Buffers.clear();
        m_Textures.clear();

    }

    void OpenGLGraphicsManager::RenderBuffers()
    {
        // Set the color shader as the current shader program and set the matrices that it will use for rendering.
        glUseProgram(m_ShaderProgram);

        SetPerFrameShaderParameters(m_ShaderProgram);

        for (auto dbc : m_DrawBatchContext)
        {
            Matrix4f trans;// = *dbc.node ->GetCalculatedTransform();

            if (void* pRigidBody = dbc.node->RigidBody())
            {
                // the geometry has rigid body bounded, we blend the simulation result here.
                Matrix4f simulatedResult = g_pPhysicsManager->GetRigidBodyTransform(pRigidBody);

                trans.SetIdentity();

                memcpy(trans.m[0], simulatedResult.m[0], sizeof(float) * 3);
                memcpy(trans.m[1], simulatedResult.m[1], sizeof(float) * 3);
                memcpy(trans.m[2], simulatedResult.m[2], sizeof(float) * 3);

                // replace the translation part of the matirx with simulation result directly
                memcpy(trans.m[3], simulatedResult.m[3], sizeof(float) * 3);
            }
            else
            {
                trans = *dbc.node ->GetCalculatedTransform();
            }
            SetPerBatchShaderParameters(m_ShaderProgram, "modelMatrix", trans);
            glBindVertexArray(dbc.vao);

            /*
                Well, we have different materials for each index buffer, so we can not draw them together.
                In feature, we need to group indices according to its material and draw them together.
                auto indexBufferCount = dbc.counts.size();
                const GLvoid ** pIndicies = new const GLvoid*[indexBufferCount];
                memset(pIndicies, 0x00, sizeof(GLvoid*) * indexBufferCount);
                // Render the vertex buffer using the index buffer.
                glMultiDrawElements(dbc.mode, dbc.counts.data(), dbc.type, pIndicies, indexBufferCount);
                delete[] pIndicies;
            */

            if (dbc.material)
            {
                Color color = dbc.material->GetBaseColor();
                if (color.ValueMap)
                {
                    SetPerBatchShaderParameters(m_ShaderProgram, "defaultSampler", m_TextureIndex[color.ValueMap->GetName()]);
                    SetPerBatchShaderParameters(m_ShaderProgram, "diffuseColor", Vector3Df(-1.0f));
                }
                else 
                {
                    SetPerBatchShaderParameters(m_ShaderProgram, "diffuseColor", color.Value.GetRGB());
                }

                color = dbc.material->GetSpecularColor();
                SetPerBatchShaderParameters(m_ShaderProgram, "specularColor", color.Value.GetRGB());

                Parameter param = dbc.material->GetSpecularPower();
                SetPerBatchShaderParameters(m_ShaderProgram, "specularPower", param.Value);
            }

            glDrawElements(dbc.mode, dbc.count, dbc.type, 0x00);
        }

#ifdef DEBUG 
        // Set the color shader as the current shader program and set the matrices that it will use for rendering.
        glUseProgram(m_debugShaderProgram);

        SetPerFrameShaderParameters(m_debugShaderProgram);

        for (auto dbc : m_DebugDrawBatchContext)
        {
            SetPerBatchShaderParameters(m_debugShaderProgram, "lineColor", dbc.color);

            glBindVertexArray(dbc.vao);
            glDrawArrays(dbc.mode, 0x00, dbc.count);
        }
#endif

        return;
    }

    bool OpenGLGraphicsManager::InitializeShaders()
    {
        const char* vsFilename = VS_SHADER_SOURCE_FILE;
        const char* fsFilename = PS_SHADER_SOURCE_FILE;
#ifdef DEBUG
        const char* debugVsFilename = DEBUG_VS_SHADER_SOURCE_FILE;
        const char* debugFsFilename = DEBUG_PS_SHADER_SOURCE_FILE;
#endif
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

#ifdef DEBUG
        std::string debugVertexShaderBuffer;
        std::string debugFragmentShaderBuffer;

        // Load the fragment shader source file into a text buffer.
        debugVertexShaderBuffer = g_pAssetLoader->SyncOpenAndReadTextFileToString(debugVsFilename);
        if(debugVertexShaderBuffer.empty())
        {
                return false;
        }

        // Load the fragment shader source file into a text buffer.
        debugFragmentShaderBuffer = g_pAssetLoader->SyncOpenAndReadTextFileToString(debugFsFilename);
        if(debugFragmentShaderBuffer.empty())
        {
                return false;
        }
#endif

        // Create a vertex and fragment shader object.
        m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
        m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
#ifdef DEBUG
        m_debugVertexShader = glCreateShader(GL_VERTEX_SHADER);
        m_debugFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
#endif

        // Copy the shader source code string into the vertex and fragment shader objects.
        const char* _v_c_str = vertexShaderBuffer.c_str();
        glShaderSource(m_VertexShader, 1, &_v_c_str, NULL);
        const char* _f_c_str = fragmentShaderBuffer.c_str();
        glShaderSource(m_FragmentShader, 1, &_f_c_str, NULL);
#ifdef DEBUG
        const char* _v_c_str_debug = debugVertexShaderBuffer.c_str();
        glShaderSource(m_debugVertexShader, 1, &_v_c_str_debug, NULL);
        const char* _f_c_str_debug = debugFragmentShaderBuffer.c_str();
        glShaderSource(m_debugFragmentShader, 1, &_f_c_str_debug, NULL);
#endif

        // Compile the shaders.
        glCompileShader(m_VertexShader);
        glCompileShader(m_FragmentShader);
#ifdef DEBUG
        glCompileShader(m_debugVertexShader);
        glCompileShader(m_debugFragmentShader);
#endif

        // Check to see if the vertex shader compiled successfully.
        glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &status);
        if(status != 1)
        {
            // If it did not compile then write the syntax error message out to a text file for review.
            OutputShaderErrorMessage(m_VertexShader, vsFilename);
            return false;
        }

#ifdef DEBUG
        // Check to see if the fragment shader compiled successfully.
        glGetShaderiv(m_debugVertexShader, GL_COMPILE_STATUS, &status);
        if(status != 1)
        {
                // If it did not compile then write the syntax error message out to a text file for review.
                OutputShaderErrorMessage(m_debugVertexShader, debugVsFilename);
                return false;
        }

        // Check to see if the fragment shader compiled successfully.
        glGetShaderiv(m_debugFragmentShader, GL_COMPILE_STATUS, &status);
        if(status != 1)
        {
                // If it did not compile then write the syntax error message out to a text file for review.
                OutputShaderErrorMessage(m_debugFragmentShader, debugFsFilename);
                return false;
        }
#endif

        // Create a shader program object.
        m_ShaderProgram = glCreateProgram();
#ifdef DEBUG
        m_debugShaderProgram = glCreateProgram();
#endif

        // Attach the vertex and fragment shader to the program object.
        glAttachShader(m_ShaderProgram, m_VertexShader);
        glAttachShader(m_ShaderProgram, m_FragmentShader);
#ifdef DEBUG
        glAttachShader(m_debugShaderProgram, m_debugVertexShader);
        glAttachShader(m_debugShaderProgram, m_debugFragmentShader);
#endif

        // Bind the shader input variables.
		glBindAttribLocation(m_ShaderProgram, 0, "inputPosition");
		glBindAttribLocation(m_ShaderProgram, 1, "inputNormal");
        glBindAttribLocation(m_ShaderProgram, 2, "inputUV");

        // Link the shader program.
        glLinkProgram(m_ShaderProgram);

#ifdef DEBUG
        // Bind the shader input variables.
        glBindAttribLocation(m_debugShaderProgram, 0, "inputPosition");

        // Link the shader program.
        glLinkProgram(m_debugShaderProgram);
#endif

        // Check the status of the link.
        glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &status);
        if (status != 1)
        {
            // if it did not link then write the syntax error message out to a text fire for review
            OutputLinkerErrorMessage(m_ShaderProgram);
            return false;
        }

#ifdef DEBUG
        // Check the status of the link.
        glGetProgramiv(m_debugShaderProgram, GL_LINK_STATUS, &status);
        if(status != 1)
        {
                // If it did not link then write the syntax error message out to a text file for review.
                OutputLinkerErrorMessage(m_debugShaderProgram);
                return false;
        }
#endif

        return true;
    }

#ifdef DEBUG
    void OpenGLGraphicsManager::ClearDebugBuffers()
    {
        for (auto dbc : m_DebugDrawBatchContext) {
            glDeleteVertexArrays(1, &dbc.vao);
        }

        m_DebugDrawBatchContext.clear();

        for (auto buf : m_DebugBuffers) {
            glDeleteBuffers(1, &buf);
        }

        m_DebugBuffers.clear();
    }
#endif    
}

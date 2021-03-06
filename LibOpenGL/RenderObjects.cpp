//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//    .--------------------------------------------------.
//    |  This file is part of NTCodeBase                 |
//    |  Created 2018 by NT (https://ttnghia.github.io)  |
//    '--------------------------------------------------'
//                            \o/
//                             |
//                            / |
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include <LibOpenGL/Camera.h>
#include <LibOpenGL/Lights.h>
#include <LibOpenGL/Material.h>
#include <LibOpenGL/OpenGLBuffer.h>
#include <LibOpenGL/OpenGLTexture.h>
#include <LibOpenGL/ShaderProgram.h>
#include <LibOpenGL/MeshObjects/CubeObject.h>
#include <LibOpenGL/MeshObjects/GridObject.h>
#include <LibOpenGL/MeshObjects/WireFrameBoxObject.h>
#include <LibOpenGL/RenderObjects.h>

#include <future>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace NTCodeBase {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SkyBox render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef NT_QT_FRAMEWORK
#include <QDir>
void SkyBoxRender::loadTextures(QString textureTopFolder) {
    QStringList texFaces =
    {
        "/posx.", "/negx.",
        "/posy.", "/negy.",
        "/posz.", "/negz."
    };

    clearTextures();
    QDir dataDir(textureTopFolder);
    dataDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

    QStringList   allTexFolders = dataDir.entryList();
    StdVT<QImage> textureImages;
    textureImages.resize(allTexFolders.count() * 6);

    StdVT<int> loadSuccess;
    loadSuccess.assign(allTexFolders.count(), 0);

    StdVT<std::future<void>> futureObjs;

    for(int i = 0; i < allTexFolders.count(); ++i) {
        QString texFolderPath = textureTopFolder + "/" + allTexFolders[i];
        futureObjs.emplace_back(std::async(std::launch::async,
                                           [&, texFolderPath, i] {
                                               ////////////////////////////////////////////////////////////////////////////////
                                               // find the extension of texture imageg
                                               QString posXFilePath = texFolderPath + "/posx.jpg";
                                               QString ext          = "jpg";

                                               if(!QFile::exists(posXFilePath)) {
                                                   ext          = "png";
                                                   posXFilePath = texFolderPath + "/posx.png";
                                                   if(!QFile::exists(posXFilePath)) {
                                                       return; // only support .png and .jpg
                                                   }
                                               }

                                               ////////////////////////////////////////////////////////////////////////////////
                                               // check if all 6 faces exist
                                               bool check = true;
                                               for(GLuint j = 0; j < 6; ++j) {
                                                   QString texFilePath = texFolderPath + texFaces[j] + ext;
                                                   if(!QFile::exists(texFilePath)) {
                                                       check = false;
                                                       break;
                                                   }
                                               }

                                               if(!check) {
                                                   return;
                                               }

                                               ////////////////////////////////////////////////////////////////////////////////
                                               // load the textures
                                               loadSuccess[i] = 1;

                                               for(GLuint j = 0; j < 6; ++j) {
                                                   QString texFilePath      = texFolderPath + texFaces[j] + ext;
                                                   textureImages[i * 6 + j] = QImage(texFilePath).convertToFormat(QImage::Format_RGBA8888);
                                               }
                                           }));
    }

    for(std::future<void>& f : futureObjs) {
        if(f.valid()) {
            f.wait();
        }
    }

    for(int i = 0; i < allTexFolders.count(); ++i) {
        if(loadSuccess[i] == 0) {
            continue;
        }

        SharedPtr<OpenGLTexture> skyboxTex = std::make_shared<OpenGLTexture>(GL_TEXTURE_CUBE_MAP);
        for(GLuint j = 0; j < 6; ++j) {
            const QImage& texImg = textureImages[i * 6 + j];
            skyboxTex->uploadData(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, GL_RGBA, texImg.width(), texImg.height(), GL_RGBA, GL_UNSIGNED_BYTE, texImg.constBits());
        }

        skyboxTex->setBestParametersNoMipMap();
        addTexture(skyboxTex);
    }
}

#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::clearTextures() {
    m_Textures.resize(0);
    m_Textures.push_back(nullptr);
    m_CurrentTexture = nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::addTexture(const SharedPtr<OpenGLTexture>& texture) {
    texture->setTextureParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    texture->setTextureParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    texture->setTextureParameter(GL_TEXTURE_WRAP_R, GL_REPEAT);

    m_Textures.push_back(texture);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::setRenderTextureIndex(int texIndex) {
    if(texIndex < 0) {
        return;
    }

    assert(static_cast<size_t>(texIndex) < m_Textures.size());

    m_CurrentTexture = m_Textures[texIndex];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::scale(float scaleX, float scaleY, float scaleZ) {
    assert(m_CubeObj != nullptr);
    m_CubeObj->scale(Vec3f(scaleX, scaleY, scaleZ));
    m_CubeObj->uploadDataToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
const SharedPtr<OpenGLTexture>& SkyBoxRender::getCurrentTexture() {
    return m_CurrentTexture;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
size_t SkyBoxRender::getNumTextures() {
    return m_Textures.size();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::render() {
    assert(m_Camera != nullptr && m_UBufferCamData != nullptr);
    if(m_CurrentTexture == nullptr) {
        return;
    }

    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),            0,                   sizeof(Mat4x4f));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()), sizeof(Mat4x4f),          sizeof(Mat4x4f));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getCameraPosition()),        5 * sizeof(Mat4x4f), sizeof(Vec3f));

    m_Shader->bind();
    m_CurrentTexture->bind();

    m_UBufferModelMatrix->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBModelMatrix, m_UBufferModelMatrix->getBindingPoint());

    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData, m_UBufferCamData->getBindingPoint());

    m_Shader->setUniformValue(m_UTexSampler, 0);

    glCall(glBindVertexArray(m_VAO));
    m_CubeObj->draw();
    m_CurrentTexture->release();
    m_Shader->release();
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void SkyBoxRender::initRenderData() {
    m_Shader = ShaderProgram::getSkyBoxShader();

    m_AtrVPosition = m_Shader->getAtributeLocation("v_Position");
    m_UTexSampler  = m_Shader->getUniformLocation("u_TexSampler");

    m_UBModelMatrix = m_Shader->getUniformBlockIndex("ModelMatrix");
    m_UBCamData     = m_Shader->getUniformBlockIndex("CameraData");

    ////////////////////////////////////////////////////////////////////////////////
    // cube object
    m_CubeObj = std::make_shared<MeshObjects::CubeObject>();
    m_CubeObj->uploadDataToGPU();

    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_CubeObj->getVertexBuffer()->bind();
    m_CubeObj->getIndexBuffer()->bind();
    glCall(glVertexAttribPointer(m_AtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glEnableVertexAttribArray(m_AtrVPosition));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    m_UBufferModelMatrix = std::make_shared<OpenGLBuffer> ();
    m_UBufferModelMatrix->createBuffer(GL_UNIFORM_BUFFER, 2 * sizeof(Mat4x4f), nullptr, GL_STATIC_DRAW);

    Mat4x4f modelMatrix = glm::scale(Mat4x4f(1.0f), Vec3f(500.0f));
    m_UBufferModelMatrix->uploadData(glm::value_ptr(modelMatrix), 0, sizeof(Mat4x4f));

    if(m_UBufferCamData == nullptr) {
        m_UBufferCamData = std::make_shared<OpenGLBuffer> ();
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(Mat4x4f) + sizeof(Vec4f), nullptr, GL_DYNAMIC_DRAW);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // default null texture
    m_Textures.push_back(nullptr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightRender::setRenderSize(GLfloat renderSize) {
    m_RenderSize = renderSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightRender::render() {
    assert(m_Camera != nullptr && m_UBufferCamData != nullptr);

    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),            0,          sizeof(Mat4x4f));
    m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()), sizeof(Mat4x4f), sizeof(Mat4x4f));

    m_Shader->bind();

    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData, m_UBufferCamData->getBindingPoint());

    m_Lights->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_UBLight, m_Lights->getBufferBindingPoint());

    glCall(glBindVertexArray(m_VAO));
    glCall(glPointSize(m_RenderSize));
    glCall(glEnable(GL_VERTEX_PROGRAM_POINT_SIZE));
    glCall(glDrawArrays(GL_POINTS, 0, m_Lights->getNumLights()));
    glCall(glBindVertexArray(0));
    m_Shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PointLightRender::initRenderData() {
    const GLchar* vertexShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "layout(std140) uniform CameraData\n"
        "{\n"
        "    mat4 viewMatrix;\n"
        "    mat4 projectionMatrix;\n"
        "    mat4 invViewMatrix;\n"
        "    mat4 invProjectionMatrix;\n"
        "    mat4 shadowMatrix;\n"
        "    vec4 camPosition;\n"
        "    vec4 camFocus;\n"
        "};\n"
        "\n"
        "#define NUM_TOTAL_LIGHTS 4\n"
        "struct PointLight\n"
        "{\n"
        "    vec4 ambient; \n"
        "    vec4 diffuse; \n"
        "    vec4 specular; \n"
        "    vec4 position; \n"
        "};\n"

        "layout(std140) uniform Lights\n"
        "{\n"
        "    PointLight lights[NUM_TOTAL_LIGHTS]; \n"
        "    int u_NumLights; \n"
        "}; \n"
        "\n"
        "out vec3 f_lightDiffuse;\n"
        "void main()\n"
        "{\n"
        "    vec4 worldCoord = vec4(vec3(lights[gl_VertexID].position), 1.0);\n"
        "    gl_Position = projectionMatrix * viewMatrix * worldCoord;\n"
        "    f_lightDiffuse = vec3(lights[gl_VertexID].diffuse);\n"
        "}";

    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "in vec3 f_lightDiffuse;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outColor = vec4(vec3(1.0 - max(max(f_lightDiffuse.x, f_lightDiffuse.y), f_lightDiffuse.z)) + f_lightDiffuse, 1.0);\n"
        "}\n";

    m_Shader = std::make_shared<ShaderProgram>(std::string("LightRenderShader"));
    m_Shader->addVertexShaderFromSource(vertexShader);
    m_Shader->addFragmentShaderFromSource(fragmentShader);
    m_Shader->link();

    glCall(glGenVertexArrays(1, &m_VAO));

    m_UBCamData = m_Shader->getUniformBlockIndex("CameraData");
    m_UBLight   = m_Shader->getUniformBlockIndex("Lights");

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    if(m_UBufferCamData == nullptr) {
        m_UBufferCamData = std::make_shared<OpenGLBuffer> ();
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(Mat4x4f) + sizeof(Vec4f), nullptr, GL_DYNAMIC_DRAW);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WireFrameBoxRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WireFrameBoxRender::transform(const Vec3f& translation, const Vec3f& scale) {
    Mat4x4f modelMatrix  = glm::scale(glm::translate(Mat4x4f(1.0), translation), scale);
    Mat4x4f normalMatrix = glm::transpose(glm::inverse(Mat3x3f(modelMatrix)));

    m_UBufferModelMatrix->uploadData(glm::value_ptr(modelMatrix),       0,          sizeof(Mat4x4f)); // model matrix
    m_UBufferModelMatrix->uploadData(glm::value_ptr(normalMatrix), sizeof(Mat4x4f), sizeof(Mat4x4f)); // normal matrix}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WireFrameBoxRender::setBox(const Vec3f& boxMin, const Vec3f& boxMax) {
    m_WireFrameBoxObj->setBox(boxMin, boxMax);
    m_WireFrameBoxObj->uploadDataToGPU();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WireFrameBoxRender::render() {
    m_Shader->bind();

    m_UBufferModelMatrix->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBModelMatrix, m_UBufferModelMatrix->getBindingPoint());

    if(m_SelfUpdateCamera) {
        m_Camera->updateCameraMatrices();
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),            0,                   sizeof(Mat4x4f));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()), sizeof(Mat4x4f),          sizeof(Mat4x4f));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getCameraPosition()),        5 * sizeof(Mat4x4f), sizeof(Vec3f));
    }
    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData, m_UBufferCamData->getBindingPoint());

    m_Shader->setUniformValue(m_UColor, m_BoxColor);

    glCall(glBindVertexArray(m_VAO));
    m_WireFrameBoxObj->draw();
    m_Shader->release();
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void WireFrameBoxRender::initRenderData() {
    m_Shader = ShaderProgram::getObjUniformColorShader();

    m_AtrVPosition  = m_Shader->getAtributeLocation("v_Position");
    m_UColor        = m_Shader->getUniformLocation("f_Color");
    m_UBModelMatrix = m_Shader->getUniformBlockIndex("ModelMatrix");
    m_UBCamData     = m_Shader->getUniformBlockIndex("CameraData");

    m_WireFrameBoxObj = std::make_shared<MeshObjects::WireFrameBoxObject> ();
    m_WireFrameBoxObj->uploadDataToGPU();

    glCall(glGenVertexArrays(1, &m_VAO));
    glCall(glBindVertexArray(m_VAO));
    m_WireFrameBoxObj->getVertexBuffer()->bind();
    m_WireFrameBoxObj->getIndexBuffer()->bind();
    glCall(glVertexAttribPointer(m_AtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    glCall(glEnableVertexAttribArray(m_AtrVPosition));
    glCall(glBindVertexArray(0));

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    if(m_UBufferCamData == nullptr) {
        m_UBufferCamData = std::make_shared<OpenGLBuffer> ();
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(Mat4x4f) + sizeof(Vec4f), nullptr, GL_DYNAMIC_DRAW);
    }

    m_UBufferModelMatrix = std::make_shared<OpenGLBuffer> ();
    m_UBufferModelMatrix->createBuffer(GL_UNIFORM_BUFFER, 2 * sizeof(Mat4x4f), nullptr, GL_STATIC_DRAW);
    transform(Vec3f(0, 0, 0), Vec3f(1, 1, 1));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// OffScreenRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::resize(int width, int height) {
    m_BufferWidth  = width;
    m_BufferHeight = height;

    glCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID));
    glCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_BufferWidth, m_BufferHeight));
    glCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    // color attachment
    for(int i = 0; i < m_NumColorBuffers; ++i) {
        assert(m_ColorBuffers[i] != nullptr);
        m_ColorBuffers[i]->uploadData(GL_TEXTURE_2D, m_FormatColorBuff, m_BufferWidth, m_BufferHeight, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::beginRender() {
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID));
    glCall(glViewport(0, 0, m_BufferWidth, m_BufferHeight));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::endRender(GLuint defaultFBO /*= 0*/) {
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::setNumColorBuffers(int numColorBuffers) {
    if(m_NumColorBuffers == numColorBuffers) {
        return;
    } else {
        glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID));

        if(m_NumColorBuffers > numColorBuffers) {
            for(int i = numColorBuffers; i < m_NumColorBuffers; ++i) {
                glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0));
            }
        } else { // m_NumColorBuffers < numColorBuffers
            for(int i = static_cast<int>(m_ColorBuffers.size()); i < numColorBuffers; ++i) {
                SharedPtr<OpenGLTexture> tex = std::make_shared<OpenGLTexture>(GL_TEXTURE_2D);
                tex->setTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                tex->setTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                tex->uploadData(GL_TEXTURE_2D, m_FormatColorBuff, m_BufferWidth, m_BufferHeight, GL_RED, GL_UNSIGNED_BYTE, nullptr);

                m_ColorBuffers.push_back(std::move(tex));
            }

            for(int i = m_NumColorBuffers; i < numColorBuffers; ++i) {
                glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorBuffers[i]->getTextureID(), 0));
            }
        }

        if(numColorBuffers == 0) {
            glCall(glDrawBuffer(GL_NONE));
            glCall(glReadBuffer(GL_NONE));
        }

        // check error
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
#ifdef NT_QT_FRAMEWORK
            NT_DIE(QString("OffScreenRender: FrameBuffer is incomplete!"));
#else
            NT_DIE(("%s: FrameBuffer is incomplete!\n", m_Shader->getProgramName().c_str()));
#endif
        }

        glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        m_NumColorBuffers = numColorBuffers;
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::setColorBufferParameter(GLenum paramName, GLenum paramValue) {
    for(SharedPtr<OpenGLTexture>& tex : m_ColorBuffers) {
        tex->setTextureParameter(paramName, paramValue);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<OpenGLTexture>& OffScreenRender::getColorBuffer(int colorBufferID /*= 0*/) {
    assert(colorBufferID < m_NumColorBuffers);
    return m_ColorBuffers[colorBufferID];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::swapColorBuffer(SharedPtr<OpenGLTexture>& colorBuffer, int bufferID) {
    std::swap(colorBuffer, m_ColorBuffers[bufferID]);

    glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID));
    glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + bufferID, GL_TEXTURE_2D, m_ColorBuffers[bufferID]->getTextureID(), 0));
    // check error
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
#ifdef NT_QT_FRAMEWORK
        NT_DIE(QString("OffScreenRender: FrameBuffer is incomplete!"));
#else
        NT_DIE(("%s: FrameBuffer is incomplete!\n", m_Shader->getProgramName().c_str()));
#endif
    }

    glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::fastSwapColorBuffer(SharedPtr<OpenGLTexture>& colorBuffer, int bufferID) {
    std::swap(colorBuffer, m_ColorBuffers[bufferID]);
    glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + bufferID, GL_TEXTURE_2D, m_ColorBuffers[bufferID]->getTextureID(), 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void OffScreenRender::initRenderData() {
    glCall(glGenFramebuffers(1, &m_FrameBufferID));
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID));

    // depth-stencil attachment
    glCall(glGenRenderbuffers(1, &m_RenderBufferID));
    glCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID));
    glCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_BufferWidth, m_BufferHeight));
    glCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    glCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID));

    // color attachment
    for(int i = 0; i < m_NumColorBuffers; ++i) {
        SharedPtr<OpenGLTexture> tex = std::make_shared<OpenGLTexture>(GL_TEXTURE_2D);
        tex->setTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        tex->setTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        tex->uploadData(GL_TEXTURE_2D, m_FormatColorBuff, m_BufferWidth, m_BufferHeight, GL_RED, GL_UNSIGNED_BYTE, nullptr);

        glCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tex->getTextureID(), 0));
        m_ColorBuffers.push_back(std::move(tex));
    }

    if(m_NumColorBuffers == 0) {
        glCall(glDrawBuffer(GL_NONE));
        glCall(glReadBuffer(GL_NONE));
    }

    // check error
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
#ifdef NT_QT_FRAMEWORK
        NT_DIE(QString("OffScreenRender: FrameBuffer is incomplete!"));
#else
        NT_DIE(("%s: FrameBuffer is incomplete!\n", m_Shader->getProgramName().c_str()));
#endif
    }

    glCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DepthBufferRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void DepthBufferRender::beginRender() {
    OffScreenRender::beginRender();

    glCall(glClearColor(m_ClearLinearDepthValue, m_ClearLinearDepthValue, m_ClearLinearDepthValue, 1.0));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    glCall(glEnable(GL_DEPTH_TEST));
    glCall(glDepthMask(GL_TRUE));
    glCall(glDepthFunc(GL_LEQUAL));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DepthBufferRender::endRender(GLuint defaultFBO /* = 0 */) {
    OffScreenRender::endRender(defaultFBO);

    glCall(glClearColor(m_DefaultClearColor[0], m_DefaultClearColor[1], m_DefaultClearColor[2], 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DepthBufferRender::setNumColorBuffers(int numColorBuffers) {
    OffScreenRender::setNumColorBuffers(numColorBuffers);

    GLfloat borderColor[] = { m_ClearLinearDepthValue, m_ClearLinearDepthValue, m_ClearLinearDepthValue, 1.0 };

    for(int i = 0; i < m_NumColorBuffers; ++i) {
        m_ColorBuffers[i]->bind();
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
        glCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
        m_ColorBuffers[i]->release();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DepthBufferRender::setDefaultClearColor(const Vec3f& clearColor) {
    m_DefaultClearColor = clearColor;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DepthBufferRender::setClearDepthValue(GLfloat clearValue) {
    m_ClearLinearDepthValue = clearValue;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<OpenGLTexture>& DepthBufferRender::getDepthBuffer() {
    assert(m_ColorBuffers.size() > 0);
    return m_ColorBuffers[0];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void DepthBufferRender::initRenderData() {
    OffScreenRender::initRenderData();

    GLfloat borderColor[] = { m_ClearLinearDepthValue, m_ClearLinearDepthValue, m_ClearLinearDepthValue, 1.0 };

    for(int i = 0; i < m_NumColorBuffers; ++i) {
        m_ColorBuffers[i]->bind();
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
        glCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
        m_ColorBuffers[i]->release();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ScreenQuadTextureRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ScreenQuadTextureRender::render() {
    assert(m_Texture != nullptr);

    m_Texture->bind();
    m_Shader->bind();
    m_Shader->setUniformValue(m_UTexSampler,               0);
    m_Shader->setUniformValue(m_UValueScale,    m_ValueScale);
    m_Shader->setUniformValue(m_UTexelSize, m_TexelSizeValue);

    glCall(glBindVertexArray(m_VAO));
    glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); );
    glCall(glBindVertexArray(0));
    m_Shader->release();
    m_Texture->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ScreenQuadTextureRender::initRenderData() {
    const GLchar* fragmentShader =
        "// This is the shader statically generated by ShaderProgram class\n"
        "#version 410 core\n"
        "\n"
        "uniform int u_TexelSize;\n"
        "uniform float u_ValueScale;\n"
        "uniform sampler2D u_TexSampler;\n"
        "\n"
        "in vec2 f_TexCoord;\n"
        "out vec4 outColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outColor = vec4(0, 0, 0, 1);\n"
        "\n"
        "    switch(u_TexelSize)\n"
        "    {\n"
        "        case 1:\n"
        "            outColor = vec4(vec3(u_ValueScale * texture(u_TexSampler, f_TexCoord).x), 1.0);\n"
        "            break;\n"
        "        case 2:\n"
        "            outColor.xy = u_ValueScale * texture(u_TexSampler, f_TexCoord).xy;\n"
        "            break;\n"
        "        case 3:\n"
        "            outColor.xyz = u_ValueScale * texture(u_TexSampler, f_TexCoord).xyz;\n"
        "            break;\n"
        "        case 4:\n"
        "            outColor = u_ValueScale * texture(u_TexSampler, f_TexCoord);\n"
        "            break;\n"
        "        default:\n"
        "            break;\n"
        "    }\n"
        "}\n";

    m_Shader = ShaderProgram::getScreenQuadShader(fragmentShader, "ScreenQuadRender");

    glCall(glGenVertexArrays(1, &m_VAO));

    m_UTexSampler = m_Shader->getUniformLocation("u_TexSampler");
    m_UValueScale = m_Shader->getUniformLocation("u_ValueScale");
    m_UTexelSize  = m_Shader->getUniformLocation("u_TexelSize");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ScreenQuadTextureRender::setValueScale(float scale) {
    m_ValueScale = scale;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ScreenQuadTextureRender::setTexture(const SharedPtr<OpenGLTexture>& texture, int texelSize /*= 1*/) {
    m_Texture        = texture;
    m_TexelSizeValue = texelSize;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CheckerboardBackgroundRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void CheckerboardBackgroundRender::render() {
    m_Shader->bind();
    m_Shader->setUniformValue(m_UScales,             m_Scales);
    m_Shader->setUniformValue(m_UScreenWidth,   m_ScreenWidth);
    m_Shader->setUniformValue(m_UScreenHeight, m_ScreenHeight);
    m_Shader->setUniformValue(m_UColor1,             m_Color1);
    m_Shader->setUniformValue(m_UColor2,             m_Color2);

    glCall(glDepthMask(GL_FALSE));
    glCall(glBindVertexArray(m_VAO));
    glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); );
    glCall(glBindVertexArray(0));
    glCall(glDepthMask(GL_TRUE));
    m_Shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void CheckerboardBackgroundRender::initRenderData() {
    m_Shader = ShaderProgram::getCheckerboardBackgroundShader();

    glCall(glGenVertexArrays(1, &m_VAO));

    m_UScales       = m_Shader->getUniformLocation("u_Scales");
    m_UScreenWidth  = m_Shader->getUniformLocation("u_ScreenWidth");
    m_UScreenHeight = m_Shader->getUniformLocation("u_ScreenHeight");
    m_UColor1       = m_Shader->getUniformLocation("u_Color1");
    m_UColor2       = m_Shader->getUniformLocation("u_Color2");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// GridBackgroundRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void GridBackgroundRender::render() {
    m_Shader->bind();
    m_Shader->setUniformValue(m_UScales,                   m_Scales);
    m_Shader->setUniformValue(m_UScreenWidth,         m_ScreenWidth);
    m_Shader->setUniformValue(m_UScreenHeight,       m_ScreenHeight);
    m_Shader->setUniformValue(m_UBackgroundColor, m_BackgroundColor);
    m_Shader->setUniformValue(m_ULineColor,             m_LineColor);

    glCall(glDepthMask(GL_FALSE));
    glCall(glBindVertexArray(m_VAO));
    glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); );
    glCall(glBindVertexArray(0));
    glCall(glDepthMask(GL_TRUE));
    m_Shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void GridBackgroundRender::initRenderData() {
    m_Shader = ShaderProgram::getGridBackgroundShader();

    glCall(glGenVertexArrays(1, &m_VAO));

    m_UScales          = m_Shader->getUniformLocation("u_Scales");
    m_UScreenWidth     = m_Shader->getUniformLocation("u_ScreenWidth");
    m_UScreenHeight    = m_Shader->getUniformLocation("u_ScreenHeight");
    m_UBackgroundColor = m_Shader->getUniformLocation("m_UBackgroundColor");
    m_ULineColor       = m_Shader->getUniformLocation("m_ULineColor");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Mesh render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MeshRender::MeshRender(const SharedPtr<MeshObjects::MeshObject>& meshObj, const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light,
                       QString textureFolder, const SharedPtr<Material>& material, const SharedPtr<OpenGLBuffer>& bufferCamData) :
    RenderObject(camera, bufferCamData), m_MeshObj(meshObj), m_Lights(light), m_Material(material), m_CurrentTexture(nullptr),
    m_ShadowBufferWidth(1024), m_ShadowBufferHeight(1024), m_Exposure(1.0f) {
    initRenderData();
    OpenGLTexture::loadTextures(m_Textures, textureFolder);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::loadTextures(QString textureFolder) {
    OpenGLTexture::loadTextures(m_Textures, textureFolder);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::clearTextures(bool insertNullTex /*= true*/) {
    m_Textures.resize(0);
    if(insertNullTex) {
        m_Textures.push_back(nullptr);
    }

    m_CurrentTexture = nullptr;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::addTexture(const SharedPtr<OpenGLTexture>& texture, GLenum texWrapMode /*= GL_REPEAT*/) {
    texture->setTextureParameter(GL_TEXTURE_WRAP_S, texWrapMode);
    texture->setTextureParameter(GL_TEXTURE_WRAP_T, texWrapMode);
    texture->setTextureParameter(GL_TEXTURE_WRAP_R, texWrapMode);
    m_Textures.push_back(texture);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::setRenderTextureIndex(int texIndex) {
    if(texIndex < 0) {
        return;
    }

    assert(static_cast<size_t>(texIndex) < m_Textures.size());
    m_CurrentTexture = m_Textures[texIndex];
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::setExternalShadowMaps(const StdVT<SharedPtr<OpenGLTexture>>& shadowMaps) {
    assert(shadowMaps.size() == static_cast<size_t>(m_Lights->getNumLights()));
    m_ExternalShadowMaps = shadowMaps;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::transform(const Vec3f& translation, const Vec3f& scales) {
    Mat4x4f modelMatrix  = glm::scale(glm::translate(Mat4x4f(1.0), translation), scales);
    Mat4x4f normalMatrix = glm::transpose(glm::inverse(Mat3x3f(modelMatrix)));

    m_UBufferModelMatrix->uploadData(glm::value_ptr(modelMatrix),       0,          sizeof(Mat4x4f)); // model matrix
    m_UBufferModelMatrix->uploadData(glm::value_ptr(normalMatrix), sizeof(Mat4x4f), sizeof(Mat4x4f)); // normal matrix
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::render() {
    assert(m_MeshObj != nullptr && m_Camera != nullptr && m_UBufferCamData != nullptr);
    if(m_MeshObj->isEmpty()) {
        return;
    }

    if(m_SelfUpdateCamera) {
        m_Camera->updateCameraMatrices();
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getViewMatrix()),            0,                   sizeof(Mat4x4f));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getProjectionMatrix()), sizeof(Mat4x4f),          sizeof(Mat4x4f));
        m_UBufferCamData->uploadData(glm::value_ptr(m_Camera->getCameraPosition()),        5 * sizeof(Mat4x4f), sizeof(Vec3f));
    }

    m_Shader->bind();
    if(m_CurrentTexture != nullptr) {
        m_CurrentTexture->bind();
        m_Shader->setUniformValue(m_UHasTexture, 1);
        m_Shader->setUniformValue(m_UTexSampler, 0);
    } else {
        m_Shader->setUniformValue(m_UHasTexture, 0);
    }

    m_UBufferModelMatrix->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBModelMatrix, m_UBufferModelMatrix->getBindingPoint());

    m_UBufferCamData->bindBufferBase();
    m_Shader->bindUniformBlock(m_UBCamData, m_UBufferCamData->getBindingPoint());

    m_Lights->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_UBLight, m_Lights->getBufferBindingPoint());

    m_Material->bindUniformBuffer();
    m_Shader->bindUniformBlock(m_UBMaterial, m_Material->getBufferBindingPoint());

    m_Shader->setUniformValue(m_UExposure, m_Exposure);

    if(m_ExternalShadowMaps.size() != 0) {
        m_Lights->bindUniformBufferLightMatrix();
        m_Shader->bindUniformBlock(m_UBLightMatrices, m_Lights->getBufferLightMatrixBindingPoint());
        m_Shader->setUniformValue(m_UHasShadow, 1);

        for(int i = 0; i < static_cast<int>(m_ExternalShadowMaps.size()); ++i) {
            m_Shader->setUniformValue(m_UShadowMap[i], i + 1);
            m_ExternalShadowMaps[i]->bind(i + 1);
        }
    } else {
        m_Shader->setUniformValue(m_UHasShadow, 0);
    }

    m_Shader->setUniformValue(u_ClipPlane, m_ClipPlane);

    glCall(glBindVertexArray(m_VAO));
    m_MeshObj->draw();
    glCall(glBindVertexArray(0));

    if(m_CurrentTexture != nullptr) {
        m_CurrentTexture->release();
    }
    if(m_ExternalShadowMaps.size() != 0) {
        for(int i = 0; i < static_cast<int>(m_ExternalShadowMaps.size()); ++i) {
            m_ExternalShadowMaps[i]->release();
        }
    }
    m_Shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::renderToLightDepthBuffer(int scrWidth, int scrHeight, GLuint defaultFBO /*= 0*/) {
    assert(m_DepthBufferInitialized);
    assert(m_MeshObj != nullptr && m_Camera != nullptr && m_UBufferCamData != nullptr);
    assert(m_Lights != nullptr && m_Lights->getNumLights() > 0);
    assert(m_LightDepthBufferRenders.size() != 0);

    if(m_MeshObj->isEmpty()) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // update the depth render objs in case the number of lights has changed
    for(int i = static_cast<int>(m_LightDepthBufferRenders.size()); i < m_Lights->getNumLights(); ++i) {
        m_LightDepthBufferRenders.push_back(std::make_shared<DepthBufferRender>(m_ShadowBufferWidth, m_ShadowBufferHeight));
    }

    m_LightDepthShader->bind();
    m_UBufferModelMatrix->bindBufferBase();
    m_LightDepthShader->bindUniformBlock(m_LDSUBModelMatrix, m_UBufferModelMatrix->getBindingPoint());

    m_Lights->bindUniformBufferLightMatrix();
    m_LightDepthShader->bindUniformBlock(m_LDSUBLightMatrices, m_Lights->getBufferLightMatrixBindingPoint());

    for(int i = 0; i < m_Lights->getNumLights(); ++i) {
        m_LightDepthShader->setUniformValue(m_LDSULightID, i);
        m_LightDepthBufferRenders[i]->beginRender();

        glCall(glBindVertexArray(m_LDSVAO));
        m_MeshObj->draw();
        glCall(glBindVertexArray(0));

        m_LightDepthBufferRenders[i]->endRender(defaultFBO);
    }

    m_LightDepthShader->release();
    glCall(glViewport(0, 0, scrWidth, scrHeight));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::renderToCameraDepthBuffer(int scrWidth, int scrHeight, GLuint defaultFBO /*= 0*/) {
    assert(m_DepthBufferInitialized);
    assert(m_MeshObj != nullptr && m_Camera != nullptr && m_UBufferCamData != nullptr);
    if(m_MeshObj->isEmpty()) {
        return;
    }

    m_CameraDepthShader->bind();
    m_UBufferModelMatrix->bindBufferBase();
    m_CameraDepthShader->bindUniformBlock(m_CDSUBModelMatrix, m_UBufferModelMatrix->getBindingPoint());
    m_UBufferCamData->bindBufferBase();
    m_CameraDepthShader->bindUniformBlock(m_CDSUBCameraData, m_UBufferCamData->getBindingPoint());

    m_CameraDepthBufferRender->beginRender();
    glCall(glBindVertexArray(m_CDSVAO));
    m_MeshObj->draw();
    glCall(glBindVertexArray(0));
    m_CameraDepthBufferRender->endRender(defaultFBO);

    m_LightDepthShader->release();
    glCall(glViewport(0, 0, scrWidth, scrHeight));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::initDepthBufferData(const Vec3f& defaultClearColor) {
    if(m_MeshObj->isEmpty()) {
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // light depth buffer
    for(int i = 0; i < m_Lights->getNumLights(); ++i) {
        m_LightDepthBufferRenders.push_back(std::make_shared<DepthBufferRender>(m_ShadowBufferWidth, m_ShadowBufferHeight));
        m_LightDepthBufferRenders[i]->setDefaultClearColor(defaultClearColor);
        m_LightDepthBufferRenders[i]->setClearDepthValue(-1000000.0);
    }

    m_LightDepthShader   = ShaderProgram::getSimpleLightSpaceDepthShader();
    m_LDSAtrVPosition    = m_LightDepthShader->getAtributeLocation("v_Position");
    m_LDSUBModelMatrix   = m_LightDepthShader->getUniformBlockIndex("ModelMatrix");
    m_LDSUBLightMatrices = m_LightDepthShader->getUniformBlockIndex("LightMatrices");
    m_LDSULightID        = m_LightDepthShader->getUniformLocation("u_LightID");

    glCall(glGenVertexArrays(1, &m_LDSVAO));
    glCall(glBindVertexArray(m_LDSVAO));
    m_MeshObj->getVertexBuffer()->bind();
    glCall(glEnableVertexAttribArray(m_LDSAtrVPosition));
    glCall(glVertexAttribPointer(m_LDSAtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    if(m_MeshObj->hasIndexBuffer()) {
        m_MeshObj->getIndexBuffer()->bind();
    }
    glCall(glBindVertexArray(0));
    m_MeshObj->getVertexBuffer()->release();
    if(m_MeshObj->hasIndexBuffer()) {
        m_MeshObj->getIndexBuffer()->release();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // camera depth buffer
    m_CameraDepthBufferRender = std::make_shared<DepthBufferRender>(m_ShadowBufferWidth, m_ShadowBufferHeight);
    m_CameraDepthBufferRender->setDefaultClearColor(defaultClearColor);
    m_CameraDepthBufferRender->setClearDepthValue(-1000000.0);
    m_CameraDepthShader = ShaderProgram::getSimpleCameraSpaceDepthShader();
    m_CDSAtrVPosition   = m_CameraDepthShader->getAtributeLocation("v_Position");
    m_CDSUBModelMatrix  = m_CameraDepthShader->getUniformBlockIndex("ModelMatrix");
    m_CDSUBCameraData   = m_CameraDepthShader->getUniformBlockIndex("CameraData");
    glCall(glGenVertexArrays(1, &m_CDSVAO));
    glCall(glBindVertexArray(m_CDSVAO));
    m_MeshObj->getVertexBuffer()->bind();
    glCall(glEnableVertexAttribArray(m_CDSAtrVPosition));
    glCall(glVertexAttribPointer(m_CDSAtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    if(m_MeshObj->hasIndexBuffer()) {
        m_MeshObj->getIndexBuffer()->bind();
    }
    glCall(glBindVertexArray(0));
    m_MeshObj->getVertexBuffer()->release();
    if(m_MeshObj->hasIndexBuffer()) {
        m_MeshObj->getIndexBuffer()->release();
    }

    ////////////////////////////////////////////////////////////////////////////////
    m_DepthBufferInitialized = true;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::resizeShadowMap(int width, int height) {
    m_ShadowBufferWidth  = width;
    m_ShadowBufferHeight = height;

    for(int i = 0; i < m_Lights->getNumLights(); ++i) {
        m_LightDepthBufferRenders[i]->resize(width, height);
    }

    m_CameraDepthBufferRender->resize(width, height);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::setupVAO() {
    if(m_MeshObj->isEmpty()) {
        return;
    }

    glCall(glBindVertexArray(m_VAO));
    m_MeshObj->getVertexBuffer()->bind();
    glCall(glEnableVertexAttribArray(m_AtrVPosition));
    glCall(glVertexAttribPointer(m_AtrVPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));

    if(m_MeshObj->hasVertexNormal()) {
        m_MeshObj->getNormalBuffer()->bind();
        glCall(glEnableVertexAttribArray(m_AtrVNormal));
        glCall(glVertexAttribPointer(m_AtrVNormal, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    }
    if(m_MeshObj->hasVertexTexCoord()) {
        m_MeshObj->getTexCoordBuffer()->bind();
        glCall(glEnableVertexAttribArray(m_AtrVTexCoord));
        glCall(glVertexAttribPointer(m_AtrVTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    }
    //if(m_MeshObj->hasVertexColor())
    //{
    //    m_MeshObj->getVertexColorBuffer()->bind();
    //    glCall(glEnableVertexAttribArray(m_AtrVTexCoord));
    //    glCall(glVertexAttribPointer(m_AtrVTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0));
    //}

    if(m_MeshObj->hasIndexBuffer()) {
        m_MeshObj->getIndexBuffer()->bind();
    }
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<OpenGLTexture>& MeshRender::getLightShadowMap(int lightID /*= 0*/) {
    assert(m_LightDepthBufferRenders.size() > 0);
    return m_LightDepthBufferRenders[lightID]->getDepthBuffer();
}

StdVT<SharedPtr<OpenGLTexture>> MeshRender::getAllLightShadowMaps() {
    StdVT<SharedPtr<OpenGLTexture>> depthBuffers;
    for(int i = 0; i < static_cast<int>(m_LightDepthBufferRenders.size()); ++i) {
        depthBuffers.push_back(m_LightDepthBufferRenders[i]->getDepthBuffer());
    }

    return depthBuffers;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
SharedPtr<OpenGLTexture>& MeshRender::getCameraShadowMap() {
    return m_CameraDepthBufferRender->getDepthBuffer();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MeshRender::initRenderData() {
    assert(m_MeshObj != nullptr);

    m_Shader = ShaderProgram::getPhongShader();

    m_AtrVPosition = m_Shader->getAtributeLocation("v_Position");
    m_AtrVNormal   = m_Shader->getAtributeLocation("v_Normal");
    m_AtrVTexCoord = m_Shader->getAtributeLocation("v_TexCoord");

    m_UHasTexture = m_Shader->getUniformLocation("u_HasTexture");
    m_UHasShadow  = m_Shader->getUniformLocation("u_HasShadow");
    m_UTexSampler = m_Shader->getUniformLocation("u_TexSampler");
    for(int i = 0; i < LightData::MaxNLights; ++i) {
        char buff[128];
        NT_SPRINT(buff, "u_ShadowMap[%d]", i);
        m_UShadowMap[i] = m_Shader->getUniformLocation(buff);
    }
    m_UExposure = m_Shader->getUniformLocation("u_Exposure");
    u_ClipPlane = m_Shader->getUniformLocation("u_ClipPlane");

    m_UBModelMatrix   = m_Shader->getUniformBlockIndex("ModelMatrix");
    m_UBCamData       = m_Shader->getUniformBlockIndex("CameraData");
    m_UBLightMatrices = m_Shader->getUniformBlockIndex("LightMatrices");
    m_UBLight         = m_Shader->getUniformBlockIndex("Lights");
    m_UBMaterial      = m_Shader->getUniformBlockIndex("Material");

    ////////////////////////////////////////////////////////////////////////////////
    m_MeshObj->uploadDataToGPU();
    glCall(glGenVertexArrays(1, &m_VAO));
    setupVAO();

    ////////////////////////////////////////////////////////////////////////////////
    // uniform buffer
    if(m_UBufferCamData == nullptr) {
        m_UBufferCamData = std::make_shared<OpenGLBuffer> ();
        m_UBufferCamData->createBuffer(GL_UNIFORM_BUFFER, 5 * sizeof(Mat4x4f) + sizeof(Vec4f), nullptr, GL_DYNAMIC_DRAW);
    }

    m_UBufferModelMatrix = std::make_shared<OpenGLBuffer> ();
    m_UBufferModelMatrix->createBuffer(GL_UNIFORM_BUFFER, 2 * sizeof(Mat4x4f), nullptr, GL_STATIC_DRAW);
    transform(Vec3f(0, 0, 0), Vec3f(1, 1, 1));

    ////////////////////////////////////////////////////////////////////////////////
    // material
    if(m_Material == nullptr) {
        m_Material = std::make_shared<Material> ();
        /*m_Material->setAmbientColor(Vec4f(1.0));
           m_Material->setDiffuseColor(Vec4f(1.0, 1.0, 0.0, 1.0));
           m_Material->setSpecularColor(Vec4f(1.0));
           m_Material->setShininess(250.0);*/
        m_Material->setMaterial(BuildInMaterials::MT_Emerald);
        m_Material->uploadDataToGPU();
    }

    ////////////////////////////////////////////////////////////////////////////////
    // default null texture
    m_Textures.push_back(nullptr);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Plane render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
PlaneRender::PlaneRender(const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light, QString textureFolder,
                         const SharedPtr<OpenGLBuffer>& bufferCamData) :
    MeshRender(std::make_shared<MeshObjects::GridObject>(), camera, light, textureFolder, nullptr, bufferCamData) {}

PlaneRender::PlaneRender(const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light, const SharedPtr<OpenGLBuffer>& bufferCamData) :
    MeshRender(std::make_shared<MeshObjects::GridObject>(), camera, light, nullptr, bufferCamData) {}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::setAllowNonTextureRender(bool allowNonTex) {
    m_AllowedNonTexRender = allowNonTex;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::scaleTexCoord(int scaleX, int scaleY) {
    MeshObjects::GridObject* gridObj = static_cast<MeshObjects::GridObject*>(m_MeshObj.get());
    assert(gridObj != nullptr);
    gridObj->scaleTexCoord(scaleX, scaleY);
    gridObj->uploadDataToGPU();
    m_ScaleTexCoord = Vec2f(scaleX, scaleY);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void PlaneRender::render() {
    if(m_CurrentTexture != nullptr || (m_CurrentTexture == nullptr && m_AllowedNonTexRender)) {
        MeshRender::render();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace NTCodeBase

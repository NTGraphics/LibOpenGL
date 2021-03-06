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

#pragma once

#include <LibOpenGL/Forward.h>
#include <LibOpenGL/OpenGLMacros.h>
#include <LibOpenGL/LightAndMaterialData.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace NTCodeBase {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// abstract base class of object render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderObject : public OpenGLCallable {
public:
    const auto& getShader() const { return m_Shader; }

protected:
    RenderObject(const SharedPtr<Camera>& camera, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        m_Camera(camera), m_UBufferCamData(bufferCamData) {
        m_SelfUpdateCamera = (m_UBufferCamData == nullptr);
    }

    virtual void initRenderData() = 0;
    virtual void render()         = 0;

    GLuint m_UBModelMatrix;
    GLuint m_UBCamData;
    GLuint m_VAO;

    bool                     m_SelfUpdateCamera;
    SharedPtr<Camera>        m_Camera;
    SharedPtr<OpenGLBuffer>  m_UBufferModelMatrix;
    SharedPtr<OpenGLBuffer>  m_UBufferCamData;
    SharedPtr<ShaderProgram> m_Shader;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// SkyBox render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class SkyBoxRender : public RenderObject {
public:
#ifdef NT_QT_FRAMEWORK
    SkyBoxRender(const SharedPtr<Camera>& camera, QString texureTopFolder, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_CurrentTexture(nullptr) {
        initRenderData();
        loadTextures(texureTopFolder);
    }

    void loadTextures(QString textureTopFolder);
#endif

    SkyBoxRender(const SharedPtr<Camera>& camera, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_CurrentTexture(nullptr) {
        initRenderData();
    }

    SkyBoxRender(const SharedPtr<Camera>& camera, const StdVT<SharedPtr<OpenGLTexture>>& textures, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Textures(textures), m_CurrentTexture(nullptr) {
        initRenderData();
    }

    const SharedPtr<OpenGLTexture>& getCurrentTexture();
    size_t                          getNumTextures();
    void                            clearTextures();
    void                            addTexture(const SharedPtr<OpenGLTexture>& texture);
    void                            setRenderTextureIndex(int texIndex);
    void                            scale(float scaleX, float scaleY, float scaleZ);

    virtual void render() override;

protected:
    virtual void initRenderData() override;

    GLuint m_AtrVPosition;
    GLuint m_UTexSampler;
    SharedPtr<MeshObjects::CubeObject> m_CubeObj;
    SharedPtr<OpenGLTexture>           m_CurrentTexture;
    StdVT<SharedPtr<OpenGLTexture>>    m_Textures;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Light render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PointLightRender : public RenderObject {
public:
    PointLightRender(const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& lights, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_Lights(lights), m_RenderSize(20.0) {
        initRenderData();
    }

    void         setRenderSize(GLfloat renderSize);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    GLfloat m_RenderSize;
    GLuint  m_UBLight;
    const SharedPtr<PointLights>& m_Lights;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WirteFrameBoxRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class WireFrameBoxRender : public RenderObject {
public:
    WireFrameBoxRender(const SharedPtr<Camera>& camera, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData) {
        initRenderData();
    }

    const auto& getColor() const { return m_BoxColor; }

    void         setColor(const Vec3f& color) { m_BoxColor = color; }
    void         transform(const Vec3f& translation, const Vec3f& scale);
    void         setBox(const Vec3f& boxMin, const Vec3f& boxMax);
    virtual void render() override;
    ////////////////////////////////////////////////////////////////////////////////
    static Vec3f getDefaultBoxColor() { return Vec3f(0.0f, 1.0f, 0.5f); }

private:
    virtual void initRenderData() override;

    Vec3f  m_BoxColor = getDefaultBoxColor();
    GLuint m_AtrVPosition;
    GLuint m_UColor;
    SharedPtr<MeshObjects::WireFrameBoxObject> m_WireFrameBoxObj;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// OffScreenRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class OffScreenRender : public RenderObject {
public:
    OffScreenRender(int width, int height, int numColorBuffers = 1, GLenum formatColorBuffers = GL_RGBA) :
        RenderObject(nullptr, nullptr), m_BufferWidth(width), m_BufferHeight(height), m_NumColorBuffers(numColorBuffers), m_FormatColorBuff(formatColorBuffers) {
        initRenderData();
    }

    virtual void resize(int width, int height);
    virtual void beginRender();
    virtual void endRender(GLuint defaultFBO = 0);

    virtual void setNumColorBuffers(int numColorBuffers);
    void         setColorBufferParameter(GLenum paramName, GLenum paramValue);

    SharedPtr<OpenGLTexture>& getColorBuffer(int colorBufferID = 0);
    void                      swapColorBuffer(SharedPtr<OpenGLTexture>& colorBuffer, int bufferID     = 0);
    void                      fastSwapColorBuffer(SharedPtr<OpenGLTexture>& colorBuffer, int bufferID = 0);

    virtual void render() override {} // do nothing

protected:
    virtual void initRenderData() override;

    int    m_BufferWidth;
    int    m_BufferHeight;
    int    m_NumColorBuffers;
    GLenum m_FormatColorBuff;
    GLuint m_FrameBufferID;
    GLuint m_RenderBufferID;
    StdVT<SharedPtr<OpenGLTexture>> m_ColorBuffers;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// DepthBufferRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class DepthBufferRender : public OffScreenRender {
public:
    DepthBufferRender(int width = 1024, int height = 1024) :
        OffScreenRender(width, height, 1, GL_R32F), m_ClearLinearDepthValue(-1.0e6), m_DefaultClearColor(Vec3f(0.8, 0.8, 0.8)) {}

    virtual void beginRender() override;
    virtual void endRender(GLuint defaultFBO /* = 0 */) override;

    virtual void setNumColorBuffers(int numColorBuffers) override;

    void                      setDefaultClearColor(const Vec3f& clearColor);
    void                      setClearDepthValue(GLfloat clearValue);
    SharedPtr<OpenGLTexture>& getDepthBuffer();

private:
    virtual void initRenderData() override;

    GLfloat m_ClearLinearDepthValue;
    Vec3f   m_DefaultClearColor;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// ScreenQuadTextureRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ScreenQuadTextureRender : public RenderObject {
public:
    ScreenQuadTextureRender() : RenderObject(nullptr, nullptr), m_Texture(nullptr), m_TexelSizeValue(1), m_ValueScale(1.0) { initRenderData(); }
    ScreenQuadTextureRender(SharedPtr<OpenGLTexture>& texture, int texelSize = 1) :
        RenderObject(nullptr, nullptr), m_Texture(texture), m_TexelSizeValue(texelSize), m_ValueScale(1.0) { initRenderData(); }

    void         setValueScale(float scale);
    void         setTexture(const SharedPtr<OpenGLTexture>& texture, int texelSize = 1);
    virtual void render() override;

private:
    virtual void initRenderData() override;

    float                    m_ValueScale;
    int                      m_TexelSizeValue;
    GLuint                   m_UTexelSize;
    GLuint                   m_UValueScale;
    GLuint                   m_UTexSampler;
    SharedPtr<OpenGLTexture> m_Texture;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CheckerboardBackgroundRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class CheckerboardBackgroundRender : public RenderObject {
public:
    CheckerboardBackgroundRender() : RenderObject(nullptr, nullptr) { initRenderData(); }
    CheckerboardBackgroundRender(const Vec3f& color1, const Vec3f& color2) : RenderObject(nullptr, nullptr) { initRenderData(); setColors(color1, color2); }
    CheckerboardBackgroundRender(const Vec4f& color1, const Vec4f& color2) : RenderObject(nullptr, nullptr) { initRenderData(); setColors(color1, color2); }

    const auto& getScales() const { return m_Scales; }
    const auto& getColor1() const { return m_Color1; }
    const auto& getColor2() const { return m_Color2; }

    void         setScales(const Vec2i& scales) { m_Scales = scales; }
    void         setScreenSize(int width, int height) { m_ScreenWidth = width; m_ScreenHeight = height; }
    void         setColor1(const Vec3f& color1) { m_Color1 = Vec4f(color1, 1.0); }
    void         setColor1(const Vec4f& color1) { m_Color1 = color1; }
    void         setColor2(const Vec3f& color2) { m_Color2 = Vec4f(color2, 1.0); }
    void         setColor2(const Vec4f& color2) { m_Color2 = color2; }
    void         setColors(const Vec3f& color1, const Vec3f& color2) { setColor1(color1); setColor2(color2); }
    void         setColors(const Vec4f& color1, const Vec4f& color2) { setColor1(color1); setColor2(color2); }
    virtual void render() override;
    ////////////////////////////////////////////////////////////////////////////////
    static Vec2i getDefaultScales() { return Vec2i(20); }
    static Vec4f getDefaultColor1() { return Vec4f(0.9f, 0.9f, 0.9f, 1.0f); }
    static Vec4f getDefaultColor2() { return Vec4f(0.5f, 0.5f, 0.5f, 1.0f); }
private:
    virtual void initRenderData() override;

    GLint m_ScreenWidth  = 2;
    GLint m_ScreenHeight = 2;
    Vec2i m_Scales       = getDefaultScales();
    Vec4f m_Color1       = getDefaultColor1();
    Vec4f m_Color2       = getDefaultColor2();

    GLuint m_UScales;
    GLuint m_UScreenWidth;
    GLuint m_UScreenHeight;
    GLuint m_UColor1;
    GLuint m_UColor2;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// GridBackgroundRender
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class GridBackgroundRender : public RenderObject {
public:
    GridBackgroundRender() : RenderObject(nullptr, nullptr) { initRenderData(); }
    GridBackgroundRender(const Vec3f& backgroundColor, const Vec3f& lineColor) :
        RenderObject(nullptr, nullptr), m_BackgroundColor(backgroundColor, 1.0f), m_LineColor(lineColor, 1.0f) { initRenderData(); }
    GridBackgroundRender(const Vec4f& backgroundColor, const Vec4f& lineColor) :
        RenderObject(nullptr, nullptr), m_BackgroundColor(backgroundColor), m_LineColor(lineColor) { initRenderData(); }

    const auto& getScales() const { return m_Scales; }
    const auto& getBackgroundColor() const { return m_BackgroundColor; }
    const auto& getLineColor() const { return m_LineColor; }

    void         setScales(const Vec2i& scales) { m_Scales = scales; }
    void         setScreenSize(int width, int height) { m_ScreenWidth = width; m_ScreenHeight = height; }
    void         setBackgroundColor(const Vec3f& backgroundColor) { m_BackgroundColor = Vec4f(backgroundColor, 1.0); }
    void         setBackgroundColor(const Vec4f& backgroundColor) { m_BackgroundColor = backgroundColor; }
    void         setLineColor(const Vec3f& lineColor) { m_LineColor = Vec4f(lineColor, 1.0); }
    void         setLineColor(const Vec4f& lineColor) { m_LineColor = lineColor; }
    void         setColors(const Vec3f& backgroundColor, const Vec3f& lineColor) { setBackgroundColor(backgroundColor); setLineColor(lineColor); }
    void         setColors(const Vec4f& backgroundColor, const Vec4f& lineColor) { setBackgroundColor(backgroundColor); setLineColor(lineColor); }
    virtual void render() override;
    ////////////////////////////////////////////////////////////////////////////////
    static Vec2i getDefaultScales() { return Vec2i(20); }
    static Vec4f getDefaultBackgroundColor() { return Vec4f(0.9f, 0.9f, 0.9f, 1.0f); }
    static Vec4f getDefaultLineColor() { return Vec4f(0.5f, 0.5f, 0.5f, 1.0f); }

private:
    virtual void initRenderData() override;

    GLint m_ScreenWidth     = 2;
    GLint m_ScreenHeight    = 2;
    Vec2i m_Scales          = getDefaultScales();
    Vec4f m_BackgroundColor = getDefaultBackgroundColor();
    Vec4f m_LineColor       = getDefaultLineColor();

    GLuint m_UScales;
    GLuint m_UScreenWidth;
    GLuint m_UScreenHeight;
    GLuint m_UBackgroundColor;
    GLuint m_ULineColor;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Mesh render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MeshRender : public RenderObject {
public:
#ifdef NT_QT_FRAMEWORK
    MeshRender(const SharedPtr<MeshObjects::MeshObject>& meshObj, const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light,
               QString textureFolder,
               const SharedPtr<Material>& material = nullptr, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr);

    void loadTextures(QString textureFolder);
#endif

    MeshRender(const SharedPtr<MeshObjects::MeshObject>& meshObj, const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light,
               const SharedPtr<Material>& material = nullptr, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr) :
        RenderObject(camera, bufferCamData), m_MeshObj(meshObj), m_Lights(light), m_Material(material), m_CurrentTexture(nullptr),
        m_ShadowBufferWidth(1024), m_ShadowBufferHeight(1024), m_Exposure(1.0f) {
        initRenderData();
    }

    const auto& getMeshObj() const { return m_MeshObj; }
    const auto& getMaterial() const { return m_Material; }
    const auto& getCurrentTexture() const { return m_CurrentTexture; }
    auto        getNumTextures() const { return m_Textures.size(); }
    const auto& getTranslation() const { return m_Translation; }
    const auto& getScales() const { return m_Scales; }

    void clearTextures(bool insertNullTex = true);
    void addTexture(const SharedPtr<OpenGLTexture>& texture, GLenum texWrapMode = GL_REPEAT);
    void setRenderTextureIndex(int texIndex);
    void setExternalShadowMaps(const StdVT<SharedPtr<OpenGLTexture>>& shadowMaps);
    void resizeShadowMap(int width, int height);
    void setExposure(float exposure) { m_Exposure = exposure; }
    void setClipPlane(const Vec4f& clipPlane) { m_ClipPlane = clipPlane; }
    void transform(const Vec3f& translation, const Vec3f& scales);
    void translate(const Vec3f& translation) { m_Translation = translation; transform(translation, m_Scales); }
    void scale(const Vec3f& scales) { m_Scales = scales; transform(m_Translation, scales); }
    void setupVAO();

    SharedPtr<OpenGLTexture>&       getLightShadowMap(int lightID = 0);
    StdVT<SharedPtr<OpenGLTexture>> getAllLightShadowMaps();
    SharedPtr<OpenGLTexture>&       getCameraShadowMap();

    virtual void render() override;

    virtual void initDepthBufferData(const Vec3f& defaultClearColor);
    virtual void renderToLightDepthBuffer(int scrWidth, int scrHeight, GLuint defaultFBO  = 0);
    virtual void renderToCameraDepthBuffer(int scrWidth, int scrHeight, GLuint defaultFBO = 0);

protected:
    virtual void initRenderData() override;

    GLuint m_AtrVPosition;
    GLuint m_AtrVNormal;
    GLuint m_AtrVTexCoord;
    GLuint m_UBLight;
    GLuint m_UBLightMatrices;
    GLuint m_LDSULightID;
    GLuint m_UBMaterial;
    GLuint m_UHasTexture;
    GLuint m_UHasShadow;
    GLuint m_UTexSampler;
    GLuint m_UShadowMap[LightData::MaxNLights];
    GLuint m_UExposure;
    GLuint u_ClipPlane;
    SharedPtr<MeshObjects::MeshObject> m_MeshObj;
    SharedPtr<PointLights>             m_Lights;
    SharedPtr<Material>                m_Material;
    StdVT<SharedPtr<OpenGLTexture>>    m_Textures;
    SharedPtr<OpenGLTexture>           m_CurrentTexture;
    StdVT<SharedPtr<OpenGLTexture>>    m_ExternalShadowMaps;

    bool    m_DepthBufferInitialized = false;
    GLint   m_ShadowBufferWidth;
    GLint   m_ShadowBufferHeight;
    GLfloat m_Exposure;
    GLuint  m_LDSAtrVPosition;
    GLuint  m_LDSUBLightMatrices;
    GLuint  m_LDSUBModelMatrix;
    GLuint  m_LDSVAO;
    GLuint  m_CDSAtrVPosition;
    GLuint  m_CDSUBModelMatrix;
    GLuint  m_CDSUBCameraData;
    GLuint  m_CDSVAO;
    Vec4f   m_ClipPlane = Vec4f(1.0f, 0.0f, 0.0f, 0.0f);

    Vec3f                               m_Translation = Vec3f(0);
    Vec3f                               m_Scales      = Vec3f(1.0);
    SharedPtr<ShaderProgram>            m_LightDepthShader;
    SharedPtr<ShaderProgram>            m_CameraDepthShader;
    StdVT<SharedPtr<DepthBufferRender>> m_LightDepthBufferRenders;
    SharedPtr<DepthBufferRender>        m_CameraDepthBufferRender;
    Mat4x4f                             m_LightView[LightData::MaxNLights];
    Mat4x4f                             m_LightProjection[LightData::MaxNLights];
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Plane render
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class PlaneRender : public MeshRender {
public:
#ifdef NT_QT_FRAMEWORK
    PlaneRender(const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light, QString textureFolder,
                const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr);
#endif

    PlaneRender(const SharedPtr<Camera>& camera, const SharedPtr<PointLights>& light, const SharedPtr<OpenGLBuffer>& bufferCamData = nullptr);

    const auto& getScaleTexCoord() const { return m_ScaleTexCoord; }
    void setAllowNonTextureRender(bool allowNonTex);
    void scaleTexCoord(int scaleX, int scaleY);

    virtual void render() override;

protected:
    Vec2f m_ScaleTexCoord       = Vec2f(1.0);
    bool  m_AllowedNonTexRender = true;
};
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace NTCodeBase

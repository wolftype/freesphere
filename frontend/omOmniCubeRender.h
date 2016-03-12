#pragma once

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#include <iostream>
#include <fstream> // std::ifstream
#include <string>

namespace om {

static const char * vGeneric = R"(
    varying vec2 T;
    void main(void) {
        // pass through the texture coordinate (normalized pixel):
        T = vec2(gl_MultiTexCoord0);
        gl_Position = vec4(T*2.-1., 0, 1);
    }
)";

static const char * fCube = R"(
    uniform sampler2D pixelMap;
    uniform sampler2D alphaMap;
    uniform samplerCube cubeMap;

    varying vec2 T;

    void main (void){
        // ray location (calibration space):
        vec3 v = normalize(texture2D(pixelMap, T).rgb);

        // index into cubemap:
        vec3 rgb = textureCube(cubeMap, v).rgb * texture2D(alphaMap, T).rgb;

        gl_FragColor = vec4(rgb, 1.);
    }
)";

class OmniCubeRender {
public:
    int res = 2048;
    int mFace = 0;
    int mCurrentEye = 0;
    float modelview[16];
    float mSphereRadius; // The radius of the sphere in OpenGL units.
    float mEyeParallax, mNear, mFar;

    GLuint mCubeTex[2];
    GLuint mRbo;
    GLuint mFbo;
    GLuint warp;
    GLuint blend;
    GLuint cubemapProgram;

    std::string cubemapvert = "cubemap.vert";
    std::string cubemapfrag = "cubemap.frag";

    void resolution(int r) {
        res = r;
    }
    void config(std::string) {

    }
    void init() {
        initCubeProgram();
        initCubemapTexture();
    }
    void warpTex(GLuint id) {
        warp = id;
    }
    void blendTex(GLuint id) {
        blend = id;
    }
    void pos(float x, float y, float z) {

    }
    void dir(float ux, float uy, float uz, float vx, float vy, float vz, float wx, float wy, float wz) {

    }
    void begin() {

    }
    bool nextFaceExists() {
        return false;
    }
    void prepareFace() {

    }
    void end() {

    }
    void draw() {

    }

private:
    void initCubeProgram();
    void initCubemapTexture();
};

std::string loadFileToString(std::string path) {
    std::ifstream fileStream(path);
    std::string fileString((std::istreambuf_iterator<char>(fileStream)), (std::istreambuf_iterator<char>()));
    return fileString;
}

inline void OmniCubeRender::initCubeProgram() {
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    cubemapProgram = glCreateProgram();

    // use this when shader gets to be separate file
    // const char* vertsource = loadFileToString(cubemapvert).c_str();
    // glShaderSource(vert, 1, &vertsource, NULL);
    // const char* fragsource = loadFileToString(cubemapfrag).c_str();
    // glShaderSource(frag, 1, &fragsource, NULL);

    glShaderSource(vert, 1, &vGeneric, NULL);
    glShaderSource(frag, 1, &fCube, NULL);

    glCompileShader(vert);
    glCompileShader(frag);

    glAttachShader(cubemapProgram, vert);
    glAttachShader(cubemapProgram, frag);

    glLinkProgram(cubemapProgram);

    GLint blendLoc = glGetUniformLocation(cubemapProgram, "alphaMap");
    GLint warpLoc = glGetUniformLocation(cubemapProgram, "pixelMap");
    GLint cubeLoc = glGetUniformLocation(cubemapProgram, "sphereMap");

    glUseProgram(cubemapProgram);

    glUniform1i(blendLoc, 2);
    glUniform1i(warpLoc, 1);
    glUniform1i(cubeLoc, 0);
    glUseProgram(0);
}

inline void OmniCubeRender::initCubemapTexture() {
    glGenTextures(2, mCubeTex);
    for (int i = 0; i < 2; i++) {
        // create cubemap texture:
        glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeTex[i]);

        // each cube face should clamp at texture edges:
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // filtering
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // TODO: verify?
        // Domagoj also has:
        glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
        glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
        glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
        float X[4] = { 1,0,0,0 };
        float Y[4] = { 0,1,0,0 };
        float Z[4] = { 0,0,1,0 };
        glTexGenfv( GL_S, GL_OBJECT_PLANE, X );
        glTexGenfv( GL_T, GL_OBJECT_PLANE, Y );
        glTexGenfv( GL_R, GL_OBJECT_PLANE, Z );

        // RGBA8 Cubemap texture, 24 bit depth texture, res x res
        // NULL means reserve texture memory, but texels are undefined
        for (int f=0; f<6; f++) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X+f,
                0, GL_RGBA8,
                res, res,
                0, GL_BGRA, GL_UNSIGNED_BYTE,
                NULL);
        }

        // clean up:
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // one FBO to rule them all...
    glGenFramebuffers(1, &mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    //Attach one of the faces of the Cubemap texture to this FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, mCubeTex[0], 0);

    glGenRenderbuffers(1, &mRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, mRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, res, res);
    // Attach depth buffer to FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRbo);

    // ...and in the darkness bind them:
    for (mFace=0; mFace<6; mFace++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+mFace, GL_TEXTURE_CUBE_MAP_POSITIVE_X+mFace, mCubeTex[0], 0);
    }

    //Does the GPU support current FBO configuration?
    GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("GPU does not support required FBO configuration\n");
    }

    // cleanup:
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace om
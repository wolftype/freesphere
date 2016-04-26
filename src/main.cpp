#include "allocore/io/al_App.hpp"
#include "omConfig.hpp"
#include "omTexture.hpp"
#include "omShader.hpp"
#include "omFBO.hpp"

using namespace al;
using namespace om;

// what needs to be sent to capture shader
// in alloutil, it's done by omni().uniforms(shader());
struct CaptureShaderUniforms {
  int omni_face;
  float omni_eye;
  float omni_radius;
  float omni_near;
  float omni_far;
  float lighting;
  float texture;
};

struct MyApp : public App {
  Config omniConfig;
  om::FBO fbo;
  om::CubeMap cubeMap;
  std::vector<om::Texture> tex;
  om::ShaderProgram captureShader, warpShader;
  Mesh mesh;

  MyApp(){
    initWindow( Window::Dim(1024,1024) );
  }

  /// OpenGL context exists when onCreate is called
  virtual void onCreate( const ViewpointWindow& w ) override {

    //Load Warp and Blend Data
    omniConfig.loadConfig( "OmniRender/configFiles/projectorConfigurationTemplate.txt" );
    omniConfig.loadData("tmp");
    omniConfig.printConfig();

    //Set up cube map and fbo
    cubeMap.init(1024);
    fbo.init(1024,1024);
    fbo.attach(cubeMap);

    //Load textures (WARP)
    for (auto& i : omniConfig.mProjector){
      tex.push_back( om::Texture(i.width, i.height ) );
      tex.back().update( i.data );
    }

    // TODO: also load blend textures

    //Init shader
    captureShader.load("OmniRender/include/omCapture.vert", "OmniRender/include/omCapture.frag", true);
    warpShader.load("OmniRender/include/omWarp.vert", "OmniRender/include/omWarp.frag");

    // glUseProgram(warpShader.program); {
    //   GLint h = glGetUniformLocation(warpShader.program, "warpMap");
    //   GLint i = glGetUniformLocation(warpShader.program, "cubeMap");
    //   glUniform1i(h, 2);
    //   glUniform1i(i, 1);
    //   // also get blend location later
    //   warpShader.validate();
    // } glUseProgram(0);

    warpShader.begin(); {
      warpShader.uniform1i("warpMap", 2);
      warpShader.uniform1i("cubeMap", 1);
      warpShader.validate();
    } warpShader.end();

    addOctahedron(mesh);
  }

  void drawQuad(Graphics &g) {
    // glEnable(GL_BLEND);
    // glEnable(GL_TEXTURE_2D);

    g.pushMatrix(g.PROJECTION);
    g.loadIdentity();
    g.pushMatrix(g.MODELVIEW);
    g.loadIdentity();
    g.depthMask(0); // write only to color buffer

    Mesh& m = g.mesh();
    m.reset();
    m.primitive(g.TRIANGLE_STRIP);
      m.texCoord	( 0, 0);
      m.vertex	(-1, -1, 0);
      m.texCoord	( 1, 0);
      m.vertex	(1, -1, 0);
      m.texCoord	( 0, 1);
      m.vertex	(-1, 1, 0);
      m.texCoord	( 1, 1);
      m.vertex	(1, 1, 0);
    g.draw(m);

    g.depthMask(1);
    g.popMatrix(g.PROJECTION);
    g.popMatrix(g.MODELVIEW);    
  }

  void drawQuad2() {
    // native gl2 calls,
    // but remember we don't have most of these in gl3
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDepthMask(GL_FALSE);

    float vertices[12] = {-1,-1, 0,
                           1,-1, 0,
                          -1, 1, 0,
                           1, 1, 0};
    float texcoords[8] = {0, 0,
                          1, 0,
                          0, 1,
                          1, 1};

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDepthMask(GL_TRUE);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // or we can also do something like:
    // which works almost same in gl2 and gl3

    /*
      [1] in cpu code, only one line, "draw 4 points" {
        glDrawArrays(GL_TRIANLGE_STRIP, 0 ,4);
      }
      
      [2] in vertex shader code {
        vec4 quad_vertices[4] = vec4[4](vec4(-1, -1, 0.5, 1.0),
                                        vec4( 1, -1, 0.5, 1.0),
                                        vec4(-1,  1, 0.5, 1.0),
                                        vec4( 1,  1, 0.5, 1.0));
        vec2 quad_texcoords[4] = vec2[4](vec2(0, 0),
                                         vec2(1, 0),
                                         vec2(0, 1),
                                         vec2(1, 1));
        gl_Position = quad_vertices[gl_VertexID];
        varying_texcoord = quad_texcoords[gl_VertexID];
      }
    */
  }

  virtual void onDraw( Graphics& g ) override {

    // glUseProgram(captureShader.program);

    // GLint a = glGetUniformLocation(captureShader.program, "omni_eye");
    // GLint b = glGetUniformLocation(captureShader.program, "omni_radius");
    // GLint c = glGetUniformLocation(captureShader.program, "omni_face");
    // GLint d = glGetUniformLocation(captureShader.program, "omni_near");
    // GLint e = glGetUniformLocation(captureShader.program, "omni_far");
    // GLint f = glGetUniformLocation(captureShader.program, "lighting");
    // GLint g2 = glGetUniformLocation(captureShader.program, "texture");

    // glUniform1f(a, 0.0);
    // glUniform1f(b, 1e10);
    // glUniform1f(d, 0.01);
    // glUniform1f(e, 100.0);
    // glUniform1f(f, 0.0);
    // glUniform1f(g2, 0.0);

    /* REMIND USER TO SEND UNIFORM VARIABLES BEFORE CAPTURING
       IF THIS IS DONE IN WHILE CAPTURING, WE SEND SAME UNIFORM 6 TIMES.
       (EXCEPT "omni_face") */

    captureShader.begin(); {
      captureShader.uniform1f("omni_eye", 0.0); // non stereo, for now
      captureShader.uniform1f("omni_radius", 1e10);
      captureShader.uniform1f("omni_near", 0.01);
      captureShader.uniform1f("omni_far", 100.0);
      captureShader.uniform1f("lighting", 0.0);
      captureShader.uniform1f("texture", 0.0);
    } captureShader.end();

    fbo.bind();
    for (int i = 0; i < 6; i++){
      // glUniform1i(c, i);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, cubeMap.id(), 0);

      // g.clearColor(Color(0.f));
      glClearColor(0.0, 0.0, 0.0, 1.0);
      // g.clear(g.COLOR_BUFFER_BIT | g.DEPTH_BUFFER_BIT);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      /* USER CODE STARTS HERE */

          // in practice, this capture shader will be written by user
          // and also bound by user.
          // [!] OR DO WE WANT USER TO REGISTER HIS/HER SHADER AND
          // LET THE LIB BIND IT (OMNIRENDER CLASS HAVING POINTER TO USER SHADER)
          // or we user "CaptureShaderUniforms" struct so user can easily
          // send the uniforms here (not caring about sending same thing 6 times)
          captureShader.begin(); {
            // below line needs fix: don't make user do this
            // find some way to get it done automatically
            captureShader.uniform1i("omni_face", i); 
            g.draw(mesh);
          } captureShader.end();
      
      /* AND ENDS HERE */
    }
    fbo.unbind();
    fbo.checkStatus();

    // Draw fbo result
    // glActiveTexture(GL_TEXTURE2);
    // glEnable(GL_TEXTURE_2D);
    // glBindTexture(GL_TEXTURE_2D, tex.back().id());
    // glDisable(GL_TEXTURE_2D);

    // captureShader.uniform1i("warpMap", 2);

    /* WHEN BLEND LOADING CODE DONE, BIND IT */
    // captureShader.uniformTexture2D(tex_blend.back(), "blendMap", 2);

    // glActiveTexture(GL_TEXTURE1);
    // glEnable(GL_TEXTURE_CUBE_MAP);
    // glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.id());
    // glDisable(GL_TEXTURE_CUBE_MAP);

    // glUseProgram(warpShader.program);
    warpShader.begin(); {
        // g.clearColor(Color(0.f));
        glClearColor(0.0, 0.0, 0.0, 1.0);
        // g.clear(g.COLOR_BUFFER_BIT | g.DEPTH_BUFFER_BIT);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        tex.back().bind(2);
        cubeMap.bind(1);
        // drawQuad(g);
        drawQuad2();
        tex.back().unbind(2);
        cubeMap.unbind(1);
    } warpShader.end();
  }

};

int main(){

  MyApp app;
  app.start();

  return 0;
}

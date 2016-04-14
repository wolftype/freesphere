


#include "allocore/io/al_App.hpp"
#include "omConfig.hpp"
#include "omTexture.hpp"
#include "omShader.hpp"
#include "omFBO.hpp"

using namespace al;
using namespace om;

struct MyApp : public App {

  Config omniConfig;
  om::FBO fbo;
  om::CubeMap cubeMap;
  std::vector<om::Texture> tex;
  om::ShaderProgram captureShader, warpShader;
  Mesh mesh;

  MyApp(){
    initWindow( Window::Dim(800,400) );
  }

  /// OpenGL context exists when onCreate is called
  virtual void onCreate( const ViewpointWindow& w ) override {

    //Load Warp and Blend Data
    omniConfig.loadConfig( "OmniRender/configFiles/projectorConfigurationTemplate.txt" );
    omniConfig.loadData("tmp");
    omniConfig.printConfig();

    //Set up cube map and fbo
    cubeMap.init(1024);
    fbo.init(800,400);
    fbo.attach(cubeMap);

    //Load textures
    for (auto& i : omniConfig.mProjector){
      tex.push_back( om::Texture(i.width, i.height ) );
      tex.back().update( i.data );
    }

    //Init shader
    captureShader.load("OmniRender/include/omCapture.vert", "OmniRender/include/omCapture.frag", true);
    warpShader.load("OmniRender/include/omWarp.vert", "OmniRender/include/omWarp.frag");

    glUseProgram(warpShader.program);
    GLint h = glGetUniformLocation(warpShader.program, "warpMap");
    GLint i = glGetUniformLocation(warpShader.program, "cubeMap");
    glUniform1i(h, 0);
    glUniform1i(i, 1);
    warpShader.validate();

    addOctahedron(mesh);
  }

  /* AFTER LUNCH:
    - build a cube map with shader
    - draw results
  */
  void drawQuad(Graphics &g) {
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    g.pushMatrix(g.PROJECTION);
    g.loadIdentity();
    g.pushMatrix(g.MODELVIEW);
    g.loadIdentity();
    g.depthMask(0); // write only to color buffer

      // QUAD PART
      // tex.back().bind();
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
      // tex.back().unbind();

    g.depthMask(1);
    g.popMatrix(g.PROJECTION);
    g.popMatrix(g.MODELVIEW);
  }

  virtual void onDraw( Graphics& g ) override {

    glUseProgram(captureShader.program);

    GLint a = glGetUniformLocation(captureShader.program, "omni_eye");
    GLint b = glGetUniformLocation(captureShader.program, "omni_radius");
    GLint c = glGetUniformLocation(captureShader.program, "omni_face");
    GLint d = glGetUniformLocation(captureShader.program, "omni_near");
    GLint e = glGetUniformLocation(captureShader.program, "omni_far");
    GLint f = glGetUniformLocation(captureShader.program, "lighting");
    GLint g2 = glGetUniformLocation(captureShader.program, "texture");

    glUniform1f(a, 0.0);
    glUniform1f(b, 1e10);
    glUniform1f(d, 0.01);
    glUniform1f(e, 100.0);
    glUniform1f(f, 0.0);
    glUniform1f(g2, 0.0);

    fbo.bind();
    for (int i = 0; i < 6; i++){
      glUniform1i(c, i);
      glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, cubeMap.id(), 0);

      // g.pushMatrix();
        // g.translate(i,0,0);
        // glColor4f((float)1/i,0,1,1);
        g.draw(mesh);
      // g.popMatrix();
    }
    fbo.unbind();
    fbo.checkStatus();


    // Draw fbo result
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.back().id());

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.id());

    glUseProgram(warpShader.program);

    drawQuad(g);

  }

};

int main(){

  MyApp app;
  app.start();

  return 0;
}

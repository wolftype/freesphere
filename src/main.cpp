


#include "allocore/io/al_App.hpp"
#include "omConfig.hpp"
#include "omTexture.hpp"
#include "omShader.hpp"

using namespace al;
using namespace om;

struct MyApp : public App {

  Config omniConfig;
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

    // //scale the data
    // for (auto &i: omniConfig.mProjector) {
    //   for (int j = 0; j < i.width*i.height; j++) {
    //     i.data[4*j+0] *= .1;
    //     i.data[4*j+1] *= .1;
    //     i.data[4*j+2] *= .1;
    //     i.data[4*j+3] *= .1;
    //   }
    // }

    //Load textures
    for (auto& i : omniConfig.mProjector){
      tex.push_back( om::Texture(i.width, i.height ) );
      tex.back().update( i.data );
    }

    //Init shader
    captureShader.load("OmniRender/include/omCapture.vert", "OmniRender/include/omCapture.frag");
    // warpShader.load("OmniRender/include/omWarp.vert", "OmniRender/include/omWarp.frag");

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
      tex.back().bind();
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
      tex.back().unbind();

    g.depthMask(1);
    g.popMatrix(g.PROJECTION);
    g.popMatrix(g.MODELVIEW);
  }

  virtual void onDraw( Graphics& g ) override {
    // drawQuad(g);

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

    for (int i = 0; i < 6; i++){
      glUniform1i(c, i);
      g.draw(mesh);
    }


  }

};

int main(){

  MyApp app;
  app.start();

  return 0;
}

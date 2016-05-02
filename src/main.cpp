#include "allocore/io/al_App.hpp"

#include "omRender.hpp"

using namespace al;

struct MyApp : public App {
  om::Render render;
  int code = 1;
  Mesh mesh;

  MyApp(){
    initWindow( Window::Dim(1024,1024) );
  }

  /// OpenGL context exists when onCreate is called
  virtual void onCreate( const ViewpointWindow& w ) override {
    // initialize om::render
    render.init("OmniRender/configFiles/projectorConfigurationTemplate.txt", "tmp");
    render.resize(1024, 1024);
    render.radius(1e10)
          .near(0.1)
          .far(1000)
          .eyeSep(0);

    addOctahedron(mesh);
  }

  void rawWorkFlow(Graphics& g) {
    render.begin();
    for (int i = 0; i < 6; i++){
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
                               render.cubeMap.id(), 0);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* USER CODE STARTS HERE */
        render.captureShader.begin(); {
            render.captureShader.uniform1i("omni_face", i);
            render.captureShader.uniform1f("omni_eye", 0);
            render.captureShader.uniform1f("lighting", 0.0);
            render.captureShader.uniform1f("texture", 0.0);
            g.draw(mesh);
        } render.captureShader.end();

        /* AND ENDS HERE */
    }
    render.end();
  }

#define OMNIRENDERBEGIN render.beginNoUserShader(); for (int i = 0; i < render.mStereo + 1; i++) { for (int j = 0; j < 6; j++) { render.faceBeginNoUserShader(i, j);
#define OMNIRENDEREND render.faceEndNoUserShader(); } } render.endNoUserShader();

  void noUserShader(Graphics& g) {
    // set these before rendering
    render.clearColor(0.0, 0.0, 0.0, 1.0)
          .lighting(0.0)
          .texture(0.0);

    OMNIRENDERBEGIN
    // render.beginNoUserShader();
    // for (int i = 0; i < render.mStereo + 1; i++) {
    //   for (int j = 0; j < 6; j++) {
    //     render.faceBeginNoUserShader(i, j);

        /* USER CODE STARTS HERE */
        g.draw(mesh);
        /* ENDS HERE */

    //     render.faceEndNoUserShader();
    //   }
    // }
    // render.endNoUserShader();
    OMNIRENDEREND
  }

#undef OMNIBEGIN
#undef OMNIEND

  virtual void onDraw( Graphics& g ) override {
    switch (code) {
      case 1: rawWorkFlow(g); break;
      case 2: noUserShader(g); break;
    }
  }

  virtual void onKeyDown(const Keyboard& k) override {
    if ('0' < k.key() && k.key() <= '9') {
      code = k.key() - '0';
      std::cout << "code: " << code << std::endl;
    }
  }
};

int main(){
  MyApp app;
  app.start();
  return 0;
}

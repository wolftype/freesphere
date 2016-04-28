#include "allocore/io/al_App.hpp"

#include "omRender.hpp"

using namespace al;

struct MyApp : public App {
  om::Render render;

  Mesh mesh;

  MyApp(){
    initWindow( Window::Dim(1024,1024) );
  }

  /// OpenGL context exists when onCreate is called
  virtual void onCreate( const ViewpointWindow& w ) override {
    // initialize om::render
    render.init("OmniRender/configFiles/projectorConfigurationTemplate.txt", "tmp");
    render.resize(1024, 1024);
    addOctahedron(mesh);
  }

  virtual void onDraw( Graphics& g ) override {

    render.begin();
    for (int i = 0; i < 6; i++){
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, render.cubeMap.id(), 0);
      glClearColor(0.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      /* USER CODE STARTS HERE */
      render.captureShader.begin(); {
        render.captureShader.uniform1i("omni_face", i);

        g.draw(mesh);

      } render.captureShader.end();
      
      /* AND ENDS HERE */
    }
    render.end();
  }

};

int main(){

  MyApp app;
  app.start();

  return 0;
}

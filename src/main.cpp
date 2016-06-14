#include "allocore/io/al_App.hpp"

#include "omRender.hpp"

using namespace al;

struct MyApp : public App {
  om::Render render;
  int code = 1;
  Mesh mesh;

  MyApp(){
    /// User MUST set window buffer to support Active stereo
    /// This isn't necessary for sequential, anaglyph, etc
    initWindow( Window::Dim(600,400),
                "Freesphere 1.0",
                60,
                Window::DEFAULT_BUF);
  }

  /// OpenGL context exists when onCreate is called
  virtual void onCreate( const ViewpointWindow& w ) override {
    // initialize om::render "tmp" doesn't get used!

    // Determine hostname:
       char hostname[1000];
       gethostname(hostname, 1000); 
       std::cout << "HOSTNAME " << hostname << std::endl;
    //
   // render.init("OmniRender/configFiles/projectorConfigurationTemplat.txt", "tmp");
    // This should depend on whether we are on a laptop
    std::string cf = "/home/sphere/calibration-current/" + std::string(hostname) + ".txt"; 
    render.init(cf);
    render.resize(600, 400);
    render.radius(1e10)
          .near(0.1)
          .far(1000)
          .eyeSep(.1)
          .stereo(1); //stereo mode

    /// If active stereo flag is found in config file, set each window buffer type to stereo
    if (render.config.mProjector[0].active) {
      for (auto& i : windows()) {
        i->displayMode(i->displayMode() | Window::STEREO_BUF);
      }
    }
    addCube(mesh);
    //addSphere(mesh, 100, 100);
    //mesh.scale( .3,.3,.3);
    //mesh.primitive
  }

  void rawWorkFlow(Graphics& g) {

    render.begin();
    for (int i = 0; i < render.isStereo() + 1; i++) {
      for (int j = 0; j < 6; j++){
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X+j,
                               render.cubeMap[i].id(), 0);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float parallax = render.mEyeSep * (i - 0.5 * render.isStereo());

        /* USER CODE STARTS HERE */
        render.captureShader.begin(); {
          render.captureShader.uniform1i("omni_face", j);
          render.captureShader.uniform1f("omni_eye", parallax);
          render.captureShader.uniform1f("lighting", 0.0);
          render.captureShader.uniform1f("texture", 0.0);
          g.draw(mesh);
        } render.captureShader.end();

        /* AND ENDS HERE */
      }
    }
    //g.polygonMode( Graphics::FILL );
    render.end();

  }

  void renderDefault(Graphics& g) {
    // set these before rendering
    // MAYBE IN SOME OTHER WAY?
    render.clearColor(0.0, 0.0, 0.0, 1.0)
          .lighting(0.0)
          .texture(0.0);

    render.beginDefault();
    for (int i = 0; i < render.isStereo() + 1; i++) {
      for (int j = 0; j < 6; j++) {
        render.faceBeginDefault(0, j);

        /* USER CODE STARTS HERE */
        //mesh.color(j<3?1:0,1-j/6.0, j/6.0,1);
      //  g.draw(mesh);
        /* ENDS HERE */

        render.faceEndDefault();
      }
    }
    render.endDefault();

  }

  virtual void onDraw( Graphics& g ) override {
      rawWorkFlow(g);
  //  switch (code) {
    //  case 1: rawWorkFlow(g); break;
    //  case 2: renderDefault(g); break;
  //  }
  }

  /// @TODO: think about maintaining aspect ratio
  virtual void onResize(const ViewpointWindow& win, int w, int h) override {
    render.resize(w, h);
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

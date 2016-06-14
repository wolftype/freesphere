#include "allocore/io/al_App.hpp"

#include "omRender.hpp"

using namespace al;

struct MyApp : public App {
  om::Render render;
  int code = 1;

  Mesh meshPlatonic[3];
 //  MeshVBO shapesVBO;
 //  int numShapes = 1000;
	// float scatterDistance = 10.0;
 //  float scatterSize = .45;

  MyApp(){
    /// User MUST set window buffer to support Active stereo
    /// This isn't necessary for sequential, anaglyph, etc
    initWindow( Window::Dim(600,400),
                "Freesphere 1.0",
                60,
                Window::DEFAULT_BUF);

    window().fullScreen(true);
  }

  // // Fill a mesh with different platonic solids
  // void scatterShapes(MeshVBO& vbo){
  //   for(int i=0; i<numShapes; ++i){
  //     int Nv = rnd::prob(0.5)
  //           ? (rnd::prob(0.5) ? addCube(vbo) : addDodecahedron(vbo))
  //           : addIcosahedron(vbo);

  //     // Scale and translate the newly added shape
  //     Mat4f xfm;
  //     xfm.setIdentity();
  //     xfm.scale(Vec3f(rnd::uniform(1.,0.1) * scatterSize, rnd::uniform(1.,0.1) * scatterSize, rnd::uniform(1.,0.1) * scatterSize));
  //     xfm.translate(Vec3f(rnd::uniformS(scatterDistance), rnd::uniformS(scatterDistance), rnd::uniformS(scatterDistance)));
  //     vbo.transform(xfm, vbo.vertices().size()-Nv);

  //     // Color shapes randomly
  //     Color randc = Color(rnd::uniform(), rnd::uniform(), rnd::uniform(0.75,1.0));
  //     for(int i=0; i<Nv; ++i){
  //       vbo.color(randc);
  //     }
  //   }

  //   // Convert to non-indexed triangles to get flat shading
  //   vbo.decompress();
  //   vbo.generateNormals();

  //   // Update the VBO after all calculations are completed
  //   vbo.update();
  // }

  /// OpenGL context exists when onCreate is called
  virtual void onCreate( const ViewpointWindow& w ) override {
    // initialize om::render "tmp" doesn't get used!

      static bool bFirstTime = true;

      if (bFirstTime){
        bFirstTime = false;
      }
      else {
        return;
      }

    // Determine hostname:
       char hostname[1000];
       gethostname(hostname, 1000); 
       std::cout << "HOSTNAME " << hostname << std::endl;
    //  render.init("OmniRender/configFiles/projectorConfigurationTemplate.txt");
    // // This should depend on whether we are on a laptop
    std::string cf = "/home/sphere/calibration-current/" + std::string(hostname) + ".txt"; 
    render.init(cf);
    // render.resize(600, 400);
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

    // Basic shapes in the middle
    addTetrahedron(meshPlatonic[0]);
    addCube(meshPlatonic[1]);
    addSphere(meshPlatonic[2]);

    for (int i = 0; i < 3; i++){
      Mat4f xfm;
      xfm.setIdentity();
      if (i == 0) xfm.translate(Vec3f(-2, 0, 0));
      if (i == 2) xfm.translate(Vec3f(2, 0, 0));
      meshPlatonic[i].transform(xfm);
      for (int j = 0; j < meshPlatonic[i].vertices().size(); j++){
        if (i == 0) meshPlatonic[i].color(1.0, 0.0, 0.0);
        if (i == 1) meshPlatonic[i].color(0.0, 1.0, 0.0);
        if (i == 2) meshPlatonic[i].color(0.0, 0.0, 1.0);
      }
    }

    // Bunch of shapes all over
    // scatterShapes(shapesVBO);
    // shapesVBO.print();
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
          for (int i=0; i<3; i++) g.draw(meshPlatonic[i]);
          // g.draw(shapesVBO);
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
        for (int i=0; i<3; i++) g.draw(meshPlatonic[i]);
        // g.draw(shapesVBO);
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

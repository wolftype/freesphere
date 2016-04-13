


#include "allocore/io/al_App.hpp"
#include "omConfig.hpp"
#include "omTexture.hpp"

using namespace al;
using namespace om;

struct MyApp : public App {

  Config omniConfig;
  std::vector<om::Texture> tex;
  MyApp(){
    initWindow( Window::Dim(800,400) );
  }

  /// OpenGL context exists when onCreate is called
  virtual void onCreate( const ViewpointWindow& w ) override {

    //Load Warp and Blend Data
    omniConfig.loadConfig( "OmniRender/configFiles/projectorConfigurationTemplate.txt" );
    omniConfig.loadData("tmp");
    omniConfig.printConfig();

    //Load textures
    for (auto& i : omniConfig.mProjector){
      tex.push_back( om::Texture(i.width, i.height ) );
      tex.back().update( i.data );
    }
  }

  virtual void onDraw( Graphics& g ) override {

  }

};

int main(){

  MyApp app;
  app.start();

  return 0;
}

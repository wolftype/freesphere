#include "omOmniCubeRender.h"
#include "../OmniRender/include/omConfig.hpp"

#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;
using namespace om;

int width = 500;
int height = 300;

// assuming that the data is properly loaded by backend of lib
class Assumption {
public:
    GLuint warp_tex;
    GLuint blend_tex;
};

class MyApp : public App {
public:
    OmniCubeRender omren;

    void onCreate(const ViewpointWindow& w) {
        omren.resolution(128);
        omren.config("path/to/config/file");
        omren.init();
        omren.warpTex(0);
        omren.blendTex(1);
    }

    void onAnimate(double dt){

    }

    void onDraw(Graphics& g){
        float x = 0;
        float y = 0;
        float z = 0;
        float ux = 0;
        float uy = 0;
        float uz = 0;
        float vx = 0;
        float vy = 0;
        float vz = 0;
        float wx = 0;
        float wy = 0;
        float wz = 0;

        omren.pos(x, y, z);
        omren.dir(ux, uy, uz, vx, vy, vz, wx, wy, wz);
        omren.begin();
        while (omren.nextFaceExists()) {
            omren.prepareFace();
            // draw
        }
        omren.end();
        omren.draw();
    }
};


int main(){
    MyApp app;
    app.initWindow(Window::Dim(width, height));
    app.start();
}

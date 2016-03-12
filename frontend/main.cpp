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

    void onDraw(Graphics& g){
        // position
        float x = 0, y = 0, z = 0;
        // rotation unit vectors
        float ux = 0, uy = 0, uz = 0;
        float vx = 0, vy = 0, vz = 0;
        float wx = 0, wy = 0, wz = 0;

        omren.pos(x, y, z);
        omren.dir(ux, uy, uz, vx, vy, vz, wx, wy, wz);
        omren.begin();
        for (omren.mEye = 0; omren.mEye < omren.stereo; omren.mEye++) {
            omren.mEyeParallax = omren.mEyeSep * (omren.mEye-0.5);
            for (omren.mFace = 0; omren.mFace < 6; omren.mFace++) {
                omren.prepareFace();
                // draw something
            }
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

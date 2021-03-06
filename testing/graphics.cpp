#include "common.hpp"
#include "SphereRenderer.hpp"

#include "omRender.hpp"
#include "Cuttlebone/Cuttlebone.hpp"

#include <iostream>

using namespace al;
using namespace std;

class OmGraphics : public SphereRenderer {
public:
	cuttlebone::Taker<State, 9000> taker;
	State* state;
	om::Render render;
	Mesh meshPlatonic[3];

	bool setup() {
		state = new State;

		cout << "host: " << getHostName() << endl;
		cout << "configPath: " << configPath() << endl;

		std::cout << "window class says," << std::endl;
		std::cout << "width: " << width() << ", "
		          << "height: " << height() << std::endl;
		printGlutWindowDim();

		//Settings object
		om::Settings rndrSettings;
		rndrSettings.configPath = configPath();
		rndrSettings.winW = glutGet(GLUT_WINDOW_WIDTH);
		rndrSettings.winH = glutGet(GLUT_WINDOW_HEIGHT);

		render.init(rndrSettings);

		addTetrahedron(meshPlatonic[0]);
		addCube(meshPlatonic[1]);
		addSphere(meshPlatonic[2]);

		return false;
	}

	void onAnimate(double dt) {
		static bool first_frame = setup();
		int popCount = taker.get(*state);
		pose = state->pose;
	}

	void onDraw(Graphics& g) {
		// send modelview and projection matrix to gpu
		g.projection(Matrix4d::identity());
		Vec3d ux, uy, uz;
		pose.unitVectors(ux, uy, uz);
		g.pushMatrix(g.MODELVIEW);
		g.loadMatrix(Matrix4d::lookAt(ux, uy, uz, pose.pos()));

		// begin om capture
		render.begin();
		for (int i = 0; i < render.isStereo() + 1; i++) {
			float parallax = render.eyeSep() * (i - 0.5);
			for (int j = 0; j < 6; j++) {
				glFramebufferTexture2D(GL_FRAMEBUFFER,
				                       GL_COLOR_ATTACHMENT0,
				                       GL_TEXTURE_CUBE_MAP_POSITIVE_X+j,
				                       render.cubeMap[i].id(), 0);
				glClearColor(.2, 0.2, 0.2, 1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				render.captureShader.begin();

				render.captureShader.uniform1i("omni_face", j);
				render.captureShader.uniform1f("omni_eyeSep", parallax);
				render.captureShader.uniform1f("lighting", 0.0);
				render.captureShader.uniform1f("texture", 0.0);

				for (int i = 0; i < 3; i++) {
					for (int jj = 0; jj < 10; jj++) {
						for (int k = 0; k < 10; k++) {
							g.pushMatrix();
							g.translate(i * 10, jj * 3, k * 3);
							glColor4f((float)k/20.f + 0.5f, (float)jj/20.f + 0.5f, (float)i/6.f + 0.5f, 1.0);
							g.draw(meshPlatonic[i]);
							g.popMatrix();
						}
					}
				}

				render.captureShader.end();
			}
		}
		g.popMatrix(g.MODELVIEW);
		render.end();
	}
};

int main() {
	OmGraphics omg;
	omg.taker.start();
	omg.start();
	return 0;
}

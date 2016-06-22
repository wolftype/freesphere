#include "common.hpp"

#include "allocore/io/al_App.hpp"
#include "Cuttlebone/Cuttlebone.hpp"

#include <iostream>

using namespace al;
using namespace std;

class OmTest : public App {
public:
	cuttlebone::Maker<State, 9000> maker;
	State* state;

	OmTest() : maker(broadcastIP()) {}
	
	bool setup() {
		cout << "bradcast ip: " << broadcastIP() << endl;
		state = new State;
		return false;
	}

	void onAnimate(double dt){
		static bool first_frame = setup();
		state->pose.set(nav());
		maker.set(*state);
	}

	void onDraw(Graphics& g){

	}
};

int main(){
	OmTest app;
	app.initWindow(Window::Dim(640, 480));
	app.maker.start();
	app.start();
	return 0;
}

#pragma once

#include "allocore/spatial/al_Pose.hpp"

#if defined AL_OSX
    #include <GLUT/glut.h>

#elif defined AL_LINUX
    #include <GL/glew.h>
    #include <GL/glut.h>

#elif defined AL_WINDOWS
    #include <GL/wglew.h> // wglSwapInterval
    #include <GL/glut.h>
#endif

#include <string>
#include <unistd.h> // gethostname
#include <iostream>

class State {
public:
    al::Pose pose;
};

std::string getHostName() {
    char hostname[256];
    gethostname(hostname, 256);
    return std::string(hostname);
}

static const char* broadcastIP() {
    std::string host = getHostName();
    if (host.compare("gr01") == 0
        || host.compare("audio.10g") == 0)
    {
        return "192.168.10.255";
    }
    return "127.0.0.1";
}

std::string configPath() {
	std::string host = getHostName();
	if (host.substr(0,2).compare("gr") == 0) {
		std::string pre = "/home/sphere/calibration-current/";
		std::string post = ".txt";
        return pre + host + post;
	}
    return "OmniRender/configFiles/projectorConfigurationTemplate.txt";
}

void printGlutWindowDim() {
    std::cout << "GLUT says..." << std:: endl;
    std::cout << "win: " << glutGet(GLUT_WINDOW_WIDTH) << ", "
              << glutGet(GLUT_WINDOW_HEIGHT) << std::endl;
    std::cout << "screen: " << glutGet(GLUT_SCREEN_WIDTH) << ", "
              << glutGet(GLUT_SCREEN_HEIGHT) << std::endl;
    std::cout << ".........." << std::endl;
}
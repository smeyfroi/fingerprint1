#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"
#include <GLFW/glfw3.h>

// ***********************************************
// ***********************************************
// #define FULLSCREEN
const int MAIN_MONITOR_ID = 0;
const int GUI_MONITOR_ID = 1;
// ***********************************************
// ***********************************************

//========================================================================
int main( ){
#ifdef FULLSCREEN
  // init GLFW manually (since no OF window yet)
  if(!glfwInit()){
      ofLogError() << "Could not init GLFW";
      return -1;
  }
  
  int count;
  GLFWmonitor** monitors = glfwGetMonitors(&count);
  vector<glm::vec2> monitorSizes(count);
  vector<glm::vec2> monitorPositions(count);
  for(int i = 0; i < count; i++){
    const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
    int x, y;
    glfwGetMonitorPos(monitors[i], &x, &y);
    ofLogNotice() << "Monitor " << i
                  << ": " << mode->width << "x" << mode->height
                  << " at position " << x << "," << y;
    monitorSizes[i] = {(float)mode->width, (float)mode->height};
    monitorPositions[i] = {(float)x, (float)y};
  }
  
  int mainWindowX = monitorPositions[MAIN_MONITOR_ID].x;
  int mainWindowY = monitorPositions[MAIN_MONITOR_ID].y;
  int mainWindowW = monitorSizes[MAIN_MONITOR_ID].x;
  int mainWindowH = monitorSizes[MAIN_MONITOR_ID].y;
  int guiWindowX = monitorPositions[GUI_MONITOR_ID].x;
  int guiWindowY = monitorPositions[GUI_MONITOR_ID].y;
  int guiWindowW = 300; //monitorSizes[guiMonitorId].x;
  int guiWindowH = monitorSizes[GUI_MONITOR_ID].y;
#endif
  
  ofGLFWWindowSettings settings;
  settings.setGLVersion(4, 1);

#ifdef FULLSCREEN
  settings.setPosition({0, 0});
  settings.setSize(mainWindowW, mainWindowH); // TODO: disable game mode notification
#else
  settings.setPosition({255, 0});
  settings.setSize(1960, 1200);
#endif
  settings.decorated = false;
  settings.resizable = false;
  auto mainWindow = ofCreateWindow(settings);
  
#ifdef FULLSCREEN
  settings.setPosition({guiWindowX, guiWindowY+20});
  settings.setSize(guiWindowW, guiWindowH-20);
#else
  settings.setPosition({0, 0});
  settings.setSize(256, 1200);
#endif
  settings.decorated = false;
  settings.resizable = false;
  auto guiWindow = ofCreateWindow(settings);

  auto mainApp = std::make_shared<ofApp>();
  ofAddListener(guiWindow->events().draw, mainApp.get(), &ofApp::drawGui);
//  ofAddListener(guiWindow->events().keyPressed, mainApp.get(), &ofBaseApp::keyPressed); // doesn't work because keyPressed doesn't take an ofEventArgs& parameter
	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();
}

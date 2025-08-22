#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
  ofGLFWWindowSettings settings;
  settings.setGLVersion(4, 1);

	settings.setSize(1960, 1200);
  settings.setPosition({255, 0}); // **************** Change the position depending on screen/projector setup
  settings.resizable = false;
	auto mainWindow = ofCreateWindow(settings);
  
  settings.setSize(256, 1200);
  settings.setPosition({0, 0});
  settings.resizable = true;
  auto guiWindow = ofCreateWindow(settings);

  auto mainApp = std::make_shared<ofApp>();
  ofAddListener(guiWindow->events().draw, mainApp.get(), &ofApp::drawGui);
//  ofAddListener(guiWindow->events().keyPressed, mainApp.get(), &ofBaseApp::keyPressed); // doesn't work because keyPressed doesn't take an ofEventArgs& parameter
	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();
}

#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
  ofGLFWWindowSettings settings;

	settings.setSize(1024, 1024);
  settings.setPosition({255, 0}); // **************** Change the position depending on screen/projector setup
  settings.resizable = false;
	auto mainWindow = ofCreateWindow(settings);
  
  settings.setSize(256, 1024);
  settings.setPosition({0, 0});
  settings.resizable = true;
  auto guiWindow = ofCreateWindow(settings);
//  guiWindow->setVerticalSync(false);

  auto mainApp = std::make_shared<ofApp>();
  ofAddListener(guiWindow->events().draw, mainApp.get(), &ofApp::drawGui);
//  ofAddListener(guiWindow->events().keyPressed, mainApp.get(), &ofApp::keyPressed);
	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();
}

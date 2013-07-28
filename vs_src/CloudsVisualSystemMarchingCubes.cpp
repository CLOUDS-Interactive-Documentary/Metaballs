//
//  CloudsVisualSystemMarchingCubes.cpp
//

#include "CloudsVisualSystemMarchingCubes.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemMarchingCubes::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
//	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
//	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
//	customGui->addButton("Custom Button", false);
	customGui->addSlider("scl1", 0, 1, &scl1 );
	customGui->addSlider("scl2", 0, 1, &scl2 );
	customGui->addToggle("Wireframe", &wireframe);
	customGui->addToggle("Smoothing", &smoothing);
	customGui->addToggle("depth test", &depthTest);
	customGui->addSlider("threshold", 0, 1, &threshold );
	
	customGui->addImageSampler("c1", &colorMap, (float)colorMap.getWidth()/2, (float)colorMap.getHeight()/2 );
	customGui->addSlider("alpha1", 0, 1, &alpha1 );
	customGui->addImageSampler("c2", &colorMap, (float)colorMap.getWidth()/2, (float)colorMap.getHeight()/2 );
	customGui->addSlider("alpha2", 0, 1, &alpha2 );
	customGui->addSlider("mixScale", 0, 2, &mixScale );
	
	vector<string> modes;
	modes.push_back("alpha");
	modes.push_back("add");
	modes.push_back("sub");
	modes.push_back("mult");
	modes.push_back("screen");
	modes.push_back("disable");
	customGui->addRadio("blending", modes); 
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemMarchingCubes::selfGuiEvent);
	
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemMarchingCubes::selfGuiEvent(ofxUIEventArgs &e){
	string name = e.widget->getName();
	
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
	
	else if(e.widget->getName() == "Smoothing"){
		
		ofxUIToggle* smooth = (ofxUIToggle *) e.widget;
		mc.setSmoothing( smooth->getValue() );
	}
	
	else if(name == "add"){
		ofxUIToggle *t = e.getToggle();
		if(t->getValue()){
			blendMode = OF_BLENDMODE_ADD;
		}
	}
	
	else if(name == "sub"){
		ofxUIToggle *t = e.getToggle();
		if(t->getValue()){
			blendMode = OF_BLENDMODE_SUBTRACT;
		}
	}
	
	
	
	else if(name == "mult"){
		ofxUIToggle *t = e.getToggle();
		if(t->getValue()){
			blendMode = OF_BLENDMODE_MULTIPLY;
		}
	}
	
	else if(name == "screen"){
		ofxUIToggle *t = e.getToggle();
		if(t->getValue()){
			blendMode = OF_BLENDMODE_SCREEN;
		}
	}
	
	else if(name == "disable"){
		ofxUIToggle *t = e.getToggle();
		if(t->getValue()){
			blendMode = OF_BLENDMODE_DISABLED;
		}
	}
	
	else if(name == "alpha"){
		ofxUIToggle *t = e.getToggle();
		if(t->getValue()){
			blendMode = OF_BLENDMODE_ALPHA;
		}
	}
	
	else if( e.widget->getName() == "c1"){
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		ofFloatColor col = sampler->getColor();
		c1.set(col.r, col.g, col.b, 1.);
	}
	else if( e.widget->getName() == "c2"){
		ofxUIImageSampler* sampler = (ofxUIImageSampler *) e.widget;
		
		ofFloatColor col = sampler->getColor();
		c2.set(col.r, col.g, col.b, 1.);
	}
	
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemMarchingCubes::selfSetupSystemGui(){
	
}

void CloudsVisualSystemMarchingCubes::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemMarchingCubes::selfSetupRenderGui(){

}

void CloudsVisualSystemMarchingCubes::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemMarchingCubes::selfSetup(){

	if(ofFile::doesFileExist(getVisualSystemDataPath() + "TestVideo/Jer_TestVideo.mov")){
		getRGBDVideoPlayer().setup(getVisualSystemDataPath() + "TestVideo/Jer_TestVideo.mov",
								   getVisualSystemDataPath() + "TestVideo/Jer_TestVideo.xml" );
		
		getRGBDVideoPlayer().swapAndPlay();
		
		for(int i = 0; i < 640; i += 2){
			for(int j = 0; j < 480; j+=2){
				simplePointcloud.addVertex(ofVec3f(i,j,0));
			}
		}
		
		pointcloudShader.load(getVisualSystemDataPath() + "shaders/rgbdcombined");
		
	}
	
	colorMap.loadImage( getVisualSystemDataPath() + "GUI/defaultColorPalette.png" );
	
	
	//MC
	normalShader.load( getVisualSystemDataPath() + "shaders/facingRatio" );
	
	drawGrid = true;
	mc.setResolution(48,16,48);
	mc.scale.set( 400, 100, 400 );
	
	
	maxVerts = 200000;
	
	scl1 = .15;
	scl2 = .1;
	
	
	mc.setSmoothing( smoothing );
	
	depthTest = true;
		
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemMarchingCubes::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemMarchingCubes::selfBegin(){
	
	float val;
	ofVec3f center( mc.resX/2, mc.resY/2, mc.resZ );
	float lengthSq = center.lengthSquared();
	ofVec3f gridPos;
	float distSq;
	for(int i=0; i<mc.resX; i++){
		for(int j=0; j<mc.resY; j++){
			for(int k=0; k<mc.resZ; k++){
				gridPos.set(i,j,k);
				distSq = gridPos.distanceSquared(center)/lengthSq;
				val = ofNoise( i*.0001, j*.0001, k*.0001 ) * distSq / lengthSq;
				mc.setIsoValue( i, j, k, val * val * val );
			}
		}
	}
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemMarchingCubes::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemMarchingCubes::selfUpdate(){
	
	float t = ofGetElapsedTimef() * 5.;
	
	float camSpeed = 1;
	ofVec3f camVec = getCameraRef().getPosition() -	((CloudsRGBDCamera*)&getCameraRef())->targetPosition;
	camVec.normalize();
	camVec *= camSpeed * t;
	
	
	
	float val;
	ofVec3f center( mc.resX, mc.resY, mc.resZ );
	center /= 2;
	
	float lengthSq = center.lengthSquared();
	ofVec3f gridPos;
	float distSq;
	for(int i=0; i<mc.resX; i++){
		for(int j=0; j<mc.resY; j++){
			for(int k=0; k<mc.resZ; k++){
				gridPos.set(i,j,k);
				distSq = gridPos.distanceSquared(center)/lengthSq;
				
				val = ofNoise( scl1 * (i + t), scl1*j, scl1*k ) * (1. - abs((float)j / (float) mc.resY - .5) * 2.);
				val *= ofNoise( scl2 * (i - t*.1), j*scl2, k*scl2 );
				
				
				mc.setIsoValue( i, j, k, val * ofClamp(1. - distSq, 0, 1 ) );
			}
		}
	}
	
	
	
	//update the mesh
	mc.update(threshold);
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemMarchingCubes::selfDraw(){
	
//	ofPushMatrix();
//	setupRGBDTransforms();
//	pointcloudShader.begin();
//	getRGBDVideoPlayer().setupProjectionUniforms(pointcloudShader);
//	simplePointcloud.drawVertices();
//	pointcloudShader.end();
//	ofPopMatrix();
	
	if(!depthTest)	glDisable( GL_DEPTH_TEST );
	else	glEnable( GL_DEPTH_TEST );
	
	ofEnableBlendMode( blendMode );
	
	//draw the mesh
	normalShader.begin();
	normalShader.setUniform3f("c1", c1.r, c1.g, c1.b);
	normalShader.setUniform3f("c2", c2.r, c2.g, c2.b);
	normalShader.setUniform1f("alpha1", alpha1 );
	normalShader.setUniform1f("alpha2", alpha2 );
	normalShader.setUniform1f("mixScale", mixScale );
	
	
	if(wireframe){
		mc.drawWireframe();
	}else{
		mc.draw();
	}
	normalShader.end();
	
	glEnable( GL_DEPTH_TEST );
	
//	camera.end();
	
}

// draw any debug stuff here
void CloudsVisualSystemMarchingCubes::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemMarchingCubes::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemMarchingCubes::selfEnd(){
	
	simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemMarchingCubes::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemMarchingCubes::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemMarchingCubes::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemMarchingCubes::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMarchingCubes::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMarchingCubes::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemMarchingCubes::selfMouseReleased(ofMouseEventArgs& data){
	
}
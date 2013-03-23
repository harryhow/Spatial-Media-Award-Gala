#include "testApp.h"
#include <string.h>


//#define BG_VIDEO_ON


void testApp::initLiveVideo(){
    myPlayer.loadMovie("maxma.mp4");
    //#ifdef BG_VIDEO_ON
    myPlayer.play();
    //#endif
    myPlayer.setLoopState(OF_LOOP_NORMAL);
    signalPlayer.loadMovie("lostSignal.mov");
    signalPlayer.setLoopState(OF_LOOP_NORMAL);
    noisePlayer.loadMovie("noise.mov");
    noisePlayer.setLoopState(OF_LOOP_NORMAL);
    blueScreen.loadImage("BSOD.jpg");
    frameScreen1.loadImage("m1_t.png");
    frameScreen2.loadImage("m2_t.png");
    frameScreen3.loadImage("m3_t.png");
    frameScreen4.loadImage("m4_t.png");
}


void testApp::InitRainbowBun(){
    changeScreen=false;
    bIsEffect = false;
    count=0;
    nowScreen="video";
	targetScreen="video";
    
    mySound.loadSound("sound.mp3");
    mySound2.loadSound("sound2.mp3");
    
    ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
    
	capacity = 20;
    
	ofImage distortionMap;
    distortionMap.loadImage("distortion5.png");
	colorImg.allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR);
	previewImage.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
	warpImage.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
    
	isFullScreen = false;
	isPaused = false;
	
	leftClamp = 0.0f;
	rightClamp = 1.0f;
    
    if(warp.isSetup()){
		warp.setCapacity(capacity);
	}
	else{
		warp.setup(WIDTH, HEIGHT, capacity, OF_IMAGE_COLOR);
		warp.setDelayMap(distortionMap);
	}
	warp.setTimeDelayAndWidth(int( leftClamp * warp.getCapacity() + .5),
							  int( (rightClamp - leftClamp) * warp.getCapacity() + .5) );
	
	framesToShow = MIN(warp.getCapacity(), 19);
	heightPerFrame = 49;
	widthPerFrame  = 1.0 * WIDTH / HEIGHT * heightPerFrame;
	filmStripWidth = widthPerFrame*framesToShow;
    
	initLiveVideo();
	
	warp.setBlending(true);
	warp.setDelayMap(distortionMap);
    
    ofBackground(0);
    
}

void testApp::InitOSC(){

    // FaceOSC sends to port 8338 by default
	receiver.setup(8338);
}

//--------------------------------------------------------------
void testApp::setup(){
    //ofBackground(0);
    // this changes our drawing rate to match our screen's refresh rate
    InitRainbowBun();
    InitOSC();

    
    
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    // allocate your FBOs
    // you MUST do this before drawing to it!
    // we'll make ours the same res as our mapper (see below)
    for (int i=0; i<FRAME_NUM; i++){
        canvas[i].allocate(WIDTH, HEIGHT);
        
        // you also need to clear out your FBO (we'll clear it to black)
        canvas[i].begin();
        ofClear(0);
        canvas[i].end();
    }
    
    // initialize your mappers; you can also pass in an x and y,
    // which we'll do until we have them set up into a cube
    
    for (int i=0; i<FRAME_NUM; i++){
        // initialize ofxSPM at a specific res (we'll say 800x600)
        // we will also offset the x + y positions a bit
        mappers[i].initialize(WIDTH, HEIGHT, i * 20, i * 20);
        
        // each mapper will have a unique settings file
        string settingsfile = "/Users/harryhow/Documents/mapsetting/mapsettings" + ofToString(i) + ".txt";
        // try loading some settings
        mappers[i].load(settingsfile);
      
    }
    
    // start off not drawing bounds; press 'd' for debug to show them
    bDrawBounds = false;
    
    // use -1 as "false" for forceSelect
    forceSelect = -1;
    
    // check for waiting osc messages
	while(receiver.hasWaitingMessages())
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
        // load face from any FaceOSC messages
        face.parseOSC(m);
	}
    
    // found face?
    if(face.found > 0) {
        cout << "---------" << endl
        << face.toString();
    }
    
}

void testApp::DoEffectA(){
    bIsEffect = true;
    changeScreen=true;
    
    if(targetScreen=="video"){
        int rand=ofRandom(0,9);
        if(rand<3){
            targetScreen="signal";
            signalPlayer.play();
        }else if(rand<6){
            targetScreen="blueScreen";
        }else {
            targetScreen="noise";
            noisePlayer.play();
        }
    }else{
        targetScreen="video";
        if(!myPlayer.isPlaying()){
            myPlayer.play();
        }
    }

}


void testApp::DoEffectB(){
    bIsEffect = false;
    
    if(mySound.getIsPlaying()){
        mySound.stop();
    }
    mySound2.play();
    count=0;
    changeScreen=false;
    if(nowScreen=="video"){
        //myPlayer.stop();
    }else if(nowScreen=="signal"){
        signalPlayer.stop();
    }else if(nowScreen=="noise"){
        noisePlayer.stop();
    }
    targetScreen="video";
    nowScreen=targetScreen;
}

int timePeriod = 500;
int lastTimeCheck = -1;

//--------------------------------------------------------------
void testApp::update(){
    
    //    bool bNewFrame1 = false;
    //    bool bNewFrame2 = false;
    //    bool bNewFrame3 = false;
    //
    // update your mappers
    for (int i=0; i<FRAME_NUM; i++){
        mappers[i].update();
    }
    
    /////////////////////////////////////////////////////////////////////
    
    bool bNewFrame1 = false;
    bool bNewFrame2 = false;
    bool bNewFrame3 = false;
    if(myPlayer.isPlaying()){
        myPlayer.update();
        bNewFrame1 = myPlayer.isFrameNew();
    }
	if(signalPlayer.isPlaying()){
        signalPlayer.update();
		bNewFrame2 = signalPlayer.isFrameNew();
	}
    if(noisePlayer.isPlaying()){
        noisePlayer.update();
        bNewFrame3=noisePlayer.isFrameNew();
    }
    if(changeScreen==true){
        //if (bNewFrame1&&bNewFrame2&& !isPaused){
        count++;
        unsigned char* inPixels;
        if(count%30<ofRandom(40)){
            //ofLogNotice("check 1");
            if(!mySound.getIsPlaying()){
                mySound.play();
            }
            inPixels=sendMovieImage(nowScreen);
        }else{
            //ofLogNotice("check 2");
            inPixels=sendMovieImage(targetScreen);
        }
        /*if(count>ofRandom(50)+50){
         if(mySound.getIsPlaying()){
         mySound.stop();
         }
         //mySound2.play();
         count=0;
         changeScreen=false;
         //myPlayer.stop();
         if(nowScreen=="video"){
         //myPlayer.stop();
         }else if(nowScreen=="signal"){
         signalPlayer.stop();
         }else if(nowScreen=="noise"){
         noisePlayer.stop();
         }
         targetScreen="video";
         nowScreen=targetScreen;
         }*/
        ofImage resized = movieImage;
        resized.resize(WIDTH, HEIGHT);
        inPixels = resized.getPixels();
        
        colorImg.setFromPixels(inPixels, WIDTH, HEIGHT, OF_IMAGE_COLOR);
        warp.addImage(colorImg);

        // }
    }
    /////////////////////////////////////////////////////////////////////
    
    // check for waiting osc messages
	while(receiver.hasWaitingMessages())
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
        // load face from any FaceOSC messages
        face.parseOSC(m);
	}
    
    if (ofGetElapsedTimeMillis() > lastTimeCheck + timePeriod){
        lastTimeCheck = ofGetElapsedTimeMillis();
        //cout << "time" << endl;
      
        // found face?
        if(face.found > 0) {
            cout << "---------" << endl
            << face.primary;
            
            
            if (face.primary.compare("smile") == 0){
                ofLog(OF_LOG_NOTICE, "smile");
                DoEffectA();
            }
            else if (face.primary.compare("eyebrows raised") == 0){
                ofLog(OF_LOG_NOTICE, "eyeraised");
                DoEffectB();
            }
            else if (!face.primary.compare("neutral") == 0){
                ofLog(OF_LOG_NOTICE, "neutral");
                if (bIsEffect)
                    DoEffectB();
                else
                    DoEffectA();
            }
            else{
                ofLog(OF_LOG_NOTICE, "CANT get expression");
                if (bIsEffect){
                    DoEffectB();
                }
                else
                    DoEffectA();
            }
            
        }
    }
}

unsigned char* testApp::sendMovieImage(string valueName){
    unsigned char* inPixels;
    if(valueName=="video"){
        movieImage.setFromPixels(myPlayer.getPixels(), myPlayer.width,myPlayer.height, OF_IMAGE_COLOR);
        inPixels = myPlayer.getPixels();
    }else if(valueName=="signal"){
        movieImage.setFromPixels(signalPlayer.getPixels(), signalPlayer.width,signalPlayer.height, OF_IMAGE_COLOR);
        inPixels = signalPlayer.getPixels();
    }else if(valueName=="blueScreen"){
        movieImage.setFromPixels(blueScreen.getPixels(), blueScreen.width,blueScreen.height, OF_IMAGE_COLOR);
        inPixels = blueScreen.getPixels();
    }else if(valueName=="noise"){
        movieImage.setFromPixels(noisePlayer.getPixels(), noisePlayer.width,noisePlayer.height, OF_IMAGE_COLOR);
        inPixels = noisePlayer.getPixels();
    }
    return inPixels;
    
}

//--------------------------------------------------------------
void testApp::draw(){    
    for (int i=0; i< FRAME_NUM; i++){
        canvas[i].begin();
        //frameCanvas[i].begin();
        ///////////////////////////////////////
        //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
        if (changeScreen){
            //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
            if (count < capacity){
                if (nowScreen == "video"){
                    myPlayer.draw(0, 0, canvas[i].getWidth(),canvas[i].getHeight());
                    //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
                }else if (nowScreen == "signal"){
                    signalPlayer.draw(0, 0, canvas[i].getWidth(),canvas[i].getHeight());
                    //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
                }else if(nowScreen=="blueScreen"){
                    blueScreen.draw(0,0, canvas[i].getWidth(),canvas[i].getHeight());
                    //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
                }else if(nowScreen=="noise"){
                    noisePlayer.draw(0, 0, canvas[i].getWidth(),canvas[i].getHeight());
                    //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
                }
            }else{
                //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
                warp.getOutputImage().draw(0,0, canvas[i].getWidth(),canvas[i].getHeight());
            }
        }
        else{
        
            if(targetScreen=="video"){
                myPlayer.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
                //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
            }else if(targetScreen=="signal"){
                signalPlayer.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
                //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
            }else if(targetScreen=="blueScreen"){
                blueScreen.draw(0,0, canvas[i].getWidth(),canvas[i].getHeight());
                //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
            }else if(targetScreen=="noise"){
                noisePlayer.draw(0, 0, canvas[i].getWidth(),canvas[i].getHeight());
                //frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
            }
        }
        ///////////////////////////////////////
        if (i >= 0 && i < 6)
            frameScreen1.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
        else if (i >= 6 && i < 12)
            frameScreen2.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
        else if (i >= 10 && i < 16)
            frameScreen3.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());
        else if (i >= 15 && i<21)
            frameScreen4.draw(0,0,canvas[i].getWidth(),canvas[i].getHeight());

        
        canvas[i].end();
    }
 
    ///////////////////////////////////////////////////////////////////
    // draw your mappers and canvases
    for (int i=0; i<FRAME_NUM; i++){
        mappers[i].startMapping();
        canvas[i].draw(0,0);
        mappers[i].stopMapping();
        
        if ( bDrawBounds ){
            ofNoFill();
            mappers[i].drawBoundingBox();
            
            // some instructions!
            ofDrawBitmapString("press '1', '2', or '3' to just select one box at a time!\npress 's' to save", 20,20);
        }
        else{
            ofNoFill();
            ofSetColor(255);
        }
    }
    //////////////////////////////////////////////////////////////////    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if ( key == 's' ){
        // save each a file
        for (int i=0; i<FRAME_NUM; i++){
            ofLog(OF_LOG_NOTICE,"saving=%d",i);
            // each mapper will have a unique settings file
            string settingsfile = "/Users/harryhow/Documents/mapsetting/mapsettings" + ofToString(i) + ".txt";
            // try loading some settings
            mappers[i].save(settingsfile);
            
        }
    } else if ( key == 'd' ){
        bDrawBounds = !bDrawBounds;
    } else if ( key == 'f' ) {
        ofToggleFullscreen();
    }
    
    // force select stuff
    if ( key == '1'){
        forceSelect = 0;
    } else if ( key == '2' ){
        forceSelect = 1;
    } else if ( key == '3' ){
        forceSelect = 2;
    }
    
    if(key=='a'){
        changeScreen=true;
        
        if(targetScreen=="video"){
            int rand=ofRandom(0,9);
            if(rand<3){
                targetScreen="signal";
                signalPlayer.play();
            }else if(rand<6){
                targetScreen="blueScreen";
            }else {
                targetScreen="noise";
                noisePlayer.play();
            }
        }else{
            targetScreen="video";
            if(!myPlayer.isPlaying()){
                myPlayer.play();
            }
        }
    }else if(key=='b'){
        if(mySound.getIsPlaying()){
            mySound.stop();
        }
        mySound2.play();
        count=0;
        changeScreen=false;
        if(nowScreen=="video"){
            //myPlayer.stop();
        }else if(nowScreen=="signal"){
            signalPlayer.stop();
        }else if(nowScreen=="noise"){
            noisePlayer.stop();
        }
        targetScreen="video";
        nowScreen=targetScreen;
    }
    
    if (key == 'P'){
        //        canvas[0].begin();
        //        //ofRect(5,5, canvas[0].getWidth()-10,  canvas[0].getHeight()-10);
        //        myPlayer.play();
        //        canvas[0].end();
        //
        //        canvas[1].begin();
        //        //ofRect(5,5, canvas[0].getWidth()-10,  canvas[0].getHeight()-10);
        //        myPlayer.play();
        //        canvas[1].end();
        //
        //        canvas[2].begin();
        //        //ofRect(5,5, canvas[0].getWidth()-10,  canvas[0].getHeight()-10);
        //        myPlayer.play();
        //        canvas[2].end();
        //
    }
    else if (key == 'S'){
        
        //        canvas[0].begin();
        //        //ofRect(5,5, canvas[0].getWidth()-10,  canvas[0].getHeight()-10);
        //        myPlayer.stop();
        //        canvas[0].end();
        //
        //        canvas[1].begin();
        //        //ofRect(5,5, canvas[0].getWidth()-10,  canvas[0].getHeight()-10);
        //        myPlayer.stop();
        //        canvas[1].end();
        //
        //        canvas[2].begin();
        //        //ofRect(5,5, canvas[0].getWidth()-10,  canvas[0].getHeight()-10);
        //        myPlayer.stop();
        //        canvas[2].end();
        
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    // always reset forceSelect when you release keys
    forceSelect = -1;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    // are we trying to select juse one?
    if ( forceSelect != -1 ){
        mappers[forceSelect].mouseDragged(x, y);
        return;
    }
    // pass along mouse events to mappers
    for (int i=0; i<FRAME_NUM; i++){
        mappers[i].mouseDragged(x, y);
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    // are we trying to select juse one?
    if ( forceSelect != -1 ){
        mappers[forceSelect].mousePressed(x, y);
        return;
    }
    for (int i=0; i<FRAME_NUM; i++){
        mappers[i].mousePressed(x, y);
    }
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    // are we trying to select juse one?
    if ( forceSelect != -1 ){
        mappers[forceSelect].mouseReleased(x, y);
        return;
    }
    for (int i=0; i<FRAME_NUM; i++){
        mappers[i].mouseReleased(x, y);
    }
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ }
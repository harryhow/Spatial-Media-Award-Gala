#pragma once

#include "ofMain.h"
#include "ofxSlitScan.h"

// include ofxSPM
#include "ofxSimpleProjectionMapper.h"

#include "ofxOsc.h"
#include "Face.h"

#define WIDTH 1280
#define HEIGHT 720
#define FRAME_NUM 20

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    /* mapping ===================================*/
        // array of ofxSPM objects
        ofxSimpleProjectionMapper mappers[FRAME_NUM];
        ofxSimpleProjectionMapper frameMappers[FRAME_NUM];
    
        // fbos to draw to
        ofFbo canvas[FRAME_NUM];
        ofFbo frameCanvas[FRAME_NUM];
    
        // do we want to show ofxSPM's bounding boxes?
        bool bDrawBounds;
    
        // force selecting just one box at a time
        int forceSelect;
    /* ===================================== mapping */
    
        void InitRainbowBun();
        ofVideoPlayer myPlayer;
        ofVideoPlayer signalPlayer;
        ofVideoPlayer noisePlayer;
    
        ofImage blueScreen;
        ofImage frameScreen1;
        ofImage frameScreen2;
        ofImage frameScreen3;
        ofImage frameScreen4;
    
        ofxSlitScan warp;
        ofVideoGrabber grabber;
        ofImage myImage;
        
        bool changeScreen;
        bool bIsEffect;
        string targetScreen;
        string nowScreen;
        
        int capacity;
        int count;
        
        ofImage	colorImg;
        ofImage movieImg;
        ofImage	warpImage;
        ofImage previewImage;
        ofImage movieImage;
        
        bool isFullScreen;
        bool isPaused;
        float rightClamp;
        float leftClamp;
        int framesToShow;
        float heightPerFrame;
        float widthPerFrame;
        float filmStripWidth;
        void initLiveVideo();
        unsigned char* sendMovieImage(string valueName);
        ofSoundPlayer mySound;
        ofSoundPlayer mySound2;
    
    
        ofxOscReceiver	receiver;
        Face face;
        void InitOSC();
        void DoEffectB();
        void DoEffectA();
};

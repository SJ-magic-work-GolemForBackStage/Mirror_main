/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxSyphon.h"
#include "ofxOsc_BiDirection.h"

#include "sj_common.h"
#include "Contents.h"

/************************************************************
************************************************************/

class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	enum{
		NUM_GOLEMS = 2,
	};
	
	OSC_TARGET Osc_Golem[NUM_GOLEMS];
	OSC_TARGET Osc_Unity;
	
	ofxUDPManager udp_Receive[NUM_GOLEMS];
	ofxUDPManager udp_SendToUnity;
	
	ofTrueTypeFont font;
	
	ofFbo fbo;
	ofxSyphonServer fbo_TextureSyphonServer;
	
	float now;
	
	/****************************************
	****************************************/
	VISUALIZE_TEXT VisualizeText;
	int VisualizeText_Disp_id;
	
	/****************************************
	****************************************/
	void ResTo_UdpFromGolem(int GolemId);
	void ResTo_OscFromUnity();

public:
	/****************************************
	****************************************/
	ofApp();
	~ofApp();
	
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};

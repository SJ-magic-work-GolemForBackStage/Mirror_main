/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/
#define DEBUG_BY_DUMMY


/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp()
: Osc_Unity("127.0.0.1", 12360, 12361)
, VisualizeText_Disp_id(0)
{
	/********************
	********************/
	font.load("font/RictyDiminishedDiscord-Bold.ttf", 15, true, true, true);
	
	/********************
	********************/
	char buf[BUF_SIZE_S];
	
#ifdef DEBUG_BY_DUMMY
	sprintf(buf, "127.0.0.1");
	
	Osc_Golem[0].setup(buf, 12359, 12358);
	Osc_Golem[1].setup(buf, 12359, 12358);
#else
	sprintf(buf, "10.0.0.5");
	Osc_Golem[0].setup(buf, 12359, 12358);
	
	sprintf(buf, "10.0.0.6");
	Osc_Golem[1].setup(buf, 12359, 12358);
#endif
}

/******************************
******************************/
ofApp::~ofApp(){
}

/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	ofSetBackgroundAuto(false);
	
	ofSetWindowTitle("Kinobi:Mirror");
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofSetWindowShape(WINDOW_WIDTH, WINDOW_HEIGHT);
	ofSetEscapeQuitsApp(false);
	
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	// ofEnableSmoothing();
	
	ofSetCircleResolution(60);
	
	
	/********************
	********************/
	udp_Receive[0].Create();
	udp_Receive[0].Bind(12355);
	udp_Receive[0].SetNonBlocking(true);
	
	udp_Receive[1].Create();
	udp_Receive[1].Bind(12356);
	udp_Receive[1].SetNonBlocking(true);

    udp_SendToUnity.Create();
	udp_SendToUnity.Connect("127.0.0.1",12357);
	udp_SendToUnity.SetNonBlocking(true);
	
	/********************
	********************/
	VisualizeText.setup();
}

/******************************
******************************/
void ofApp::ResTo_UdpFromGolem(int GolemId)
{
	bool b_MessageReceived;
	
	do{
		/********************
		********************/
		string message_with_id;
		char buf[BUF_SIZE_S];
		sprintf(buf, "GolemId|%d<p>", GolemId);
		message_with_id = buf;
		
		/********************
		********************/
		char udpMessage[BUF_SIZE_UDP];
		udp_Receive[GolemId].Receive(udpMessage, BUF_SIZE_UDP);
		
		string message = udpMessage;
		message_with_id += message;
		
		if(message!=""){
			b_MessageReceived = true;
			
			vector<string> block = ofSplitString(message,"<p>");
			
			if(block[0] == "/Golem/SkeletonDefinition"){
				udp_SendToUnity.Send(message_with_id.c_str(), message_with_id.length()); // bypass
				
			}else if(block[0] == "/Golem/SkeletonData"){
				udp_SendToUnity.Send(message_with_id.c_str(), message_with_id.length()); // bypass
				FromGolem_FrameDataAll.set(block);
			}
			
		}else{
			b_MessageReceived = false;
		}
		
	}while(b_MessageReceived);
}

/******************************
******************************/
void ofApp::ResTo_OscFromUnity(){

	while(Osc_Unity.OscReceive.hasWaitingMessages()){
		ofxOscMessage m_receive;
		Osc_Unity.OscReceive.getNextMessage(&m_receive);
		
		if(m_receive.getAddress() == "/Message/ToGolem_0"){
			ofxOscMessage m;
			m.setAddress("/Message/ToGolem");
			
			m.addFloatArg(float(m_receive.getArgAsFloat(0)));							// message id
			for(int i = 1; i < 11; i++) { m.addFloatArg(m_receive.getArgAsFloat(i)); }	// params
			
			Osc_Golem[0].OscSend.sendMessage(m);
			
		}else if(m_receive.getAddress() == "/Message/ToGolem_1"){
			ofxOscMessage m;
			m.setAddress("/Message/ToGolem");
			
			m.addFloatArg(float(m_receive.getArgAsFloat(0)));							// message id
			for(int i = 1; i < 11; i++) { m.addFloatArg(m_receive.getArgAsFloat(i)); }	// params
			
			Osc_Golem[1].OscSend.sendMessage(m);
		}
	}
}

/******************************
******************************/
void ofApp::update(){
	/********************
	********************/
	ofSoundUpdate(); // 全体まとめて.
	
	/********************
	********************/
	now = ofGetElapsedTimef();
	
	/********************
	********************/
	for(int i = 0; i < NUM_GOLEMS; i++)	ResTo_UdpFromGolem(i);
	
	ResTo_OscFromUnity();
	
	/********************
	********************/
	FromGolem_FrameDataAll.b_set = false;
	
	/********************
	********************/
	VisualizeText.update(now);
}

/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	// Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	monitor.
	********************/
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	// ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnableSmoothing();
	
	ofSetColor(255, 255, 255, 255);
	
	/********************
	********************/
	VisualizeText.draw_and_sendsyphon(VisualizeText_Disp_id);
	
	/********************
	********************/
	/*
	char buf[BUF_SIZE_S];
	sprintf(buf, "%4d", int(ofGetFrameRate() + 0.5));
	ofSetColor(255, 255, 255, 210);
	font.drawString(buf, 800, 20);
	*/
}

/******************************
******************************/
void ofApp::keyPressed(int key){
	switch(key){
		case '0':
			VisualizeText_Disp_id = 0;
			break;
			
		case '1':
			VisualizeText_Disp_id = 1;
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(const char* Golem0_IP, const char* Golem1_IP)
: Osc_Unity("127.0.0.1", 12360, 12361)
, VisualizeText_Disp_id(0)
{
	/********************
	********************/
	font.load("font/RictyDiminishedDiscord-Bold.ttf", 15, true, true, true);
	
	/********************
	********************/
	Osc_Golem[0].setup(Golem0_IP, 12359, 12358);
	Osc_Golem[1].setup(Golem1_IP, 12359, 12362);
	
	const int PORT_OFFSET = 12363;
	for(int i = 0; i < NUM_DUMMY_GOLEMS; i++){
		Osc_Golem[i + NUM_REAL_GOLEMS].setup("127.0.0.1", PORT_OFFSET + i * 3 + 2, PORT_OFFSET + i * 3 + 1);
	}
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
	
	const int PORT_OFFSET = 12363;
	for(int i = 0; i < NUM_DUMMY_GOLEMS; i++){
		udp_Receive[i + NUM_REAL_GOLEMS].Create();
		udp_Receive[i + NUM_REAL_GOLEMS].Bind(PORT_OFFSET + i * 3 + 0);
		udp_Receive[i + NUM_REAL_GOLEMS].SetNonBlocking(true);
	}
	
	/********************
	********************/
	VisualizeText.setup();
}

/******************************
******************************/
void ofApp::ResTo_UdpFromGolem(int GolemId)
{
	bool b_MessageReceived;
	bool b_Sended = false;
	
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
				if(!b_Sended){
					udp_SendToUnity.Send(message_with_id.c_str(), message_with_id.length()); // bypass
					b_Sended = true;
				}
				
			}else if(block[0] == "/Golem/SkeletonData"){
				if(!b_Sended){
					udp_SendToUnity.Send(message_with_id.c_str(), message_with_id.length()); // bypass
					FromGolem_FrameDataAll.set(block);
					b_Sended = true;
				 }
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
		
		if(m_receive.getAddress() == "/Message/ToGolem"){
			ofxOscMessage m;
			m.setAddress("/Message/ToGolem");
			
			m.addFloatArg(float(m_receive.getArgAsFloat(0)));							// message id
			m.addFloatArg(0.0f);														// delete Golem id
			for(int i = 2; i < 11; i++) { m.addFloatArg(m_receive.getArgAsFloat(i)); }	// params
			
			int GolemId = int(m_receive.getArgAsFloat(1));
			if(GolemId < NUM_GOLEMS) Osc_Golem[GolemId].OscSend.sendMessage(m);
			
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

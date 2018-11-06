/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include <time.h>

#include "ofMain.h"
#include "ofxSyphon.h"

#include "sj_common.h"

/************************************************************
************************************************************/

/**************************************************
**************************************************/
class VISUALIZE_TEXT : Noncopyable{
private:
	/****************************************
	****************************************/
	enum{
		FBO_BASE_BLACK,
		FBO_BASE_WHITE,
		
		NUM_FBO_BASE__BLACK_WHITE,
	};
	
	enum{
		NUM_CONTENTS = 2,
		NUM_DISPLINES_CONTENTS = 50,
	};
	
	/****************************************
	****************************************/
	class CHAIN_CONTENTS : Noncopyable{
	private:
		
	public:
		string str_line;
		ofColor TextColor[NUM_FBO_BASE__BLACK_WHITE];
		CHAIN_CONTENTS* Next;
		
		CHAIN_CONTENTS(string& s)
		: Next(NULL)
		, str_line(s)
		{
			TextColor[FBO_BASE_BLACK] = ofColor(255, 255, 255, 255);
			// TextColor[FBO_BASE_WHITE] = ofColor(0, 0, 0, 255);
			TextColor[FBO_BASE_WHITE] = ofColor(0, 0, 0, 10);
		}
		
		void draw(ofTrueTypeFont& font, int Base_BlackOrWhite)
		{
			ofSetColor(TextColor[Base_BlackOrWhite]);
			font.drawString(str_line.c_str(), 0, 0);
		}
	};
	
	/****************************************
	****************************************/
	/********************
	********************/
	int c_syphon;
	
	ofTrueTypeFont font;
	
	ofFbo fbo[NUM_FBO_BASE__BLACK_WHITE];
	ofxSyphonServer fbo_SyphonServer[NUM_FBO_BASE__BLACK_WHITE];
	
	FILE* fp_Contents[NUM_CONTENTS];
	
	double N_Lines_PerSec[NUM_CONTENTS];
	double NumLines_Proceed[NUM_CONTENTS];
	
	float now;
	float t_LastInt;
	
	CHAIN_CONTENTS* TopOfContents[NUM_CONTENTS];
	
	/********************
	********************/
	float CursorPos[NUM_CONTENTS];
	float CursorSpeed[NUM_CONTENTS];
	
	
	/****************************************
	****************************************/
	void TabToSpacex4(string& s);
	void YennToCR(string& s);
	string CR_To_Yenn(string s);

	int Count_NumContent_inChain(CHAIN_CONTENTS* top);
	void Add_ContentsChain(int ContentsId, string& s);
	void Align_StringOfData(string& s);
	void Read_1Line_FromFile(int ContentsId, string& str_Line);
	void update_ContentsChain(int ContentsId, string& s);
	void draw_Contents_toFbo(int Base_BlackOrWhite);
	void Contents_LineFeed(int Contents_id, double dt);
	double get_FontHeight();
	
public:
	/****************************************
	****************************************/
	VISUALIZE_TEXT();
	~VISUALIZE_TEXT();
	
	void setup();
	void update(float _now);
	void draw_and_sendsyphon(int VisualizeText_Disp_id);
};

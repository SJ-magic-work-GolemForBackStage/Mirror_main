/************************************************************
************************************************************/
#include "Contents.h"

/************************************************************
************************************************************/

/******************************
******************************/
VISUALIZE_TEXT::VISUALIZE_TEXT()
: c_syphon(0)
, now(0)
, t_LastInt(0)
{
	/********************
	********************/
	srand((unsigned) time(NULL));
	
	/********************
	********************/
	for(int i = 0; i < NUM_CONTENTS; i++)	fp_Contents[i] = NULL;
	
	fp_Contents[0] = fopen("../../../data/sample.cpp", "r");
	fp_Contents[1] = fopen("../../../data/sample.asm", "r");

	for(int i = 0; i < NUM_CONTENTS; i++){
		if(fp_Contents[i] == NULL) { ERROR_MSG(); std::exit(1); }
	}
	
	/********************
	********************/
	/*
	N_Lines_PerSec[0] = 1.1;
	N_Lines_PerSec[1] = 8.5;
	*/
	N_Lines_PerSec[0] = 0.6;
	N_Lines_PerSec[1] = 9.3;
	
	for(int i = 0; i < NUM_CONTENTS; i++) { NumLines_Proceed[i] = 0; }
	
	/********************
	********************/
	CursorPos[0] = 0;
	CursorPos[1] = 30;
	
	CursorSpeed[0] = 50;
	CursorSpeed[1] = 35;

	
	/********************
	********************/
	for(int i = 0; i < NUM_CONTENTS; i++)	TopOfContents[i] = NULL;
	
	for(int i = 0; i < NUM_CONTENTS; i++){
		for(int j = 0; j < NUM_DISPLINES_CONTENTS; j++){
			string str_Line;
			
			Read_1Line_FromFile(i, str_Line);
			Add_ContentsChain(i, str_Line);
		}
	}
}

/******************************
******************************/
VISUALIZE_TEXT::~VISUALIZE_TEXT()
{
	/********************
	********************/
	for(int i = 0; i < NUM_CONTENTS; i++){
		if(fp_Contents[i]) fclose(fp_Contents[i]);
	}
	
	/********************
	********************/
	for(int i = 0; i < NUM_CONTENTS; i++){
		CHAIN_CONTENTS* temp = TopOfContents[i];
		CHAIN_CONTENTS* temp_to_del;
		
		while(temp){
			temp_to_del = temp;
			temp = temp->Next;
			delete temp_to_del;
		}
	}
}

/******************************
******************************/
void VISUALIZE_TEXT::Add_ContentsChain(int ContentsId, string& s)
{
	CHAIN_CONTENTS** temp = &TopOfContents[ContentsId];
	while(*temp){
		temp = &((*temp)->Next);
	}
	(*temp) = new CHAIN_CONTENTS(s);
}

/******************************
******************************/
int VISUALIZE_TEXT::Count_NumContent_inChain(CHAIN_CONTENTS* top)
{
	int counter = 0;
	CHAIN_CONTENTS** temp = &top;
	while(*temp){
		temp = &((*temp)->Next);
		counter++;
	}
	return counter;
}

/******************************
******************************/
void VISUALIZE_TEXT::update_ContentsChain(int ContentsId, string& s)
{
	/********************
	********************/
	if(Count_NumContent_inChain(TopOfContents[ContentsId]) < 2) return;
	
	/********************
	********************/
	CHAIN_CONTENTS* temp = TopOfContents[ContentsId];
	TopOfContents[ContentsId] = temp->Next;
	temp->Next = NULL;
	temp->str_line = s;
	
	CHAIN_CONTENTS** Last = &TopOfContents[ContentsId];
	while(*Last){
		Last = &((*Last)->Next);
	}
	(*Last) = temp;
}

/******************************
******************************/
void VISUALIZE_TEXT::Align_StringOfData(string& s)
{
	size_t pos;
	// while((pos = s.find_first_of(" 　\t\n\r")) != string::npos){ // 半角・全角space, \t 改行 削除
	while((pos = s.find_first_of(" 　\n\r")) != string::npos){ // 半角・全角space, \t 改行 削除
		s.erase(pos, 1);
	}
}

/******************************
description
	font.drawStringAsShapes(str_line.c_str(), 0, 0);
	では、str_line.c_str()に"\t"が入っていても無視される.
	そこで、space x 4にreplaceしておく.
******************************/
void VISUALIZE_TEXT::TabToSpacex4(string& s)
{
	string Tab = "\t";
	string Space   = "    ";
	
	// size_t pos = s.find_first_of(YenN, 0); // NG. don't use find_first_of here.
	size_t pos = s.find(Tab, 0);

	while(pos != string::npos){
		s.replace(pos, Tab.length(), Space);
		pos = s.find(Tab, pos);
	}
}

/******************************
description
	外部fileから文字列を読み込んだ時、
	string文字列中の"\n"は実は"\\n"(\\ + n の２文字)、改行文字のリテラルではない.
	本methodは、これを変換するためのmethod.
	
参考
	http://d.hatena.ne.jp/p95095yy/20050222/1109119001
	注意)
		本URLでは、find_first_of を使っているが、間違い.
		find_first_of()は、「列挙された1文字ずつを」探しに行く.
		正解は、「文字列」を探しに行く find().
******************************/
void VISUALIZE_TEXT::YennToCR(string& s)
{
	string YenN = "\\n";
	string CR   = "\n";
	
	// size_t pos = s.find_first_of(YenN, 0); // NG. don't use find_first_of here.
	size_t pos = s.find(YenN, 0);

	while(pos != string::npos){
		s.replace(pos, YenN.length(), CR);
		pos = s.find(YenN, pos);
	}
}

/******************************
description
	外部fileに戻す時は、
	"\n" -> "\\n"
******************************/
string VISUALIZE_TEXT::CR_To_Yenn(string s)
{
	string YenN = "\\n";
	string CR   = "\n";
	
	size_t pos = s.find(CR, 0);

	while(pos != string::npos){
		s.replace(pos, CR.length(), YenN);
		pos = s.find(CR, pos);
	}
	
	return s;
}

/******************************
******************************/
void VISUALIZE_TEXT::Read_1Line_FromFile(int ContentsId, string& str_Line)
{
	/********************
	********************/
	char buf[BUF_SIZE_S];
	
	if(fgets(buf, BUF_SIZE_S, fp_Contents[ContentsId]) == NULL){
		fseek(fp_Contents[ContentsId], 0, SEEK_SET);
		
		if(fgets(buf, BUF_SIZE_S, fp_Contents[ContentsId]) == NULL){
			printf("No Contents in fp_Contents[%d]\n", ContentsId);
			std::exit(1);
		}else{
			/*
			printf("File_%d Loop\n", ContentsId);
			fflush(stdout);
			*/
		}
	}
	
	/********************
	********************/
	str_Line = buf;
	TabToSpacex4(str_Line);
	// Align_StringOfData(str_Line);
}

/******************************
******************************/
void VISUALIZE_TEXT::setup()
{
	/********************
	********************/
	font.load("font/RictyDiminishedDiscord-Bold.ttf", 11, true, true, true);
	// font.load("font/RictyDiminished-Regular.ttf", 13, true, true, true);
	
	/********************
	********************/
	for(int i = 0; i < NUM_FBO_BASE__BLACK_WHITE; i++) fbo[i].allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 0);
	
	fbo_SyphonServer[FBO_BASE_WHITE].setName("oFMirror_White");
	fbo_SyphonServer[FBO_BASE_BLACK].setName("oFMirror_Black");
	
	/********************
	********************/
	t_LastInt = ofGetElapsedTimef();
}

/******************************
******************************/
void VISUALIZE_TEXT::update(float _now)
{
	/********************
	********************/
	now = _now;
	
	/********************
	********************/
	Contents_LineFeed(0, now - t_LastInt);
	Contents_LineFeed(1, now - t_LastInt);
	
	for(int i = 0; i < NUM_CONTENTS; i++){
		CursorPos[i] += CursorSpeed[i] * (now - t_LastInt);
		if(fbo[FBO_BASE_BLACK].getHeight() + 30 < CursorPos[i]) CursorPos[i] = 0;
	}
	
	/********************
	********************/
	t_LastInt = now;
}

/******************************
******************************/
void VISUALIZE_TEXT::Contents_LineFeed(int Contents_id, double dt)
{
	/********************
	********************/
	NumLines_Proceed[Contents_id] += N_Lines_PerSec[Contents_id] * dt;
	int int_NumLines_Proceed = int(NumLines_Proceed[Contents_id]);
	NumLines_Proceed[Contents_id] = NumLines_Proceed[Contents_id] - int_NumLines_Proceed;
	
	for(int i = 0; i < int_NumLines_Proceed; i++){
		string str_Line;
		Read_1Line_FromFile(Contents_id, str_Line);
		
		update_ContentsChain(Contents_id, str_Line);
	}
}

/******************************
******************************/
void VISUALIZE_TEXT::draw_and_sendsyphon(int VisualizeText_Disp_id)
{
	/********************
	********************/
	for(int i = 0; i < NUM_FBO_BASE__BLACK_WHITE; i++) draw_Contents_toFbo(i);
	
	/********************
	********************/
	ofEnableAlphaBlending();
	ofEnableSmoothing();
	ofSetColor(255, 255, 255, 255);
	fbo[VisualizeText_Disp_id].draw(0, 0);
	
	/********************
	publish syphon.
	********************/
	c_syphon++;
	if(c_syphon % 2 == 0){
		c_syphon = 0;
		return;
	}
	
	for(int i = 0; i < NUM_FBO_BASE__BLACK_WHITE; i++){
		ofTexture tex = fbo[i].getTextureReference();
		fbo_SyphonServer[i].publishTexture(&tex);
	}
}


/******************************
******************************/
double VISUALIZE_TEXT::get_FontHeight()
{
	// return font.stringHeight("0123456789abcdefghijklmnopqrstuvwxyz") * 1.5;
	return font.stringHeight("0123456789abcdefghijklmnopqrstuvwxyz") * 1.5;
}

/******************************
******************************/
void VISUALIZE_TEXT::draw_Contents_toFbo(int Base_BlackOrWhite)
{
	/********************
	********************/
	ofDisableAlphaBlending();
	ofEnableSmoothing();
	
	/********************
	********************/
	float stringHeight = get_FontHeight();;
	
	fbo[Base_BlackOrWhite].begin();
		if(Base_BlackOrWhite == FBO_BASE_BLACK){
			ofClear(0, 0, 0, 0);
			ofSetColor(255, 255, 255, 255);
		}else{
			ofClear(255, 255, 255, 255);
			ofSetColor(0, 0, 0, 255);
		}
		
		/********************
		contentd[0]
		********************/
		ofPushMatrix();
		{
			ofSetColor(255, 0, 0, 140);
			ofDrawRectangle(0, CursorPos[0], fbo[Base_BlackOrWhite].getWidth()*2/3, get_FontHeight()/2);
			
			double WrittenHeight = 0;
			CHAIN_CONTENTS* temp = TopOfContents[0];
			
			while(temp){
				ofTranslate(0, stringHeight);
				temp->draw(font, Base_BlackOrWhite);
				temp = temp->Next;
				
				WrittenHeight += stringHeight;
				if(fbo[Base_BlackOrWhite].getHeight() < WrittenHeight) break;
			}
		}
		ofPopMatrix();
		
		/********************
		contentd[1]
		********************/
		ofPushMatrix();
		{
			ofTranslate(fbo[Base_BlackOrWhite].getWidth(), fbo[Base_BlackOrWhite].getHeight());
			ofScale(-1, -1, 0);
			
			ofSetColor(0, 100, 255, 140);
			ofDrawRectangle(0, CursorPos[1], fbo[Base_BlackOrWhite].getWidth()*2/3, get_FontHeight()/2);
			
			
			double WrittenHeight = 0;
			CHAIN_CONTENTS* temp = TopOfContents[1];
			
			while(temp){
				ofTranslate(0, stringHeight);
				temp->draw(font, Base_BlackOrWhite);
				temp = temp->Next;
				
				WrittenHeight += stringHeight;
				if(fbo[Base_BlackOrWhite].getHeight() < WrittenHeight) break;
			}
		}
		ofPopMatrix();
	fbo[Base_BlackOrWhite].end();
}

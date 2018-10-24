/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "stdio.h"
#include <time.h>

#include "ofMain.h"

/************************************************************
************************************************************/
#define ERROR_MSG(); printf("Error in %s:%d\n", __FILE__, __LINE__);
#define WARNING_MSG(); printf("Warning in %s:%d\n", __FILE__, __LINE__);


enum{
	WINDOW_WIDTH	= 1000,
	WINDOW_HEIGHT	= 1000,
};

enum{
	BUF_SIZE_S = 1000,
	BUF_SIZE_L = 6000,
	
	BUF_SIZE_UDP = 100000,
};

/************************************************************
************************************************************/

/**************************************************
Derivation
	class MyClass : private Noncopyable {
	private:
	public:
	};
**************************************************/
class Noncopyable{
protected:
	Noncopyable() {}
	~Noncopyable() {}

private:
	void operator =(const Noncopyable& src);
	Noncopyable(const Noncopyable& src);
};

/**************************************************
**************************************************/
class SOUND_FADE{
private:
	enum STATE{
		STATE_STOP,
		STATE_PAUSE,
		STATE_PLAY,
	};
	
	string str_FileName;
	ofSoundPlayer Sound;
	STATE State;
	const float vol_max;
	float vol_down_speed;
	float vol_up_speed;
	
	float now;
	float t_LastUpdate;
	
	void vol_down(){
		float LastVol = Sound.getVolume();
		float vol = LastVol;
		
		vol -= vol_down_speed * (now - t_LastUpdate);
		if(vol < 0) vol = 0;
		
		if(vol != LastVol) Sound.setVolume(vol);
	}
	
	void vol_up(){
		float LastVol = Sound.getVolume();
		float vol = LastVol;
		
		vol += vol_up_speed * (now - t_LastUpdate);
		if(vol_max < vol) vol = vol_max;
		
		if(vol != LastVol) Sound.setVolume(vol);
	}
	
public:
	SOUND_FADE(string FileName, float _vol_max, float d_volDown, float d_volUp)
	: State(STATE_STOP)
	, vol_max(_vol_max)
	, t_LastUpdate(0)
	, str_FileName(FileName)
	{
		vol_down_speed = (vol_max - 0) / d_volDown;
		vol_up_speed = (vol_max - 0) / d_volUp;
		
		Sound.loadSound(FileName.c_str());
		Sound.setLoop(true);
		Sound.setMultiPlay(true);
		Sound.setVolume(0);
	}
	
	void update(){
		now = ofGetElapsedTimef();
		
		ofSoundUpdate();
		
		switch(State){
			case STATE_STOP:
				vol_down();
				if(Sound.getVolume() <= 0){
					if(Sound.isPlaying()) Sound.stop();
				}
				break;
				
			case STATE_PAUSE:
				vol_down();
				if(Sound.getVolume() <= 0) Sound.setPaused(true);
				break;
				
			case STATE_PLAY:
				vol_up();
				break;
		}
		
		t_LastUpdate = now;
	}
	
	void play(){
		switch(State){
			case STATE_STOP:
				if(!Sound.isPlaying()) Sound.play();
				Sound.setPaused(false);
				break;
				
			case STATE_PAUSE:
				if(!Sound.isPlaying()) Sound.play();
				Sound.setPaused(false);
				break;
				
			case STATE_PLAY:
				break;
		}
		
		State = STATE_PLAY;
	}
	
	void stop(){
		State = STATE_STOP;
	}
	
	void pause(){
		switch(State){
			case STATE_STOP:
				break;
				
			case STATE_PAUSE:
				break;
				
			case STATE_PLAY:
				State = STATE_PAUSE;
				break;
		}
	}
	
	void CheckStatus(){
		printf("[%s] isLoaded:%d, isPlaying:%d, vol:%f, state:%d\n", str_FileName.c_str(), Sound.isLoaded(), Sound.isPlaying(), Sound.getVolume(), State);
	}
};

/**************************************************
**************************************************/
class FROM_GOLEM__FRAMEDATA_ALL{
public:
	/****************************************
	****************************************/
	enum{
		NUM_REALSENSOR	= 6,
		NUM_AI_SENSOR	= 27,
	};
	
	/****************************************
	****************************************/
	bool b_set;
	
	ofVec3f INS_pos[NUM_REALSENSOR];
	ofVec3f AI1_pos[NUM_REALSENSOR];
	ofVec3f AI2_pos[NUM_AI_SENSOR];
	ofVec4f AI2_quartenion[NUM_AI_SENSOR];
	
	float ZUPT_L;
	float ZUPT_R;
	float Hand_VelNorm_L;
	float Hand_VelNorm_R;
	float Hand_Theta_L;
	float Hand_Theta_R;
	
	int FrameId;

public:
	/****************************************
	****************************************/
	
	/******************************
	******************************/
	FROM_GOLEM__FRAMEDATA_ALL()
	: b_set(false)
	{
	}
	
	/******************************
	******************************/
	void set(vector<string> &block){
		/********************
		********************/
		vector<string> data = ofSplitString(block[1],"|");
		
		if( data.size() < 232) { WARNING_MSG(); return; }
		
		/********************
		********************/
		b_set = true;
		
		
		int ofs = 1;
		for(int i = 0; i < 6; i++){
			INS_pos[i].set( atof(data[i * 3 + ofs + 0].c_str()), atof(data[i * 3 + ofs + 1].c_str()), atof(data[i * 3 + ofs + 2].c_str()) );
		}
		
		ofs = 19;
		for(int i = 0; i < 6; i++){
			AI1_pos[i].set( atof(data[i * 3 + ofs + 0].c_str()), atof(data[i * 3 + ofs + 1].c_str()), atof(data[i * 3 + ofs + 2].c_str()) );
		}
		
		ofs = 37;
		for(int i = 0; i < 27; i++){
			AI2_pos[i].set( atof(data[i * 7 + ofs + 0].c_str()), atof(data[i * 7 + ofs + 1].c_str()), atof(data[i * 7 + ofs + 2].c_str()) );
			AI2_quartenion[i].set( atof(data[i * 7 + ofs + 3].c_str()), atof(data[i * 7 + ofs + 4].c_str()), atof(data[i * 7 + ofs + 5].c_str()), atof(data[i * 7 + ofs + 6].c_str()) );
		}
		
		ofs = 226;
		ZUPT_L = atof(data[ofs].c_str());
		
		ofs++;
		ZUPT_R = atof(data[ofs].c_str());
		
		ofs++;
		Hand_VelNorm_L = atof(data[ofs].c_str());
		
		ofs++;
		Hand_Theta_L = atof(data[ofs].c_str());
		
		ofs++;
		Hand_VelNorm_R = atof(data[ofs].c_str());
		
		ofs++;
		Hand_Theta_R = atof(data[ofs].c_str());
		
		// FrameId is for debug.
		// so, Real Golem system will not send this item.
		ofs++;
		if(233 <= data.size())	FrameId = atof(data[ofs].c_str());
		else					FrameId = 0;
	}
};




/************************************************************
************************************************************/
extern FILE* fp_Log;

extern FROM_GOLEM__FRAMEDATA_ALL FromGolem_FrameDataAll;

extern int GPIO_0;
extern int GPIO_1;

extern unsigned int float2hex(float val);

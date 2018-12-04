#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( int argc, char** argv ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	if(argc < 3){
		printf("exe [Golem_0 IP] [Golem_1 IP]\n");
		printf("\tGolem_0:127.0.0.1 or 10.0.0.5\n");
		printf("\tGolem_1:127.0.0.1 or 10.0.0.6\n");
		return -1;
		
	}else{
		if( (strcmp(argv[1], "127.0.0.1") != 0) && (strcmp(argv[1], "10.0.0.5") != 0) ){
			printf("invalid Golem_0 IP\n");
			return -1;
		}
		
		if( (strcmp(argv[2], "127.0.0.1") != 0) && (strcmp(argv[2], "10.0.0.6") != 0) ){
			printf("invalid Golem_1 IP\n");
			return -1;
		}
		
		ofRunApp(new ofApp(argv[1], argv[2]));
	}
}

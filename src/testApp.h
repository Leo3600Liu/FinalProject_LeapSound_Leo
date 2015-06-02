//Created by Jiteng Liu (Leo)

#pragma once

#include "ofMain.h"
#include "ofxLeapMotion.h"
#include "ofxStrip.h"
#include "ofxGenSound.h"

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
    void exit();
    
    void audioOut(float * output, int bufferSize, int nChannels);       //void drawWaveform(ofVec2f& position);
		
	ofxLeapMotion leap;     //set up the leap motion
	vector <ofxLeapMotionSimpleHand> simpleHands;       //set the hands input
    
	vector <int> fingersFound;      //the fingers that can be found by leap motion
	ofEasyCam cam;      //set a easy cam
	ofLight l1;       //set the light 1
	ofLight l2;       //set the light 2
	ofMaterial m1;      //set the material 1
	
	map <int, ofPolyline> fingerTrails;     //set the finger trails
    //void audioOut(float * input, int bufferSize, int nChannels);
    
    ofSoundStream soundStream;      //set the sound stream
    int sampleRate;     //set the rate
    vector <float> audioOutLeft;        //left audio out
    vector <float> audioOutRight;       //right audio out
    float pan;      //set the pan
    
    //void drawWaveform(ofVec2f& position);
    //ofxGenSound
    
    ofxGenSoundOsc triWave, sineWave;       //set the gen sound tril wave and sine waave
    ofxGenSoundEnvelope env;        //set the gen sound env
    ofxGenSoundDelay delay;     //set the gen sound delay
    ofxGenSoundFilter filter;       //set the filterk
    
    //GUI
    //ofxPanel gui;
    //ofxFloatSlider freq1, freq2, filterCutoff, delayFeedback;
};

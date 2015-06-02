//Created by Jiteng Liu (Leo)

/* Note on OS X, you must have this in the Run Script Build Phase of your project.
 where the first path ../../../addons/ofxLeapMotion/ is the path to the ofxLeapMotion addon.
 
 cp -f ../../../addons/ofxLeapMotion/libs/lib/osx/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/libLeap.dylib"; install_name_tool -change ./libLeap.dylib @executable_path/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/$PRODUCT_NAME";
 
 If you don't have this you'll see an error in the console: dyld: Library not loaded: @loader_path/libLeap.dylib
 */

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){      //To define the basic environment conditions before the program starts.

    ofSetFrameRate(60);     //the frame rate will be 60
    ofSetVerticalSync(true);    //make the vertical work
	ofSetLogLevel(OF_LOG_VERBOSE);      //set the level of log

	leap.open();    //open the leap motion

	l1.setPosition(200, 300, 50);   //set up the 1st position
	l2.setPosition(-200, -200, 50);     //set up the 2nd position

	cam.setOrientation(ofPoint(-20, 0, 0));     //cam point setting

	glEnable(GL_DEPTH_TEST);    //load a file
    glEnable(GL_NORMALIZE);     //load a file
    
    // WARNING: ALWAYS SET OFMAP TO "TRUE" TO CLAMP WHEN WORKING WITH AUDIO AND CAREFUL WEARING HEADPHONES OTHERWISE YOU CAN DAMAGE YOUR EARS
    
    ofBackground(250);      //set the color of background
    
    // 2 output channels,
    // 0 input channels
    // 22050 samples per second
    // 512 samples per buffer
    // 4 num buffers (latency)
    
    int bufferSize = 512;       //set the buffer size
    sampleRate = 44100;     //set the rate
    audioOutLeft.resize(bufferSize);    //set the left audio
    audioOutRight.resize(bufferSize);       //set the right audio
    
    soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);       //set up the sound stream
    pan = 1.0;      //set the pan equal to 1
    
    // Set up one oscillator but passing in sample rate and buffersize
    // Set intial oscillator frequency
    // Set envelope parameters
    
    triWave.setup(soundStream.getSampleRate(), soundStream.getBufferSize());    //set the buffer wave
    triWave.setFrequency(440);      //set the frequency
    
    sineWave.setup(soundStream.getSampleRate(), soundStream.getBufferSize());       //set the sine wave
    sineWave.setFrequency(220);     //set the frequency
    
    filter.setCutoff(.5);       //set the filter
    delay.setFeedback(.8);      //the feedback time
    
    env.set(0.5, 0.07);     //attack and release
    //these are both 0.0 - 1.0 anything above is dangerous
    //attack 0.0 - 1.0
    //release 0.0 - 1.0
    
    //setup ofxGui
    //gui.setup("ofxGenSound");
    //gui.add(freq1.setup("Triangle Wave Freq", 440, 220, 600));
    //gui.add(freq2.setup("Sine Wave Freq", 220, 220, 600));
    //gui.add(filterCutoff.setup("Filter Cutoff", 0.5, 0.0, 0.5));
    //gui.add(delayFeedback.setup("Delay Feedback", 0.8, 0.0, 0.9));
    
    ofSetWindowTitle("ofxGenSound Example");        //load the GenSound
}

//--------------------------------------------------------------
void testApp::update(){     //set up the update
    env.trigger(ofGetKeyPressed());     //set up the trigger and keyboard in
	fingersFound.clear();       //tracking the fingers
	
	//here is a simple example of getting the hands and using them to draw trails from the fingertips.
	//the leap data is delivered in a threaded callback - so it can be easier to work with this copied hand data
	
	//if instead you want to get the data as it comes in then you can inherit ofxLeapMotion and implement the onFrame method. 
	//there you can work with the frame data directly. 

    //Option 1: Use the simple ofxLeapMotionSimpleHand - this gives you quick access to fingers and palms.
    //
    //simpleHands = leap.getSimpleHands();
    //
    //if( leap.isFrameNew() && simpleHands.size() ){
    //
    //  leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
    //	leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
    //  leap.setMappingZ(-150, 150, -200, 200);
    //
    //  for(int i = 0; i < simpleHands.size(); i++){
    //
    //      for(int j = 0; j < simpleHands[i].fingers.size(); j++){
    //          int id = simpleHands[i].fingers[j].id;
    //
    //           ofPolyline & polyline = fingerTrails[id];
    //           ofPoint pt = simpleHands[i].fingers[j].pos;
    //
    ////if the distance between the last point and the current point is too big - lets clear the line
    ////this stops us connecting to an old drawing
    //       if( polyline.size() && (pt-polyline[polyline.size()-1] ).length() > 50 ){
    //          polyline.clear();
    //       }
    //
    ////add our point to our trail
    //       polyline.addVertex(pt);
    //
    ////store fingers seen this frame for drawing
    //       fingersFound.push_back(id);
    //       }
    //   }
    //}
    //

    //Option 2: Work with the leap data / sdk directly - gives you access to more properties than the simple approach
    //uncomment code below and comment the code above to use this approach. You can also inhereit ofxLeapMotion and get the data directly via the onFrame callback.
    
	vector <Hand> hands = leap.getLeapHands();      //set the hands leap motion
	if( leap.isFrameNew() && hands.size() ){        //set the size

		//leap returns data in mm - lets set a mapping to our world space. 
		//you can get back a mapped point by using ofxLeapMotion::getMappedofPoint with the Leap::Vector that tipPosition returns  
		
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);       //set x axis in 3D
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);       //set y axis in 3D
		leap.setMappingZ(-150, 150, -200, 200);     //set z axis in 3D
				
		for(int i = 0; i < hands.size(); i++){      //set the min and max hands size number
            for(int j = 0; j < hands[i].fingers().count(); j++){        //set the min and max fingers size number
				ofPoint pt;     //set pt
				const Finger & finger = hands[i].fingers()[j];      //the number of fingers
								
				//here we convert the Leap point to an ofPoint - with mapping of coordinates
				//if you just want the raw point - use ofxLeapMotion::getofPoint 
				pt = leap.getMappedofPoint( finger.tipPosition() );     //set the position of leap motion
      
				//lets get the correct trail (ofPolyline) out of our map - using the finger id as the key 
				ofPolyline & polyline = fingerTrails[finger.id()];      //set the polyline
				
				//if the distance between the last point and the current point is too big - lets clear the line 
				//this stops us connecting to an old drawing
				if( polyline.size() && (pt-polyline[polyline.size()-1] ).length() > 50 ){       //poline size number
					polyline.clear();       //work the polyline
				}
				
				//add our point to our trail
				polyline.addVertex(pt);     //add more point
				
				//store fingers seen this frame for drawing
				fingersFound.push_back(finger.id());        //draw the back
			}
		}
        
        ofPoint pt;     //more points
        //const Finger & finger = hands[0].fingers()[0];
        const Finger & finger = hands[0].fingers()[1];      //choose the finger
        //const Finger & finger = hands[0].fingers()[2];
        //const Finger & finger = hands[0].fingers()[3];
        //const Finger & finger = hands[0].fingers()[4];
								
        //here we convert the Leap point to an ofPoint - with mapping of coordinates
        //if you just want the raw point - use ofxLeapMotion::getofPoint
        pt = leap.getMappedofPoint( finger.tipPosition() );     //the position of finger in leap motion
        triWave.setFrequency(ofMap(pt.y,-600, 600, 100, 220));      //set the tril wave
        sineWave.setFrequency(ofClamp(pt.y, 50, 100));      //set the sine wave
        
        filter.setCutoff(ofClamp(pt.x, 0.5, 1));        //set the filter on and off
	}

	//IMPORTANT! - tell ofxLeapMotion that the frame is no longer new.
	leap.markFrameAsOld();      //set the leap motion
    
    //trigger an envelope
    //control frequency
    
    //triWave.setFrequency(freq1);
    //sineWave.setFrequency(freq2);
    //control filter cutoff
    
    //filter.setCutoff(filterCutoff);
    //delay.setFeedback(delayFeedback);
    delay.setMix(0.5);      //set the mix rate
}

//--------------------------------------------------------------
void testApp::draw(){       //To excute the code.
	ofDisableLighting();        //the background lighting
    ofBackgroundGradient(ofColor(60, 60, 60), ofColor(60, 120, 30),  OF_GRADIENT_BAR);       //set up the background
	
	ofSetColor(100);        //set up the color
	ofDrawBitmapString("ofxLeapMotion - Example App\nLeap Connected? " + ofToString(leap.isConnected()), 50, 50);       //draw the string

	cam.begin();        //get start

	ofPushMatrix();     //start the push matrix
		ofRotate(90, 0, 0, 1);      //set the rotate
		ofSetColor(20);     //set the color
		ofDrawGridPlane(800, 20, false);        //draw the grid plane
	ofPopMatrix();      //finish the push matrix
	
	ofEnableLighting();     //the background lighting
	l1.enable();        //set the lighting 1
	l2.enable();        //set the lighting 2
	
	m1.begin();         //begin the m1
	m1.setShininess(1);     //set the shininess of m1
	
	for(int i = 0; i < fingersFound.size(); i++){       //set the min and max size of fingers
		ofxStrip strip;     //set the strip
		int id = fingersFound[i];       //fingers' number
		
		ofPolyline & polyline = fingerTrails[id];       //set the polyline
        polyline.getSmoothed(10);       //set the smooth of polyline
		strip.generate(polyline.getVertices(),      //set the polyline point
                       5, ofPoint(100, 100, 10) );
        ofSetColor(250, 18, 150, 180);     //set the color
		//ofSetColor(255 - id * 15, 0, id * 25);
		strip.getMesh().draw();     //draw the mesh
    }

    l2.disable();       //dinsh working of l2
    
    //for(int i = 0; i < simpleHands.size(); i++){
    //  simpleHands[i].debugDraw();
    //}
    
    ofVec2f vec = ofVec2f(0, ofGetHeight() * .5);
    //drawWaveform(vec);
    //gui.draw();
    ofDrawBitmapString("Press Any Key to Generate a Tone", ofGetWidth() * .10, ofGetHeight() * .25);        //set the width and height
	
    m1.end();       //finish the m1
	cam.end();      //finish the cam
}

//--------------------------------------------------------------
void testApp::audioOut(float * output, int bufferSize, int nChannels){      //set the audio output
    
    // This is your left and right speakers //
    
    float leftScale = -1.0;     //set the left scale
    float rightScale = 1.0;     //set the right scale
    
    // This runs through the audio buffer at the rate of the audioOut core audio event //
    
    for (int i = 0; i < bufferSize; i++){       //the min and max buffer size
        
        // Signal Chain //
        float waveOut = (triWave.setOscillatorType(OF_TRIANGLE_WAVE) * sineWave.setOscillatorType(OF_SINE_WAVE)) * env.addEnvelope();       //set the wave out
        float filterOut = filter.addFilter(OF_FILTER_LP, waveOut);      //set the filter out
        float delayOut = delay.addDelay(filterOut);     //set the delay out
        
        // Output Sound //
        audioOutRight[i] = output[ i * nChannels    ] = delayOut * rightScale;      //output right sound
        audioOutLeft[i] = output[ i * nChannels + 1 ] = delayOut * leftScale;       //output left sound
    }
}

//void testApp::drawWaveform(ofVec2f& position) {
//    
//    ofPushStyle();
//    ofPushMatrix();
//    ofTranslate(position);
//    ofColor c = c.turquoise;
//    ofSetColor(c);
//    ofNoFill();
//    ofBeginShape();
//    
//    for (int i = 0; i < audioOutRight.size(); i++) {
//        ofVertex(i * 5, audioOutRight[i] * 100 );
//    }
//    ofEndShape();
//    
//    ofPopMatrix();
//    ofPopStyle();
//    
//}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
  
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void testApp::exit(){
// let's close down Leap and kill the controller
    leap.close();       //close the leap motion
}

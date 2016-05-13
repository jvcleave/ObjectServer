#pragma once

#include "ofMain.h"
#include "ofxTCPObjects.h"

class ofApp : public ofBaseApp{

public:

    vector<ofImage> images;

    ofxTCPObjects tcpHelper;
    ofBuffer receiveBuffer;
    
    ofEasyCam cam;

    void setup();
    void update();
    void draw();

    ofMesh myMesh;
};


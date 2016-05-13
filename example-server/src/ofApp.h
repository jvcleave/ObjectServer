#pragma once

#include "ofMain.h"
#include "ofxTCPObjects.h"

class ofApp : public ofBaseApp
{

public:

    bool doSendImage;
    bool doSendMesh;
    
    ofMesh mesh;
    ofSpherePrimitive sphere;

    ofBuffer imageFileBuffer;
    ofxTCPObjects server;
    ofEasyCam cam;

    void setup()
    {
        ofSetLogLevel(OF_LOG_VERBOSE);
        
        server.createImageServer();
        server.createMeshServer();
        doSendImage = false;
        doSendMesh = false;
        imageFileBuffer = ofBufferFromFile("bikers.jpg");
        
       // mesh.setMode(OF_PRIMITIVE_POINTS);
        //mesh.enableColors();
        
        sphere.set(500, 500, OF_PRIMITIVE_TRIANGLES);
        mesh = sphere.getMesh();
       
        for(size_t i=0; i<mesh.getVertices().size(); i++)
        {
            mesh.addColor(ofColor(ofRandom(0, 255),
                                  ofRandom(0, 255),
                                  ofRandom(0, 255),
                                  255));
        }
        //cam.setDistance(20);
        ofSetWindowPosition(ofGetWidth()*2, 0);
    }
    
    void update()
    {
        
        if(doSendImage)
        {
            server.sendImage(imageFileBuffer);
            doSendImage = false;
        }
        
        if(doSendMesh)
        {  
            server.sendMesh(mesh);
            doSendMesh = false;
        }
        
    }
    
    void draw(){
        
        cam.begin();
            mesh.drawWireframe();
        cam.end();
        ofSetWindowTitle(ofToString(cam.getDistance()));
        stringstream info;
        info << server.getImageServerInfo() << endl;
        info << server.getMeshServerInfo() << endl;
        
        ofDrawBitmapString(info.str(), 20, 20);
        
    }
    
    //--------------------------------------------------------------
    void keyPressed(int key)
    {
        switch (key)
        {
            case '1':
            {
                doSendImage = true;
                break;
            }
            case '2':
            {
                doSendMesh = true;
                break;
            }
            default:
                break;
        }
    }
    





};


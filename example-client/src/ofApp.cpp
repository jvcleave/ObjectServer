#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    tcpHelper.createImageClient();
    tcpHelper.createMeshClient();
    cam.setDistance(20);
}


void ofApp::update()
{
    ofImage* image = tcpHelper.getImage();
    if(image)
    {
        images.push_back(*image);
    }
    
    ofMesh* mesh = tcpHelper.getMesh();
    if(mesh)
    {
        myMesh = *mesh;
    }
    
}

void ofApp::draw()
{
    /*
     int x=0;
     for(size_t i=0; i<images.size(); i++)
     {
     ofImage& image = images[i];
     image.draw(x, 0, image.getWidth(), image.getHeight());
     x+=image.getWidth();
     }
     */
    stringstream info;
    cam.begin();
    
        info << myMesh.getVertices().size() << endl;
        myMesh.drawWireframe();
    cam.end();
    
    ofDrawBitmapStringHighlight(info.str(), 20, 20, ofColor::black, ofColor::yellow);
}

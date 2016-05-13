#pragma once
#include "ofMain.h"
#include "ofxNetwork.h"
#include "MeshBuffer.h"


class TCPMessageThread : public ofThread
{
public:
    ofxTCPServer* server;
    ofBuffer* buffer;
    bool doAbort;
    bool didSend;
    TCPMessageThread()
    {
        server = NULL;
        buffer = NULL;
        doAbort = false;
        didSend = false;
    }
    ~TCPMessageThread()
    {
        ofLogVerbose() << "~TCPMessageThread";
        server = NULL;
        buffer = NULL;
    }
    void setup(ofxTCPServer* server_, ofBuffer* buffer_)
    {
        server = server_;
        buffer = buffer_;
        
    }
    void send()
    {
        startThread();
    }
    void threadedFunction()
    {
        while(!doAbort)
        {
            for(int i = 0; i < server->getLastID(); i++)
            {
                if( server->isClientConnected(i) )
                {
                    didSend = server->sendRawBytes(i, (const char *)buffer->getData(), buffer->size());
                    if(didSend)
                    {
                        ofLogVerbose() << "didSend: " << didSend << " buffer->size(): " << buffer->size() << "server->getNumReceivedBytes(i): " << server->getNumReceivedBytes(i);
                        doAbort = true;
                    }else
                    {
                        ofLogError() << "FAILED" << server->getNumReceivedBytes(i);
                        doAbort = true;
                    }
                }
            }
        }
        stopThread();
    }
};

class ofxTCPObjects : public ofThread
{
public:
    
    ofxTCPServer imageServer;
    ofxTCPClient imageReceiver;
    
    
    ofxTCPServer meshServer;
    ofxTCPClient meshReceiver;
    vector<TCPMessageThread*> messages;
    ofxTCPObjects()
    {

        
    }
    void threadedFunction()
    {
        while(isThreadRunning())
        {
            for(int i = 0; i < messages.size(); i++)
            {
                if(!messages[i]->isThreadRunning() && messages[i]->didSend)
                {
                    //ofLogVerbose() << "message should be deleted: " << i;
                    TCPMessageThread* instance = messages[i];
                    messages.erase (messages.begin()+i);
                    delete instance;
                    
                }
            }
        }
        stopThread();
    }
    
    void sendMessage(TCPMessageThread* message)
    {
        if(!isThreadRunning())
        {
            startThread();
        }
        messages.push_back(message);
        message->send();
    }
    
    void sendImage(ofBuffer& imageFileBuffer)
    {
        
        TCPMessageThread* message = new TCPMessageThread();
        message->setup(&imageServer, &imageFileBuffer);
        sendMessage(message);
    }
    
    void sendMesh(ofMesh& mesh)
    {
        
        MeshBuffer meshBufferObject;
        ofBuffer* meshBuffer = meshBufferObject.getBuffer(mesh);
        TCPMessageThread* message = new TCPMessageThread();
        message->setup(&meshServer, meshBuffer);
        sendMessage(message);        
    }
    void createImageClient(string ip="127.0.0.1", int port=11999)
    {
        imageReceiver.setup(ip, port);
    }
    void createImageServer(int port=11999)
    {
        imageServer.setup(port, true);
    }
    
    void createMeshClient(string ip="127.0.0.1", int port=11998)
    {
        meshReceiver.setup(ip, port);
    }
    
    void createMeshServer(int port=11998)
    {
        meshServer.setup(port, true);
    }
    
    string getImageServerInfo()
    {
        stringstream info;
        info << "Image Server port: " << imageServer.getPort() << endl;
        info << "CLIENTS" << endl;
        for(size_t i = 0; i <imageServer.getLastID(); i++)
        {
            
            if( imageServer.isClientConnected(i) )
            {
                
                // get the ip and port of the client
                string port = ofToString( imageServer.getClientPort(i) );
                string ip   = imageServer.getClientIP(i);
                
                info << i << " port: " << port << " ip: " << ip  << endl;
            }
        }
        return info.str();

    }
    
    string getMeshServerInfo()
    {
        stringstream info;
        info << "Mesh Server port: " << meshServer.getPort() << endl;
        info << "CLIENTS" << endl;
        for(size_t i = 0; i <meshServer.getLastID(); i++)
        {
            
            if( meshServer.isClientConnected(i) )
            {
                
                // get the ip and port of the client
                string port = ofToString( meshServer.getClientPort(i) );
                string ip   = meshServer.getClientIP(i);
                
                info << i << " port: " << port << " ip: " << ip  << endl;
            }
        }
        return info.str();

    }
    char * getLocalBuffer(int amount=512)
    {
        return new char[amount];
    }
    
    void appendAndDelete(char * localBuffer, ofBuffer* receiveBuffer, int bytes)
    {
        receiveBuffer->append(localBuffer, bytes);
        localBuffer = NULL;
        delete[] localBuffer;
        ofLogVerbose() << "bytes: " << bytes;
        
    }
    
    
    ofImage* getImage()
    {
        ofBuffer* receiveBuffer = new ofBuffer();
        if(imageReceiver.isConnected())
        {
            int bytes=0;
            int totalBytes=0;
            char * localBuffer;
            bool isFinished = false;
            while((bytes = imageReceiver.receiveRawBytes((localBuffer = getLocalBuffer()), 512)) > 0)
            {
                appendAndDelete(localBuffer, receiveBuffer, bytes);
                totalBytes+=bytes;
                
            }
        }
        if(receiveBuffer->size() == 0)
        {
            return NULL;
        }
        ofImage* image = new ofImage();
        ofLoadImage(*image, *receiveBuffer);
        delete receiveBuffer;
        return image;
    }
    
    ofMesh* getMesh()
    {
        ofBuffer receiveBuffer;
        int totalBytes=0;
        if(meshReceiver.isConnected())
        {
            int bytes=0;
            
            char * localBuffer;
            bool isFinished = false;
            while((bytes = meshReceiver.receiveRawBytes((localBuffer = getLocalBuffer(0)), 512)) > 0)
            {
                appendAndDelete(localBuffer, &receiveBuffer, bytes);
                totalBytes+=bytes;
                
            }
        }
        if(receiveBuffer.size() == 0)
        {
            return NULL;
        }
        ofLogVerbose() << "totalBytes: " << totalBytes;
        
        MeshBuffer meshBufferObject;
        return meshBufferObject.getMesh(receiveBuffer);

    }
    
};
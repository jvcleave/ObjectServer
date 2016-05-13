#pragma once
#include "ofMain.h"


class MeshBuffer
{
    
public:
    
    const string MESH_TYPE="ofMesh";
    const string VERTS_BEGIN="[verts]";
    const string VERTS_END="[/verts]";
    const string NUM_VERTS_BEGIN="[numverts]";
    const string NUM_VERTS_END="[/numverts]";
    const string VERTS_VALUES_BEGIN="[verts_values]";
    const string VERTS_VALUES_END="[/verts_values]";
    
    const string COLORS_BEGIN="[colors]";
    const string COLORS_END="[/colors]";
    const string NUM_COLORS_BEGIN="[numcolors]";
    const string NUM_COLORS_END="[/numcolors]";
    const string COLOR_VALUES_BEGIN="[color_values]";
    const string COLOR_VALUES_END="[/color_values]";
    
    
    const string NORMALS_BEGIN="[normals]";
    const string NORMALS_END="[/normals]";
    const string NUM_NORMALS_BEGIN="[numnormals]";
    const string NUM_NORMALS_END="[/numnormals]";
    const string NORMALS_VALUES_BEGIN="[normals_values]";
    const string NORMALS_VALUES_END="[/normals_values]";
    
    
    const string INDICES_BEGIN="[indices]";
    const string INDICES_END="[/indices]";
    const string NUM_INDICES_BEGIN="[numindices]";
    const string NUM_INDICES_END="[/numindices]";
    const string INDICES_VALUES_BEGIN="[indices_values]";
    const string INDICES_VALUES_END="[/indices_values]";
    
    MeshBuffer()
    {
        
    }
    
    int findDelimiter(ofBuffer& inputBuffer, string delimiter)
    {
        char * data = (char *)inputBuffer.getData();
        int size = inputBuffer.size();
        
        unsigned int posInDelimiter=0;
        for(int i=0; i<size; i++)
        {
            if(data[i]==delimiter[posInDelimiter])
            {
                posInDelimiter++;
                if(posInDelimiter==delimiter.size())
                {
                    return i-delimiter.size()+1;
                }
            }else
            {
                posInDelimiter=0;
            }
        }
        return -1;
    }
    
    
    int getValue(ofBuffer& buffer, string openTag, string closeTag)
    {
        unsigned char * data = (unsigned char *)buffer.getData();
        int start = findDelimiter(buffer, openTag);
        if(start < 0)
        {
            ofLogError() << openTag << " NOT FOUND";
            return;
        }
        
        int end = findDelimiter(buffer, closeTag);
        if(end < 0)
        {
            ofLogError() << closeTag << " NOT FOUND";
            return;
        }
        
        int startPos = start+openTag.length();
        
        data+=startPos;
        
        int counter = startPos;
        stringstream ss;
        while(counter != end)
        {
            ss << data[0];
            data++;
            counter++;
        }
        int result =  ofToInt(ss.str());
        return result;
        
    }
    
    template<typename T>
    void fillVector(vector<T>& targetCollection, ofBuffer& buffer, int numObjects, string openTag, string closeTag)
    {
        unsigned char * data = (unsigned char *)buffer.getData();
        int start = findDelimiter(buffer, openTag);
        if(start < 0)
        {
            ofLogError() << openTag << " NOT FOUND";
            return;
        }
        int end = findDelimiter(buffer, closeTag);
        if(end < 0)
        {
            ofLogError() << closeTag << " NOT FOUND";
            return;
        }
        
        int startPos = start+openTag.length();
        data+=startPos;
        int counter = startPos;
        int stepSize = sizeof(T);
        for(int i=0; i<numObjects; i++)
        {
            T value;
            memcpy(&value, data, stepSize);
            data+=stepSize;
            targetCollection.push_back(value);
        }
    }
    
    
    template<typename T>
    void addTags(vector<T>& source, ofBuffer& buffer,
                 string openSectionTag, string closeSectionTag,
                 string openNumObjectsTag, string closeNumObjectsTag,
                 string openTag, string closeTag)
    {
        int numObjects = source.size();
        buffer.append(openSectionTag);
        buffer.append(openNumObjectsTag);
        buffer.append(ofToString(numObjects));
        buffer.append(closeNumObjectsTag);
        
        buffer.append(openTag);
        T* data = &source[0];
        int dataSize = sizeof(T)*numObjects;
        buffer.append((const char *)data, dataSize);
        buffer.append(closeTag);
        
        buffer.append(closeSectionTag);
        
    }
    
    ofBuffer* getBuffer(ofMesh& mesh)
    {
        ofBuffer* buffer = new ofBuffer();
        buffer->append(MESH_TYPE);
        addTags<ofVec3f>(mesh.getVertices(), *buffer,
                         VERTS_BEGIN, VERTS_END,
                         NUM_VERTS_BEGIN, NUM_VERTS_END,
                         VERTS_VALUES_BEGIN, VERTS_VALUES_END);
        
        addTags<ofFloatColor>(mesh.getColors(), *buffer,
                              COLORS_BEGIN, COLORS_END,
                              NUM_COLORS_BEGIN, NUM_COLORS_END,
                              COLOR_VALUES_BEGIN, COLOR_VALUES_END);
        
        addTags<ofVec3f>(mesh.getNormals(), *buffer,
                         NORMALS_BEGIN, NORMALS_END,
                         NUM_NORMALS_BEGIN, NUM_NORMALS_END,
                         NORMALS_VALUES_BEGIN, NORMALS_VALUES_END);
        
        addTags<ofIndexType>(mesh.getIndices(), *buffer,
                             INDICES_BEGIN, INDICES_END,
                             NUM_INDICES_BEGIN, NUM_INDICES_END,
                             INDICES_VALUES_BEGIN, INDICES_VALUES_END);
        
        return buffer;
        
    }
    
    ofMesh* getMesh(ofBuffer& buffer)
    {
        ofMesh* mesh = new ofMesh();
        int numVerts = getValue(buffer,
                                NUM_VERTS_BEGIN,
                                NUM_VERTS_END);
        
        fillVector<ofVec3f>(mesh->getVertices(),
                            buffer,
                            numVerts,
                            VERTS_VALUES_BEGIN, VERTS_VALUES_END);
        
        int numNormals = getValue(buffer,
                                  NUM_NORMALS_BEGIN, NUM_NORMALS_END);
        
        fillVector<ofVec3f>(mesh->getNormals(),
                            buffer,
                            numNormals,
                            NORMALS_VALUES_BEGIN, NORMALS_VALUES_END);
        
        int numIndicies = getValue(buffer,
                                   NUM_INDICES_BEGIN, NUM_INDICES_END);
        fillVector<ofIndexType>(mesh->getIndices(),
                                buffer,
                                numIndicies,
                                INDICES_VALUES_BEGIN, INDICES_VALUES_END);
        
        int numColors = getValue(buffer,
                                 NUM_COLORS_BEGIN, NUM_COLORS_END);
        fillVector<ofFloatColor>(mesh->getColors(),
                                 buffer,
                                 numColors,
                                 COLOR_VALUES_BEGIN, COLOR_VALUES_END);
        return mesh;
    }
    
};

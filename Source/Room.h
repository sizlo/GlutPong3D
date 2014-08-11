//
//  Room.h
//  GlutPong3D
//
//  Created by Tim Brier on 08/08/2014.
//  Copyright (c) 2014 tbrier. All rights reserved.
//

#ifndef __GlutPong3D__Room__
#define __GlutPong3D__Room__

#include <GLTools.h>
#include <GLMatrixStack.h>

class Room
{
public:
    Room();
    ~Room();
    
    void Init();
    void Render(GLMatrixStack *theModelViewMatrix);
    
private:
    M3DVector4f     mColour;
    
    GLTriangleBatch mWallBatch;
};

#endif /* defined(__GlutPong3D__Room__) */

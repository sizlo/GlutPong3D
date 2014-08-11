//
//  Ball.h
//  GlutPong3D
//
//  Created by Tim Brier on 08/08/2014.
//  Copyright (c) 2014 tbrier. All rights reserved.
//

#ifndef __GlutPong3D__Ball__
#define __GlutPong3D__Ball__

#include <GLTools.h>
#include <GLMatrixStack.h>

class Ball
{
public:
    Ball();
    ~Ball();
    
    void Init();
    void Center();
    void GiveStartingVelocity();
    void Render(GLMatrixStack *theModelViewMatrix);
    void Update(float deltaTimeMillis);
    void Hit(unsigned int playerNumber, M3DVector3f positionOnPad);
    
    float * GetPosition()
    {
        return mPosition;
    }
    
    float GetRadius()
    {
        return mRadius;
    }
    
private:
    M3DVector3f     mPosition;
    M3DVector3f     mMaxPosition;
    M3DVector3f     mMinPosition;
    M3DVector3f     mVelocity;
    M3DVector3f     mMaxVelocity;
    
    GLTriangleBatch mSphereBatch;
    float           mRadius;
    
    M3DVector4f     mColour;
};

#endif /* defined(__GlutPong3D__Ball__) */

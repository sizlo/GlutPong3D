//
//  Player.h
//  GlutPong3D
//
//  Created by Tim Brier on 11/08/2014.
//  Copyright (c) 2014 tbrier. All rights reserved.
//

#ifndef __GlutPong3D__Player__
#define __GlutPong3D__Player__

#include <GLTools.h>
#include <GLMatrixStack.h>

class Player
{
public:
    Player(unsigned int playerNumber);
    ~Player();
    
    void Init();
    void Render(GLMatrixStack *theModelViewMatrix);
    virtual void UpdatePosition(float deltaTimeMillis);
    void Update(float deltaTimeMillis);
    void Reset();
    void Score();

    unsigned int GetScore()
    {
        return mScore;
    }
    
protected:
    M3DVector3f     mPosition;
    M3DVector3f     mMaxPosition;
    M3DVector3f     mMinPosition;
    
private:
    GLBatch         mPadBatch;
    GLTriangleBatch mFarFaceBatch;
    GLTriangleBatch mNearFaceBatch;
    float           mXYRadius;
    float           mZRadius;
    
    bool            mRenderFarFace;
    bool            mRenderNearFace;
    
    M3DVector4f     mColour;
    
    unsigned int    mPlayerNumber;
    unsigned int    mScore;
};

#endif /* defined(__GlutPong3D__Player__) */

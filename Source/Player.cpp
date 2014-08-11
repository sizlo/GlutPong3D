//
//  Player.cpp
//  GlutPong3D
//
//  Created by Tim Brier on 11/08/2014.
//  Copyright (c) 2014 tbrier. All rights reserved.
//

#include "Player.h"
#include "Game.h"

//==============================================================================
// Player constructor
Player::Player(unsigned int playerNumber)
{
    mPlayerNumber = playerNumber;
    
    mXYRadius = 0.1f;
    mZRadius = mXYRadius / 2.0f;
    
    mColour[0] = mPlayerNumber == 2 ? 1.0f : 0.0f;
    mColour[1] = 0.0f;
    mColour[2] = mPlayerNumber == 1 ? 1.0f : 0.0f;
    mColour[3] = 1.0f;
    
    mRenderFarFace = false;
    mRenderNearFace = false;
    
    // Offset to prevent pad clipping into wall
    float offset = 0.001f;
    
    mMaxPosition[0] = (MAP_WIDTH / 2.0f) - (mXYRadius + offset);
    mMaxPosition[1] = (MAP_HEIGHT / 2.0f) - (mXYRadius + offset);
    mMaxPosition[2] = 1000000.0f; // Won't be used
    
    mMinPosition[0] = -(MAP_WIDTH / 2.0f) + (mXYRadius + offset);
    mMinPosition[1] = -(MAP_HEIGHT / 2.0f) + (mXYRadius + offset);
    mMinPosition[2] = -1000000.0f; // Won't be used
    
    mPosition[0] = 0.0f;
    mPosition[1] = 0.0f;
    if (mPlayerNumber == 1)
    {
        mPosition[2] = (MAP_LENGTH / 2.0f) - (mZRadius);
    }
    else if (mPlayerNumber == 2)
    {
        mPosition[2] = -(MAP_LENGTH / 2.0f) + (mZRadius);
    }
    else
    {
        printf("***** ERROR: Invalid Player number *****");
    }
}

//==============================================================================
// Player destructor
Player::~Player()
{

}


//==============================================================================
// Init player object
void Player::Init()
{
    // X Y and Z coords needed
    float left      = -mXYRadius;
    float right     = mXYRadius;
    float top       = mXYRadius;
    float bottom    = -mXYRadius;
    float front     = -mZRadius;
    float back      = mZRadius;
    
    
    // ----- Wireframe of pad
    //
    mPadBatch.Begin(GL_LINES, 24);
    
    // Back face
    //
    // Top line
    mPadBatch.Vertex3f(left, top, back);
    mPadBatch.Vertex3f(right, top, back);
    // Bottom line
    mPadBatch.Vertex3f(left, bottom, back);
    mPadBatch.Vertex3f(right, bottom, back);
    // Left line
    mPadBatch.Vertex3f(left, top, back);
    mPadBatch.Vertex3f(left, bottom, back);
    // Right line
    mPadBatch.Vertex3f(right, top, back);
    mPadBatch.Vertex3f(right, bottom, back);
    
    // Front face
    //
    // Top line
    mPadBatch.Vertex3f(left, top, front);
    mPadBatch.Vertex3f(right, top, front);
    // Bottom line
    mPadBatch.Vertex3f(left, bottom, front);
    mPadBatch.Vertex3f(right, bottom, front);
    // Left line
    mPadBatch.Vertex3f(left, top, front);
    mPadBatch.Vertex3f(left, bottom, front);
    // Right line
    mPadBatch.Vertex3f(right, top, front);
    mPadBatch.Vertex3f(right, bottom, front);
    
    // Face connecting lines
    //
    // Top left
    mPadBatch.Vertex3f(left, top, front);
    mPadBatch.Vertex3f(left, top, back);
    // Top right
    mPadBatch.Vertex3f(right, top, front);
    mPadBatch.Vertex3f(right, top, back);
    // Bottom left
    mPadBatch.Vertex3f(left, bottom, front);
    mPadBatch.Vertex3f(left, bottom, back);
    // Bottom right
    mPadBatch.Vertex3f(right, bottom, front);
    mPadBatch.Vertex3f(right, bottom, back);
    
    mPadBatch.End();
    
    
    // ----- Face furthest from camera of pad
    //
    mFarFaceBatch.BeginMesh(6);
    
    // The 4 corners of the front pad face
    M3DVector3f theFarVerts[4] =
    {
        {left, bottom, front},
        {left, top, front},
        {right, top, front},
        {right, bottom, front}
    };
    M3DVector3f theNorms[3] =
    {
        {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}
    };
    M3DVector2f theTexs[3] =
    {
        {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}
    };
    
    mFarFaceBatch.AddTriangle(theFarVerts, theNorms, theTexs);
    
    memcpy(theFarVerts[1], theFarVerts[2], sizeof(M3DVector3f));
    memcpy(theFarVerts[2], theFarVerts[3], sizeof(M3DVector3f));
    
    mFarFaceBatch.AddTriangle(theFarVerts, theNorms, theTexs);
    
    mFarFaceBatch.End();
    
    
    // ----- Face closest to camera of pad
    //
    mNearFaceBatch.BeginMesh(6);
    
    // The 4 corners of the front pad face
    M3DVector3f theNearVerts[4] =
    {
        {left, bottom, back},
        {left, top, back},
        {right, top, back},
        {right, bottom, back}
    };
    
    mNearFaceBatch.AddTriangle(theNearVerts, theNorms, theTexs);
    
    memcpy(theNearVerts[1], theNearVerts[2], sizeof(M3DVector3f));
    memcpy(theNearVerts[2], theNearVerts[3], sizeof(M3DVector3f));
    
    mNearFaceBatch.AddTriangle(theNearVerts, theNorms, theTexs);
    
    mNearFaceBatch.End();
}


//==============================================================================
// Draw the player pad
void Player::Render(GLMatrixStack *theModelViewMatrix)
{
    Game *theGame = Game::GetGame();
    
    // Save the current matrix
    theModelViewMatrix->PushMatrix();
    
    // Translate to the pads position
    theModelViewMatrix->Translate(mPosition[0], mPosition[1], mPosition[2]);
    
    // Draw the pad
    theGame->UseShader(GLT_SHADER_POINT_LIGHT_DIFF, mColour);
    mPadBatch.Draw();
    
    // Draw the far face
    if (mRenderFarFace)
    {
        theGame->UseShader(GLT_SHADER_DEFAULT_LIGHT, mColour);
        mFarFaceBatch.Draw();
    }
    
    // Draw the near face
    if (mRenderNearFace)
    {
        theGame->UseShader(GLT_SHADER_DEFAULT_LIGHT, mColour);
        mNearFaceBatch.Draw();
    }
    
    // Restore the matrix
    theModelViewMatrix->PopMatrix();
}


//==============================================================================
// Perform any game logic for the player
void Player::Update(float deltaTimeMillis)
{
    Game *theGame = Game::GetGame();
    
    int *theMousePosScreen  = theGame->GetMousePos();
    int *theWindowSize      = theGame->GetWindowSize();
    
    // Get the mouse position as a proportion of the window size
    float theMousePosProp[2];
    theMousePosProp[0] = (float)theMousePosScreen[0] / (float)theWindowSize[0];
    theMousePosProp[1] = (float)theMousePosScreen[1] / (float)theWindowSize[1];
    
    // Adjust this proportion to be centred on the middle of the window, with
    // the borders being +/- 1.0f
    theMousePosProp[0] = (theMousePosProp[0] * 2.0f) - 1.0f;
    theMousePosProp[1] = (theMousePosProp[1] * 2.0f) - 1.0f;
    
    // Invert the y pos so that the top of the window is positive
    theMousePosProp[1] = -theMousePosProp[1];
    
    // Use this mouse position to set the player position
    mPosition[0] = theMousePosProp[0] * (MAP_WIDTH / 2.0);
    mPosition[1] = theMousePosProp[1] * (MAP_HEIGHT / 2.0);
    
    // Clamp position
    for (int dir = 0; dir < 2; dir++)
    {
        if (mPosition[dir] > mMaxPosition[dir])
        {
            mPosition[dir] = mMaxPosition[dir];
        }
        if (mPosition[dir] < mMinPosition[dir])
        {
            mPosition[dir] = mMinPosition[dir];
        }
    }
    
    
    // Check to see if we're colliding with the ball
    float *theBallPos = theGame->GetBall()->GetPosition();
    float theBallRadius = theGame->GetBall()->GetRadius();
    
    // Find the vector from the centre of the pad to the centre of the ball
    M3DVector3f padToBall;
    m3dSubtractVectors3(padToBall, theBallPos, mPosition);
    
    // Use this vector to find the vector from the centre of the pad to the
    // closest point on the outer frame of the pad to the ball
    //
    // Find the vector to the outer pad in each direction by using a proportion
    // of the vector to the ball and use the one with the smallest magnitude
    //
    // Initialise this magnitude to something absurd so the check for a smaller
    // vector succeeds
    M3DVector3f centrePadToOuterPad;
    float centrePadToOuterPadMagnitude = 5000.0f;
    for (int dir = 0; dir < 3; dir++)
    {
        float theDirRadius = dir == 2 ? mZRadius : mXYRadius;
        float proportionOfVectorToOuterPad = theDirRadius / padToBall[dir];
        M3DVector3f currentAttempt;
        m3dCopyVector3(currentAttempt, padToBall);
        m3dScaleVector3(currentAttempt, proportionOfVectorToOuterPad);
        float currentAttemptMagnitude = m3dGetMagnitude3(currentAttempt);
        if (currentAttemptMagnitude < centrePadToOuterPadMagnitude)
        {
            m3dCopyVector3(centrePadToOuterPad, currentAttempt);
            centrePadToOuterPadMagnitude = currentAttemptMagnitude;
        }
    }
    
    // Find the magnitude of the vector from the centre of the pad to the centre
    // of the ball
    float padToBallMagnitude = m3dGetMagnitude3(padToBall);
    
    // Find the distance from the outer pad to the centre of the ball
    float outerPadToCentreBallDistance = padToBallMagnitude - centrePadToOuterPadMagnitude;
    
    // If this distance is less than or equal to the radius of the ball we have
    // a collision
    if (outerPadToCentreBallDistance <= theBallRadius)
    {
        theGame->GetBall()->Hit(mPlayerNumber, centrePadToOuterPad);
        
        if (mPlayerNumber == 1)
        {
            mRenderFarFace = true;
        }
        else if (mPlayerNumber == 2)
        {
            mRenderNearFace = true;
        }
    }
    else
    {
        mRenderNearFace = false;
        mRenderFarFace = false;
    }
}


//==============================================================================
// Reset state to game start
void Player::Reset()
{
    mScore = 0;
}


//==============================================================================
// Handle a goal for this player
void Player::Score()
{
    mScore++;
    
    // Set the clear colour to our colour so the background will flash on a goal
    glClearColor(mColour[0], mColour[1], mColour[2], mColour[3]);
}

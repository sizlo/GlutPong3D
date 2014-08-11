//
//  Ball.cpp
//  GlutPong3D
//
//  Created by Tim Brier on 08/08/2014.
//  Copyright (c) 2014 tbrier. All rights reserved.
//

#include "Ball.h"
#include "Game.h"


//==============================================================================
// Ball constructor
Ball::Ball()
{
    mRadius = 0.05f;
    
    mColour[0] = 0.0f;
    mColour[1] = 1.0f;
    mColour[2] = 0.0f;
    mColour[3] = 1.0f;
    
    mMaxPosition[0] = (MAP_WIDTH / 2.0f) - mRadius;
    mMaxPosition[1] = (MAP_HEIGHT / 2.0f) - mRadius;
    mMaxPosition[2] = (MAP_LENGTH / 2.0f) - mRadius;
    
    mMinPosition[0] = (-MAP_WIDTH / 2.0f) + mRadius;
    mMinPosition[1] = (-MAP_HEIGHT / 2.0f) + mRadius;
    mMinPosition[2] = (-MAP_LENGTH / 2.0f) + mRadius;
    
    mMaxVelocity[0] = MAP_WIDTH / 1000;     // 1000 for 1000ms = 1s
    mMaxVelocity[1] = MAP_HEIGHT / 1000;    // 1000 for 1000ms = 1s
    mMaxVelocity[2] = MAP_LENGTH / 500;     // 500 for 500ms = 1/2s
}

//==============================================================================
// Ball destructor
Ball::~Ball()
{
    
}


//==============================================================================
// Initialise the ball
void Ball::Init()
{    
    gltMakeSphere(mSphereBatch, mRadius, 100, 50);
}


//==============================================================================
// Place the ball in the center not moving
void Ball::Center()
{
    mPosition[0] = 0.0f;
    mPosition[1] = 0.0f;
    mPosition[2] = 0.0f;
    
    mVelocity[0] = 0.0f;
    mVelocity[1] = 0.0f;
    mVelocity[2] = 0.0f;
}


//==============================================================================
// Give the ball a random velocity in XY directions and the starting z velocity
void Ball::GiveStartingVelocity()
{
    for (int dir = 0; dir < 2; dir++)
    {
        int theRandomPercentage = rand() % 201; // Range 0 - 200
        theRandomPercentage -= 100;             // Range -100 - 200
        float theRandomMultiplier = theRandomPercentage / 100.0f;
        
        // Cap at half max velocity
        mVelocity[dir] = (mMaxVelocity[dir] / 2.0f) * theRandomMultiplier;
    }
    
    // Starting z velocity
    mVelocity[2] = MAP_LENGTH / 1000; // 1000 for 1000ms = 1s
}


//==============================================================================
// Draw the ball
void Ball::Render(GLMatrixStack *theModelViewMatrix)
{
    Game *theGame = Game::GetGame();
    
    // Save the current matrix
    theModelViewMatrix->PushMatrix();

    // Translate to the balls position
    //
    // There is a bug in GLTools where if you use GLMatrixStack::Translatev it
    // accepts a M3DVector3f but uses it as if it were a M3DVector3f so the last
    // value put into the translation matrix is undefined. Because of this the
    // following line doesn't work, use the method that accepts x,y,z instead
    // theModelViewMatrix->Translatev(mPosition);
    theModelViewMatrix->Translate(mPosition[0], mPosition[1], mPosition[2]);
    
    // Draw the ball
    theGame->UseShader(GLT_SHADER_POINT_LIGHT_DIFF, mColour);
    mSphereBatch.Draw();
    
    // Restore the matrix
    theModelViewMatrix->PopMatrix();
}


//==============================================================================
// Perform any game logic for the ball
void Ball::Update(float deltaTimeMillis)
{
    Game *theGame = Game::GetGame();
    
    // Move the ball based on velocity and delta time
    for (int direction = 0; direction < 3; direction++)
    {
        mPosition[direction] += (mVelocity[direction] * deltaTimeMillis);
        
        // Bounce if needed
        //
        // Min and Max positions are checked in seperate statements to avoid the
        // case where the ball is so far beyond the threshold it is detected as
        // still out of bounds in the next frame and ends up wobbling back and
        // forth for a few frames
        float thePreviousVelocity = mVelocity[direction];
        float theAbsoluteVelocity = fabsf(mVelocity[direction]);
        
        if (mPosition[direction] >= mMaxPosition[direction])
        {
            mVelocity[direction] = -theAbsoluteVelocity;
            
            // If z direction and velocity actually changed then we hit a goal
            if (direction == 2 && mVelocity[direction] != thePreviousVelocity)
            {
                // Player 2 has scored
                theGame->Score(2);
            }
        }
        else if (mPosition[direction] <= mMinPosition[direction])
        {
            mVelocity[direction] = theAbsoluteVelocity;
            
            // If z direction and velocity actually changed then we hit a goal
            if (direction == 2 && mVelocity[direction] != thePreviousVelocity)
            {
                // Player 1 has scored
                theGame->Score(1);
            }
        }
    }
}


//==============================================================================
// React to a player hitting the ball
void Ball::Hit(unsigned int playerNumber, M3DVector3f positionOnPad)
{
    float theAbsZVelocity = fabsf(mVelocity[2]);
    
    if (playerNumber == 1)
    {
        mVelocity[2] = -theAbsZVelocity;
    }
    else if (playerNumber == 2)
    {
        mVelocity[2] = theAbsZVelocity;
    }
}

//
//  AIPlayer.cpp
//  GlutPong3D
//
//  Created by Tim Brier on 13/08/2014.
//  Copyright (c) 2014 tbrier. All rights reserved.
//

#include "AIPlayer.h"
#include "Game.h"


//==============================================================================
// AIPlayer constructor
AIPlayer::AIPlayer(unsigned int playerNumber) : Player(playerNumber)
{
    mSpeed[0] = MAP_WIDTH / 2000.0f; // 2000 for 2000ms = 2s
    mSpeed[1] = MAP_HEIGHT / 2000.0f; // 2000 for 2000ms = 2s
    mSpeed[2] = 0.0f; // Pad must stay in the same z plane
}

//==============================================================================
// AIPlayer destructor
AIPlayer::~AIPlayer()
{
    
}


//==============================================================================
// Update position based on balls position
void AIPlayer::UpdatePosition(float deltaTimeMillis)
{
    Game *theGame = Game::GetGame();
    float *theBallPos = theGame->GetBall()->GetPosition();
    
    // Handle x and y directions
    for (int dir = 0; dir < 2; dir++)
    {
        float distanceToBall = theBallPos[dir] - mPosition[dir];
        float maxMoveDistance = mSpeed[dir] * deltaTimeMillis;
     
        // If we can just move to the balls position in this update do it
        // otherwise move the max distance we can
        if (fabsf(distanceToBall) <= maxMoveDistance)
        {
            mPosition[dir] = theBallPos[dir];
        }
        else
        {
            if (distanceToBall < 0)
            {
                mPosition[dir] -= maxMoveDistance;
            }
            else if (distanceToBall > 0)
            {
                mPosition[dir] += maxMoveDistance;
            }
        }
        
        // Clamp position
        if (mPosition[dir] > mMaxPosition[dir])
        {
            mPosition[dir] = mMaxPosition[dir];
        }
        if (mPosition[dir] < mMinPosition[dir])
        {
            mPosition[dir] = mMinPosition[dir];
        }
    }
}
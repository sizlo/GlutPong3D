//
//  AIPlayer.h
//  GlutPong3D
//
//  Created by Tim Brier on 13/08/2014.
//  Copyright (c) 2014 tbrier. All rights reserved.
//

#ifndef __GlutPong3D__AIPlayer__
#define __GlutPong3D__AIPlayer__

#include "Player.h"

class AIPlayer: public Player
{
public:
    AIPlayer(unsigned int playerNumber);
    ~AIPlayer();
    
    virtual void UpdatePosition(float deltaTimeMillis);
    
private:
    // Speed and not velocity because direction will depend on where the ball is
    M3DVector3f mSpeed;
};

#endif /* defined(__GlutPong3D__AIPlayer__) */

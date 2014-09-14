//
//  Game.h
//  GlutPong3D
//
//  Created by Tim Brier on 08/08/2014.
//  Copyright (c) 2014 tbrier. All rights reserved.
//

#ifndef GlutPong3D_Game_h
#define GlutPong3D_Game_h

#include <GLShaderManager.h>
#include <GLGeometryTransform.h>
#include <GLMatrixStack.h>

#include <StopWatch.h>

#include "Ball.h"
#include "Room.h"
#include "Player.h"
#include "AIPlayer.h"


#define MAP_LENGTH 5.0f
#define MAP_WIDTH 1.0f
#define MAP_HEIGHT 1.0f

class Game
{
public:
    Game();
    ~Game();
    
    void ChangeSize(int width, int height);
    void SetupGL(int argc, char* argv[]);
    void Render();
    void Init(bool player1IsAI, bool player2IsAI);
    void ResetGame();
    void Run();
    void HandleKeypress(unsigned char key, int x, int y);
    void HandleMouseMove(int x, int y);
    void Score(unsigned int player);
    void UseShader(GLT_STOCK_SHADER theShader, M3DVector4f theColour);
    
    Ball * GetBall()
    {
        return mBall;
    }
    
    int * GetMousePos()
    {
        return mMousePos;
    }
    
    int * GetWindowSize()
    {
        return mWindowSize;
    }
    
    static Game * GetGame()
    {
        static Game *theGame = NULL;
        if (!theGame)
            theGame = new Game();
        return theGame;
    }

private:
    int                 mWindowSize[2];
    char                *mWindowTitle;
    float               mFOV;
    float               mNearPlane;
    float               mFarPlane;
    
    GLShaderManager     mShaderManager;
    GLGeometryTransform mTransformPipeline;
    GLMatrixStack       mModelViewMatrix;
    GLMatrixStack       mProjectionMatrix;
    GLFrame             mCameraFrame;
    
    M3DVector4f         mLightPos;
    M3DVector4f         mLightEyePos;
    
    int                 mMousePos[2];
    
    Ball                *mBall;
    Room                *mRoom;
    Player              *mPlayer1;
    Player              *mPlayer2;
    
    bool                mIsPaused;
    unsigned int        mPlayer1Score;
    unsigned int        mPlayer2Score;
    
    CStopWatch          mClearColourResetTimer;
};

#endif

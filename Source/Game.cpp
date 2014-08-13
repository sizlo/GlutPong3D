//
//  Game.cpp
//  GlutPong3D
//
//  Created by Tim Brier on 08/08/2014.
//  Copyright (c) 2014 tbrier. All rights reserved.
//

// Disable warnings for sprintf not being safe in MSVC
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <GLTools.h>
#include <GLFrustum.h>

#include <time.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#include "Game.h"


//==============================================================================
// Game constructor
Game::Game()
{
    mWindowSize[0]  = 600;
    mWindowSize[1]  = 600;
    
    mWindowTitle = (char *)malloc(8 * sizeof(char));
    sprintf(mWindowTitle, "%s", "Pong 3D");
    
    // This FOV allows us to have the ball just in frame when it is at the far
    // top/bottom/left/right of the nearest goal
    mFOV        = 28.0f;
    mNearPlane  = 1.0f;
    mFarPlane   = MAP_LENGTH + 3.0f;
    
    mLightPos[0] = 0.0f;
    mLightPos[1] = 0.1f;
    mLightPos[2] = 0.0f;
    mLightPos[3] = 1.0f;
    
    mIsPaused = true;
}

//==============================================================================
// Game destructor
Game::~Game()
{
    free(mWindowTitle);
    
    delete mBall;
    delete mRoom;
    delete mPlayer1;
    delete mPlayer2;
}


//==============================================================================
// Glut callbacks to call the Game's member functions
void GlutChangeSize(int width, int height)
{
    static Game *theGame = Game::GetGame();
    theGame->ChangeSize(width, height);
}
void GlutRender()
{
    static Game *theGame = Game::GetGame();
    theGame->Render();
}
void GlutKeyboard(unsigned char key, int x, int y)
{
    static Game *theGame = Game::GetGame();
    theGame->HandleKeypress(key, x, y);
}
void GlutMouse(int x, int y)
{
    static Game *theGame = Game::GetGame();
    theGame->HandleMouseMove(x, y);
}


//==============================================================================
// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void Game::ChangeSize(int width, int height)
{
	glViewport(0, 0, width, height);
    
    // Create the projection matrix, and load it on the projection matrix stack
    GLFrustum viewFrustum;
	viewFrustum.SetPerspective(mFOV,
                               float(width)/float(height),
                               mNearPlane,
                               mFarPlane);
	mProjectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // Set the transformation pipeline to use the two matrix stacks
    mTransformPipeline.SetMatrixStacks(mModelViewMatrix, mProjectionMatrix);
}


//==============================================================================
// This function does any needed initialization on the rendering context.
// This is the first opportunity to do any OpenGL related tasks.
void Game::SetupGL(int argc, char* argv[])
{
    // Setup glut
    gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(mWindowSize[0], mWindowSize[1]);
	glutCreateWindow(mWindowTitle);
    glutReshapeFunc(GlutChangeSize);
    glutDisplayFunc(GlutRender);
    glutKeyboardFunc(GlutKeyboard);
    glutPassiveMotionFunc(GlutMouse);
    
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		exit(1);
    }
    
    // White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glEnable(GL_DEPTH_TEST);
    
	mShaderManager.InitializeStockShaders();
}



//==============================================================================
// Called to draw scene
void Game::Render(void)
{
    static CStopWatch theStopWatch;
    static float prevElapsedTimeMillis = 0.0f;
    
    // Get time since last frame
    float elapsedTimeSeconds = theStopWatch.GetElapsedSeconds();
    float elapsedTimeMillis = elapsedTimeSeconds * 1000.0f;
    float deltaTimeMillis = elapsedTimeMillis - prevElapsedTimeMillis;
    
    // Update the previous time
    prevElapsedTimeMillis = elapsedTimeMillis;
    
    // Count frames
    static int fps = 0;
    static int numFrames = 0;
    numFrames++;
    
    // Check fps every second
    static float nextFPSCheckTime = 1.0f;
    if (elapsedTimeSeconds >= nextFPSCheckTime)
    {
        fps = numFrames;
        // Reset frame count and update the next check time
        numFrames = 0;
        nextFPSCheckTime += 1.0f;
    }
    
    // Update the title with the current FPS and score
    static char theTitle[64];
    sprintf(theTitle, "%s | %d fps | P1: %d | P2: %d%s",
            mWindowTitle,
            fps,
            mPlayer1->GetScore(),
            mPlayer2->GetScore(),
            mIsPaused ? " | Paused" : "");
    glutSetWindowTitle(theTitle);
    
    // Update any relavant game objects
    if (!mIsPaused)
    {
        mBall->Update(deltaTimeMillis);
		mPlayer1->Update(deltaTimeMillis);
		mPlayer2->Update(deltaTimeMillis);
    }
    
    
	// Clear the window with current clearing color
    if (mClearColourResetTimer.GetElapsedSeconds() >= 0.5)
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Push the identity matrix
    mModelViewMatrix.PushMatrix();
    
    // Push the camera matrix
    M3DMatrix44f theCameraMatrix;
    mCameraFrame.GetCameraMatrix(theCameraMatrix);
    mModelViewMatrix.PushMatrix(theCameraMatrix);
    
    // Update the light z pos to just in front of the ball
    mLightPos[2] = mBall->GetPosition()[2] + 1.0f;
    mLightPos[3] = 1.0f;
    
    // Transform the light position into eye coordinates
    m3dTransformVector4(mLightEyePos, mLightPos, theCameraMatrix);
    
    // Draw all game objects
    mBall->Render(&mModelViewMatrix);
    mRoom->Render(&mModelViewMatrix);
    mPlayer1->Render(&mModelViewMatrix);
    mPlayer2->Render(&mModelViewMatrix);
    
    // Pop camera matrix
    mModelViewMatrix.PopMatrix();
    // Pop identity matrix
    mModelViewMatrix.PopMatrix();
    
	// Perform the buffer swap to display back buffer
	glutSwapBuffers();
    
    // Request the next frame
    glutPostRedisplay();
}


//==============================================================================
// Sets up the objects needed for the game
void Game::Init(bool player1IsAI, bool player2IsAI)
{
    // Initialise the random seed
    srand(time(NULL));
    
    mCameraFrame.SetOrigin(0.0f, 0.0f, (MAP_LENGTH / 2.0f)+2.0f);
    
    mBall = new Ball();
    mRoom = new Room();
    
    if (player1IsAI)
    {
        mPlayer1 = new AIPlayer(1);
    }
    else
    {
        mPlayer1 = new Player(1);
    }
    if (player2IsAI)
    {
        mPlayer2 = new AIPlayer(2);
    }
    else
    {
        mPlayer2 = new Player(2);
    }
    
    mBall->Init();
    mRoom->Init();
    mPlayer1->Init();
    mPlayer2->Init();
    
    ResetGame();
}


//==============================================================================
// Reset the game state
void Game::ResetGame(void)
{
    mIsPaused = true;
    
    mBall->Center();
    mBall->GiveStartingVelocity();
    
    mPlayer1->Reset();
    mPlayer2->Reset();
    
    mClearColourResetTimer.Reset();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}


//==============================================================================
// Starts the game main loop
void Game::Run(void)
{
    glutMainLoop();
}


//==============================================================================
// Handle keyboard input
void Game::HandleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'p':   mIsPaused = !mIsPaused; break;
        case 'r':   ResetGame();            break;
        default:    break; // Do nothing
    }
}


//==============================================================================
// Handle mouse move input
void Game::HandleMouseMove(int x, int y)
{
    mMousePos[0] = x;
    mMousePos[1] = y;
}


//==============================================================================
// Handle goals
void Game::Score(unsigned int player)
{
    switch (player)
    {
        case 1:     mPlayer1->Score();    break;
        case 2:     mPlayer2->Score();    break;
        default:    break; // Do nothing
    }
    
    // We'll need to reset the clear colour after it has been set to the player
    // colour for the goal
    mClearColourResetTimer.Reset();
}


//==============================================================================
// Utility function to setup the lighting shader for objects
void Game::UseShader(GLT_STOCK_SHADER theShader, M3DVector4f theColour)
{
    switch (theShader)
    {
        case GLT_SHADER_POINT_LIGHT_DIFF:
            mShaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,
                                          mTransformPipeline.GetModelViewMatrix(),
                                          mTransformPipeline.GetProjectionMatrix(),
                                          mLightEyePos,
                                          theColour);
            break;
        
        case GLT_SHADER_DEFAULT_LIGHT:
            mShaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,
                                          mTransformPipeline.GetModelViewMatrix(),
                                          mTransformPipeline.GetProjectionMatrix(),
                                          theColour);
        
        default: break; // Do nothing
    }
}


//==============================================================================
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{
    bool player1IsAI = false;
    bool player2IsAI = true;
    
    if (argc > 1)
    {
        player1IsAI = strcmp(argv[1], "true") == 0;
    }
    if (argc > 2)
    {
        player2IsAI = strcmp(argv[2], "true") == 0;
    }
    
    Game *theGame = Game::GetGame();
    
	theGame->SetupGL(argc, argv);
    
    theGame->Init(player1IsAI, player2IsAI);
    
    theGame->Run();
    
    delete theGame;
    
	return 0;
}

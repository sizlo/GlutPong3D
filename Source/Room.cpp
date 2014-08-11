//
//  Room.cpp
//  GlutPong3D
//
//  Created by Tim Brier on 08/08/2014.
//  Copyright (c) 2014 tbrier. All rights reserved.
//

#include "Room.h"
#include "Game.h"
#include "Ball.h"

//==============================================================================
// Room constructor
Room::Room()
{
    mColour[0] = 1.0f;
    mColour[1] = 1.0f;
    mColour[2] = 0.0f;
    mColour[3] = 1.0f;
}

//==============================================================================
// Room destructor
Room::~Room()
{
    
}


//==============================================================================
// Init room object
void Room::Init()
{
    M3DVector3f theVerts[4];
    // Add an offset to the width of all the walls to prevent them not filling
    // the entire area
    float offset = 0.1f;
    
    theVerts[0][0] = (-MAP_WIDTH / 2.0f) - offset;
    theVerts[0][1] = 0.0f;
    theVerts[0][2] = MAP_LENGTH / 2.0f;
    
    theVerts[1][0] = (-MAP_WIDTH / 2.0f) - offset;
    theVerts[1][1] = 0.0f;
    theVerts[1][2] = -MAP_LENGTH / 2.0f;
    
    theVerts[2][0] = (MAP_WIDTH / 2.0f) + offset;
    theVerts[2][1] = 0.0f;
    theVerts[2][2] = -MAP_LENGTH / 2.0f;
    
    theVerts[3][0] = (MAP_WIDTH / 2.0f) + offset;
    theVerts[3][1] = 0.0f;
    theVerts[3][2] = MAP_LENGTH / 2.0f;
    
    M3DVector3f theNorms[6];
    
    theNorms[0][0] = 0.0f;
    theNorms[0][1] = 1.0f;
    theNorms[0][2] = 0.0f;
    
    theNorms[1][0] = 0.0f;
    theNorms[1][1] = 1.0f;
    theNorms[1][2] = 0.0f;
    
    theNorms[2][0] = 0.0f;
    theNorms[2][1] = 1.0f;
    theNorms[2][2] = 0.0f;
    
    M3DVector2f theTexCoords[3];
    
    theTexCoords[0][0] = 0.0f;
    theTexCoords[0][1] = 0.0f;
    
    theTexCoords[1][0] = 0.0f;
    theTexCoords[1][1] = 0.0f;
    
    theTexCoords[2][0] = 0.0f;
    theTexCoords[2][1] = 0.0f;
    
    mWallBatch.BeginMesh(6);
    // Add the left triangle
    mWallBatch.AddTriangle(theVerts, theNorms, theTexCoords);
    // Move verts around for right triangle
    memcpy(theVerts[1], theVerts[2], sizeof(M3DVector3f));
    memcpy(theVerts[2], theVerts[3], sizeof(M3DVector3f));
    // Add the right triangle
    mWallBatch.AddTriangle(theVerts, theNorms, theTexCoords);
    mWallBatch.End();
    
}

//==============================================================================
// Render room
void Room::Render(GLMatrixStack *theModelViewMatrix)
{
    Game *theGame = Game::GetGame();

    // Save the current matrix
    theModelViewMatrix->PushMatrix();
    
    // Translate to and draw the floor
    theModelViewMatrix->PushMatrix();
    theModelViewMatrix->Translate(0.0f, -MAP_HEIGHT / 2.0f, 0.0f);
    theGame->UseShader(GLT_SHADER_POINT_LIGHT_DIFF, mColour);
    mWallBatch.Draw();
    
    // Erase the translation
    theModelViewMatrix->PopMatrix();
    
    // Translate and rotate to the left wall, then draw
    theModelViewMatrix->PushMatrix();
    theModelViewMatrix->Translate(-MAP_WIDTH / 2.0f, 0.0f, 0.0f);
    theModelViewMatrix->Rotate(270.0f, 0.0f, 0.0f, 1.0f);
    theGame->UseShader(GLT_SHADER_POINT_LIGHT_DIFF, mColour);
    mWallBatch.Draw();
    
    // Erase the translation / rotation
    theModelViewMatrix->PopMatrix();
    
    // Translate and rotate to the ceiling, then draw
    theModelViewMatrix->PushMatrix();
    theModelViewMatrix->Translate(0.0f, MAP_HEIGHT / 2.0f, 0.0f);
    theModelViewMatrix->Rotate(180.0f, 0.0f, 0.0f, 1.0f);
    theGame->UseShader(GLT_SHADER_POINT_LIGHT_DIFF, mColour);
    mWallBatch.Draw();
    
    // Erase the translation / rotation
    theModelViewMatrix->PopMatrix();
    
    // Translate and rotate to the left wall, then draw
    theModelViewMatrix->PushMatrix();
    theModelViewMatrix->Translate(MAP_WIDTH / 2.0f, 0.0f, 0.0f);
    theModelViewMatrix->Rotate(90.0f, 0.0f, 0.0f, 1.0f);
    theGame->UseShader(GLT_SHADER_POINT_LIGHT_DIFF, mColour);
    mWallBatch.Draw();
    
    // Erase the translation / rotation
    theModelViewMatrix->PopMatrix();
    
    // Restore the matrix
    theModelViewMatrix->PopMatrix();
}

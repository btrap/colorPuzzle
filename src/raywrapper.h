#pragma once

#include "raylib.h"
#include "raymath.h"

typedef Vector2 vec2;

#define FUNC_SIG inline

struct Player
{
    vec2 position;
    Rectangle rectangle;
    float speed;
    bool canJump;
};

struct Walls
{
    Rectangle rect;
    int blocking;
    Color color;
};

struct Gui_Window
{
    char* title;
    vec2 position;
    vec2 open_Size;
    Rectangle rectangle;
    bool open;
    bool alive;
};

struct Debug_Window
{
    bool alive = false;

    Rectangle window;

    char allTextBox[12][64];
    char *previousTextBox;
    char *activeTextBox;
    int command_Count;

    Rectangle text_Box;
    bool textBoxEditMode = false;
};

struct Debug_Lines
{
    int line_Count;
    vec2 start_Lines[256];
    vec2 end_Lines[256];
    Color line_Colors[256];
};
Debug_Lines debug_Lines = {0};

FUNC_SIG int DEBUG_GetLine()
{
    if(debug_Lines.line_Count >= 256)
        return -1;

    ++debug_Lines.line_Count;
    return debug_Lines.line_Count - 1;
}

FUNC_SIG void DEBUG_AddLine(int index, vec2 * start, vec2 * end, Color color)
{
    if(index > 256)
        return;

    debug_Lines.start_Lines[index].x = start->x;
    debug_Lines.start_Lines[index].y = start->y;
    debug_Lines.end_Lines[index].x = end->x;
    debug_Lines.end_Lines[index].y = end->y;
    debug_Lines.line_Colors[index] = color;
}

FUNC_SIG void DEBUG_DrawLines()
{
    if(debug_Lines.line_Count > 256)
        debug_Lines.line_Count = 256;

    for(int i = 0; i < debug_Lines.line_Count; ++i)
    {
        DrawLineEx(debug_Lines.start_Lines[i], debug_Lines.end_Lines[i], 5, debug_Lines.line_Colors[i]);
    }
}

FUNC_SIG void UpdatePlayer(Player *player, Walls *walls, int walls_Length, float delta)
{
    static float GRAVITY = 400.0f;
    static float PLAYER_JUMP_SPD = 350.0f;
    static float PLAYER_HOR_SPD = 200.0f;
    
    if(IsKeyDown(KEY_A))
    {
        player->position.x -= PLAYER_HOR_SPD * delta;
    }

    if(IsKeyDown(KEY_D))
    {
         player->position.x += PLAYER_HOR_SPD * delta;
    }

    if(IsKeyDown(KEY_SPACE) && player->canJump) 
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }

    int hitObstacle = 0;
    for(int i = 0; i < walls_Length; i++) 
    {
        Walls *ei = walls + i;
        vec2 *p = &(player->position);
        if(ei->blocking
            && ei->rect.x <= p->x + player->speed*delta
            && ei->rect.x + ei->rect.width >= p->x 
            && ei->rect.y >= p->y 
            && ei->rect.y < p->y + player->speed*delta
            ) 
        {
            hitObstacle = 1;
            player->speed = 0.0f;
            p->y = ei->rect.y;
        }
    }

    if(!hitObstacle) 
    {
        player->position.y += player->speed*delta;
        player->speed += GRAVITY * delta;
        player->canJump = false;
    } 
    else
    {
        player->canJump = true;
    }
}

FUNC_SIG void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, Walls *walls, int walls_Length, float delta, int width, int height) 
{
    static vec2 bbox = { 0.2f, 0.2f };

    vec2 bboxWorldMin = GetScreenToWorld2D({ (1.0f - bbox.x) * 0.5f * width, (1.0f - bbox.y) * 0.5f * height }, *camera);
    vec2 bboxWorldMax = GetScreenToWorld2D({ (1.0f + bbox.x) * 0.5f * width, (1.0f + bbox.y) * 0.5f * height }, *camera);
    camera->offset = { (1.0f - bbox.x) * 0.5f * width, (1 - bbox.y)*0.5f*height };

    vec2 edge = camera->target;

    if(player->position.x < bboxWorldMin.x)
    {
        edge.x = player->position.x;
    }
    else if(player->position.x > bboxWorldMax.x)
    {
        edge.x = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
    }

    if(player->position.y < bboxWorldMin.y)
    {
        edge.y = player->position.y;
    }
    else if(player->position.y > bboxWorldMax.y)
    {
        edge.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
    }

    static float minSpeed = 60.0f;
    static float minEffectLength = 1.0f;
    static float fractionSpeed = 0.8f;

    vec2 diff = Vector2Subtract(edge, camera->target);
    float length = Vector2Length(diff);

    vec2 test_Point = Vector2Add(camera->target, diff);
    static int line_Index = DEBUG_GetLine();
    DEBUG_AddLine(line_Index, &camera->target, &test_Point, BLUE);
    
    if(length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed*delta/length));
    }
 }

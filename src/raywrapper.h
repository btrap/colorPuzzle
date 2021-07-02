#pragma once

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION

#define FUNC_SIG inline

enum Debug_Types
{
    WALLS_TYPE,
    PLAYER_TYPE,
    WINDOW_TYPE,

    VEC2_TYPE,
    VEC3_TYPE,
    RECT_TYPE,
    COLOR_TYPE,

    CHAR_TYPE,
    INT32_TYPE,
    UINT32_TYPE,
    FLOAT32_TYPE,
    BOOL_TYPE,
    STR_TYPE,
};

struct Player
{
    Vector2 position;
    Rectangle rectangle;
    float speed;
    bool canJump;
    Color color;
};

struct Walls
{
    Rectangle rectangle;
    bool blocking;
    Color color;
};

struct Debug_Item
{
    const char *name;
    Rectangle *rect;
    Debug_Types type;
    char *entity;

    int gui_Entity_Count;
    char** gui_Entities;
};

struct Debug_Items
{
    int item_Count;
    Debug_Item items[256];
    int item_Hover;
    int item_Selected;
};

struct Gui_Vector2
{
    Gui_Control group;
    Gui_Control labels[3];
};

struct Gui_Window
{
    char* title;
    Vector2 position;
    Vector2 open_Size;
    Rectangle rectangle;
    Vector2 viewScroll;

    bool hover;
    bool clicked;

    bool open;
    bool alive;
    bool drag;
};

FUNC_SIG void InitGuiWIndow(Gui_Window gui_Window)
{
    gui_Window = {0};

    gui_Window.alive = true;
    gui_Window.open = true;
    gui_Window.drag = false;

    gui_Window.hover = false;
    gui_Window.clicked = false;

    gui_Window.title = (char*)"#198# New Window";
    gui_Window.position = { 0.0f, 0.0f };
    gui_Window.open_Size = { 100.0f, 100.0f};
    gui_Window.rectangle = { 0.0f, 0.0f, 100.0f, 100.0f};

    gui_Window.viewScroll = {100.0f, 100.0f};
}

struct Debug_Window
{
    bool alive = false;

    Rectangle window;

    char allTextBox[12][64];
    char *previousTextBox;
    char *activeTextBox;
    int command_Count;

    Rectangle text_Box;
};

struct Debug_Lines
{
    int line_Count;
    Vector2 start_Lines[256];
    Vector2 end_Lines[256];
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

FUNC_SIG void DEBUG_AddLine(int index, Vector2 * start, Vector2 * end, Color color)
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
    {
        debug_Lines.line_Count = 256;
    }

    for(int i = 0; i < debug_Lines.line_Count; ++i)
    {
        DrawLineEx(debug_Lines.start_Lines[i], debug_Lines.end_Lines[i], 5, debug_Lines.line_Colors[i]);
    }
}

FUNC_SIG void MoveEntityData(Debug_Item *debug_Item, Vector2 position)
{
    switch(debug_Item->type)
    {
        case PLAYER_TYPE:
        {
            Player *temp_Player = (Player*)debug_Item->entity;
            temp_Player->position.x = position.x;
            temp_Player->position.y = position.y;
        }break;
        case WALLS_TYPE:
        {
            Walls *temp_Wall = (Walls*)debug_Item->entity;
            temp_Wall->rectangle.x = position.x;
            temp_Wall->rectangle.y = position.y;
        }break;
    }
}

FUNC_SIG void SetupDebugGuiData(Debug_Item *debug_Item)
{ 
    switch(debug_Item->type)
    {
        case PLAYER_TYPE:
        {
            Player *temp_Player = (Player*)debug_Item->entity;

            Gui_EditValueFloatControl *posX = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(posX);
            posX->value = &temp_Player->position.x;
            posX->text = "X";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *posY = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(posY);
            posY->value = &temp_Player->position.y;
            posY->text = "Y";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectX = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectX);
            rectX->value = &temp_Player->rectangle.x;
            rectX->text = "X";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectY = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectY);
            rectY->value = &temp_Player->rectangle.y;
            rectY->text = "Y";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectW = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectW);
            rectW->value = &temp_Player->rectangle.width;
            rectW->text = "W";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectH = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectH);
            rectH->value = &temp_Player->rectangle.height;
            rectH->text = "H";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *floatSpeed = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(floatSpeed);
            floatSpeed->value = &temp_Player->speed;
            floatSpeed->text = "Speed";
            ++debug_Item->gui_Entity_Count;

            debug_Item->gui_Entities = (char**)malloc(sizeof(Gui_EditValueFloatControl*) * debug_Item->gui_Entity_Count);

            debug_Item->gui_Entities[0] = (char*)posX;
            debug_Item->gui_Entities[1] = (char*)posY;
            debug_Item->gui_Entities[2] = (char*)rectX;
            debug_Item->gui_Entities[3] = (char*)rectY;
            debug_Item->gui_Entities[4] = (char*)rectW;
            debug_Item->gui_Entities[5] = (char*)rectH;
            debug_Item->gui_Entities[6] = (char*)floatSpeed;
        }break;
        case WALLS_TYPE:
        {
            Walls *temp_Wall = (Walls*)debug_Item->entity;

            Gui_EditValueFloatControl *rectX = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectX);
            rectX->value = &temp_Wall->rectangle.x;
            rectX->text = "X";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectY = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectY);
            rectY->value = &temp_Wall->rectangle.y;
            rectY->text = "Y";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectW = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectW);
            rectW->value = &temp_Wall->rectangle.width;
            rectW->text = "W";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectH = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectH);
            rectH->value = &temp_Wall->rectangle.height;
            rectH->text = "H";
            ++debug_Item->gui_Entity_Count;

            debug_Item->gui_Entities = (char**)malloc(sizeof(Gui_EditValueFloatControl*) * debug_Item->gui_Entity_Count);

            debug_Item->gui_Entities[0] = (char*)rectX;
            debug_Item->gui_Entities[1] = (char*)rectY;
            debug_Item->gui_Entities[2] = (char*)rectW;
            debug_Item->gui_Entities[3] = (char*)rectH;
        }break;
        case WINDOW_TYPE:
        {
            Gui_Window *temp_Window = (Gui_Window*)debug_Item->entity;

            Gui_EditValueFloatControl *posX = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(posX);
            posX->value = &temp_Window->position.x;
            posX->text = "X";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *posY = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(posY);
            posY->value = &temp_Window->position.y;
            posY->text = "Y";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectX = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectX);
            rectX->value = &temp_Window->rectangle.x;
            rectX->text = "X";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectY = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectY);
            rectY->value = &temp_Window->rectangle.y;
            rectY->text = "Y";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectW = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectW);
            rectW->value = &temp_Window->rectangle.width;
            rectW->text = "W";
            ++debug_Item->gui_Entity_Count;

            Gui_EditValueFloatControl *rectH = (Gui_EditValueFloatControl*)(malloc(sizeof(Gui_EditValueFloatControl)));
            SetupGuiValueBox(rectH);
            rectH->value = &temp_Window->rectangle.height;
            rectH->text = "H";
            ++debug_Item->gui_Entity_Count;

            debug_Item->gui_Entities = (char**)malloc(sizeof(Gui_EditValueFloatControl*) * debug_Item->gui_Entity_Count);

            debug_Item->gui_Entities[0] = (char*)posX;
            debug_Item->gui_Entities[1] = (char*)posY;
            debug_Item->gui_Entities[2] = (char*)rectX;
            debug_Item->gui_Entities[3] = (char*)rectY;
            debug_Item->gui_Entities[4] = (char*)rectW;
            debug_Item->gui_Entities[5] = (char*)rectH;
        }break;
    }
}

FUNC_SIG void MouseTests()
{

}

FUNC_SIG void DrawType(Rectangle window_Slice, Debug_Types type, char** gui_Entities, int gui_Entity_Count)
{
    float x_Edge = 5.0f;
    float y_Edge = 5.0f;

    float text_Height = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textBox_Padding = GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)  + GuiGetStyle(TEXTBOX, TEXT_LINES_PADDING);
    float total = text_Height + textBox_Padding + 10.0f;

    float vec_Width = (window_Slice.width - x_Edge * 3.0f - textBox_Padding * 2.0f) / 2.0f;
    float vec_Height = (window_Slice.height - y_Edge * 3.0f) / 2.0f;

    switch(type)
    {
        case PLAYER_TYPE:
        {
            Rectangle position_Rect = {window_Slice.x + x_Edge, 
                                       window_Slice.y + y_Edge, 
                                       window_Slice.width - 2.0f * x_Edge, 
                                       total};
            GuiGroupBox(position_Rect, "Position");
            DrawType(position_Rect, VEC2_TYPE, gui_Entities, 2);

            Rectangle rect_Rect = {window_Slice.x + x_Edge, 
                                   window_Slice.y + position_Rect.height + y_Edge * 2.0f, 
                                   window_Slice.width - 2.0f * x_Edge, 
                                   total * 2.0f};
            GuiGroupBox(rect_Rect, "Rect");
            DrawType(rect_Rect, RECT_TYPE, &gui_Entities[2], 4);

            Rectangle speed_Rect = {window_Slice.x + x_Edge, 
                                    window_Slice.y + position_Rect.height + rect_Rect.height + y_Edge * 3.0f,
                                    window_Slice.width - 2.0f * x_Edge, 
                                    text_Height + 10.0f};
            DrawType(speed_Rect, FLOAT32_TYPE, &gui_Entities[6], 1);
            //DrawRectangleLinesEx(speed_Rect, 1, RED);
        }break;
        case WALLS_TYPE:
        {
            Rectangle rect_Rect = {window_Slice.x + x_Edge, 
                                   window_Slice.y + y_Edge, 
                                   window_Slice.width - 2.0f * x_Edge, 
                                   total * 2.0f};
            GuiGroupBox(rect_Rect, "Rect");
            DrawType(rect_Rect, RECT_TYPE, gui_Entities, 4);
        }break;
        case WINDOW_TYPE:
        {
            Rectangle position_Rect = {window_Slice.x + x_Edge, 
                                       window_Slice.y + y_Edge, 
                                       window_Slice.width - 2.0f * x_Edge, 
                                       total};
            GuiGroupBox(position_Rect, "Position");
            DrawType(position_Rect, VEC2_TYPE, gui_Entities, 2);

            Rectangle rect_Rect = {window_Slice.x + x_Edge, 
                                   window_Slice.y + position_Rect.height + y_Edge * 2.0f, 
                                   window_Slice.width - 2.0f * x_Edge, 
                                   total * 2.0f};
            GuiGroupBox(rect_Rect, "Rect");
            DrawType(rect_Rect, RECT_TYPE, &gui_Entities[2], 4);
        }break;
        case FLOAT32_TYPE:
        {
            Gui_EditValueFloatControl *temp_FloatControl = (Gui_EditValueFloatControl*)(gui_Entities[0]);

            temp_FloatControl->bounds =
            {
                window_Slice.x + text_Height * 5.0f + textBox_Padding, 
                window_Slice.y, 
                window_Slice.width - textBox_Padding - text_Height * 5.0f , 
                window_Slice.height 
            };

            if(GuiValueBox(temp_FloatControl))
            {
                temp_FloatControl->edit_Mode = !temp_FloatControl->edit_Mode;
            }
            DrawRectangleLinesEx(window_Slice, 1, RED);
            DrawRectangleLinesEx(temp_FloatControl->bounds, 1, RED);
        }break;
        case RECT_TYPE:
        {
            Gui_EditValueFloatControl *temp_RectX = (Gui_EditValueFloatControl*)(gui_Entities[0]);
            Gui_EditValueFloatControl *temp_RectY = (Gui_EditValueFloatControl*)(gui_Entities[1]);
            Gui_EditValueFloatControl *temp_RectW = (Gui_EditValueFloatControl*)(gui_Entities[2]);
            Gui_EditValueFloatControl *temp_RectH = (Gui_EditValueFloatControl*)(gui_Entities[3]);

            temp_RectX->bounds = {window_Slice.x + x_Edge + textBox_Padding, 
                                  window_Slice.y + y_Edge, 
                                  vec_Width, 
                                  vec_Height};
            if(GuiValueBox(temp_RectX))
            {
                temp_RectX->edit_Mode = !temp_RectX->edit_Mode;
            }

            temp_RectY->bounds = temp_RectX->bounds;
            temp_RectY->bounds.x += x_Edge + textBox_Padding + vec_Width;
            if(GuiValueBox(temp_RectY))
            {
                temp_RectY->edit_Mode = !temp_RectY->edit_Mode;
            }

            temp_RectW->bounds = temp_RectX->bounds;
            temp_RectW->bounds.y += y_Edge + vec_Height;
            if(GuiValueBox(temp_RectW))
            {
                temp_RectW->edit_Mode = !temp_RectW->edit_Mode;
            }

            temp_RectH->bounds = temp_RectX->bounds;
            temp_RectH->bounds.x += x_Edge + textBox_Padding + vec_Width;
            temp_RectH->bounds.y += y_Edge + vec_Height;
            if(GuiValueBox(temp_RectH))
            {
                temp_RectH->edit_Mode = !temp_RectH->edit_Mode;
            }
        }break;
        case VEC2_TYPE:
        {
            Gui_EditValueFloatControl *temp_VecX = (Gui_EditValueFloatControl*)(gui_Entities[0]);
            Gui_EditValueFloatControl *temp_VecY = (Gui_EditValueFloatControl*)(gui_Entities[1]);

            temp_VecX->bounds = {window_Slice.x + x_Edge + textBox_Padding, 
                               window_Slice.y + y_Edge, 
                               vec_Width, 
                               window_Slice.height - y_Edge * 2.0f};
            if(GuiValueBox(temp_VecX))
            {
                temp_VecX->edit_Mode = !temp_VecX->edit_Mode;
            }

            temp_VecY->bounds = temp_VecX->bounds;
            temp_VecY->bounds.x += x_Edge + textBox_Padding + vec_Width;
            if(GuiValueBox(temp_VecY))
            {
                temp_VecY->edit_Mode = !temp_VecY->edit_Mode;
            }
        }break;
    }
}

FUNC_SIG void DebugEditor(Gui_Window *editor_Window, Debug_Item * debug_Item)
{
    float text_Height = GuiGetStyle(DEFAULT, TEXT_SIZE) / 2.0f;
    int statusBarHeight = WINDOW_STATUSBAR_HEIGHT + 2;
    int border_Width = 2 * GuiGetStyle(STATUSBAR, BORDER_WIDTH);
    int slider_Size = GuiGetStyle(SLIDER, SLIDER_WIDTH);

    float x_Edge = 5.0f;
    float y_Edge = 15.0f;

    Rectangle view = GuiScrollPanel( 
        { editor_Window->rectangle.x + x_Edge, 
        editor_Window->rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2, 
        editor_Window->rectangle.width - 10.0f, 
        editor_Window->rectangle.height - 7.0f - WINDOW_STATUSBAR_HEIGHT}, 
        
        { editor_Window->rectangle.x + 5.0f, 
        editor_Window->rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2, 
        (float)GetScreenWidth(), 
        (float)GetScreenHeight() }, &editor_Window->viewScroll);

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);

    Rectangle window_Slice = 
    {
        editor_Window->rectangle.x + x_Edge,
        editor_Window->rectangle.y + statusBarHeight + y_Edge,
        editor_Window->rectangle.width - slider_Size - 5.0f,
        editor_Window->rectangle.height - slider_Size - statusBarHeight - y_Edge
    };

    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    GuiLabel({window_Slice.x, window_Slice.y, window_Slice.width, 0.0f}, debug_Item->name);

    window_Slice.y += text_Height;
    window_Slice.height -= text_Height;
    DrawType(window_Slice, debug_Item->type, debug_Item->gui_Entities, debug_Item->gui_Entity_Count);

    EndScissorMode();
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
        Vector2 *p = &(player->position);
        if(ei->blocking
            && ei->rectangle.x <= p->x + player->speed*delta
            && ei->rectangle.x + ei->rectangle.width >= p->x 
            && ei->rectangle.y >= p->y 
            && ei->rectangle.y < p->y + player->speed*delta
            ) 
        {
            hitObstacle = 1;
            player->speed = 0.0f;
            p->y = ei->rectangle.y;
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
    static Vector2 bbox = { 0.2f, 0.2f };

    Vector2 bboxWorldMin = GetScreenToWorld2D({ (1.0f - bbox.x) * 0.5f * width, (1.0f - bbox.y) * 0.5f * height }, *camera);
    Vector2 bboxWorldMax = GetScreenToWorld2D({ (1.0f + bbox.x) * 0.5f * width, (1.0f + bbox.y) * 0.5f * height }, *camera);
    camera->offset = { (1.0f - bbox.x) * 0.5f * width, (1 - bbox.y)*0.5f*height };

    Vector2 edge = camera->target;

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
    }    else if(player->position.y > bboxWorldMax.y)
    {
        edge.y = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
    }

    static float minSpeed = 60.0f;
    static float minEffectLength = 1.0f;
    static float fractionSpeed = 0.8f;

    Vector2 diff = Vector2Subtract(edge, camera->target);
    float length = Vector2Length(diff);

    Vector2 test_Point = Vector2Add(camera->target, diff);
    static int line_Index = DEBUG_GetLine();
    DEBUG_AddLine(line_Index, &camera->target, &test_Point, BLUE);
    
    if(length > minEffectLength)
    {
        float speed = fmaxf(fractionSpeed*length, minSpeed);
        camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed*delta/length));
    }
 }

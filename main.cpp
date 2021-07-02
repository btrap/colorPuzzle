#include "raywrapper.h"

#include "temp_gui.h"

enum Editor_Mode_Settings
{
    EDITOR_MODE_GAME = 0,
    EDITOR_MODE_UI,
    EDITOR_MODE_DEBUG,
    EDITOR_MODE_SETTINGS
};

enum Editor_Control_Settings
{
    EDITOR_CONTROL_PLAY = 0,
    EDITOR_CONTROL_EDIT,
    EDITOR_CONTROL_SETTINGS
};

struct Game_State
{
    Editor_Mode_Settings editor_Mode_Settings;
    Editor_Control_Settings editor_Control_Settings;
};

struct System_State
{
    int screen_Width;
    int screen_Height;
    int fps;
    Color clear_Color;
    const char* window_Title;
};

FUNC_SIG void SystemStateSetup(System_State *system_State)
{
    system_State->window_Title = "Game";
    system_State->screen_Width = 1920;
    system_State->screen_Height = 1080;
    system_State->clear_Color = {255, 255, 255, 255}; // hot pink
    system_State->fps = 120;
}

FUNC_SIG void SetupRaylib(System_State *system_State)
{
    InitWindow(system_State->screen_Width, system_State->screen_Height, system_State->window_Title);
    SetTargetFPS(system_State->fps);

    GuiLoadStyle("assets/styles/cyber/cyber.rgs");

    SetExitKey(0);
}

FUNC_SIG void CleanupRaylib()
{
    CloseWindow();
}

typedef unsigned long long u64;
typedef u64 umax;

struct CharArray
{
    umax char_Length;
    umax allocated_Bytes;
    char *chars;
};

bool RemoveFront(CharArray *char_Array)
{
    if(char_Array->char_Length > 0)
    {
        --char_Array->char_Length;
        return true;
    }
    return false;
}

bool IsWhiteSpace(CharArray *char_Array)
{
    char test = char_Array->chars[0];
    bool result = (test == ' ') || (test == '\r') || (test == '\n') || (test == '\t') || (test == '\f') || (test == '\v');
    return result;
}

bool AddChar(CharArray *char_Array, char token)
{
    if(char_Array->allocated_Bytes > char_Array->char_Length)
    {
        char_Array->chars[char_Array->char_Length] = token;
        ++char_Array->char_Length;
        return true;
    }

    return false;
}

int main(int argc, char** argv)
{
    System_State system_State = {0};
    SystemStateSetup(&system_State);
    SetupRaylib(&system_State);
    
    Game_State game_State = {};

    Player player = { 0 };
    player.position = { 400, 280 };
    player.speed = 0;
    player.canJump = false;
    player.rectangle = { player.position.x - 20, player.position.y - 40, 40, 40 };

    Walls walls[] =
    {
        {{ 0.0f, 0.0f, 10000.0f, 4000.0f }, 0, LIGHTGRAY },
        {{ -2500.0f, 400.0f, 5000.0f, 200.0f }, 1, GRAY },
        {{ 300.0f, 200.0f, 400.0f, 10.0f }, 1, GRAY },
        {{ 250.0f, 300.0f, 100.0f, 10.0f }, 1, GRAY },
        {{ 650.0f, 300.0f, 100.0f, 10.0f }, 1, GRAY },
        {{ 0.0f, 0.0f, 10.0f, 500.0f }, 1, GRAY },
        {{ 10.0f, 10.0f, 20.0f, 20.0f }, 1, BLUE },
        {{ 20.0f, 20.0f, 20.0f, 20.0f }, 1, BLUE },
        {{ 30.0f, 30.0f, 20.0f, 20.0f }, 1, BLUE },
        {{ 40.0f, 40.0f, 20.0f, 20.0f }, 1, BLUE },
    };
    
    int walls_Length = sizeof(walls)/sizeof(walls[0]);

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = { system_State.screen_Width/2.0f, system_State.screen_Height/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    TempGui temp_Gui = {0};
    SetupTempGui(&temp_Gui, system_State.screen_Width, system_State.screen_Height);
    const char *temp_ListView[8] = { "This", "is", "a", "list view", "with", "disable", "elements", "amazing!" };
    temp_Gui.listViewExList = temp_ListView;

    Gui_Window editor_Window = {0};
    InitGuiWIndow(editor_Window);
    editor_Window.title = (char*)"#198# Editor Window";
    editor_Window.position = { system_State.screen_Width * 0.75f, 0.0f };
    editor_Window.rectangle = { 20.0f, 20.0f, (float)system_State.screen_Width / 2.0f, (float)system_State.screen_Height / 2.0f};
    editor_Window.open_Size.x = editor_Window.rectangle.width;
    editor_Window.open_Size.y = editor_Window.rectangle.height;

    //----------------------------------------------------------------------------------

    // General variables
    Vector2 mousePosition = { 0.0f, 0.0f };
    Vector2 panOffset = mousePosition;

    Vector2 touchPosition = { 0.0f, 0.0f };
    Rectangle touchArea = { 0.0f, 0.0f, (float)system_State.screen_Width, (float)system_State.screen_Height };
    int currentGesture = GESTURE_NONE;
    int lastGesture = GESTURE_NONE;

    bool mouseScaleMode = false;
    bool mouseScaleReady = false;
    float window_Corner = 12.0f;
    
    Rectangle active_Rect = {0};
    float width_Corner =  0.0f;
    float height_Corner = 0.0f;
    bool active_ScaleReady = false;
    bool active_ScaleMode = false;

    bool drag_Player = false;
    Vector2 drag_Offset = {0};

    Debug_Window debug_Window = {};
    debug_Window.alive = false;
    debug_Window.previousTextBox = debug_Window.allTextBox[0];
    debug_Window.activeTextBox = debug_Window.allTextBox[0];
    debug_Window.activeTextBox[0] = '>';
    int frame_Counter = 0;

    CharArray command = {};
    command.char_Length = 0;
    command.allocated_Bytes = 512;
    command.chars = (char*)malloc(command.allocated_Bytes * sizeof(char));
    CharArray token = {};
    token.char_Length = 0;
    token.allocated_Bytes = 512;
    token.chars = (char*)malloc(command.allocated_Bytes * sizeof(char));

    Debug_Items scene_Debug_Items = {0};
    scene_Debug_Items.item_Count = 0;
    scene_Debug_Items.item_Hover = -1;
    scene_Debug_Items.item_Selected = -1;

    for(int i = 0; i < walls_Length; ++i)
    {
        scene_Debug_Items.items[scene_Debug_Items.item_Count] = { "Wall", &walls[i].rectangle, WALLS_TYPE, (char*)&walls[i] };
        SetupDebugGuiData(&scene_Debug_Items.items[scene_Debug_Items.item_Count]);
        ++scene_Debug_Items.item_Count;
    }
    scene_Debug_Items.items[scene_Debug_Items.item_Count] = { "Player 1", &player.rectangle, PLAYER_TYPE, (char*)&player };
    SetupDebugGuiData(&scene_Debug_Items.items[scene_Debug_Items.item_Count]);
    ++scene_Debug_Items.item_Count;

    Debug_Items gui_Debug_Items = {0};
    gui_Debug_Items.item_Count = 0;
    gui_Debug_Items.item_Hover = -1;
    gui_Debug_Items.item_Selected = -1;

    // &temp_Gui.gui_Window.open_Size
    gui_Debug_Items.items[gui_Debug_Items.item_Count] = { "Gui Window",  &temp_Gui.gui_Window.rectangle, WINDOW_TYPE, (char*)&temp_Gui.gui_Window };
    SetupDebugGuiData(&gui_Debug_Items.items[gui_Debug_Items.item_Count]);
    ++gui_Debug_Items.item_Count;
    // &editor_Window.open_Size
    gui_Debug_Items.items[gui_Debug_Items.item_Count] = { "Editor Window", &editor_Window.rectangle, WINDOW_TYPE, (char*)&editor_Window };
    SetupDebugGuiData(&gui_Debug_Items.items[gui_Debug_Items.item_Count]);
    ++gui_Debug_Items.item_Count;

    Color debug_Pink = {255, 0, 255, 255};
    Color debug_Green = {0, 255, 0, 255};
    Color debug_Yellow = {0, 255, 255, 255};
    int debug_Line_Size = 2;

    Debug_Item *active_Item = 0;//&scene_Debug_Items.items[scene_Debug_Items.item_Selected];
    Vector2 active_Test = {0};

    Vector2 hot_Spots[4] = {0};
    int hot_Spot_Count = 4;
    float hot_Spot_Size = 5.0f;

    bool isMouseLeftPressed = false;
    bool isMouseLeftReleased = false;

    // Main game loop
    while (!WindowShouldClose()) 
    {
        float deltaTime = GetFrameTime();

        // detect input
        //----------------------------------------------------------------------------------
        if(IsKeyPressed(KEY_E)) // TOGGLE CONTROL MODE
        {
            if((game_State.editor_Control_Settings == EDITOR_CONTROL_EDIT))
            {
                game_State.editor_Control_Settings = EDITOR_CONTROL_PLAY;
            }
            else
            {
                game_State.editor_Control_Settings = EDITOR_CONTROL_EDIT;
            }
        }

        if(IsKeyPressed(KEY_R)) // RESET THE LEVEL
        {
            game_State.editor_Control_Settings = EDITOR_CONTROL_EDIT;
            camera.zoom = 1.0f;
            player.position = { 400.0f, 280.0f };
        }

        if((game_State.editor_Control_Settings == EDITOR_CONTROL_EDIT))
        {
            if(IsKeyPressed(KEY_Q)) // TOGGLE EDIT MODE
            {
                if(game_State.editor_Mode_Settings == EDITOR_MODE_GAME)
                {
                    game_State.editor_Mode_Settings = EDITOR_MODE_UI;
                }
                else
                {
                    game_State.editor_Mode_Settings = EDITOR_MODE_GAME;
                }
            }

            if(IsKeyPressed(KEY_ESCAPE))
            {
                temp_Gui.showMessageBox = !temp_Gui.showMessageBox;
                CloseWindow();
            }

            // Update Debug Menu
            if(IsKeyPressed('`') && IsKeyDown(KEY_LEFT_SHIFT))
            {
                debug_Window.alive = !debug_Window.alive;
            }
            if(IsKeyPressed(KEY_UP))
            {
                if(debug_Window.command_Count > 0)
                {
                    TextCopy(debug_Window.activeTextBox, debug_Window.previousTextBox);
                }
            }
        }

        isMouseLeftPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        isMouseLeftReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

        //mousePosition = GetMousePosition();
        lastGesture = currentGesture;
        currentGesture = GetGestureDetected();
        touchPosition = GetTouchPosition(0);
        Vector2 screen_Space_Touch_Position = GetScreenToWorld2D(touchPosition, camera);

        // update
        //----------------------------------------------------------------------------------
        UpdatePlayer(&player, walls, walls_Length, deltaTime);
        player.rectangle.x = player.position.x - 20;
        player.rectangle.y = player.position.y - 40;
        UpdateCameraPlayerBoundsPush(&camera, &player, walls, walls_Length, deltaTime, system_State.screen_Width, system_State.screen_Height);

        // Have second camera for zooming out
        // draw rect for original camera

        scene_Debug_Items.item_Hover = -1;
        gui_Debug_Items.item_Hover = -1;

        // if(game_State.editor_Control_Settings == EDITOR_CONTROL_PLAY)
        // {
        // }
        // else if(game_State.editor_Control_Settings == EDITOR_CONTROL_EDIT)
        // {
            camera.zoom += ((float)GetMouseWheelMove()*0.05f);
            if (camera.zoom > 3.0f)
            {
                camera.zoom = 3.0f;
            }
            else if (camera.zoom < 0.25f)
            {
                camera.zoom = 0.25f;
            }

            //find the item a mouse is over
            if(game_State.editor_Mode_Settings == EDITOR_MODE_GAME)
            {
                for(int i = 0; i < scene_Debug_Items.item_Count; ++i)
                {
                    if(CheckCollisionPointRec(screen_Space_Touch_Position, *scene_Debug_Items.items[i].rect))
                    {
                        active_Test = screen_Space_Touch_Position;
                        scene_Debug_Items.item_Hover = i;
                    }
                }
            }
            else if(game_State.editor_Mode_Settings == EDITOR_MODE_UI)
            {
                for(int i = 0; i < gui_Debug_Items.item_Count; ++i)
                {
                    if(CheckCollisionPointRec(touchPosition, *gui_Debug_Items.items[i].rect))
                    {
                        active_Test = touchPosition;
                        gui_Debug_Items.item_Hover = i;
                    }
                }
            }
        // }

        
        if(active_Item)
        {
            active_Rect = *active_Item->rect;

            hot_Spots[0] = {active_Rect.x, active_Rect.y};
            hot_Spots[1] = {active_Rect.x + active_Rect.width, active_Rect.y};
            hot_Spots[2] = {active_Rect.x, active_Rect.y + active_Rect.height};
            hot_Spots[3] = {active_Rect.x + active_Rect.width, active_Rect.y + active_Rect.height};
        }

        if(isMouseLeftPressed)
        {
            if(CheckCollisionPointRec(touchPosition, { editor_Window.rectangle.x, editor_Window.rectangle.y, editor_Window.rectangle.width, 20.0f }))
            {
                panOffset.x = touchPosition.x - editor_Window.rectangle.x;
                panOffset.y = touchPosition.y - editor_Window.rectangle.y;

                if(currentGesture == GESTURE_DOUBLETAP)
                {
                    editor_Window.open = !editor_Window.open;
                }

                if(currentGesture == GESTURE_TAP)
                {
                    editor_Window.drag = true;
                }
            }
            else if(CheckCollisionPointRec(touchPosition, { temp_Gui.gui_Window.rectangle.x, temp_Gui.gui_Window.rectangle.y, temp_Gui.gui_Window.rectangle.width, 20.0f }))
            {
                panOffset.x = touchPosition.x - temp_Gui.gui_Window.rectangle.x;
                panOffset.y = touchPosition.y - temp_Gui.gui_Window.rectangle.y;

                if(currentGesture == GESTURE_DOUBLETAP)
                {
                    temp_Gui.gui_Window.open = !temp_Gui.gui_Window.open;
                }

                if(currentGesture == GESTURE_TAP)
                {
                    temp_Gui.gui_Window.drag = true;
                }
            }


            if(CheckCollisionPointCircle(active_Test, hot_Spots[3], hot_Spot_Size))
            {
            }
            else
            {
                active_Item = 0;

                if(!CheckCollisionPointRec(touchPosition, editor_Window.rectangle))
                {
                    scene_Debug_Items.item_Selected = scene_Debug_Items.item_Hover;
                    gui_Debug_Items.item_Selected = gui_Debug_Items.item_Hover;
                    if(scene_Debug_Items.item_Selected > -1)
                    {
                        active_Item = &scene_Debug_Items.items[scene_Debug_Items.item_Selected];
                    }
                    else if(gui_Debug_Items.item_Selected > -1)
                    {
                        active_Item = &gui_Debug_Items.items[gui_Debug_Items.item_Selected];
                    }
                }
            }

            if(active_Item && CheckCollisionPointRec(screen_Space_Touch_Position, *active_Item->rect))
            {
                if(currentGesture == GESTURE_TAP)
                {
                    drag_Player = true;
                    drag_Offset = {screen_Space_Touch_Position.x - active_Item->rect->x, screen_Space_Touch_Position.y - active_Item->rect->y};
                }
            }
        }

        if(temp_Gui.gui_Window.drag)
        {
            temp_Gui.gui_Window.rectangle.x = (touchPosition.x - panOffset.x);
            temp_Gui.gui_Window.rectangle.y = (touchPosition.y - panOffset.y);
            
            if(isMouseLeftReleased)
            {
                temp_Gui.gui_Window.drag = false;
            }
        }

        if(editor_Window.drag) 
        {
            editor_Window.rectangle.x = (touchPosition.x - panOffset.x);
            editor_Window.rectangle.y = (touchPosition.y - panOffset.y);
            
            if(isMouseLeftReleased)
            {
                editor_Window.drag = false;
            }
        }

        // if(CheckCollisionPointRec(touchPosition, temp_Gui.gui_Window.rectangle) && temp_Gui.gui_Window.open &&
        //     CheckCollisionPointRec(touchPosition, { temp_Gui.gui_Window.rectangle.x + temp_Gui.gui_Window.rectangle.width - window_Corner, temp_Gui.gui_Window.rectangle.y + temp_Gui.gui_Window.rectangle.height - window_Corner, window_Corner, window_Corner }))
        // {
        //     mouseScaleReady = true;
        //     if(isMouseLeftPressed)
        //     {
        //         mouseScaleMode = true;
        //     }
        // }
        // else
        // {
        //     mouseScaleReady = false;
        // }

        // if(mouseScaleMode)
        // {
        //     mouseScaleReady = true;

        //     temp_Gui.gui_Window.open_Size.x = (touchPosition.x - temp_Gui.gui_Window.rectangle.x);
        //     temp_Gui.gui_Window.open_Size.y = (touchPosition.y - temp_Gui.gui_Window.rectangle.y);

        //     if(temp_Gui.gui_Window.open_Size.x < window_Corner)
        //     {
        //         temp_Gui.gui_Window.open_Size.x = window_Corner;
        //     }

        //     if(temp_Gui.gui_Window.open_Size.y < window_Corner)
        //     {
        //         temp_Gui.gui_Window.open_Size.y = window_Corner;
        //     }

        //     if(isMouseLeftReleased)
        //     {
        //         mouseScaleMode = false;
        //     }
        // }

        if(active_Item)
        {
            active_Rect = *active_Item->rect;

            hot_Spots[0] = {active_Rect.x, active_Rect.y};
            hot_Spots[1] = {active_Rect.x + active_Rect.width, active_Rect.y};
            hot_Spots[2] = {active_Rect.x, active_Rect.y + active_Rect.height};
            hot_Spots[3] = {active_Rect.x + active_Rect.width, active_Rect.y + active_Rect.height};

            if(CheckCollisionPointCircle(active_Test, hot_Spots[3], hot_Spot_Size))
            {
                active_ScaleReady = true;
                if (isMouseLeftPressed)
                {
                    active_ScaleMode = true;
                }
            }
            else
            {
                active_ScaleReady = false;
            }

            if(drag_Player && !active_ScaleMode)
            {
                //player.position.x = active_Test.x;
                //player.position.y = active_Test.y;
                MoveEntityData(active_Item, {active_Test.x - drag_Offset.x, active_Test.y - drag_Offset.y});

                if(isMouseLeftReleased)
                {
                    drag_Player = false;
                }
            }

            if(active_ScaleMode)
            {
                active_ScaleReady = true;

                active_Rect.width = (active_Test.x - active_Rect.x);
                active_Rect.height = (active_Test.y - active_Rect.y);

                if(active_Rect.width < 0.0f)
                {
                    active_Rect.width = 0.0f;
                }
                if(active_Rect.height < 0.0f)
                {
                    active_Rect.height = 0.0f;
                }

                *active_Item->rect = active_Rect;

                if(isMouseLeftReleased)
                {
                    active_ScaleMode = false;
                    drag_Player = false;
                }
            }
        }

        if(temp_Gui.gui_Window.open)
        {
            temp_Gui.gui_Window.rectangle.width = temp_Gui.gui_Window.open_Size.x;
            temp_Gui.gui_Window.rectangle.height = temp_Gui.gui_Window.open_Size.y;
        }
        else
        {
            temp_Gui.gui_Window.rectangle.height = 1.0f;
        }

        if(editor_Window.open)
        {
            editor_Window.rectangle.width = editor_Window.open_Size.x;
            editor_Window.rectangle.height = editor_Window.open_Size.y;
        }
        else
        {
            editor_Window.rectangle.height = 1.0f;
        }

        // draw
        //----------------------------------------------------------------------------------

        BeginDrawing();

            ClearBackground(system_State.clear_Color);

            BeginMode2D(camera);
           
                for(int i = 0; i < walls_Length; i++)
                {
                    DrawRectangleRec(walls[i].rectangle, walls[i].color);
                }

                DrawRectangleRec(player.rectangle, temp_Gui.colorPickerValue);
               
            EndMode2D();

            if(mouseScaleMode) GuiLock();
            DrawTempGui(&temp_Gui);
            if(mouseScaleMode) GuiUnlock();

            editor_Window.clicked - GuiWindowBox(editor_Window.rectangle, editor_Window.title);
            if(editor_Window.open)
            {
                if(scene_Debug_Items.item_Selected > -1)
                {
                    DebugEditor(&editor_Window, &scene_Debug_Items.items[scene_Debug_Items.item_Selected]);
                }
                else if(gui_Debug_Items.item_Selected > -1)
                {
                    DebugEditor(&editor_Window, &gui_Debug_Items.items[gui_Debug_Items.item_Selected]);
                }
            }

            if(debug_Window.alive)
            {
                //GuiLock();

                debug_Window.window = {0.0f, 0.0f, (float)system_State.screen_Width, (float)system_State.screen_Height / 2.0f};
                DrawRectangleRec(debug_Window.window, { 80, 80, 80, 255 });

                // Font info
                Font gui_Font = GuiGetFont();
                const char* working_Directory = GetWorkingDirectory();
                Vector2 font_Size = MeasureTextEx(gui_Font, working_Directory, gui_Font.baseSize, 0);

                // Path Label
                GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
                Rectangle label_Rect = {debug_Window.window.x, debug_Window.window.y, debug_Window.window.width, font_Size.y};
                GuiLabel(label_Rect, working_Directory);

                // Text box
                GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
                debug_Window.text_Box = {debug_Window.window.x, debug_Window.window.height - 10.0f - font_Size.y, debug_Window.window.width, font_Size.y + 10.0f};
                if(GuiCommandBox(debug_Window.text_Box, debug_Window.activeTextBox, 64, debug_Window.alive, &frame_Counter))
                {
                    if(debug_Window.command_Count < 10)
                    {
                        //CopyString(debug_Window.activeTextBox, &command);
                        TextCopy(command.chars, debug_Window.activeTextBox);

                        // if((command.length > 0) &&  (command.string[0] == '>'))
                        // {
                        //     RemoveFront(&command);

                        //     while((command.length > 0) && IsWhiteSpace(command.string[0]))
                        //     {
                        //         RemoveFront(&command);
                        //     }

                        //     while((command.length > 0) && !IsWhiteSpace(command.string[0]))
                        //     {
                        //         AddChar(&token, command.string[0]);
                        //         RemoveFront(&command);
                        //     }
                        // }

                        if((command.char_Length > 0) &&  (command.chars[0] == '>'))
                        {
                            RemoveFront(&command);

                            while((command.char_Length > 0) && IsWhiteSpace(&command))
                            {
                                RemoveFront(&command);
                            }

                            while((command.char_Length > 0) && !IsWhiteSpace(&command))
                            {
                                AddChar(&token, command.chars[0]);
                                RemoveFront(&command);
                            }
                        }

                        ++debug_Window.command_Count;
                        debug_Window.previousTextBox = debug_Window.allTextBox[debug_Window.command_Count-1];
                        debug_Window.activeTextBox = debug_Window.allTextBox[debug_Window.command_Count];
                        debug_Window.activeTextBox[0] = '>';
                    }
                }

                GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
                for(int i = 0; i < debug_Window.command_Count; ++i)
                {
                    float step1 = debug_Window.command_Count - i;
                    float step2 = step1 + 1;
                    Rectangle label_OldCommand = {debug_Window.window.x, debug_Window.window.height - 10.0f * step2 - font_Size.y * step2, debug_Window.window.width, font_Size.y * step1 - 10.0f * step1};
                    GuiLabel(label_OldCommand, debug_Window.allTextBox[i]);
                }
                //GuiUnlock();
            }

            if(game_State.editor_Mode_Settings == EDITOR_MODE_GAME)
            {
                BeginMode2D(camera);
                // for(int i = 0; i < scene_Debug_Items.item_Count; ++i)
                // {
                //     DrawRectangleLinesEx(*scene_Debug_Items.items[i].rect, debug_Line_Size, YELLOW);
                //     DrawLineEx({scene_Debug_Items.items[i].rect->x, scene_Debug_Items.items[i].rect->y}, {scene_Debug_Items.items[i].rect->x + scene_Debug_Items.items[i].rect->width, scene_Debug_Items.items[i].rect->y + scene_Debug_Items.items[i].rect->height}, debug_Line_Size, YELLOW);
                // }
                if(scene_Debug_Items.item_Hover > -1)
                {
                    DrawRectangleLinesEx(*scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect, debug_Line_Size, debug_Green);
                    DrawLineEx({scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->x, scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->y}, {scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->x + scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->width, scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->y + scene_Debug_Items.items[scene_Debug_Items.item_Hover].rect->height}, debug_Line_Size, debug_Green);
                }

                if(scene_Debug_Items.item_Selected > -1)
                {
                    DrawCircleV(hot_Spots[0], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[1], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[2], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[3], hot_Spot_Size, YELLOW);
                    DrawRectangleLinesEx(*scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect, debug_Line_Size, debug_Yellow);
                    DrawLineEx({scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->x, scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->y}, {scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->x + scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->width, scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->y + scene_Debug_Items.items[scene_Debug_Items.item_Selected].rect->height}, debug_Line_Size, debug_Yellow);
                }
                EndMode2D();
            }
            else if(game_State.editor_Mode_Settings == EDITOR_MODE_UI)
            {
                if(gui_Debug_Items.item_Hover > -1)
                {
                    DrawRectangleLinesEx(*gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect, debug_Line_Size, debug_Green);
                    DrawLineEx({gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->x, gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->y}, {gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->x + gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->width, gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->y + gui_Debug_Items.items[gui_Debug_Items.item_Hover].rect->height}, debug_Line_Size, debug_Green);
                }

                if(gui_Debug_Items.item_Selected > -1)
                {
                    DrawCircleV(hot_Spots[0], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[1], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[2], hot_Spot_Size, YELLOW);
                    DrawCircleV(hot_Spots[3], hot_Spot_Size, YELLOW);
                    DrawRectangleLinesEx(*gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect, debug_Line_Size, debug_Yellow);
                    DrawLineEx({gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->x, gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->y}, {gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->x + gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->width, gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->y + gui_Debug_Items.items[gui_Debug_Items.item_Selected].rect->height}, debug_Line_Size, debug_Yellow);
                }
            }

            //DEBUG_DrawLines();

            DrawFPS(system_State.screen_Width - 90, system_State.screen_Height - 30);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CleanupRaylib();

    return 0;
}
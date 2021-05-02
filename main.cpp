#include "raywrapper.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION

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
    system_State->screen_Width = 1980 / 2;
    system_State->screen_Height = 1020 / 2;
    system_State->clear_Color = {255, 0, 255, 255}; // hot pink
    system_State->fps = 120;
}

FUNC_SIG void SetupRaylib(System_State *system_State)
{
    InitWindow(system_State->screen_Width, system_State->screen_Height, system_State->window_Title);
    SetTargetFPS(system_State->fps);
    
    SetExitKey(0);
}

FUNC_SIG void CleanupRaylib()
{
    CloseWindow();
}

int main(int argc, char** argv)
{
    System_State system_State = {0};
    SystemStateSetup(&system_State);
    SetupRaylib(&system_State);

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
    };
    
    int walls_Length = sizeof(walls)/sizeof(walls[0]);

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = { system_State.screen_Width/2.0f, system_State.screen_Height/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Gui_Window gui_Window;
    gui_Window.alive = true;
    gui_Window.open = true;
    gui_Window.title = "#198# Controls Window";
    gui_Window.position = {0.0f, 0.0f};
    gui_Window.rectangle = { 200, 200, system_State.screen_Width/2.0f, system_State.screen_Height/2.0f };

    // GUI controls initialization
    //----------------------------------------------------------------------------------
    int dropdownBox000Active = 0;
    bool dropDown000EditMode = false;

    int dropdownBox001Active = 0;
    bool dropDown001EditMode = false;

    int spinner001Value = 0;
    bool spinnerEditMode = false;

    int valueBox002Value = 0;
    bool valueBoxEditMode = false;

    char textBoxText[64] = "Text box";
    bool textBoxEditMode = false;

    int listViewScrollIndex = 0;
    int listViewActive = -1;

    int listViewExScrollIndex = 0;
    int listViewExActive = 2;
    int listViewExFocus = -1;
    const char *listViewExList[8] = { "This", "is", "a", "list view", "with", "disable", "elements", "amazing!" };

    char multiTextBoxText[256] = "Multi text box";
    bool multiTextBoxEditMode = false;
    Color colorPickerValue = RED;

    int sliderValue = 50;
    int sliderBarValue = 60;
    float progressValue = 0.4f;

    bool forceSquaredChecked = false;

    float alphaValue = 0.5f;

    int comboBoxActive = 1;

    int toggleGroupActive = 0;

    Vector2 viewScroll = { 5, 5 };

    bool exitWindow = false;
    bool showMessageBox = false;

    char textInput[256] = { 0 };
    bool showTextInputBox = false;

    char textInputFileName[256] = { 0 };
    //----------------------------------------------------------------------------------

    // General variables
    Vector2 mousePosition = { 0.0f, 0.0f };
    Vector2 windowPosition = { 500.0f, 200.0f };
    Vector2 panOffset = mousePosition;
    bool dragWindow = false;
    bool collapseWindow = false;
    
    SetWindowPosition(windowPosition.x, windowPosition.y);
    
    bool exitNewWindow = false;
    bool guiNewWindow = false;

    Vector2 touchPosition = { 0.0f, 0.0f };
    Rectangle touchArea = { 0.0f, 0.0f, system_State.screen_Width, system_State.screen_Height };
    int currentGesture = GESTURE_NONE;
    int lastGesture = GESTURE_NONE;

    bool mouseScaleReady = false;
    bool mouseScaleMode = false;
    float window_Corner = 12.0f;

    Vector2 open_Size = { 0.0f, 0.0f };
    open_Size.x = gui_Window.rectangle.width;
    open_Size.y = gui_Window.rectangle.height;

    bool drag_Player = false;

    Debug_Window debug_Window = {};
    debug_Window.alive = true;
    //debug_Window.previousTextBox = debug_Window.allTextBox[1];
    //debug_Window.activeTextBox = debug_Window.allTextBox[0];
    debug_Window.previousTextBox = debug_Window.allTextBox[2];
    debug_Window.activeTextBox = debug_Window.allTextBox[3];
    debug_Window.command_Count = 3;

    TextCopy(debug_Window.allTextBox[0], "THIS IS TEST 0");
    TextCopy(debug_Window.allTextBox[1], "THIS IS TEST 1");
    TextCopy(debug_Window.allTextBox[2], "THIS IS TEST 2");

    // Main game loop
    while (!WindowShouldClose()) 
    {
        float deltaTime = GetFrameTime();
        
        UpdatePlayer(&player, walls, walls_Length, deltaTime);
        player.rectangle.x = player.position.x - 20;
        player.rectangle.y = player.position.y - 40;

        camera.zoom += ((float)GetMouseWheelMove()*0.05f);
        
        if (camera.zoom > 3.0f)
        {
            camera.zoom = 3.0f;
        }
        else if (camera.zoom < 0.25f)
        {
            camera.zoom = 0.25f;
        }
        
        if(IsKeyPressed(KEY_R)) 
        {
            camera.zoom = 1.0f;
            player.position = { 400.0f, 280.0f };
        }

        UpdateCameraPlayerBoundsPush(&camera, &player, walls, walls_Length, deltaTime, system_State.screen_Width, system_State.screen_Height);

        // window
        //----------------------------------------------------------------------------------

        //mousePosition = GetMousePosition();
        lastGesture = currentGesture;
        currentGesture = GetGestureDetected();
        touchPosition = GetTouchPosition(0);

        int char_Pressed = GetCharPressed();
        if(KEY_TILDA == char_Pressed)
        {
            debug_Window.alive = !debug_Window.alive;
        }

        if(IsKeyPressed(KEY_ENTER))
        {
            if(debug_Window.command_Count < 10)
            {
                //TextCopy(debug_Window.previousTextBox, debug_Window.activeTextBox);
                ++debug_Window.command_Count;
                debug_Window.previousTextBox = debug_Window.allTextBox[debug_Window.command_Count+1];
                debug_Window.activeTextBox = debug_Window.allTextBox[debug_Window.command_Count];
            }
        }
        
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if(CheckCollisionPointRec(touchPosition, { gui_Window.rectangle.x, gui_Window.rectangle.y, gui_Window.rectangle.width, 20.0f }))
            {
                panOffset.x = touchPosition.x - gui_Window.rectangle.x;
                panOffset.y = touchPosition.y - gui_Window.rectangle.y;

                if(currentGesture == GESTURE_DOUBLETAP)
                {
                    collapseWindow = !collapseWindow;
                }

                if(currentGesture == GESTURE_TAP)
                {
                    dragWindow = true;
                }
            }

            // Convert to screen space
            if(CheckCollisionPointRec(touchPosition, player.rectangle))
            {
                panOffset.x = touchPosition.x - player.rectangle.x;
                panOffset.y = touchPosition.y - player.rectangle.y;

                if(currentGesture == GESTURE_TAP)
                {
                    drag_Player = true;
                }
            }
        }

        if(dragWindow)
        {            
            gui_Window.rectangle.x = (touchPosition.x - panOffset.x);
            gui_Window.rectangle.y = (touchPosition.y - panOffset.y);
            
            if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                dragWindow = false;
            }
        }

        if(drag_Player)
        {
            player.position.x = (touchPosition.x - panOffset.x);
            player.position.y = (touchPosition.y - panOffset.y);

            if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                drag_Player = false;
            }
        }

        if(CheckCollisionPointRec(touchPosition, gui_Window.rectangle) && !collapseWindow &&
            CheckCollisionPointRec(touchPosition, { gui_Window.rectangle.x + gui_Window.rectangle.width - window_Corner, gui_Window.rectangle.y + gui_Window.rectangle.height - window_Corner, window_Corner, window_Corner }))
        {
            mouseScaleReady = true;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                mouseScaleMode = true;
            }
        }
        else
        {
            mouseScaleReady = false;
        }

        if(mouseScaleMode)
        {
            mouseScaleReady = true;

            open_Size.x = (touchPosition.x - gui_Window.rectangle.x);
            open_Size.y = (touchPosition.y - gui_Window.rectangle.y);

            if(open_Size.x < window_Corner)
            {
                open_Size.x = window_Corner;
            }

            if(open_Size.y < window_Corner)
            {
                open_Size.y = window_Corner;
            }

            if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                mouseScaleMode = false;
            }
        }

        if(collapseWindow)
        {
            gui_Window.rectangle.height = 1.0f;
        }
        else
        {
            gui_Window.rectangle.width = open_Size.x;
            gui_Window.rectangle.height = open_Size.y;
        }

        //----------------------------------------------------------------------------------


        BeginDrawing();

            ClearBackground(system_State.clear_Color);

            BeginMode2D(camera);
           
                for (int i = 0; i < walls_Length; i++)
                {
                    DrawRectangleRec(walls[i].rect, walls[i].color);
                }

                DrawRectangleRec(player.rectangle, colorPickerValue);
               
            EndMode2D();

            // exitNewWindow = GuiWindowBox(gui_Window.rectangle, "#198# Controls Window");
            // if(!collapseWindow)
            // {
            //     const char* mouse_Position_Text = TextFormat("- Mouse Position: [ %.0f, %.0f ]", touchPosition.x, touchPosition.y);
            //     float text_Height = 20.0f * 3.0f;
            //     GuiLabel({ gui_Window.rectangle.x + 20.0f, gui_Window.rectangle.y + 20.0f, gui_Window.rectangle.width - 20.0f, text_Height }, "Mouse Info:");
            //     GuiLabel({ gui_Window.rectangle.x + 40.0f, gui_Window.rectangle.y + 40.0f, gui_Window.rectangle.width - 20.0f, text_Height }, "- Mouse Wheel to Zoom in-out, R to reset zoom");
            //     GuiLabel({ gui_Window.rectangle.x + 40.0f, gui_Window.rectangle.y + 60.0f, gui_Window.rectangle.width - 20.0f, text_Height }, mouse_Position_Text);
            // }

            guiNewWindow = GuiWindowBox(gui_Window.rectangle, gui_Window.title);
            //guiWindowScrollIndex
            if(!collapseWindow)
            {
                // NOTE: View rectangle could be used to perform some scissor test
                Rectangle view = GuiScrollPanel( { gui_Window.rectangle.x + 5.0f, gui_Window.rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2, gui_Window.rectangle.width - 10.0f, gui_Window.rectangle.height - 7.0f - WINDOW_STATUSBAR_HEIGHT}, { gui_Window.rectangle.x + 5.0f, gui_Window.rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2, GetScreenWidth(), GetScreenHeight() }, &viewScroll);
                
                //BeginScissorMode((int)(gui_Window.rectangle.x + 5.0f), (int)(gui_Window.rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2.0f), (int)(gui_Window.rectangle.width - 10.0f), (int)(gui_Window.rectangle.height - 7.0f - WINDOW_STATUSBAR_HEIGHT));
                BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);


                // raygui: controls drawing
                //----------------------------------------------------------------------------------
                if(dropDown000EditMode || dropDown001EditMode)
                {
                    GuiLock();
                }

                // First GUI column
                forceSquaredChecked = GuiCheckBox({ view.x + 25, view.y + 108, 15, 15 }, "FORCE CHECK!", forceSquaredChecked);

                GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
                if (GuiSpinner({ view.x + 25,  view.y + 135, 125, 30 }, NULL, &spinner001Value, 0, 100, spinnerEditMode))
                {
                    spinnerEditMode = !spinnerEditMode;
                }
                if (GuiValueBox({ view.x + 25,  view.y + 175, 125, 30 }, NULL, &valueBox002Value, 0, 100, valueBoxEditMode))
                {
                    valueBoxEditMode = !valueBoxEditMode;
                }

                GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
                if (GuiTextBox({ view.x + 25,  view.y + 215, 125, 30 }, textBoxText, 64, textBoxEditMode))
                {
                    textBoxEditMode = !textBoxEditMode;
                }

                GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

                if (GuiButton({ view.x + 25,  view.y + 255, 125, 30 }, GuiIconText(RICON_FILE_SAVE, "Save File")))
                {
                    showTextInputBox = true;
                }

                GuiGroupBox({ view.x + 25,  view.y + 310, 125, 150 }, "STATES");
                GuiLock();
                    GuiSetState(GUI_STATE_NORMAL); if(GuiButton({ view.x + 30,  view.y + 320, 115, 30 }, "NORMAL")) { }
                    GuiSetState(GUI_STATE_FOCUSED); if (GuiButton({ view.x + 30,  view.y + 355, 115, 30 }, "FOCUSED")) { }
                    GuiSetState(GUI_STATE_PRESSED); if (GuiButton({ view.x + 30,  view.y + 390, 115, 30 }, "#15#PRESSED")) { }
                    GuiSetState(GUI_STATE_DISABLED); if (GuiButton({ view.x + 30,  view.y + 425, 115, 30 }, "DISABLED")) { }
                    GuiSetState(GUI_STATE_NORMAL);
                GuiUnlock();

                comboBoxActive = GuiComboBox({ view.x + 25,  view.y + 470, 125, 30 }, "ONE;TWO;THREE;FOUR", comboBoxActive);

                // NOTE: GuiDropdownBox must draw after any other control that can be covered on unfolding
                GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
                if (GuiDropdownBox({ view.x + 25, view.y + 65, 125, 30 }, "#01#ONE;#02#TWO;#03#THREE;#04#FOUR", &dropdownBox001Active, dropDown001EditMode))
                {
                    dropDown001EditMode = !dropDown001EditMode;
                }

                GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
                if(GuiDropdownBox({ view.x + 25,  view.y + 25, 125, 30 }, "ONE;TWO;THREE", &dropdownBox000Active, dropDown000EditMode))
                {
                    dropDown000EditMode = !dropDown000EditMode;
                }

                // Second GUI column
                listViewActive = GuiListView({ view.x + 165, view.y + 25, 140, 140 }, "Charmander;Bulbasaur;#18#Squirtel;Pikachu;Eevee;Pidgey", &listViewScrollIndex, listViewActive);
                listViewExActive = GuiListViewEx({ view.x + 165, view.y + 180, 140, 200 }, listViewExList, 8, &listViewExFocus, &listViewExScrollIndex, listViewExActive);

                toggleGroupActive = GuiToggleGroup({ view.x + 165, view.y + 400, 140, 25 }, "#1#ONE\n#3#TWO\n#8#THREE\n#23#", toggleGroupActive);

                // Third GUI column
                if (GuiTextBoxMulti({ view.x + 320, view.y + 25, 225, 140 }, multiTextBoxText, 256, multiTextBoxEditMode))
                {
                    multiTextBoxEditMode = !multiTextBoxEditMode;
                }
                colorPickerValue = GuiColorPicker({ view.x + 320, view.y + 185, 196, 192 }, colorPickerValue);

                sliderValue = GuiSlider({ view.x + 355, view.y + 400, 165, 20 }, "TEST", TextFormat("%2.2f", (float)sliderValue), sliderValue, -50, 100);
                sliderBarValue = GuiSliderBar({ view.x + 320, view.y + 430, 200, 20 }, NULL, TextFormat("%i", (int)sliderBarValue), sliderBarValue, 0, 100);
                progressValue = GuiProgressBar({ view.x + 320, view.y + 460, 200, 20 }, NULL, NULL, progressValue, 0, 1);
                alphaValue = GuiColorBarAlpha({ view.x + 320, view.y + 490, 200, 30 }, alphaValue);

                GuiStatusBar({ view.x, view.y + view.height - 20.0f, view.x + view.width, 20.0f }, "This is a status bar");

                if (showMessageBox)
                {
                    DrawRectangle(view.x, view.y, view.width, view.height, Fade(RAYWHITE, 0.8f));
                    int result = GuiMessageBox({ view.x + view.width/2 - 125, view.y + view.height/2 - 50, 250, 100 }, GuiIconText(RICON_EXIT, "Close Window"), "Do you really want to exit?", "Yes;No");

                    if ((result == 0) || (result == 2)) showMessageBox = false;
                    else if (result == 1) exitWindow = true;
                }

                if (showTextInputBox)
                {
                    DrawRectangle(view.x, view.y, view.width, view.height, Fade(RAYWHITE, 0.8f));
                    int result = GuiTextInputBox({ view.x + view.width/2 - 120, view.y + view.height/2 - 60, 240, 140 }, GuiIconText(RICON_FILE_SAVE, "Save file as..."), "Introduce a save file name", "Ok;Cancel", textInput);

                    if(result == 1)
                    {
                        // TODO: Validate textInput value and save
                        strcpy(textInputFileName, textInput);
                    }

                    if ((result == 0) || (result == 1) || (result == 2))
                    {
                        showTextInputBox = false;
                        strcpy(textInput, "\0");
                    }
                }

                GuiUnlock();
                EndScissorMode();
            }

            if(debug_Window.alive)
            {
                //GuiLock();

                debug_Window.window = {0.0f, 0.0f, (float)system_State.screen_Width, (float)system_State.screen_Height / 2.0f};
                DrawRectangleRec(debug_Window.window, { 80, 80, 80, 255 });

                Font gui_Font = GuiGetFont();
                const char* working_Directory = GetWorkingDirectory();
                vec2 font_Size = MeasureTextEx(gui_Font, working_Directory, gui_Font.baseSize, 0);
                Rectangle label_Rect = {debug_Window.window.x, debug_Window.window.y, debug_Window.window.width / 0.5f, font_Size.y};

                GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
                GuiLabel(label_Rect, working_Directory);

                GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
                debug_Window.text_Box = {debug_Window.window.x + 10.0f, debug_Window.window.height - 10.0f - font_Size.y, debug_Window.window.width - 20.0f, font_Size.y + 10.0f};
                //debug_Window.activeTextBox[0] = '>';
                if(GuiTextBox(debug_Window.text_Box, debug_Window.activeTextBox, 64, debug_Window.textBoxEditMode))
                {
                    debug_Window.textBoxEditMode = !debug_Window.textBoxEditMode;
                }

                // GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
                // for(int i = 1; i < debug_Window.command_Count; ++i)
                // {
                //     Rectangle label_OldCommand = {debug_Window.window.x, debug_Window.window.y + debug_Window.window.height - font_Size.y * (i - 1), debug_Window.window.width, debug_Window.window.height - font_Size.y * i};
                //     GuiLabel(label_OldCommand, debug_Window.allTextBox[i]);
                // }
                
                //GuiUnlock();
            }

            DrawFPS(system_State.screen_Width - 90, system_State.screen_Height - 30);
            
            DEBUG_DrawLines();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CleanupRaylib();

    return 0;
}

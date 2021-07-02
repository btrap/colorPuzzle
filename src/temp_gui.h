#pragma once

#include "raywrapper.h"

struct TempGui
{
    Gui_Window gui_Window;
    Gui_EditComboControl gui_Drop_Down_Box_B;
    Gui_EditComboControl gui_Drop_Down_Box_A;
    Gui_EditActiveControl gui_CheckBox;

    Gui_EditValueControl gui_Spinner;
    Gui_EditValueControl gui_Value_Box;
    char textBoxText[64];
    Gui_EditStringControl gui_Text_Box;
    Gui_Control gui_Button_Save;

    Gui_Control gui_Button_Edit_Mode;

    Gui_Control gui_Group_Box;
    Gui_Control gui_Button_Normal;
    Gui_Control gui_Button_Focused;
    Gui_Control gui_Button_Pressed;
    Gui_Control gui_Button_Disabled;

    Gui_ComboControl gui_Combo_Box;

    Gui_EditStringControl gui_Multi_Text_Box;
    Gui_ComboControl gui_Toggle_Group;

    Color colorPickerValue;

    int listViewScrollIndex;
    int listViewActive;
    int listViewExScrollIndex;
    int listViewExActive;
    int listViewExFocus;
    const char **listViewExList;

    char multiTextBoxText[256];
    char textInput[256];
    char textInputFileName[256];
    
    int sliderValue;
    int sliderBarValue;
    float progressValue;
    float alphaValue;

    bool exitWindow;
    bool showTextInputBox;
    bool showMessageBox;
};

void SetupTempGui(TempGui *tempGui, float width, float height)
{
    InitGuiWIndow(tempGui->gui_Window);
    tempGui->gui_Window.open = false;
    tempGui->gui_Window.title = (char*)"#198# Controls Window";
    tempGui->gui_Window.position = { 500.0f, 200.0f };
    tempGui->gui_Window.rectangle = { 0, 0, width/4.0f, height/2.0f };

    tempGui->gui_Window.open_Size.x = tempGui->gui_Window.rectangle.width;
    tempGui->gui_Window.open_Size.y = tempGui->gui_Window.rectangle.height;

    tempGui->gui_Drop_Down_Box_B = { { 25, 25, 125, 30 }, "ONE;TWO;THREE", 0, false };
    tempGui->gui_Drop_Down_Box_A = { { 25, 65, 125, 30 }, "#01#ONE;#02#TWO;#03#THREE;#04#FOUR", 0, false };

    tempGui->gui_CheckBox = {{ 25, 108, 15, 15 }, "FORCE CHECK!", false};

    tempGui->gui_Spinner = {{ 25, 135, 125, 30 }, 0, 0, 0, 100, false, 0};
    tempGui->gui_Value_Box = {{ 25, 175, 125, 30 }, 0, 0, 0, 255, false, 0};

    //CopyString("Text box", tempGui->textBoxText);
    TextCopy(tempGui->textBoxText, "Text box");
    tempGui->gui_Text_Box = {{ 25, 215, 125, 30 }, tempGui->textBoxText, 64, false};
    //Gui_Button gui_Button_Save = {{ 25,  255, 125, 30 }, GuiIconText(RICON_FILE_SAVE, "Save File")};
    tempGui->gui_Button_Save = {{ 25,  255, 125, 30 }, "#6#Save File"};

    float button_Height = height * 0.1f;
    float button_Width = width * 0.1f;
    Rectangle button_Rect = {width - 2.0f * button_Width, height - 2.0f * button_Height, button_Height, button_Width};

    tempGui->gui_Button_Edit_Mode = {button_Rect, "Edit Mode"};

    tempGui->gui_Group_Box = {{ 25, 310, 125, 150 }, "STATES"};
    tempGui->gui_Button_Normal = {{ 30, 320, 115, 30 }, "NORMAL"};
    tempGui->gui_Button_Focused = {{ 30, 355, 115, 30 }, "FOCUSED"};
    tempGui->gui_Button_Pressed = {{ 30, 390, 115, 30 }, "#15#PRESSED"};
    tempGui->gui_Button_Disabled = {{ 30, 425, 115, 30 }, "DISABLED"};

    tempGui->gui_Combo_Box = {{ 25, 470, 125, 30 }, "ONE;TWO;THREE;FOUR", 1};

    tempGui->listViewExList = 0;

    tempGui->listViewScrollIndex = 0;
    tempGui->listViewActive = -1;
    tempGui->listViewExScrollIndex = 0;
    tempGui->listViewExActive = 2;
    tempGui->listViewExFocus = -1;

    //CopyString("Multi text box", tempGui->multiTextBoxText);
    TextCopy(tempGui->multiTextBoxText, "Multi text box");
    tempGui->gui_Multi_Text_Box = {{ 320, 25, 225, 140 }, tempGui->multiTextBoxText, 256, false, 0};

    tempGui->sliderValue = 50;
    tempGui->sliderBarValue = 60;
    tempGui->progressValue = 0.4f;
    tempGui->alphaValue = 0.5f;

    tempGui->gui_Toggle_Group = {{ 165, 400, 140, 25 }, "#1#ONE\n#3#TWO\n#8#THREE\n#23#", 0 };

    tempGui->exitWindow = false;
    tempGui->showMessageBox = false;

    tempGui->colorPickerValue = RED;

    tempGui->textInput[256] = { 0 };
    tempGui->showTextInputBox = false;

    tempGui->textInputFileName[256] = { 0 };
}

void UpdateTempGui(TempGui *tempGui)
{

}

void DrawTempGui(TempGui *tempGui)
{
    tempGui->gui_Window.clicked = GuiWindowBox(tempGui->gui_Window.rectangle, tempGui->gui_Window.title);
    if(tempGui->gui_Window.open)
    {
        // NOTE: View rectangle could be used to perform some scissor test
        Rectangle view = GuiScrollPanel( { tempGui->gui_Window.rectangle.x + 5.0f, tempGui->gui_Window.rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2, tempGui->gui_Window.rectangle.width - 10.0f, tempGui->gui_Window.rectangle.height - 7.0f - WINDOW_STATUSBAR_HEIGHT}, { tempGui->gui_Window.rectangle.x + 5.0f, tempGui->gui_Window.rectangle.y + WINDOW_STATUSBAR_HEIGHT + 2, (float)GetScreenWidth(), (float)GetScreenHeight() }, &tempGui->gui_Window.viewScroll);

        Vector2 origin = {view.x, view.y};
        BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);

        if(tempGui->gui_Drop_Down_Box_A.edit_Mode || tempGui->gui_Drop_Down_Box_B.edit_Mode)
        {
            GuiLock();
        }  
        
        // First GUI column
        tempGui->gui_CheckBox.active = GuiCheckBox(&tempGui->gui_CheckBox, origin);

        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
        if(GuiSpinner(&tempGui->gui_Spinner, origin))
        {
            tempGui->gui_Spinner.edit_Mode = !tempGui->gui_Spinner.edit_Mode;
        }

        if(GuiValueBox(&tempGui->gui_Value_Box, origin))
        {
            tempGui->gui_Value_Box.edit_Mode = !tempGui->gui_Value_Box.edit_Mode;
        }

        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
        if(GuiTextBox(&tempGui->gui_Text_Box, origin))
        {
            tempGui->gui_Text_Box.edit_Mode = !tempGui->gui_Text_Box.edit_Mode;
        }

        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
        if(GuiButton(&tempGui->gui_Button_Save, origin))
        {
            tempGui->showTextInputBox = true;
        }

        if(tempGui->gui_Drop_Down_Box_A.edit_Mode || tempGui->gui_Drop_Down_Box_B.edit_Mode)
        {
            GuiUnlock();
        }

        GuiGroupBox(&tempGui->gui_Group_Box, origin);
        GuiLock();
            GuiSetState(GUI_STATE_NORMAL);
            if(GuiButton(&tempGui->gui_Button_Normal, origin)) { }
            GuiSetState(GUI_STATE_FOCUSED);
            if (GuiButton(&tempGui->gui_Button_Focused, origin)) { }
            GuiSetState(GUI_STATE_PRESSED);
            if (GuiButton(&tempGui->gui_Button_Pressed, origin)) { }
            GuiSetState(GUI_STATE_DISABLED);
            if (GuiButton(&tempGui->gui_Button_Disabled, origin)) { }
            GuiSetState(GUI_STATE_NORMAL);
        GuiUnlock();

        tempGui->gui_Combo_Box.active = GuiComboBox(&tempGui->gui_Combo_Box, origin);

        /////////// NOTE: GuiDropdownBox must draw after any other control that can be covered on unfolding
        GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
        if(GuiDropdownBox(&tempGui->gui_Drop_Down_Box_A, origin))
        {
            tempGui->gui_Drop_Down_Box_A.edit_Mode = !tempGui->gui_Drop_Down_Box_A.edit_Mode;
        }

        GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
        if(GuiDropdownBox(&tempGui->gui_Drop_Down_Box_B, origin))
        {
            tempGui->gui_Drop_Down_Box_B.edit_Mode = !tempGui->gui_Drop_Down_Box_B.edit_Mode;
        }
        ///////////// End DropDowns

        // Second GUI column
        tempGui->listViewActive = GuiListView({ view.x + 165, view.y + 25, 140, 140 }, "Charmander;Bulbasaur;#18#Squirtel;Pikachu;Eevee;Pidgey", &tempGui->listViewScrollIndex, tempGui->listViewActive);
        tempGui->listViewExActive = GuiListViewEx({ view.x + 165, view.y + 180, 140, 200 }, (&tempGui->listViewExList[0]), 8, &tempGui->listViewExFocus, &tempGui->listViewExScrollIndex, tempGui->listViewExActive);

        tempGui->gui_Toggle_Group.active = GuiToggleGroup(&tempGui->gui_Toggle_Group, origin);

        // Third GUI column
        if(GuiTextBoxMulti(&tempGui->gui_Multi_Text_Box, origin))
        {
            tempGui->gui_Multi_Text_Box.edit_Mode = !tempGui->gui_Multi_Text_Box.edit_Mode;
        }
        tempGui->colorPickerValue = GuiColorPicker({ view.x + 320, view.y + 185, 196, 192 }, tempGui->colorPickerValue);

        tempGui->sliderValue = GuiSlider({ view.x + 355, view.y + 400, 165, 20 }, "TEST", TextFormat("%2.2f", (float)tempGui->sliderValue), tempGui->sliderValue, -50, 100);
        tempGui->sliderBarValue = GuiSliderBar({ view.x + 320, view.y + 430, 200, 20 }, NULL, TextFormat("%i", (int)tempGui->sliderBarValue), tempGui->sliderBarValue, 0, 100);
        tempGui->progressValue = GuiProgressBar({ view.x + 320, view.y + 460, 200, 20 }, NULL, NULL, tempGui->progressValue, 0, 1);
        tempGui->alphaValue = GuiColorBarAlpha({ view.x + 320, view.y + 490, 200, 30 }, tempGui->alphaValue);

        GuiStatusBar({ view.x, view.y + view.height - 20.0f, view.x + view.width, 20.0f }, "This is a status bar");

        if(tempGui->showMessageBox)
        {
            DrawRectangle(view.x, view.y, view.width, view.height, Fade(RAYWHITE, 0.8f));
            int result = GuiMessageBox({ view.x + view.width/2 - 125, view.y + view.height/2 - 50, 250, 100 }, GuiIconText(RICON_EXIT, "Close Window"), "Do you really want to exit?", "Yes;No");

            if ((result == 0) || (result == 2))
                tempGui->showMessageBox = false;
            else if (result == 1)
                tempGui->exitWindow = true;
        }

        if(tempGui->showTextInputBox)
        {
            DrawRectangle(view.x, view.y, view.width, view.height, Fade(RAYWHITE, 0.8f));
            int result = GuiTextInputBox({ view.x + view.width/2 - 120, view.y + view.height/2 - 60, 240, 140 }, GuiIconText(RICON_FILE_SAVE, "Save file as..."), "Introduce a save file name", "Ok;Cancel", tempGui->textInput);

            if(result == 1)
            {
                // TODO: Validate textInput value and save
                strcpy(tempGui->textInputFileName, tempGui->textInput);
            }

            if((result == 0) || (result == 1) || (result == 2))
            {
                tempGui->showTextInputBox = false;
                strcpy(tempGui->textInput, "\0");
            }
        }

        EndScissorMode();
    }
}

#include "panel.h"

#include <stdio.h>

const float outlineRecThickness = 2.0f;

void InitPanel(Panel *panel)
{
    panel->panelRec.x = 0.0f;

    panel->panelRec.y = 0.0f;

    panel->panelRec.width = GetScreenWidth();

    panel->panelRec.height = GetScreenHeight() * 0.05f;

    InitButton(
        &panel->saveBtn,
        0.0f, 0.0f,
        100.0f, panel->panelRec.height,
        "Save", 20,
        BLACK
    );

    panel->colors[0] = BLACK;
    panel->colors[1] = RAYWHITE;
    panel->colors[2] = RED;
    panel->colors[3] = GREEN;
    panel->colors[4] = BLUE;
    panel->colors[5] = YELLOW;
    panel->colors[6] = MAGENTA;
    panel->colors[7] = SKYBLUE;

    //drop down menus

    for(int i = 0; i < MAX_DROP_DOWN_MENUS; i++)
    {
        const char* text = "";

        Options currentMenuOptions[MAX_OPTIONS];

        for(int o = 0; o < MAX_OPTIONS; o++)
        {
            currentMenuOptions[i] = NONE;
        }

        if(i == 0) //brush type
        {
            currentMenuOptions[0] = CIRC_BRUSH;
            currentMenuOptions[1] = SQR_BRUSH;

            text = "Brush type";
        }

        if(i == 1) //fill shape
        {
            currentMenuOptions[0] = CIRC_FILL;
            currentMenuOptions[1] = CIRC_LINES;

            currentMenuOptions[2] = REC_FILL;
            currentMenuOptions[3] = REC_LINES;

            text = "Fill shape";
        }

        float btnWidth = 150.0f;

        float currentX = 100.0f;

        if(i > 0) currentX += btnWidth;

        InitiDropDownMenu(
            &panel->dropdownMenus[i],
            text,
            currentX,
            0.0f,
            btnWidth,
            panel->panelRec.height,
            panel->panelRec.height,
            currentMenuOptions
        );
    }
}

void UpdatePanel(
    Panel* panel,
    Color* brushColor,
    Options* currentOption,
    bool* dragging,
    bool* drawing,
    bool* drawingShape,
    Texture2D texture
)
{
    UpdateButton(&panel->saveBtn, NULL);

    Vector2 mousePos = GetMousePosition();

    for(int i = 0; i < MAX_COLORS; i++)
    {
        Rectangle colorRec = GetColorRectangle(panel, i);

        if(CheckCollisionPointRec(mousePos, colorRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            *brushColor = panel->colors[i];

            break;
        }
    }

    if(panel->saveBtn.clicked)
    {
        Image exportImage = LoadImageFromTexture(texture);
        ImageFlipVertical(&exportImage);
        
        ExportImage(exportImage, "testExport.png");

        UnloadImage(exportImage);
    }

    for(int i = 0; i < MAX_DROP_DOWN_MENUS; i++)
    {
        UpdateDropDownMenu(&panel->dropdownMenus[i]);
    }

    for(int i = 0; i < MAX_DROP_DOWN_MENUS; i++)
    {
        DropDownMenu* currentMenu = &panel->dropdownMenus[i];

        if(!currentMenu) continue;

        if(currentMenu->deployed)
        {
            for(int i = 0; i < MAX_OPTIONS; i++)
            {
                MenuButton* currentBtn = &currentMenu->optionsBtns[i];

                if(currentBtn->text == "NULL") continue;

                if(currentBtn->clicked)
                {
                    if(currentBtn->text == "CIRC_BRUSH") *currentOption = CIRC_BRUSH;

                    if(currentBtn->text == "SQR_BRUSH") *currentOption = SQR_BRUSH;

                    if(currentBtn->text == "REC_FILL") *currentOption = REC_FILL;
                    
                    if(currentBtn->text == "REC_LINES") *currentOption = REC_LINES;

                    if(currentBtn->text == "CIRC_FILL") *currentOption = CIRC_FILL;

                    if(currentBtn->text == "CIRC_LINES") *currentOption = CIRC_LINES;
                }

                if(currentBtn->text == GetOptionsText(*currentOption) || currentBtn->hovering)
                {
                    currentBtn->currentColor = currentBtn->accentColor;
                }
                else
                {
                    currentBtn->currentColor = currentBtn->color;
                }

                if(currentBtn->hovering)
                {
                    dragging = false;
                    drawing = false;
                    drawingShape = false;

                    continue;
                }
            }
        }
    }
}

void DrawPanel(Panel *panel)
{
    DrawRectangleRec(panel->panelRec, DARKGRAY);

    for(int i = 0; i < MAX_COLORS; i++)
    {
        DrawRectangleRec(GetColorRectangle(panel, i), panel->colors[i]);
    }

    DrawButton(&panel->saveBtn);

    for(int i = 0; i < MAX_DROP_DOWN_MENUS; i++)
    {
        DrawDropDownMenu(&panel->dropdownMenus[i]);
    }

    DrawRectangleLinesEx(panel->panelRec, outlineRecThickness, BLACK);
}
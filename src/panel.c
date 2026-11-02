#include "panel.h"

#include <stdio.h>

Panel panel;

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

    panel->maxColors = 8;

    panel->colors[0] = BLACK;
    panel->colors[1] = RAYWHITE;
    panel->colors[2] = RED;
    panel->colors[3] = GREEN;
    panel->colors[4] = BLUE;
    panel->colors[5] = YELLOW;
    panel->colors[6] = MAGENTA;
    panel->colors[7] = SKYBLUE;

    //drop down menus

    Options testMenuOptions[panel->testMenu.maxOptions];

    for(int i = 0; i < panel->testMenu.maxOptions; i++)
    {
        testMenuOptions[i] = NONE;
    }

    testMenuOptions[0] = CIRC_BRUSH;
    testMenuOptions[1] = SQR_BRUSH;

    InitiDropDownMenu(
        &panel->testMenu,
        "Brush type",
        100.0f,
        0.0f,
        150.0f,
        panel->panelRec.height,
        panel->panelRec.height,
        testMenuOptions
    );
}

void UpdatePanel(Panel *panel, Color* brushColor, Texture2D texture)
{
    UpdateButton(&panel->saveBtn, NULL);

    Vector2 mousePos = GetMousePosition();

    for(int i = 0; i < panel->maxColors; i++)
    {
        Rectangle colorRec = GetColorRectangle(panel, i);

        if(CheckCollisionPointRec(mousePos, colorRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            brushColor = &panel->colors[i];

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

    UpdateDropDownMenu(&panel->testMenu);
}

void DrawPanel(Panel *panel)
{
    DrawRectangleRec(panel->panelRec, DARKGRAY);

    for(int i = 0; i < panel->maxColors; i++)
    {
        DrawRectangleRec(GetColorRectangle(panel, i), panel->colors[i]);
    }

    DrawButton(&panel->saveBtn);

    DrawDropDownMenu(&panel->testMenu);

    DrawRectangleLinesEx(panel->panelRec, outlineRecThickness, BLACK);
}
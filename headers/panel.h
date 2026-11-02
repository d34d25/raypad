#ifndef PANEL_H
#define PANEL_H

#include "dropdown.h"

extern const float outlineRecThickness;

typedef struct Panel
{
    DropDownMenu testMenu;

    Color colors[8];

    MenuButton saveBtn;

    Rectangle panelRec;

    Color panelColor;

    int maxColors;

} Panel;

extern Panel panel;

void InitPanel(Panel* panel);

void UpdatePanel(Panel* panel, Color* brushColor, Texture2D texture);

void DrawPanel(Panel* panel);

inline Rectangle GetColorRectangle(Panel* panel, int index)
{
    float recSize = panel->panelRec.height;

    Rectangle rec;

    rec.x = panel->panelRec.width - panel->maxColors * panel->panelRec.height + index * recSize;
    rec.y = 0.0f;
    rec.width = recSize;
    rec.height = recSize;

    return rec;
}

#endif
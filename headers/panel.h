#ifndef PANEL_H
#define PANEL_H

#include "dropdown.h"

extern const float outlineRecThickness;

#define MAX_COLORS 8

#define MAX_DROP_DOWN_MENUS 2

typedef struct Panel
{
    DropDownMenu dropdownMenus[MAX_DROP_DOWN_MENUS];

    Color colors[MAX_COLORS];

    MenuButton saveBtn;

    Rectangle panelRec;

    Color panelColor;

} Panel;

void InitPanel(Panel* panel);

void UpdatePanel(Panel* panel, Color* brushColor, Texture2D texture);

void DrawPanel(Panel* panel);

inline Rectangle GetColorRectangle(Panel* panel, int index)
{
    float recSize = panel->panelRec.height;

    Rectangle rec;

    rec.x = panel->panelRec.width - MAX_COLORS * panel->panelRec.height + index * recSize;
    rec.y = 0.0f;
    rec.width = recSize;
    rec.height = recSize;

    return rec;
}

#endif
#ifndef DROPDOWN_H
#define DROPDOWN_H

#include "button.h"

#define MAX_OPTIONS 5

typedef enum Options
{
    NONE,

    CIRC_BRUSH,
    SQR_BRUSH,
    
    CIRC_FILL,
    CIRC_LINES,

    REC_FILL,
    REC_LINES,

    DRAW_LINE

} Options;

inline const char* GetOptionsText(Options option)
{
    switch (option)
    {
    case CIRC_BRUSH: return "CIRC_BRUSH";
    case SQR_BRUSH: return "SQR_BRUSH";

    case CIRC_FILL: return "CIRC_FILL";
    case CIRC_LINES: return "CIRC_LINES";

    case REC_FILL: return "REC_FILL";
    case REC_LINES: return "REC_LINES";

    case DRAW_LINE: return "DRAW_LINE";
    
    default: return "NULL";
    }
}

typedef struct DropDownMenu
{
    MenuButton optionsBtns[MAX_OPTIONS];

    Options options[MAX_OPTIONS];

    MenuButton menuButton;

    float height;

    bool deployed;

} DropDownMenu;

void InitiDropDownMenu(
    DropDownMenu* dropdown,
    const char* text,
    float x,
    float y,
    float width,
    float height,
    float optionHeight,
    float fontSize,
    Options options[5]
);

void UpdateDropDownMenu(DropDownMenu* dropdown);

void DrawDropDownMenu(DropDownMenu* dropdown);

#endif
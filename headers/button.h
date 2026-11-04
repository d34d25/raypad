#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

typedef struct MenuButton
{
    Color color;

    Color accentColor;

    Color currentColor;

    Rectangle button;

    const char* text;

    int fontSize;
    
    bool hovering;

    bool clicked;

} MenuButton;

void InitButton(
    MenuButton* button,
    int x, int y,
    int width,
    int height,
    const char* text,
    int fontSize,
    Color color
);

void UpdateButton(MenuButton* button, Camera2D* camera);

void DrawButton(MenuButton* button);

#endif
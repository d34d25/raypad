#include "button.h"

void InitButton(
    MenuButton *button,
    int x, int y,
    int width,
    int height,
    const char *text,
    int fontSize,
    Color color
)
{
    button->clicked = false;
    button->hovering = false;

    button->button.x = x;
    button->button.y = y;

    button->button.width = width;
    button->button.height = height;

    button->text = text;
    button->fontSize = fontSize;

    button->color = color;

    button->accentColor = DARKBLUE;

    button->currentColor = color;
}

void UpdateButton(MenuButton *button, Camera2D* camera)
{
    Vector2 mousePos = GetMousePosition();

    if(camera) mousePos = GetScreenToWorld2D(mousePos, *camera);
    
    button->hovering = CheckCollisionPointRec(mousePos, button->button);

    button->clicked = button->hovering && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void DrawButton(MenuButton *button)
{
    DrawRectangleRec(button->button, button->currentColor);

    DrawText(
        button->text,
        button->button.x + button->button.width * 0.15f,
        button->button.y + button->button.height * 0.15f,
        button->fontSize,
        RAYWHITE
    );

    DrawRectangleLinesEx(button->button, 2.0f, BLACK);
}
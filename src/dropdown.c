#include "dropdown.h"

#include <stdio.h>

void InitiDropDownMenu(
    DropDownMenu *dropdown,
    const char *text,
    float x,
    float y,
    float width,
    float height,
    float optionHeight,
    float fontSize,
    Options options[MAX_OPTIONS]
)
{
    dropdown->menuButton.text = text;

    dropdown->deployed = false;

    dropdown->height = MAX_OPTIONS * optionHeight;

    InitButton(
        &dropdown->menuButton,
        x, y, width, height,
        text, fontSize, DARKGRAY
    );

    for(int i = 1; i < MAX_OPTIONS + 1; i++)
    {
        dropdown->options[i - 1] = options[i - 1];

        MenuButton* optionBtn = &dropdown->optionsBtns[i - 1];

        InitButton(
            optionBtn,
            x, i * optionHeight,
            width, optionHeight,
            GetOptionsText(dropdown->options[i - 1]), //maps option to text
            fontSize,
            DARKGRAY
        );
    }
}

void UpdateDropDownMenu(DropDownMenu *dropdown)
{
    Vector2 mousePos = GetMousePosition();

    UpdateButton(&dropdown->menuButton, NULL);

    if(dropdown->menuButton.clicked) dropdown->deployed = !dropdown->deployed;

    if(dropdown->deployed)
    {
        for(int i = 0; i < MAX_OPTIONS; i++)
        {
            MenuButton* optionBtn = &dropdown->optionsBtns[i];

            if(optionBtn->text == "NONE") continue;

            UpdateButton(optionBtn, NULL);
        }
    }
    
}

void DrawDropDownMenu(DropDownMenu *dropdown)
{
    if(dropdown->deployed)
    {
        for(int i = 0; i < MAX_OPTIONS; i++)
        {
            MenuButton* optionBtn = &dropdown->optionsBtns[i];

            UpdateButton(optionBtn, NULL);

            if(optionBtn->text == "NONE") continue;

            DrawButton(optionBtn);
        }
    }

    DrawButton(&dropdown->menuButton);
}
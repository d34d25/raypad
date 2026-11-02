#include "canvas.h"

#include <stdio.h>

#include "raymath.h"

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

const float outlineRecThickness = 2.0f;

Canvas canvas;

Panel panel;

//panel

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
    DrawRectangleRec(button->button, button->color);

    DrawText(
        button->text,
        button->button.x + button->button.width * 0.15f,
        button->button.y + button->button.height * 0.15f,
        button->fontSize,
        RAYWHITE
    );

    DrawRectangleLinesEx(button->button, 2.0f, BLACK);
}

void InitiDropDownMenu(
    DropDownMenu *dropdown,
    const char *text,
    float x,
    float y,
    float width,
    float height,
    float optionHeight,
    Options options[5]
)
{
    dropdown->menuButton.text = text;

    dropdown->deployed = false;

    dropdown->maxOptions = 5;

    dropdown->height = dropdown->maxOptions * optionHeight;

    InitButton(
        &dropdown->menuButton,
        x, y, width, height,
        "Brush type", 20, DARKBLUE
    );

    for(int i = 1; i < dropdown->maxOptions + 1; i++)
    {
        dropdown->options[i - 1] = options[i - 1];

        MenuButton* optionBtn = &dropdown->optionsBtns[i - 1];

        InitButton(
            optionBtn,
            x, i * optionHeight,
            width, optionHeight,
            GetOptionsText(dropdown->options[i - 1]), //maps option to text
            20,
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
        for(int i = 0; i < dropdown->maxOptions; i++)
        {
            MenuButton* optionBtn = &dropdown->optionsBtns[i];

            UpdateButton(optionBtn, NULL);

            if(optionBtn->text == "NULL") continue;

            if(optionBtn->clicked)
            {
                
            }
        }
    }
    
}

void DrawDropDownMenu(DropDownMenu *dropdown)
{
    if(dropdown->deployed)
    {
        for(int i = 0; i < dropdown->maxOptions; i++)
        {
            MenuButton* optionBtn = &dropdown->optionsBtns[i];

            UpdateButton(optionBtn, NULL);

            if(optionBtn->text == "NULL") continue;

            DrawButton(optionBtn);
        }
    }

    DrawButton(&dropdown->menuButton);
}

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

void UpdatePanel(Panel *panel, Canvas* canvas)
{
    UpdateButton(&panel->saveBtn, NULL);

    Vector2 mousePos = GetMousePosition();

    for(int i = 0; i < panel->maxColors; i++)
    {
        Rectangle colorRec = GetColorRectangle(panel, i);

        if(CheckCollisionPointRec(mousePos, colorRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            canvas->currentBrushColor = panel->colors[i];

            break;
        }
    }

    if(panel->saveBtn.clicked)
    {
        Image exportImage = LoadImageFromTexture(canvas->renderTexture.texture);
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

//canvas

void InitCanvas(int width, int height, Canvas *canvas)
{
    canvas->canvasWidth = width;

    canvas->canvasHeight = height;

    canvas->mousePosition = GetMousePosition();

    canvas->previousMousePosition = canvas->mousePosition;

    canvas->camera.rotation = 0.0f;

    canvas->camera.zoom = 1.0f;

    canvas->camera.offset = (Vector2){width * 0.5f, height * 0.5f};

    canvas->camera.target = (Vector2){width * 0.5f, height * 0.5f};

    canvas->sourceRec.x = 0.0f;
    canvas->sourceRec.y = 0.0f;

    canvas->sourceRec.width = width;
    canvas->sourceRec.height = -height;

    canvas->drawing = false;

    canvas->erasing = false;

    canvas->clearingBg = false;

    canvas->showGrid = false;

    canvas->squareBrush = false;

    canvas->backgroundColor = RAYWHITE;

    canvas->brushColor = BLACK;

    canvas->currentBrushColor = canvas->brushColor;

    canvas->brushSize = 8.0f;

    canvas->eraserSize = 12.0f;

    canvas->renderTexture = LoadRenderTexture(width, height);

    canvas->overlay = LoadRenderTexture(width, height);

    ClearCanvas(canvas);
}

void UpdateCanvas(Canvas* canvas, Panel* panel)
{
    canvas->previousMousePosition = canvas->mousePosition;

    canvas->mousePosition = GetMousePosition();

    canvas->mousePosition = GetScreenToWorld2D(canvas->mousePosition, canvas->camera);

    Vector2 screenMousePos = GetWorldToScreen2D(canvas->mousePosition, canvas->camera); 

    if(CheckCollisionPointRec(screenMousePos, panel->panelRec))
    {
        canvas->dragging = false;
        canvas->drawing = false;
    
        return;
    }

    if(panel->testMenu.deployed)
    {
        for(int i = 0; i < panel->testMenu.maxOptions; i++)
        {
            MenuButton* optionBtn = &panel->testMenu.optionsBtns[i];

            if(optionBtn->text == "NULL") continue;

            if(optionBtn->clicked)
            {
                if(optionBtn->text == "CIRC_BRUSH") canvas->squareBrush = false;

                if(optionBtn->text == "SQR_BRUSH") canvas->squareBrush = true;
            }

            if(optionBtn->hovering)
            {
                canvas->dragging = false;
                canvas->drawing = false;

                return;
            }
        }
    }

    float mouseWheel = GetMouseWheelMove();

    int modKey = KEY_LEFT_CONTROL;

    int clearKey = KEY_C;
    int eraserKey = KEY_E;

    int brushSizeUpKey = KEY_KP_ADD;
    int brushSizeDownKey = KEY_KP_SUBTRACT;

    int drawButton = MOUSE_BUTTON_LEFT;

    int showGridKey = KEY_G;

    bool modKeyDown = IsKeyDown(modKey);

    bool inceasingBrushSize = IsKeyDown(brushSizeUpKey);
    bool decreasingBrushSize = IsKeyDown(brushSizeDownKey);

    canvas->drawing = IsMouseButtonDown(drawButton) && !modKeyDown;

    if(IsKeyPressed(eraserKey)) canvas->erasing = !canvas->erasing;

    if(IsKeyPressed(showGridKey)) canvas->showGrid = !canvas->showGrid;

    if(modKeyDown && IsKeyPressed(clearKey)) canvas->clearingBg = true;

    canvas->dragging = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);

    if(canvas->erasing) canvas->brushColor = canvas->backgroundColor;
    else canvas->brushColor = canvas->currentBrushColor;

    if(canvas->dragging)
    {
        Vector2 mouseDelta = GetMouseDelta();

        canvas->camera.offset.x += mouseDelta.x;
        canvas->camera.offset.y += mouseDelta.y;
    }

    if(mouseWheel != 0)
    {
        float cameraStep = 0.075f;

        if(mouseWheel > 0) canvas->camera.zoom += cameraStep;
        else canvas->camera.zoom -= cameraStep;

        canvas->camera.zoom = Clamp(canvas->camera.zoom, cameraStep, 100.0f);
    }

    float brushStep = 1.0f;

    if(canvas->erasing)
    {
        if(inceasingBrushSize) canvas->eraserSize += brushStep;
        else if(decreasingBrushSize) canvas->eraserSize -= brushStep;
    }
    else
    {
        if(inceasingBrushSize) canvas->brushSize += brushStep;
        else if(decreasingBrushSize) canvas->brushSize -= brushStep;
    }

    canvas->brushSize = Clamp(canvas->brushSize, 1.0f, 100.0f);

    canvas->eraserSize = Clamp(canvas->eraserSize, 1.0f, 100.0f);
}

void DrawCanvas(Canvas* canvas)
{
    BeginTextureMode(canvas->overlay);

        ClearBackground(BLANK);

        if(canvas->showGrid) DrawGrid2D(canvas);

        if(!canvas->squareBrush)
        {
            if(canvas->erasing) DrawCircleLinesV(canvas->mousePosition, canvas->eraserSize * 0.5f, BLACK);
            else DrawCircleV(canvas->mousePosition, canvas->brushSize * 0.5f, canvas->brushColor);
        }
        else
        {
            if(canvas->erasing) DrawRectangleLines(
                canvas->mousePosition.x - canvas->eraserSize * 0.5f,
                canvas->mousePosition.y - canvas->eraserSize * 0.5f,
                canvas->eraserSize, canvas->eraserSize, BLACK
            );

            else DrawRectangle(
                canvas->mousePosition.x - canvas->brushSize * 0.5f,
                canvas->mousePosition.y - canvas->brushSize * 0.5f,
                canvas->brushSize, canvas->brushSize, canvas->brushColor
            );
        }

    EndTextureMode();

    BeginTextureMode(canvas->renderTexture);

        if(canvas->drawing)
        {
            float size;

            if(canvas->erasing) size = canvas->eraserSize;
            else size = canvas->brushSize;

            DrawLineEx(canvas->previousMousePosition, canvas->mousePosition, size, canvas->brushColor);

            if(!canvas->squareBrush)
            {
                DrawCircleV(canvas->mousePosition, size * 0.5f, canvas->brushColor);
            }
            else
            {
                DrawRectangle(
                    canvas->mousePosition.x - size * 0.5f,
                    canvas->mousePosition.y - size * 0.5f,
                    canvas->brushSize, size, canvas->brushColor
                );
            }
        }
    
    EndTextureMode();

    if(canvas->clearingBg)
    {
        ClearCanvas(canvas);
        canvas->clearingBg = false;
    }

    ClearBackground(GRAY);

    BeginMode2D(canvas->camera);

    DrawFinderLine(canvas);

    DrawTextureRec(canvas->renderTexture.texture, canvas->sourceRec, (Vector2){0.0f,0.0f}, WHITE);

    DrawTextureRec(canvas->overlay.texture, canvas->sourceRec, (Vector2){0.0f,0.0f}, WHITE);

    EndMode2D();
}

void UnloadCanvas(Canvas *canvas)
{
    UnloadRenderTexture(canvas->overlay);
    UnloadRenderTexture(canvas->renderTexture);
}

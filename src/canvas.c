#include "canvas.h"

#include <stdio.h>

#include "raymath.h"

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

Canvas canvas;

Panel panel;

//panel

void InitPanel(Panel *panel)
{
    panel->panelRec.x = 0.0f;

    panel->panelRec.y = 0.0f;

    panel->panelRec.width = GetScreenWidth();

    panel->panelRec.height = GetScreenHeight() * 0.05f;

    panel->saveRec.x = 0.0f;
    
    panel->saveRec.y = 0.0f;

    panel->saveRec.width = 100.0f;
    panel->saveRec.height = panel->panelRec.height;

    panel->maxColors = 8;

    panel->colors[0] = BLACK;
    panel->colors[1] = RAYWHITE;
    panel->colors[2] = RED;
    panel->colors[3] = GREEN;
    panel->colors[4] = BLUE;
    panel->colors[5] = YELLOW;
    panel->colors[6] = MAGENTA;
    panel->colors[7] = SKYBLUE;
}

void UpdatePanel(Panel *panel, Canvas* canvas)
{
    Vector2 mousePos = GetMousePosition();

    for(int i = 0; i < panel->maxColors; i++)
    {
        Rectangle colorRec = GetColorRectangle(panel, i);

        if(CheckCollisionPointRec(mousePos, colorRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            printf("color changed \n");

            printf("color: \n");
            printf("%i", (int)panel->colors[i].r);
            printf("%i", (int)panel->colors[i].g);
            printf("%i\n", (int)panel->colors[i].b);

            canvas->currentBrushColor = panel->colors[i];

            break;
        }
    }

    if(CheckCollisionPointRec(mousePos, panel->saveRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Image exportImage = LoadImageFromTexture(canvas->renderTexture.texture);
        ImageFlipVertical(&exportImage);
        
        ExportImage(exportImage, "testExport.png");

        UnloadImage(exportImage);
    }
}

void DrawPanel(Panel *panel)
{
    DrawRectangleRec(panel->panelRec, DARKGRAY);

    for(int i = 0; i < panel->maxColors; i++)
    {
        DrawRectangleRec(GetColorRectangle(panel, i), panel->colors[i]);
    }

    DrawRectangleRec(panel->saveRec, BLACK);

    const char* saveText = "Save";

    int saveTextLength = (int)TextLength(saveText);

    DrawText(
        saveText,
        panel->saveRec.x + panel->saveRec.width * 0.15f,
        panel->saveRec.y + panel->saveRec.height * 0.15f,
        30,
        RAYWHITE
    );

    DrawRectangleLinesEx(panel->panelRec, 2.0f, BLACK);
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

    if(CheckCollisionPointRec(GetWorldToScreen2D(canvas->mousePosition, canvas->camera), panel->panelRec))
    {
        canvas->dragging = false;
        canvas->drawing = false;
        
        return;
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

        if(canvas->erasing) DrawCircleLinesV(canvas->mousePosition, canvas->eraserSize * 0.5f, BLACK);
        else DrawCircleV(canvas->mousePosition, canvas->brushSize * 0.5f, canvas->brushColor);

    EndTextureMode();

    BeginTextureMode(canvas->renderTexture);

        if(canvas->drawing)
        {
            float size;

            if(canvas->erasing) size = canvas->eraserSize;
            else size = canvas->brushSize;

            DrawLineEx(canvas->previousMousePosition, canvas->mousePosition, size, canvas->brushColor);
            DrawCircleV(canvas->mousePosition, size * 0.5f, canvas->brushColor);
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

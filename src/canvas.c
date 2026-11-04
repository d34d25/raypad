#include "canvas.h"

#include <stdio.h>

#include "raymath.h"

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

void InitCanvas(int width, int height, Canvas *canvas)
{
    canvas->canvasWidth = width;

    canvas->canvasHeight = height;

    canvas->mousePosition = GetMousePosition();

    canvas->previousMousePosition = canvas->mousePosition;

    canvas->initialMousePosition = (Vector2){0.0f,0.0f};

    canvas->camera.rotation = 0.0f;

    canvas->camera.zoom = 1.0f;

    canvas->camera.offset = (Vector2){width * 0.5f, height * 0.5f};

    canvas->camera.target = (Vector2){width * 0.5f, height * 0.5f};

    canvas->sourceRec.x = 0.0f;
    canvas->sourceRec.y = 0.0f;

    canvas->sourceRec.width = width;
    canvas->sourceRec.height = -height;

    canvas->currentOption = CIRC_BRUSH;

    canvas->mouseInPanel = false;

    canvas->drawingShape = false;

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

    canvas->tempFillRec.x = -100;
    canvas->tempFillRec.y = -100;

    canvas->tempFillRec.width = 10;
    canvas->tempFillRec.height = 10;
}

void UpdateCanvas(Canvas* canvas, Panel* panel)
{
    canvas->previousMousePosition = canvas->mousePosition;

    canvas->mousePosition = GetMousePosition();

    canvas->mousePosition = GetScreenToWorld2D(canvas->mousePosition, canvas->camera);

    Vector2 screenMousePos = GetWorldToScreen2D(canvas->mousePosition, canvas->camera); 

    canvas->mouseInPanel = CheckCollisionPointRec(screenMousePos, panel->panelRec);

    canvas->drawingShape = canvas->currentOption == REC_FILL || 
    canvas->currentOption == REC_LINES || canvas->currentOption == CIRC_FILL || 
    canvas->currentOption == CIRC_LINES;

    float mouseWheel = GetMouseWheelMove();

    bool modKeyDown = IsKeyDown(KEY_LEFT_CONTROL);

    bool inceasingBrushSize = IsKeyDown(KEY_KP_ADD);
    bool decreasingBrushSize = IsKeyDown(KEY_KP_SUBTRACT);

    if(IsKeyPressed(KEY_E)) canvas->erasing = !canvas->erasing;

    if(IsKeyPressed(KEY_G)) canvas->showGrid = !canvas->showGrid;

    if(modKeyDown && IsKeyPressed(KEY_C)) canvas->clearingBg = true;

    canvas->drawing = IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !modKeyDown && !canvas->drawingShape;

    canvas->dragging = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);

    if(canvas->mouseInPanel)
    {
        canvas->dragging = false;
        canvas->drawing = false;
        canvas->drawingShape = false;
    
        return;
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
                    if(currentBtn->text == "CIRC_BRUSH") canvas->currentOption = CIRC_BRUSH;

                    if(currentBtn->text == "SQR_BRUSH") canvas->currentOption = SQR_BRUSH;

                    if(currentBtn->text == "REC_FILL") canvas->currentOption = REC_FILL;
                    
                    if(currentBtn->text == "REC_LINES") canvas->currentOption = REC_LINES;

                    if(currentBtn->text == "CIRC_FILL") canvas->currentOption = CIRC_FILL;

                    if(currentBtn->text == "CIRC_LINES") canvas->currentOption = CIRC_LINES;
                }

                if(currentBtn->text == GetOptionsText(canvas->currentOption) || currentBtn->hovering)
                {
                    currentBtn->currentColor = currentBtn->accentColor;
                }
                else
                {
                    currentBtn->currentColor = currentBtn->color;
                }

                if(currentBtn->hovering)
                {
                    canvas->dragging = false;
                    canvas->drawing = false;
                    canvas->drawingShape = false;

                    continue;
                }
            }
        }
    }

    if(canvas->erasing) canvas->brushColor = canvas->backgroundColor;
    else canvas->brushColor = canvas->currentBrushColor;

    //camera drag

    if(canvas->dragging)
    {
        Vector2 mouseDelta = GetMouseDelta();

        canvas->camera.offset.x += mouseDelta.x;
        canvas->camera.offset.y += mouseDelta.y;
    }

    //camera zoom

    if(mouseWheel != 0)
    {
        float cameraStep = 0.075f;

        if(mouseWheel > 0) canvas->camera.zoom += cameraStep;
        else canvas->camera.zoom -= cameraStep;

        canvas->camera.zoom = Clamp(canvas->camera.zoom, cameraStep, 100.0f);
    }

    //brush size

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

    //fill rec

    switch (canvas->currentOption)
    {
    case REC_FILL:
    case REC_LINES:
    {
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            canvas->initialMousePosition = canvas->mousePosition;
        }

        Vector2 tempRecSize = Vector2Subtract(canvas->mousePosition, canvas->initialMousePosition);

        canvas->tempFillRec.x = fminf(canvas->initialMousePosition.x, canvas->mousePosition.x);
        canvas->tempFillRec.y = fminf(canvas->initialMousePosition.y, canvas->mousePosition.y);

        canvas->tempFillRec.width = fabsf(tempRecSize.x);
        canvas->tempFillRec.height = fabsf(tempRecSize.y);
    }
    break;
    
    default: break;
    }
}

void DrawCanvas(Canvas* canvas)
{
    BeginTextureMode(canvas->overlay);

        ClearBackground(BLANK);

        if(canvas->showGrid) DrawGrid2D(canvas);

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && canvas->drawingShape)
        {
            switch (canvas->currentOption)
            {
            case REC_FILL: DrawRectangleRec(canvas->tempFillRec, canvas->brushColor); break;
            
            case REC_LINES: DrawRectangleLinesEx(canvas->tempFillRec, canvas->brushSize, canvas->brushColor); break;

            default: break;
            }
        }
        else
        {
            if(canvas->currentOption == CIRC_BRUSH)
            {
                if(canvas->erasing) DrawCircleLinesV(canvas->mousePosition, canvas->eraserSize * 0.5f, BLACK);
                else DrawCircleV(canvas->mousePosition, canvas->brushSize * 0.5f, canvas->brushColor);
            }
            else if(canvas->currentOption == SQR_BRUSH)
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
        }

    EndTextureMode();

    BeginTextureMode(canvas->renderTexture);

        if(canvas->drawing)
        {
            float size;

            if(canvas->erasing) size = canvas->eraserSize;
            else size = canvas->brushSize;
            
            DrawLineEx(canvas->previousMousePosition, canvas->mousePosition, size, canvas->brushColor);

            if(canvas->currentOption == CIRC_BRUSH)
            {
                DrawCircleV(canvas->mousePosition, size * 0.5f, canvas->brushColor);
            }
            else if(canvas->currentOption == SQR_BRUSH)
            {
                DrawRectangle(
                    canvas->mousePosition.x - size * 0.5f,
                    canvas->mousePosition.y - size * 0.5f,
                    canvas->brushSize, size, canvas->brushColor
                );
            }
        }
        else if(canvas->drawingShape)
        {
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                switch (canvas->currentOption)
                {
                case REC_FILL: DrawRectangleRec(canvas->tempFillRec, canvas->brushColor); break;
                
                case REC_LINES: DrawRectangleLinesEx(canvas->tempFillRec, canvas->brushSize, canvas->brushColor); break;

                default: break;
                }
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

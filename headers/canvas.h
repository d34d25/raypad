#ifndef CANVAS_H
#define CANVAS_H

#include "raylib.h"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

typedef struct Canvas
{
    RenderTexture2D renderTexture;

    RenderTexture2D overlay;

    Camera2D camera;

    Rectangle sourceRec;

    Color backgroundColor;

    Color currentBrushColor;

    Color brushColor;

    Vector2 mousePosition;

    Vector2 previousMousePosition;

    float brushSize;

    float eraserSize;
    
    int canvasWidth;

    int canvasHeight;

    bool drawing;

    bool erasing;

    bool dragging;

    bool clearingBg;

    bool showGrid;

} Canvas;

typedef struct Panel
{
    Color colors[8];

    Rectangle panelRec;

    Color panelColor;

    int maxColors;

} Panel;


//UI

extern Panel panel;

void InitPanel(Panel* panel);

void UpdatePanel(Panel* panel, Canvas* canvas);

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

//canvas

extern Canvas canvas;

void InitCanvas(int width, int height, Canvas* canvas);

void UpdateCanvas(Canvas* canvas, Panel* panel);

void DrawCanvas(Canvas* canvas);

void UnloadCanvas(Canvas* canvas);

inline void ClearCanvas(Canvas* canvas)
{
    BeginTextureMode(canvas->renderTexture);

    ClearBackground(canvas->backgroundColor);

    EndTextureMode();
}

inline void DrawGrid2D(Canvas* canvas)
{
    int spacing = 20;

    float gridThickness = 1.0f / canvas->camera.zoom;

    if(gridThickness <= 1.0f) gridThickness = 1.0f;

    for(int i = 0; i < canvas->canvasWidth; i+= spacing)
    {
        DrawLineEx((Vector2){i, canvas->canvasHeight}, (Vector2){i, 0}, gridThickness, GRAY);
    }

    for(int i = 0; i < canvas->canvasHeight; i+= spacing)
    {
        DrawLineEx((Vector2){canvas->canvasWidth, i}, (Vector2){0, i}, gridThickness, GRAY);
    }
}

inline void DrawFinderLine(Canvas* canvas)
{
    float baseLineThickness = 3.0f;

    float lineThickness = baseLineThickness / canvas->camera.zoom;

    if(lineThickness <= baseLineThickness) lineThickness = baseLineThickness;

    DrawLineEx(canvas->mousePosition, (Vector2){canvas->canvasWidth * 0.5f, canvas->canvasHeight * 0.5f}, lineThickness, RED);
}


#endif
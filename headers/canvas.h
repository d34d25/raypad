#ifndef CANVAS_H
#define CANVAS_H

#include "panel.h"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

#define STAR_VERTICES_COUNT 10

typedef struct Elipse
{
    Vector2 position;
    Vector2 size;
} Elipse;

typedef struct Star
{
    Vector2 transformedVertices[STAR_VERTICES_COUNT];
    Vector2 vertices[STAR_VERTICES_COUNT];
    Vector2 position;
    float size;
} Star;

typedef struct Canvas
{
    RenderTexture2D renderTexture;

    RenderTexture2D overlay;

    Camera2D camera;

    Star tempStar;

    Rectangle tempFillRec;

    Rectangle sourceRec;

    Elipse tempElipse;

    Color backgroundColor;

    Color currentBrushColor;

    Color brushColor;

    Vector2 mousePosition;

    Vector2 initialMousePosition;

    Vector2 previousMousePosition; 

    float brushSize;

    float eraserSize;

    Options currentOption;
    
    int canvasWidth;

    int canvasHeight;

    bool drawingShape;

    bool mouseInPanel;

    bool drawing;

    bool erasing;

    bool dragging;

    bool clearingBg;

    bool showGrid;

} Canvas;

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

inline void DrawCircleLinesThickness(Vector2 center, float radius, float thickness, Color color)
{
    DrawRing(center, radius - thickness, radius, 0.0f, 360.0f, 36, color);
}

inline void DrawStar(Star star, float thickness, Color color)
{
    for(int i = 0; i < STAR_VERTICES_COUNT; i++)
    {
        int nextI = i + 1;

        if(nextI >= STAR_VERTICES_COUNT) nextI = 0;

        DrawLineEx(star.transformedVertices[i], star.transformedVertices[nextI], thickness, color);
    }
}

#endif
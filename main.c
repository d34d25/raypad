#include <stdio.h>

#include "canvas.h"

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raypad");

    SetTargetFPS(144);

    InitPanel(&panel);

    InitCanvas(1400, 1900, &canvas);

    while (!WindowShouldClose())
    {
        UpdatePanel(&panel, &canvas);

        UpdateCanvas(&canvas, &panel);

        BeginDrawing();

        ClearBackground(canvas.backgroundColor);

        DrawCanvas(&canvas);
        
        DrawPanel(&panel);

        EndDrawing();
    }

    UnloadCanvas(&canvas);

    CloseWindow();

    return 0;
};
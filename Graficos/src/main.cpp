#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define TAM 35

int operacion();

int main()
{
    srand(time(NULL));
    int res1, res2, band;
    int pos1 = 0, pos2 = 0, fin = TAM;
    int jugadorActual = 1;
    char respuesta[3] = ""; // Almacenar la respuesta del jugador
    int n1 = 0, n2 = 0;     // Números para la operación

    const int screenWidth = GetMonitorWidth(0);   // Ancho de pantalla completa
    const int screenHeight = GetMonitorHeight(0); // Alto de pantalla completa

    InitWindow(screenWidth, screenHeight, "Juego de Carrera");
    ToggleFullscreen(); // Alternar a pantalla completa

    SetTargetFPS(60);

    // Cargar la imagen de fondo
    Image background = LoadImage("board.png");
    Texture2D backgroundTexture = LoadTextureFromImage(background);
    UnloadImage(background);

    // Definir los botones numéricos
    Rectangle numButtons[6];
    for (int i = 0; i < 6; i++)
    {
        numButtons[i].x = 10 + i * 100;
        numButtons[i].y = screenHeight - 60;
        numButtons[i].width = 80;
        numButtons[i].height = 40;
    }

    while (!WindowShouldClose() && (pos1 < fin || pos2 < fin))
    {
        // Generar operación y números aleatorios solo si no se ha generado una operación previamente
        if (n1 == 0 && n2 == 0)
        {
            int oper = 1; // Solo suma en esta versión
            n1 = rand() % 6 + 1;
            n2 = rand() % 6 + 1;
            res1 = n1 + n2;
        }

        // Actualizar posición del jugador cuando se hace clic en un botón
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (pos1 < fin || pos2 < fin))
        {
            Vector2 mousePosition = GetMousePosition();
            for (int i = 0; i < 6; i++)
            {
                if (CheckCollisionPointRec(mousePosition, numButtons[i]))
                {
                    // Agregar el número presionado a la respuesta
                    if (strlen(respuesta) < 2)
                    {
                        respuesta[strlen(respuesta)] = '0' + i + 1;
                    }
                }
            }
        }

        // Cambiar al siguiente jugador
        if (IsKeyPressed(KEY_SPACE) && (pos1 < fin || pos2 < fin))
        {
            jugadorActual = (jugadorActual == 1) ? 2 : 1;
            respuesta[0] = '\0'; // Borrar la respuesta
        }

        // Dibuja la interfaz
        BeginDrawing();

        // Dibujar la imagen de fondo
        DrawTexture(backgroundTexture, 0, 0, RAYWHITE);

        Texture2D backgroundTexture = LoadTextureFromImage(background);
        if (backgroundTexture.id == 0)
        {
            // La carga de la imagen falló, muestra un mensaje de error o ajusta la ruta de la imagen.
            // También verifica que la imagen esté en el formato correcto.
        }

        // Muestra las posiciones de ambos jugadores en el centro de la ventana
        DrawText(TextFormat("Jugador 1: %d", pos1), screenWidth / 4, screenHeight / 4, 30, DARKGRAY);
        DrawText(TextFormat("Jugador 2: %d", pos2), screenWidth * 3 / 4, screenHeight / 4, 30, DARKGRAY);

        // Dibujar botones numéricos
        for (int i = 0; i < 6; i++)
        {
            DrawRectangleRec(numButtons[i], DARKGRAY);
            DrawText(TextFormat("%d", i + 1), numButtons[i].x + 20, numButtons[i].y + 10, 20, RAYWHITE);
        }

        // Mostrar campo de respuesta
        DrawText("Respuesta:", screenWidth / 2 - 70, screenHeight - 100, 20, DARKGRAY);
        DrawText(respuesta, screenWidth / 2 + 30, screenHeight - 100, 20, DARKGRAY);

        // Mostrar operación en una línea separada
        if (pos1 < fin && pos2 < fin)
        {
            DrawText("Operacion:", 10, screenHeight / 2, 20, DARKGRAY);
            char operacionStr[50];
            snprintf(operacionStr, sizeof(operacionStr), "%d + %d", n1, n2);
            DrawText(operacionStr, 10, screenHeight / 2 + 30, 20, DARKGRAY);
            DrawText("Presiona ESPACIO para avanzar al siguiente turno", 10, screenHeight / 2 + 60, 20, DARKGRAY);
        }

        EndDrawing();
    }

    UnloadTexture(backgroundTexture); // Liberar la textura de la imagen de fondo
    CloseWindow();

    return 0;
}

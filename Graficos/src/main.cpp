#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Algunas definiciones
//----------------------------------------------------------------------------------
#define SNAKE_LENGTH 256
#define SQUARE_SIZE 30

//----------------------------------------------------------------------------------
// Tipos y definiciones de estructuras
//----------------------------------------------------------------------------------
typedef struct Snake
{
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food
{
    Vector2 position;
    Vector2 size;
    bool active;
    char operation;
    Color color;
} Food;

//------------------------------------------------------------------------------------
// Declaraciones de variables globales
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;

static Food fruit = {0};
static Snake snake[SNAKE_LENGTH] = {0};
static Vector2 snakePosition[SNAKE_LENGTH] = {0};
static bool allowMove = false;
static Vector2 offset = {0};
static int counterTail = 0;
static float menuAlpha = 1.0f;
static bool menuFadeOut = false;
int selectedOption = 0;
bool fruitCollected = false;
int collected = 0;
char numberText[5];
int fruitsEaten = 0;
static int currentNumber = 0;
static bool numberSelected = false;
static int firstNumber = 0;
static int secondNumber = 0;

// Music backgroundMusic;

// Textura para el fondo del juego
static Texture2D backgroundGame;

// Textura para el fondo del menú
static Texture2D backgroundMenu;

// Estados del juego
typedef enum
{
    MENU,
    GAME,
    PAUSED
} GameState;

static GameState gameState = MENU;

//------------------------------------------------------------------------------------
// Declaraciones de funciones locales
//------------------------------------------------------------------------------------
static void InitGame(void);        // Inicializa el juego
static void UpdateGame(void);      // Actualiza el juego (un frame)
static void DrawGame(void);        // Dibuja el juego (un frame)
static void UnloadGame(void);      // Descarga recursos del juego
static void UpdateDrawFrame(void); // Actualiza y dibuja (un frame)
static void DrawMenu(void);        // Dibuja el menú
static void UpdateMenu(void);      // Actualiza el menú
void ResetGame(void);              // Reinicia el juego
void HandleCalculatorInput();      // Maneja la entrada de la calculadora
void DrawCalculatorButtons();      // Dibuja los botones de la calculadora
Font font;
//------------------------------------------------------------------------------------
// Punto de entrada principal del programa
//------------------------------------------------------------------------------------
int main(void)
{
    // Inicialización (windowTitle no se usa en Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Serpent Snake");

    // Cargar imágenes de fondo
    backgroundGame = LoadTexture("snakefondo2.png");
    backgroundMenu = LoadTexture("fondo.png");
    font = LoadFont("Valoon.ttf");
    // backgroundMusic = LoadMusicStream("C:\\Users\\nanoj\\OneDrive\\Escritorio\\musicafondo.mp3");

    // PlayAudioStream(backgroundMusic);

    InitGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Bucle principal del juego
    while (!WindowShouldClose()) // Detectar botón de cierre de la ventana o tecla ESC
    {
        // Actualizar y Dibujar
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }
#endif
    // Descarga de recursos
    //--------------------------------------------------------------------------------------
    UnloadGame(); // Descargar datos cargados (texturas, sonidos, modelos...)

    CloseWindow(); // Cierra la ventana y el contexto de OpenGL
    //--------------------------------------------------------------------------------------

    return 0;
}

// Inicializa las variables del juego
void InitGame(void)
{
    framesCounter = 0;
    gameOver = false;
    pause = false;

    counterTail = 1;
    allowMove = false;

    offset.x = screenWidth % SQUARE_SIZE;
    offset.y = screenHeight % SQUARE_SIZE;

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snake[i].position = (Vector2){offset.x / 2, offset.y / 2};
        snake[i].size = (Vector2){SQUARE_SIZE, SQUARE_SIZE};
        snake[i].speed = (Vector2){SQUARE_SIZE, 0};

        if (i == 0)
            snake[i].color = DARKBLUE;
        else
            snake[i].color = BLUE;
    }

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snakePosition[i] = (Vector2){0.0f, 0.0f};
    }

    fruit.size = (Vector2){SQUARE_SIZE, SQUARE_SIZE};
    fruit.color = SKYBLUE;
    fruit.active = false;
}

// Actualiza el juego (un frame)
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P'))
            pause = !pause;

        if (!pause)
        {
            // Control del jugador
            if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){SQUARE_SIZE, 0};
                allowMove = false;
            }
            if (IsKeyPressed(KEY_LEFT) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){-SQUARE_SIZE, 0};
                allowMove = false;
            }
            if (IsKeyPressed(KEY_UP) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){0, -SQUARE_SIZE};
                allowMove = false;
            }
            if (IsKeyPressed(KEY_DOWN) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){0, SQUARE_SIZE};
                allowMove = false;
            }

            // Movimiento de la serpiente
            for (int i = 0; i < counterTail; i++)
                snakePosition[i] = snake[i].position;

            if ((framesCounter % 14) == 0)
            {
                for (int i = 0; i < counterTail; i++)
                {
                    if (i == 0)
                    {
                        snake[0].position.x += snake[0].speed.x;
                        snake[0].position.y += snake[0].speed.y;
                        allowMove = true;
                    }
                    else
                        snake[i].position = snakePosition[i - 1];
                }
            }

            // Comportamiento de la pared
            if (((snake[0].position.x) > (screenWidth - offset.x)) ||
                ((snake[0].position.y) > (screenHeight - offset.y)) ||
                (snake[0].position.x < 0) || (snake[0].position.y < 0))
            {
                gameOver = true;
                ResetGame();
            }

            // Colisión con uno mismo
            for (int i = 1; i < counterTail; i++)
            {
                if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y))
                    gameOver = true;
            }

            // Cálculo de la posición de la fruta
            if (!fruit.active)
            {
                fruit.active = true;
                int randomNumber = GetRandomValue(1, 20);
                sprintf(numberText, "%d", randomNumber);
                fruit.position = (Vector2){GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2};

                if (collected == 2)
                {
                    // Genera frutas con operaciones
                    char operations[] = {'+', 'x'};
                    fruit.operation = operations[GetRandomValue(0, 1)];

                    // Muestra el mensaje con el signo aleatorio en pantalla
                    char operationText[30];
                    sprintf(operationText, "La operacion a resolver es una %c", fruit.operation);
                    DrawText(operationText, 10, 10, 20, RED);
                }
                else
                {
                    // Genera frutas con números
                    randomNumber = GetRandomValue(1, 20);
                    sprintf(numberText, "%d", randomNumber);
                    fruit.operation = ' '; // Espacio indica que es una fruta con número
                }
            }

            // Colisión
            if ((snake[0].position.x == fruit.position.x) && (snake[0].position.y == fruit.position.y))
            {
                snake[counterTail].position = snakePosition[counterTail - 1];
                counterTail += 1;
                fruit.active = false;
                fruitCollected = true;
                fruitsEaten += 1;

                if (fruitsEaten == 2)
                {
                    pause = true;
                    gameState = PAUSED; // Cambia el estado del juego a pausado
                }

                // Restablece el contador después de dos frutas comidas
                if (fruitsEaten == 2)
                {
                    fruitsEaten = 0;
                }

                // Aumentar el contador y asignar un nuevo signo cada dos frutas recolectadas
                if (fruitCollected && (counterTail % 2 == 0))
                {
                    char operations[] = {'+', 'x'};
                    fruit.operation = operations[GetRandomValue(0, 1)];

                    // Almacena los dos números comidos
                    firstNumber = atoi(numberText);
                    secondNumber = GetRandomValue(1, 20);
                }
            }
            framesCounter++;
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER) && !fruitCollected)
        {
            InitGame();
            gameOver = false;
            gameState = GAME; // Cambia al estado del juego después de presionar ENTER
        }
        else
        {
            fruitCollected = false; // Reinicia la variable para la próxima vez
        }
    }
}

// Dibuja el juego (un frame)
void DrawGame(void)
{
    char numberText[5];
    char operationText[30];
    Vector2 bkPosition = {0.0f, 0.0f};
    BeginDrawing();

    // Dibuja la imagen de fondo del juego
    DrawTextureEx(backgroundGame, bkPosition, 0, 0.62, RAYWHITE);

    if (!gameOver)
    {
        // Dibuja las líneas de la cuadrícula
        for (int i = 0; i < screenWidth / SQUARE_SIZE + 1; i++)
        {
            DrawLineV((Vector2){SQUARE_SIZE * i + offset.x / 2, offset.y / 2}, (Vector2){SQUARE_SIZE * i + offset.x / 2, screenHeight - offset.y / 2}, DARKGREEN);
        }

        for (int i = 0; i < screenHeight / SQUARE_SIZE + 1; i++)
        {
            DrawLineV((Vector2){offset.x / 2, SQUARE_SIZE * i + offset.y / 2}, (Vector2){screenWidth - offset.x / 2, SQUARE_SIZE * i + offset.y / 2}, DARKGREEN);
        }

        // Dibuja la serpiente
        for (int i = 0; i < counterTail; i++)
            DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);

        // Cálculo de la posición de la fruta
        if (!fruit.active)
        {
            fruit.active = true;
            int randomNumber = GetRandomValue(1, 20);
            sprintf(numberText, "%d", randomNumber);
            fruit.position = (Vector2){GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2};

            if (collected == 2)
            {
                // Genera frutas con operaciones
                char operations[] = {'+', 'x'};
                fruit.operation = operations[GetRandomValue(0, 1)];

                // Muestra el mensaje con el signo aleatorio en pantalla
                sprintf(operationText, "La operacion a resolver es una %c", fruit.operation);
                DrawText(operationText, screenWidth - 200, 10, 20, RED);
            }
            else
            {
                // Genera frutas con números
                randomNumber = GetRandomValue(1, 20);
                sprintf(numberText, "%d", randomNumber);
                fruit.operation = ' '; // Espacio indica que es una fruta con número
            }
            printf("Mensaje de operación: %s\n", operationText);
        }

        // Dibuja un cuadro azul en la posición de la fruta
        DrawRectangleV(fruit.position, fruit.size, BLUE);

        // Muestra el número sobre el cuadro azul
        DrawText(numberText, fruit.position.x + fruit.size.x / 2 - MeasureText(numberText, 20) / 2, fruit.position.y + fruit.size.y / 2 - 10, 20, RED);

        // Muestra el signo en la esquina superior derecha
        if (fruitCollected)
        {
            char operationText[2] = {fruit.operation, '\0'};
            DrawText(operationText, screenWidth - MeasureText(operationText, 20) - 10, 10, 20, RED);
        }

        if (pause)
            DrawText("PAUSA", screenWidth / 2 - MeasureText("PAUSA", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
    }
    else
        DrawText("PRESIONA [ENTER] PARA JUGAR DE NUEVO", GetScreenWidth() / 2 - MeasureText("PRESIONA [ENTER] PARA JUGAR DE NUEVO", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);

    EndDrawing();
}

// Descarga recursos del juego
void UnloadGame(void)
{
    // Descarga las texturas de fondo
    UnloadTexture(backgroundGame);
    UnloadTexture(backgroundMenu);
    // UnloadMusicStream(backgroundMusic);
    UnloadFont(font);
}

// Actualiza y dibuja (un frame)
void UpdateDrawFrame(void)
{
    if (gameState == MENU)
    {
        UpdateMenu();
        DrawMenu();
        if (menuAlpha == 0.0f)
        {
            if (selectedOption == 1)
            {
                // Inicia el juego después de que el menú se desvanece
                InitGame();
                gameOver = false;
                gameState = GAME;
            }
            else if (selectedOption == 2)
            {
                // Lógica para la opción 2 después de que el menú se desvanece
                // ... Agrega la lógica adicional aquí ...
            }
            else if (selectedOption == 3)
            {
                // Cierra la ventana después de que el menú se desvanece
                CloseWindow();
            }
        }
    }
    else if (gameState == GAME)
    {
        UpdateGame();
        DrawGame();

        // Verifica si el juego ha terminado y se ha presionado Enter
        if (gameOver && IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
            gameState = GAME;
            fruitCollected = false; // Asegúrate de reiniciar la variable para la próxima vez
            ResetGame();
        }
    }
    else if (gameState == PAUSED)
    {
        BeginDrawing();
        ClearBackground(BLACK);

        // Dibuja el mensaje de pausa en la nueva ventana
        DrawText("Juego en pausa", screenWidth / 2 - MeasureText("Juego en pausa", 40) / 2, screenHeight / 2 - 40, 40, GRAY);
        DrawText("Presiona [ENTER] para continuar", screenWidth / 2 - MeasureText("Presiona [ENTER] para continuar", 20) / 2, screenHeight / 2, 20, GRAY);
        DrawText(TextFormat("Números comidos: %d, %d", firstNumber, secondNumber), screenWidth / 2 - MeasureText("Números comidos: 00, 00", 20) / 2, screenHeight / 2 + 30, 20, GRAY);

        DrawCalculatorButtons();

        EndDrawing();

        if (IsKeyPressed(KEY_ENTER))
        {
            pause = false;
            gameState = GAME; // Vuelve al juego
        }

        // Maneja la entrada de la calculadora
        HandleCalculatorInput();
    }
}

// Dibuja el menú
void DrawMenu(void)
{
    Vector2 bkPosition = {0.0f, 0.0f};
    BeginDrawing();

    // Dibuja la imagen de fondo del menú con opacidad
    DrawTextureEx(backgroundMenu, bkPosition, 0, 0.85, Fade(RAYWHITE, menuAlpha));

    // Realiza conversiones explícitas de int a float para evitar warnings
    DrawTextEx(font, "Operation Snake", (Vector2){(float)(screenWidth / 2 - MeasureText("Operation Snake", 40) / 2), (float)(screenHeight / 2 - 40)}, 40, 0, BLACK);
    DrawTextEx(font, "1. Nivel Facil", (Vector2){(float)(screenWidth / 2 - MeasureText("1. Nivel Facil", 20) / 2), (float)(screenHeight / 2)}, 20, 0, Fade(BLACK, menuAlpha));
    DrawTextEx(font, "2. Nivel Dificil", (Vector2){(float)(screenWidth / 2 - MeasureText("2. Nivel Dificil", 20) / 2), (float)(screenHeight / 2 + 30)}, 20, 0, Fade(BLACK, menuAlpha));
    DrawTextEx(font, "3. Salir", (Vector2){(float)(screenWidth / 2 - MeasureText("3. Salir", 20) / 2), (float)(screenHeight / 2 + 60)}, 20, 0, Fade(BLACK, menuAlpha));

    EndDrawing();
}

// Actualiza el menú
void UpdateMenu(void)
{
    // Actualiza el desvanecimiento del menú
    menuAlpha -= 0.01f; // Puedes ajustar este valor para controlar la velocidad del desvanecimiento
    if (menuAlpha < 2.0f)
        menuAlpha = 2.0f;

    if (menuFadeOut)
    {
        // Realiza acciones específicas según la opción seleccionada
        if (selectedOption == 1)
        {
            InitGame(); // Inicia el juego
            gameOver = false;
            gameState = GAME; // Cambia al estado del juego después de seleccionar la opción 1
        }
        else if (selectedOption == 2)
        {
            // Lógica para la opción 2 del menú
            // Puedes agregar la funcionalidad específica que desees aquí
        }
        else if (selectedOption == 3)
        {
            CloseWindow(); // Cierra la ventana después de seleccionar la opción 3
        }

        menuFadeOut = false; // Restablece la animación de desvanecimiento después de realizar la acción
    }

    if (IsKeyPressed(KEY_ONE))
    {
        // Inicia el juego con desvanecimiento
        menuFadeOut = true; // Inicia la animación de desvanecimiento cuando se selecciona una opción
        selectedOption = 1;
    }
    else if (IsKeyPressed(KEY_TWO))
    {
        // Lógica para la opción 2 del menú
        // Puedes agregar la funcionalidad específica que desees aquí
        selectedOption = 2;
    }
    else if (IsKeyPressed(KEY_THREE))
    {
        // Cierra la ventana con desvanecimiento
        menuFadeOut = true; // Inicia la animación de desvanecimiento cuando se selecciona una opción
        selectedOption = 3;
    }
}

void ResetGame(void)
{
    InitGame();
    gameOver = false;
    gameState = MENU;
    menuFadeOut = false;
    menuAlpha = 1.0f;
    selectedOption = 0;
}

void DrawCalculatorButtons()
{
    int buttonWidth = 50;
    int buttonHeight = 50;
    int margin = 10;
    int startX = screenWidth / 2 - (9.5 * (buttonWidth + margin)) / 2;
    int startY = screenHeight / 2 + 50;

    for (int i = 0; i < 10; i++)
    {
        DrawRectangle(startX + i * (buttonWidth + margin), startY, buttonWidth, buttonHeight, GRAY);
        DrawText(TextFormat("%d", i), startX + i * (buttonWidth + margin) + buttonWidth / 2 - MeasureText(TextFormat("%d", i), 20) / 2, startY + buttonHeight / 2 - 10, 20, BLACK);
    }
}

void HandleCalculatorInput()
{
    int buttonWidth = 50;
    int buttonHeight = 50;
    int margin = 10;
    int startX = screenWidth / 2 - (5 * (buttonWidth + margin)) / 2;
    int startY = screenHeight / 2 + 50;

    // Verifica clics en los botones de la calculadora
    for (int i = 0; i < 10; i++)
    {
        Rectangle buttonRect = {startX + i * (buttonWidth + margin), startY, buttonWidth, buttonHeight};
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), buttonRect))
        {
            // Realiza la operación correspondiente (puedes ajustar esto según tus necesidades)
            currentNumber = i;
            numberSelected = true;
        }
    }
}
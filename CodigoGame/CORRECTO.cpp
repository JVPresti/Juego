/*

    Este juego fue programado por: Jorge Vazquez Y Gilberto Diarte

    Serpent Operation
    El juego consiste en una serpiente que se mueve por la pantalla y debe comerse las frutas que aparecen en la pantalla
    para poder crecer, cada fruta tiene un numero y un signo de operacion, el jugador debe resolver la operacion y escribir el resultado

    Juego creado sin fines de lucro, solo con fines educativos

    ©Todos los derechos reservados
    ©VSDEVS 2024
*/

#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

// Estructura para almacenar los datos de la serpiente
typedef struct Snake
{
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

// Estructura para almacenar los datos de la fruta
typedef struct Food
{
    Vector2 position;
    Vector2 size;
    bool active;
    char operation;
    Color color;
    Texture2D texture;
} Food;

// Estructura para almacenar los datos de la calculadora
typedef struct
{
    int num1;
    int num2;
    char operation;
    int result;
    int userAnswer;

} CalculatorData;

// Estructura para almacenar los datos de la puntuación
typedef struct _score
{
    int bestScore;
} Score;

// Estados del juego
typedef enum
{
    MENU,
    GAME,
    PAUSED
} GameState;

//------------------------------------------------------------------------------------
// Declaraciones de variables globales
//------------------------------------------------------------------------------------

static const int screenWidth = 800;
static const int screenHeight = 450;

static int framesCounter = 0;
static bool gameOver = false;
static bool pause = false;
static bool dificultad = false; // falso es facil y verdadero es dificil
static int speedDificultad;     // velocidad de la serpiente

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
static int score = 0;

// Fuente para el texto
Font font;

// Textura para el icono de la ventana
static Texture2D icono;

// Textura para el fondo del juego
static Texture2D backgroundGame;

// Textura para el fondo del menú
static Texture2D backgroundMenu;

// Textura para la calculadora
static Texture2D backgroundCalculator;

// Textura para la cabeza de la serpiente
static Texture2D snakeHead;

// Indica el estado actual del juego
static GameState gameState = MENU;

// Musica de fondo
static Music backgroundMusic;

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
void RunCalculatorWindow(void);    // Ejecuta la ventana de la calculadora
void binFileAdd(int newScore);     // Función para guardar la puntuación en un archivo binario
int loadScore(void);               // Función para cargar la puntuación desde el archivo binario
int NumTextLength(int num);        // Función para calcular la longitud de un número entero

//------------------------------------------------------------------------------------
// Punto de entrada principal del programa
//------------------------------------------------------------------------------------

int main(void)
{
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Serpent Operation");
    SetWindowIcon(LoadImage("icon.png"));
    InitAudioDevice();

    // Cargar imágenes de fondo
    backgroundGame = LoadTexture("newfondo.png");
    backgroundMenu = LoadTexture("fondo.png");
    backgroundCalculator = LoadTexture("calculadora2.png");
    snakeHead = LoadTexture("cabeza.png");
    font = LoadFont("abs.otf");
    backgroundMusic = LoadMusicStream("zelda.mp3");
    icono = LoadTexture("icon.png");

    // Inicia la música de fondo
    PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, 0.2f); // Establece el volumen de la música de fondo

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
        UpdateMusicStream(backgroundMusic);
        UpdateDrawFrame();

        if (gameState == PAUSED)
        {
            UpdateMusicStream(backgroundMusic);
            RunCalculatorWindow();
        }
        //----------------------------------------------------------------------------------
    }
    UpdateMusicStream(backgroundMusic);

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

    counterTail = 1; // Longitud inicial de la serpiente
    allowMove = false;

    offset.x = screenWidth % SQUARE_SIZE;
    offset.y = screenHeight % SQUARE_SIZE;

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snake[i].position = (Vector2){offset.x / 2, offset.y / 2};
        snake[i].size = (Vector2){SQUARE_SIZE, SQUARE_SIZE};
        snake[i].speed = (Vector2){SQUARE_SIZE, 0};

        if (i == 0)
            snake[i].color = RAYWHITE;
        else
            snake[i].color = LIME;
    }

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snakePosition[i] = (Vector2){0.0f, 0.0f};
    }

    fruit.size = (Vector2){SQUARE_SIZE, SQUARE_SIZE};
    fruit.color = ORANGE;
    fruit.active = false;
    fruit.texture = LoadTexture("manzana.png"); // Carga la textura de la fruta

    // Guarda el puntaje actual en el archivo binari
    binFileAdd(score);

    // Restablece el puntaje actual a 0 al iniciar una nueva partida
    score = 0;
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

            if ((framesCounter % speedDificultad) == 0)
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
                fruit.position = (Vector2){GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2};

                fruit.operation = '1'; // Espacio indica que es una fruta con número
            }

            // Colisión
            if ((snake[0].position.x == fruit.position.x) && (snake[0].position.y == fruit.position.y))
            {
                snake[counterTail].position = snakePosition[counterTail - 1];
                counterTail += 1;
                fruit.active = false;
                fruitCollected = true;
                fruitsEaten += 1;
                score += 1;

                if (fruitsEaten == 3)
                {
                    pause = true;
                    gameState = PAUSED; // Cambia el estado del juego a pausado
                }

                // Restablece el contador después de dos frutas comidas
                if (fruitsEaten == 3)
                {
                    fruitsEaten = 0;
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
    char currentScoreText[20];
    char bestScoreText[20];
    Vector2 bkPosition = {0.0f, 0.0f};
    BeginDrawing();

    // Dibuja la imagen de fondo del juego
    ClearBackground(BLACK);
    DrawTextureEx(backgroundGame, bkPosition, 0, 0.62, Fade(RAYWHITE, 0.8f));

    if (!gameOver)
    {
        // Dibuja las líneas de la cuadrícula
        for (int i = 0; i < screenWidth / SQUARE_SIZE + 1; i++)
        {
            DrawLineV((Vector2){SQUARE_SIZE * i + offset.x / 2, offset.y / 2}, (Vector2){SQUARE_SIZE * i + offset.x / 2, screenHeight - offset.y / 2}, DARKGRAY);
        }

        for (int i = 0; i < screenHeight / SQUARE_SIZE + 1; i++)
        {
            DrawLineV((Vector2){offset.x / 2, SQUARE_SIZE * i + offset.y / 2}, (Vector2){screenWidth - offset.x / 2, SQUARE_SIZE * i + offset.y / 2}, DARKGRAY);
        }

        // Dibuja la serpiente
        for (int i = 0; i < counterTail; i++)
        {
            DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);
        }
        DrawTextureEx(snakeHead, snake[0].position, 0, 0.061, snake[0].color); // Dibuja la cabeza de la serpiente

        // Cálculo de la posición de la fruta
        if (!fruit.active)
        {
            fruit.active = true;
            fruit.position = (Vector2){GetRandomValue(0, (screenWidth / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (screenHeight / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2};
            fruit.operation = '1'; // Espacio indica que es una fruta con número
        }

        // Dibuja una manzana en la posición de la fruta
        DrawTextureEx(fruit.texture, fruit.position, 0, 0.095, WHITE);

        // Muestra el número sobre el cuadro azul
        DrawText(numberText, fruit.position.x + fruit.size.x / 2 - MeasureText(numberText, 20) / 2, fruit.position.y + fruit.size.y / 2 - 10, 20, WHITE);

        // Dibuja el puntaje actual
        snprintf(currentScoreText, sizeof(currentScoreText), "Score: %d", score);
        DrawText(currentScoreText, 10, 10, 18, WHITE);

        // Dibuja el mejor puntaje
        int beScor = loadScore();
        snprintf(bestScoreText, sizeof(bestScoreText), "Best Score: %d", beScor);
        DrawText(bestScoreText, 670, 10, 18, WHITE);

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
    StopMusicStream(backgroundMusic);
    UnloadMusicStream(backgroundMusic);
    UnloadTexture(backgroundGame);
    UnloadTexture(backgroundMenu);
    UnloadFont(font);
}

// Actualiza y dibuja (un frame)
void UpdateDrawFrame(void)
{
    if (gameState == MENU)
    {
        UpdateMenu();
        DrawMenu();
        if (menuAlpha == 0.0f) // Verifica si el menú se ha desvanecido
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
                InitGame();
                gameOver = false;
                gameState = GAME;
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
        if (IsKeyPressed(KEY_ENTER))
        {
            pause = false;
            gameState = GAME; // Vuelve al juego
        }

        // Maneja la entrada de la calculadora
        RunCalculatorWindow();
    }
}

// Dibuja el menú
void DrawMenu(void)
{
    Vector2 bkPosition = {0.0f, 0.0f};
    BeginDrawing();

    // Dibuja la imagen de fondo del menú con opacidad
    DrawTextureEx(backgroundMenu, bkPosition, 0, 1, Fade(RAYWHITE, menuAlpha));

    // Realiza conversiones explícitas de int a float para evitar warnings
    DrawTextEx(font, "Operation Snake", (Vector2){(float)(screenWidth / 2 - MeasureText("Operation Snake", 89) / 2), (float)(screenHeight / 2 - 185)}, 75, 0, BLACK);
    DrawTextEx(font, "1. Nivel Facil", (Vector2){(float)(screenWidth / 2 - MeasureText("1. Nivel Facil", 40) / 2), (float)(screenHeight / 2 - 30)}, 30, 0, Fade(BLACK, menuAlpha));
    DrawTextEx(font, "2. Nivel Dificil", (Vector2){(float)(screenWidth / 2 - MeasureText("2. Nivel Dificil", 40) / 2), (float)(screenHeight / 2 + 30)}, 30, 0, Fade(BLACK, menuAlpha));
    DrawTextEx(font, "3. Salir", (Vector2){(float)(screenWidth / 2 - MeasureText("3. Salir", 45) / 2), (float)(screenHeight / 2 + 90)}, 30, 0, Fade(BLACK, menuAlpha));

    EndDrawing();
}

// Actualiza el menú
void UpdateMenu(void)
{
    // Actualiza el desvanecimiento del menú
    menuAlpha -= 0.01f; // Puedes ajustar este valor para controlar la velocidad del desvanecimiento
    if (menuAlpha < 2.0f)
        menuAlpha = 2.0f;

    if (menuFadeOut) // Verifica si se ha seleccionado una opción
    {
        // Realiza acciones específicas según la opción seleccionada
        if (selectedOption == 1)
        {
            dificultad = false;
            speedDificultad = 14;
            InitGame(); // Inicia el juego
            gameOver = false;
            gameState = GAME; // Cambia al estado del juego después de seleccionar la opción 1
        }
        else if (selectedOption == 2)
        {
            dificultad = true;
            speedDificultad = 9;
            InitGame(); // Inicia el juego
            gameOver = false;
            gameState = GAME; // Cambia al estado del juego después de seleccionar la opción 1
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
        menuFadeOut = true; // Inicia la animación de desvanecimiento cuando se selecciona una opción
        selectedOption = 2;
    }
    else if (IsKeyPressed(KEY_THREE))
    {
        // Cierra la ventana con desvanecimiento
        menuFadeOut = true; // Inicia la animación de desvanecimiento cuando se selecciona una opción
        selectedOption = 3;
    }
}

// Reinicia el juego
void ResetGame(void)
{
    InitGame();
    gameOver = false;
    gameState = MENU;
    menuFadeOut = false;
    menuAlpha = 1.0f;
    selectedOption = 0;
    binFileAdd(score); // Guarda el puntaje actual en el archivo binario
    score = 0;         // Restablece el puntaje actual a 0 al iniciar una nueva partida
}

// Ejecuta la ventana de la calculadora
void RunCalculatorWindow(void)
{
    UpdateMusicStream(backgroundMusic);
    //  Datos de la calculadora
    CalculatorData calculator = {0};
    if (dificultad == false) // facil
    {
        char oper[2] = {'+', 'x'};
        calculator.operation = oper[GetRandomValue(0, 1)];
        if (calculator.operation == 'x')
        {
            calculator.num1 = GetRandomValue(1, 5);
            calculator.num2 = GetRandomValue(1, 5);
        }
        else if (calculator.operation == '+')
        {
            calculator.num1 = GetRandomValue(1, 20);
            calculator.num2 = GetRandomValue(1, 20);
        }
    }
    else if (dificultad == true) // dificil
    {
        char oper[2] = {'+', 'x'};
        calculator.operation = oper[GetRandomValue(0, 1)];
        if (calculator.operation == 'x')
        {
            calculator.num1 = GetRandomValue(1, 10);
            calculator.num2 = GetRandomValue(1, 10);
        }
        else if (calculator.operation == '+')
        {
            calculator.num1 = GetRandomValue(1, 40);
            calculator.num2 = GetRandomValue(1, 40);
        }
    }

    // Bucle principal
    while (!WindowShouldClose())
    {
        UpdateMusicStream(backgroundMusic);
        // Limpiar la pantalla
        Vector2 bkPosition = {0.0f, 0.0f};
        BeginDrawing();
        ClearBackground(RED);
        DrawTextureEx(backgroundCalculator, bkPosition, 0.0, 1.0, WHITE);

        // Dibujar los datos en la ventana
        DrawText("La operacion a realizar es la siguiente", 197, 12, 20, WHITE);

        // Dibujar la operación
        DrawText(TextFormat("%d %c %d = ?", calculator.num1, calculator.operation, calculator.num2), 252, 63, 60, DARKGREEN);
        DrawText("Ingresa tu respuesta y presiona Enter", 195, 142, 20, WHITE);

        // Dibujar la respuesta del usuario
        DrawText("Tu respuesta:", 275, 175, 20, DARKPURPLE);
        DrawText(TextFormat("%d", calculator.userAnswer), 260, 240, 100, DARKGREEN);

        // Dibujar el mensaje de respuesta correcta o incorrecta
        if (calculator.result != 0)
        {
            DrawText(calculator.result == 1 ? "¡Respuesta correcta!" : "¡Respuesta incorrecta!", 232, 360, 23, DARKPURPLE);

            // Cerrar la ventana al desplegar la respuesta
            if (IsKeyPressed(KEY_ENTER))
            {
                UpdateDrawFrame();
                break;
            }
        }

        // Capturar la entrada del usuario
        int key = GetKeyPressed();
        if (key != 0)
        {
            if (key >= '0' && key <= '9' && NumTextLength(calculator.userAnswer) < 4) // Asegurarse de que sea un número y no haya más de 4 dígitos
            {
                calculator.userAnswer = calculator.userAnswer * 10 + (key - '0');
            }
            else if (key == KEY_BACKSPACE)
            {
                // Eliminar el último dígito si se presiona la tecla de retroceso
                calculator.userAnswer /= 10;
            }
            else if (key == KEY_ENTER)
            {
                // Calcular el resultado si se presiona Enter
                int userAnswer = calculator.userAnswer;

                switch (calculator.operation)
                {
                case '+':
                    calculator.result = calculator.num1 + calculator.num2;
                    break;
                case 'x':
                    calculator.result = calculator.num1 * calculator.num2;
                    break;
                default:
                    printf("Error: Operador no válido\n");
                    break;
                }

                // Verificar si la respuesta del usuario es correcta
                calculator.result = (userAnswer == calculator.result);

                // Limpiar la respuesta del usuario después de mostrar el resultado
                calculator.userAnswer = 0;
            }
        }

        // Finalizar el dibujo
        EndDrawing();
    }
}

// Función para calcular la longitud de un número entero
int NumTextLength(int num)
{
    int length = 0;
    if (num == 0)
    {
        return 1;
    }
    while (num != 0)
    {
        num /= 10;
        length++;
    }
    return length;
}

// Función para guardar la puntuación en un archivo binario
void binFileAdd(int newScore)
{
    FILE *file;
    Score x;

    // Abrir el archivo binario en modo lectura y escritura
    file = fopen("score.bin", "r+b");

    if (file == NULL)
    {
        // Si el archivo no existe, crearlo en modo escritura binaria
        file = fopen("score.bin", "wb");
        if (file == NULL)
        {
            printf("Error al abrir el archivo\n");
            return;
        }
    }

    // Leer la puntuación actual desde el archivo
    fread(&x, sizeof(Score), 1, file);

    // Comparar y actualizar el mejor puntaje
    if (newScore > x.bestScore)
    {
        x.bestScore = newScore;
    }

    // Moverse al principio del archivo y escribir los datos actualizados
    fseek(file, 0, SEEK_SET);
    fwrite(&x, sizeof(Score), 1, file);

    fclose(file);
}

// Función para cargar la puntuación desde el archivo binario
int loadScore(void)
{
    FILE *file;
    Score x;

    file = fopen("score.bin", "rb");
    if (file == NULL)
    {
        printf("Error al abrir el archivo\n");
        return 0; // Devuelve 0 en caso de error
    }

    // Leer la puntuación desde el archivo
    fread(&x, sizeof(Score), 1, file);

    fclose(file);

    // Devuelve la puntuación almacenada
    return x.bestScore;
}

// © Todos los derechos reservados 2024
// © VSDEVS 2024
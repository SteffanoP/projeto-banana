#include "raylib.h"

#define screenWidth 1024
#define screenHeight 768

#define NOME_JOGO "É o jogo de Steffano agora"

#define GRAVIDADE 500
#define JOGADOR_PULO_VELOCIDADE 500.0f
#define JOGADOR_MOVIMENTO_VELOCIDADE 500.0f

typedef struct Jogador
{
    Vector2 posicao;
    float velocidade;
    bool podePular;
} Jogador;

typedef struct EnvItem
{
    Rectangle retangulo;
    int colisao;
    Color cor;
} EnvItem;

//Protótipo das funções
void UpdatePlayer(Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height);

int main()
{
    // Inicialização do Jogo
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, NOME_JOGO);

    //Configurações Iniciais do jogador
    Jogador jogador = {0};
    jogador.posicao = (Vector2){400, 280}; //Posição Inicial
    jogador.velocidade = 0; //Velocidade Inicial
    jogador.podePular = false; //Habilitação de pulo

    //Configurações Iniciais do Cenário
    EnvItem envItems[] = {
        {{0, 0, 1000, 400}, 0, LIGHTGRAY},
        {{0, 400, 1000, 200}, 1, GRAY},
        {{300, 200, 400, 10}, 1, GRAY},
        {{250, 300, 100, 10}, 1, GRAY},
        {{650, 300, 100, 10}, 1, GRAY}};
    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

    //Configurações Iniciais de Câmera
    Camera2D camera = {0};
    camera.target = jogador.posicao; //Câmera centralizada inicialmente no jogador
    camera.offset = (Vector2){screenWidth / 2, screenHeight / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    //O Jogo
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();

        UpdatePlayer(&jogador, envItems, envItemsLength, deltaTime);

        UpdateCameraCenter(&camera, &jogador, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        BeginMode2D(camera);

        for (int i = 0; i < envItemsLength; i++)
            DrawRectangleRec(envItems[i].retangulo, envItems[i].cor);

        Rectangle playerRect = {jogador.posicao.x - 20, jogador.posicao.y - 40, 40, 40}; //Desenho do jogador
        DrawRectangleRec(playerRect, RED); //Desenha o desenho do jogador

        EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
} 

void UpdatePlayer(Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta)
{
    if (IsKeyDown(KEY_LEFT)) //Movimentação para a Esquerda
        jogador->posicao.x -= JOGADOR_MOVIMENTO_VELOCIDADE * delta; //Decrementa o valor da posição do player
    if (IsKeyDown(KEY_RIGHT)) //Movimentação para a Direita
        jogador->posicao.x += JOGADOR_MOVIMENTO_VELOCIDADE * delta; //Incrementa o valor da posição do player

    if (IsKeyDown(KEY_UP) && jogador->podePular)
    {
        jogador->velocidade = -JOGADOR_PULO_VELOCIDADE;
        jogador->podePular = false;
    }

    int colisaoObjeto = 0;
    for (int i = 0; i < envItemsLength; i++) //Preechimento da áre dos pixels dos objetos colidiveis
    {
        EnvItem *objeto = envItems + i;
        Vector2 *j = &(jogador->posicao);
        if (objeto->colisao &&                                             
            objeto->retangulo.x <= j->x &&                                      //
            objeto->retangulo.x + objeto->retangulo.width >= j->x &&            // Definindo a invasão da área do player com a área do objeto(área de colisão)   
            objeto->retangulo.y >= j->y &&                    
            objeto->retangulo.y < j->y + jogador->velocidade * delta)
        {
            colisaoObjeto = 1; 
            jogador->velocidade = 0.0f; //Reduzindo a velocidade do player para 0, para freiar ele             
            j->y = objeto->retangulo.y; //Atualiza a variável do movimento
        }
    }

    if (!colisaoObjeto) //Se não há colisão com objeto
    {
        jogador->posicao.y += jogador->velocidade * delta; //Aumentar a posição do Y do jogador
        jogador->velocidade += GRAVIDADE * delta; //Vai sofrer com a Gravidade
        jogador->podePular = false; //Não pode pular no ar
    } else
        jogador->podePular = true;
}

void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = (Vector2){width / 2, height / 2};
    camera->target = jogador->posicao;
}
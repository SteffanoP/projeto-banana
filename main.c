#include "raylib.h"
#include "libraries/defines.c"

bool colisaoJogador;

typedef struct Jogador
{
    Vector2 posicao;
    float velocidade;
    bool podePular;
    //Poder Poder; //Especificar qual o poder que um jogador tem
} Jogador;

/* Sobre os inimigos:
tipo: Tipo de inimigos
tipo = 1 = minion
tipo = 2 = gado

Hitbox: Retângulo do Minion no cenário
velocidade: velocidade de movimentação
vida: quantidade de vidas do inimigo
cor: Cor do inimigo
*/
typedef struct Inimigo
{
    int tipo;
    Rectangle hitbox;
    float velocidade;
    int vida;
    Color cor;
} Inimigo;

/* Sobre o poder:

posicao: posicao do poder ao jogador
velocidade: velocidade da movimentação
raio: raio (o poder tem forma de círculo)

poder_ativo: verificação se está ativo ou não
vida_util: vida útil do poder no jogo
cor: cor do poder */
typedef struct Poder
{
    Vector2 posicao;
    float velocidade;
    float raio;
    bool poder_ativo;
    int vida_util;
    Color cor;
} Poder;

typedef struct EnvItem
{
    Rectangle retangulo;
    int colisao;
    Color cor;
} EnvItem;

//Protótipo das funções
void UpdatePlayer(Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void UpdateInimigos(Inimigo *inimigo, int inimigosTamanho, float delta);
void UpdatePoder(Poder *poderes, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height);

int main()
{
    // Inicialização do Jogo
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, NOME_JOGO);
    SetTargetFPS(60);

    //Configurações Iniciais do jogador
    Jogador jogador = {0};
    jogador.posicao = (Vector2){400, 280}; //Posição Inicial
    jogador.velocidade = 0; //Velocidade Inicial
    jogador.podePular = false; //Habilitação de pulo

    //Configurações Iniciais dos inimigos
    Inimigo inimigos[] = {
        { 0, {0, 0, 0, 0}, 0, 0, BLANK},
        { 1, {1850, 360, TAMANHO_MINION_X, TAMANHO_MINION_Y}, 0, 2, YELLOW},
        { 1, {1950, 360, TAMANHO_MINION_X, TAMANHO_MINION_Y}, 0, 2, YELLOW}
    };
    int inimigosTamanho = sizeof(inimigos) / sizeof(inimigos[0]);

    //Configurações iniciais do poder
    Poder poderes[PODER_MAX_PERSONAGEM] = {
        {{0, 0}, 0, 10, false, 0, BLACK}
    };

    //Configurações Iniciais dos Elementos do Cenário
    EnvItem envItems[] = {
        {{0, 0, TAMANHO_X_CENARIO, TAMANHO_Y_CENARIO}, 0, SKYBLUE}, //Background
        {{0, 400, 2000, 200}, 1, GRAY},
        {{300, 200, 400, 10}, 1, GRAY},
        {{250, 300, 100, 10}, 1, GRAY},
        {{650, 300, 100, 10}, 1, GRAY},
        {{900, 350,  50, 50}, 1, PURPLE},
        {{1050, 311,  50, 50}, 1, PURPLE},
        {{1200, 309,  50, 50}, 1, PURPLE},
        {{1350, 330,  50, 50}, 1, PURPLE}
    };
    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

    //Configurações Iniciais de Câmera
    Camera2D camera = {0};
    camera.target = jogador.posicao; //Câmera centralizada inicialmente no jogador
    camera.offset = (Vector2){screenWidth / 2, screenHeight / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    //--------------------------------------------------------------------------------------

    //O Jogo
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();
        
        //Atualiza os dados do jogador
        UpdatePlayer(&jogador, envItems, envItemsLength, deltaTime);

        //Atualiza os dados dos inimigos
        UpdateInimigos(inimigos, inimigosTamanho, deltaTime);

        //Atualiza os poderes do jogador
        UpdatePoder(poderes, &jogador, envItems, envItemsLength, deltaTime);

        //Atualiza a Câmera focada no jogador
        UpdateCameraCenter(&camera, &jogador, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        //Desenho do Background do restante da Janela que não é objeto
        ClearBackground(LIGHTGRAY);

        BeginMode2D(camera);

        //Desenho dos Retângulos referentes aos obstáculos de EnvItems
        for (int i = 0; i < envItemsLength; i++)
            DrawRectangleRec(envItems[i].retangulo, envItems[i].cor);
        
        //Desenho do poder bola
        for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
        {
            if (poderes[p].poder_ativo)
            {
                DrawCircleV(poderes[p].posicao, poderes[p].raio, BLACK);
            }
        }

        //Desenho dos Retângulos referentes aos inimigos
        for (int i = 0; i < inimigosTamanho; i++)
            DrawRectangleRec(inimigos[i].hitbox, inimigos[i].cor);

        //Criação e Desenho do jogador
        Rectangle playerRect = {jogador.posicao.x - TAMANHO_X_JOGADOR / 2, jogador.posicao.y - TAMANHO_Y_JOGADOR, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR}; //Desenho do jogador
        DrawRectangleRec(playerRect, RED); //Desenha o desenho do jogador

        DrawText(FormatText("Colisão : %01i", colisaoJogador), 1000, 450, 20, BLACK);

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
    
    //Limites da area de movimentação do jogador
    if ((jogador->posicao.x + TAMANHO_X_JOGADOR / 2) > TAMANHO_X_CENARIO)
    {
        jogador->posicao.x = TAMANHO_X_CENARIO - TAMANHO_X_JOGADOR / 2; //Limites para direita
    } else if (jogador->posicao.x < TAMANHO_X_JOGADOR / 2)
    {
        jogador->posicao.x = TAMANHO_X_JOGADOR / 2; //Limites para a esquerda
    }
    
    if ((jogador->posicao.y) > TAMANHO_Y_CENARIO)  //Limites na vertical
    {
        jogador->posicao.y = TAMANHO_Y_CENARIO; 
    } else if (jogador->posicao.y < TAMANHO_Y_JOGADOR)
    {
        jogador->posicao.y = TAMANHO_Y_JOGADOR;
    }

    colisaoJogador = 0;
    int colisaoObjeto = 0;
    for (int i = 0; i < envItemsLength; i++) //Preechimento da área dos pixels dos objetos colidiveis
    {
        EnvItem *objeto = envItems + i;
        Vector2 *j = &(jogador->posicao);
        
        //Condição de colisão para pulo e andar encima de plataformas
        if (objeto->colisao &&                                             
            objeto->retangulo.x - TAMANHO_X_JOGADOR/2 <= j->x &&                                      //
            objeto->retangulo.x + objeto->retangulo.width + TAMANHO_X_JOGADOR/2 >= j->x &&            // Definindo a invasão da área do player com a área do objeto(área de colisão)   
            objeto->retangulo.y >= j->y &&                    
            objeto->retangulo.y < j->y + jogador->velocidade * delta)
        {
            colisaoObjeto = 1; 
            jogador->velocidade = 0.0f; //Reduzindo a velocidade do player para 0, para freiar ele             
            j->y = objeto->retangulo.y; //Atualiza a variável do movimento
        }
        
        //Condição de colisão em objetos Universais
        if (objeto->colisao && //Detecta se o objeto é colidível
            objeto->retangulo.x - TAMANHO_X_JOGADOR/2 <= j->x && //Detecta a borda esquerda do objeto
            objeto->retangulo.x + objeto->retangulo.width + TAMANHO_X_JOGADOR/2 >= j->x && //Detecta a borda direita do objeto
            objeto->retangulo.y < j->y && //Detecta colisão acima do objeto
            objeto->retangulo.y + objeto->retangulo.height + TAMANHO_Y_JOGADOR > j->y) //Detecta colisão abaixo do objeto
        {
            if (objeto->retangulo.x - TAMANHO_X_JOGADOR / 2 <= j->x &&
                objeto->retangulo.x > j->x) //Detecta a colisão com a esquerda do objeto
            {
                jogador->posicao.x = objeto->retangulo.x - TAMANHO_X_JOGADOR / 2;
            }
            else if (objeto->retangulo.x + objeto->retangulo.width + TAMANHO_X_JOGADOR / 2 >= j->x &&
                     objeto->retangulo.x + objeto->retangulo.width < j->x) //Detecta a colisão a direita do objeto
            {
                jogador->posicao.x = objeto->retangulo.x + objeto->retangulo.width + TAMANHO_X_JOGADOR / 2;
            }
            else if (objeto->retangulo.y + objeto->retangulo.height + TAMANHO_Y_JOGADOR > j->y) //Detecta a colisão abaixo do objeto
            {
                jogador->posicao.y = objeto->retangulo.y + objeto->retangulo.height + TAMANHO_Y_JOGADOR;
                jogador->velocidade = GRAVIDADE * delta;
            }
            colisaoJogador = 1;
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

void UpdateInimigos(Inimigo *inimigos, int inimigosTamanho, float delta) {
    for (int i = -1; i < inimigosTamanho; i++)
    {
        Inimigo *minion = inimigos + i;
        minion->hitbox.x -= VELOCIDADE_INIMIGO_MINION * delta;
        inimigos->hitbox.x = minion->hitbox.x;
    }
}

void UpdatePoder(Poder *poderes, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta) {
    
    if (IsKeyDown(KEY_SPACE))  //Usando o poder
    {
        if (!poderes->poder_ativo) //Caso não haja poder ativo, seta as configurações abaixo
        { 
            poderes->posicao = (Vector2){jogador->posicao.x - TAMANHO_X_JOGADOR/2 + 10, jogador->posicao.y - TAMANHO_Y_JOGADOR/2};
            poderes->velocidade = JOGADOR_MOVIMENTO_VELOCIDADE;
            poderes->poder_ativo = true;
        }
    }

    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        if (poderes->poder_ativo) //Caso haja poder ativo
        {
            poderes->vida_util++; //Sua "vida" é incrementada
            poderes->posicao.x += JOGADOR_MOVIMENTO_VELOCIDADE * delta; //Posição é incrementada
        }
        for (int i = 0; i < envItemsLength; i++)
        {
            EnvItem *objeto = envItems + i;
            if (objeto->colisao && CheckCollisionCircleRec(poderes->posicao, poderes->raio, objeto->retangulo)) //Havendo colisão, a bola atirada desaparece
            {
                poderes->poder_ativo = false;
                poderes->vida_util = 0;
            }
            if (!CheckCollisionCircleRec(poderes->posicao, poderes->raio, objeto->retangulo)) //Não houver confirmação de colisão
            {
                if (poderes->vida_util >= 200) //A vida útil da bola atirada é limitada, após isso a bola desaparece
                {
                    poderes->posicao = (Vector2){0,0}; 
                    poderes->velocidade = 0;
                    poderes->vida_util = 0;
                    poderes->poder_ativo = false;
                }
                if (poderes->posicao.x >= TAMANHO_X_CENARIO) //Movimentação da bola atirada é limitada até o fim do cenário, então desaparece
                {
                    poderes->poder_ativo = false;
                }
            }
        }
    }
}

void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = (Vector2){width / 2, height / 2};
    camera->target = jogador->posicao;
}
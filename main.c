#include "raylib.h"
#include "libraries/defines.c"

bool colisaoJogador;

typedef struct Jogador
{
    Vector2 posicao;
    Vector2 posicaoAnterior;
    float velocidade;
    bool podePular;
} Jogador;

/* Sobre os inimigos:
tipo: Tipo de inimigos
tipo = 1 = minion
tipo = 2 = gado

posicao: Refere-se a posicao dele no cenário
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

typedef struct EnvItem
{
    Rectangle retangulo;
    int colisao;
    Color cor;
} EnvItem;

typedef struct Personagem
{
    Vector2 posicao;
    Texture2D texture;
    float frameWidth;
    float frameHeight;
    Rectangle frameRect;
} Personagem;

typedef struct FPS_Animacao
{
    int counter;
    int speed;
    int currentFrame;
} FPS_Animacao;

//Protótipo das funções
void UpdatePlayer(Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void AnimacaoMovimento(FPS_Animacao *frames, Jogador *jogador,Personagem *personagem);
void AnimacaoParado(Jogador *jogador, Personagem *personagem);
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
    
    //Configurações Iniciais da animação
    FPS_Animacao frames;
    frames.counter = 0; //Conta as FPS
    frames.speed = 10;  //FPS da animação
    frames.currentFrame = 0; //Controla a passagem de frames
    Personagem personagem;
    Texture2D spritesPersonagem = LoadTexture("sprites/companheiro-da-silva.png"); //Carregamento da sprite sheet
    personagem.texture = (Texture2D)spritesPersonagem;
    personagem.frameWidth = personagem.texture.width / 4; //Largura da sprite
    personagem.frameHeight = personagem.texture.height / 4; //Altura da sprite
    personagem.frameRect = (Rectangle){2*personagem.frameWidth, 0.0f, personagem.frameWidth, personagem.frameHeight}; //Sprite inicial
    personagem.posicao.x = 116 - TAMANHO_X_JOGADOR; //Posiçâo x do personagem em relação à posição x do jogador
    personagem.posicao.y = 190 - TAMANHO_Y_JOGADOR; //Posiçâo y do personagem em relação à posição y do jogador

    //Configurações Iniciais dos inimigos
    Inimigo inimigos[] = {
        { 1, {1700, 360, 40, 40}, 0, 2, YELLOW},
        { 1, {1850, 360, 40, 40}, 0, 2, YELLOW}
    };
    int inimigosTamanho = sizeof(inimigos) / sizeof(inimigos[0]);

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
        SetTargetFPS(60);
        float deltaTime = GetFrameTime();

        jogador.posicaoAnterior = jogador.posicao;        
        
        //Atualiza os dados do jogador
        UpdatePlayer(&jogador, envItems, envItemsLength, deltaTime);

        //Atualiza a animação quando o jogador está em Movimento
        AnimacaoMovimento(&frames, &jogador, &personagem);

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

        //Desenho dos Retângulos referentes aos inimigos
        for (int i = 0; i < inimigosTamanho; i++)
            DrawRectangleRec(inimigos[i].hitbox, inimigos[i].cor);

        //Criação e Desenho do jogador
        Rectangle playerRect = {jogador.posicao.x - TAMANHO_X_JOGADOR / 2, jogador.posicao.y - TAMANHO_Y_JOGADOR, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR}; //Desenho do jogador
        DrawRectangleRec(playerRect, RED); //Desenha o desenho do jogador

        DrawTextureRec(personagem.texture, personagem.frameRect, (Vector2){jogador.posicao.x - (personagem.posicao.x + TAMANHO_X_JOGADOR), jogador.posicao.y - (personagem.posicao.y + TAMANHO_Y_JOGADOR)}, RAYWHITE);

        DrawText(FormatText("Colisão : %01i", colisaoJogador), 1000, 450, 20, BLACK);

        EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
        //Atualiza a animação quando o jogador está parado
        AnimacaoParado(&jogador, &personagem);
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------

    UnloadTexture(personagem.texture); //Descarregamento da sprite sheet

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
    for (int i = 0; i < envItemsLength; i++) //Preechimento da áre dos pixels dos objetos colidiveis
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

void AnimacaoMovimento(FPS_Animacao *frames, Jogador *jogador, Personagem *personagem)
{
    frames->counter++; //Armazena o valor da frame do jogo 

    if (frames->counter >= (60/frames->speed)) //Altera as FPS do jogo para a desejada para a movimentação do jogador
    {
        frames->counter = 0;
        frames->currentFrame++;
        if (frames->currentFrame > 2) frames->currentFrame = 1; //Controle da alternância dos passos

        if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && frames->currentFrame == 1) //Passo 1 esquerda
        {
            personagem->posicao.x = 140 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 2*personagem->frameWidth;
            personagem->frameRect.y = 2*personagem->frameHeight;
        }
        if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && frames->currentFrame == 2) //Passo 2 esquerda
        {
            personagem->posicao.x = 140 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 0.0f;
            personagem->frameRect.y = 3*personagem->frameHeight;
        }
        if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && frames->currentFrame == 1) //Passo 1 direita
        {
            personagem->posicao.x = 116 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 0.0f;
            personagem->frameRect.y = personagem->frameHeight;
        }
        if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && frames->currentFrame == 2) //Passo 2 direita
        {
            personagem->posicao.x = 116 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 2*personagem->frameWidth;
            personagem->frameRect.y = personagem->frameHeight;
        }

        if ((IsKeyDown(KEY_UP) && 
        ((personagem->frameRect.x == 0.0f && personagem->frameRect.y == 2*personagem->frameHeight) ||
        (personagem->frameRect.x == 2*personagem->frameWidth && personagem->frameRect.y == 2*personagem->frameHeight) ||
        (personagem->frameRect.x == 0.0f && personagem->frameRect.y == 3*personagem->frameHeight))) ||
        ((IsKeyDown(KEY_UP)) && (IsKeyDown(KEY_LEFT))) || ((jogador->podePular == false) && (IsKeyDown(KEY_LEFT)))) //Pulo esquerda
        {
            personagem->posicao.x = 140 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 2*personagem->frameWidth;
            personagem->frameRect.y = 2*personagem->frameHeight;
        }
        if ((IsKeyDown(KEY_UP) && 
        ((personagem->frameRect.x == 2*personagem->frameWidth && personagem->frameRect.y == 0.0f) ||
        (personagem->frameRect.x == 0.0f && personagem->frameRect.y == personagem->frameHeight) ||
        (personagem->frameRect.x == 2*personagem->frameWidth && personagem->frameRect.y == personagem->frameHeight))) ||
        ((IsKeyDown(KEY_UP)) && (IsKeyDown(KEY_RIGHT))) || ((jogador->podePular == false) && (IsKeyDown(KEY_RIGHT)))) //Pulo direita
        {
            personagem->posicao.x = 116 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 0.0f;
            personagem->frameRect.y = personagem->frameHeight;
        }
    }
}

void AnimacaoParado(Jogador *jogador, Personagem *personagem)
{
    if ((((IsKeyDown(KEY_LEFT) && jogador->podePular == true) || 
    ((personagem->frameRect.x == 2*personagem->frameWidth && personagem->frameRect.y == 2*personagem->frameHeight)
    && jogador->podePular == true)) ||
    ((personagem->frameRect.x == 0.0f && personagem->frameRect.y == 3*personagem->frameHeight) && 
    jogador->podePular == true)) 
    && (jogador->posicao.x == jogador->posicaoAnterior.x)) //Parado esquerda
    {
        personagem->posicao.x = 140 - TAMANHO_X_JOGADOR;
        personagem->frameRect.x = 0.0f;
        personagem->frameRect.y = 2*personagem->frameHeight;
    }
    if ((((IsKeyDown(KEY_RIGHT) && jogador->podePular == true) || 
    ((personagem->frameRect.x == 0.0f && personagem->frameRect.y == personagem->frameHeight) && 
    jogador->podePular == true)) ||
    ((personagem->frameRect.x == 2*personagem->frameWidth && personagem->frameRect.y == personagem->frameHeight) && 
    jogador->podePular == true)) 
    && (jogador->posicao.x == jogador->posicaoAnterior.x)) //Parado direita
    {
        personagem->posicao.x = 116 - TAMANHO_X_JOGADOR;
        personagem->frameRect.x = 2*personagem->frameWidth;
        personagem->frameRect.y = 0.0f;
    }
}

void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = (Vector2){width / 2, height / 2};
    camera->target = jogador->posicao;
}
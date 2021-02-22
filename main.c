#include "raylib.h"
#include "libraries/defines.c"
#include "time.h"

bool colisaoJogador;

/* Sobre o jogador:
posicao: Posição X e Y
velocidade: velocidade de movimento do jogador
podePular: condição em que pode pular
vida: quantidade de vidas do jogador 
direcao_movimento = player segue para a direita ou esquerda */

typedef struct Jogador
{
    Vector2 posicao;
    Vector2 posicaoAnterior;
    float velocidade;
    bool podePular;
    int vida;
    int direcao_movimento;
} Jogador;

/* Sobre os inimigos:
tipo: Tipo de inimigos
tipo = 1 = minion
tipo = 2 = gado
posicao: Posição do Minion no cenário
velocidade: velocidade de movimentação
direcao_movimento: direção em que se movimenta
vida: quantidade de vidas do inimigo
cor: Cor do inimigo*/
typedef struct Inimigo
{
    int tipo;
    Vector2 posicao;
    float velocidade;
    bool direcao_movimento;
    int vida;
    Color cor;
} Inimigo;

/* Sobre os poderes:
posicao: posicao em relação ao jogador
velocidade: velocidade da movimentação
raio: raio da forma (o poder representa um círculo)
poder_ativo: verificação se está ativo ou não
direcao_movimento: direção do poder independente da do jogador 
cor: cor do poder */
typedef struct Poder
{
    Vector2 posicao;
    float raio;
    bool poder_ativo;
    bool direcao_movimento;
    Color cor;
} Poder;

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
    int framesCounter;
    float framesSpeed;
    int currentFrame;
} Personagem;

typedef struct Frame_inimigo
{
    Vector2 posicao;
    Texture2D texture;
    float frameWidth;
    float frameHeight;
    Rectangle frameRect;
    int framesCounter;
    float framesSpeed;
    int currentFrame;
} Frame_inimigo;

int updateplayer;
clock_t t;
time_t s;
time_t sc;

static Poder imune_19[PODER_MAX_PERSONAGEM] = {0}; //A variável inicializa zerada em suas posições

//Protótipo das funções

void UpdatePlayer(Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void UpdateInimigo(Inimigo *inimigo, EnvItem *envItems, Jogador *jogador, int tamanhoInimigos, int envItemsLength, float delta);
void UpdatePoder(Poder *imune_19, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void AnimacaoJogadorMovimento(Jogador *jogador,Personagem *personagem, float deltaTime);
void AnimacaoInimigo(Inimigo *inimigo, Frame_inimigo *frameInimigoT1, Frame_inimigo *frameInimigoT2, Texture2D spritesMinion, Texture2D spritesGado, int tamanhoInimigos, float deltaTime);
void AnimacaoJogadorParado(Jogador *jogador, Personagem *personagem, float delta);
void Draw(Camera2D camera, EnvItem *envItems, int envItemsLength, int tamanhoInimigo, Inimigo *inimigo, Jogador *jogador, Personagem *personagem, Frame_inimigo *frameInimigoT1, Frame_inimigo *frameInimigoT2, Texture2D spritesMinion, Texture2D spritesGado);
void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
int VerificaColisaoBordasED(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto);
bool VerificaColisaoBordaS(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto);
int VerificaRangeGado(Vector2 posicao_inicial, float tamanho_gado_x, float tamanho_gado_y, Rectangle jogador, float range);

int main()
{
    // Inicialização do Jogo
    //--------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, NOME_JOGO);

    updateplayer = 1;

    //Configurações Iniciais do jogador
    Jogador jogador = {0};
    jogador.posicao = (Vector2){400, 280}; //Posição Inicial
    jogador.velocidade = 0; //Velocidade Inicial
    jogador.podePular = false; //Habilitação de pulo
    jogador.vida = 1;
    jogador.direcao_movimento = 1;


    
    //Configurações Iniciais da animação do joagdor
    Personagem personagem;
    Texture2D spritesPersonagem = LoadTexture("sprites/companheiro-da-silva.png"); //Carregamento da sprite sheet
    //Texture2D spritesPersonagem = LoadTexture("sprites/dr-cake.png");
    //Texture2D spritesPersonagem = LoadTexture("sprites/john-dorivac.png");
    personagem.texture = (Texture2D)spritesPersonagem;
    personagem.frameWidth = personagem.texture.width / 4; //Largura da sprite
    personagem.frameHeight = personagem.texture.height / 4; //Altura da sprite
    personagem.frameRect = (Rectangle){2*personagem.frameWidth, 0.0f, personagem.frameWidth, personagem.frameHeight}; //Sprite inicial
    personagem.posicao.x = 116 - TAMANHO_X_JOGADOR; //Posiçâo x do personagem em relação à posição x do jogador
    personagem.posicao.y = 190 - TAMANHO_Y_JOGADOR; //Posiçâo y do personagem em relação à posição y do jogador
    personagem.framesCounter = 0; //Conta as FPS
    personagem.framesSpeed = 18;  //FPS da animação
    personagem.currentFrame = 0; //Controla a passagem de frames

    //Configurações Iniciais dos inimigos
    Inimigo inimigo[] = {
        {1, {1650, 280}, 0, 0, 2, YELLOW},
        {1, {1750, 280}, 0, 0, 2, YELLOW},
        {1, {1850, 280}, 0, 0, 2, YELLOW},
        {2, {2050, 280}, 0, 1, 2, ORANGE},
        {2, {2150, 280}, 0, 1, 2, ORANGE}
    };
    const int tamanhoInimigo = sizeof(inimigo) / sizeof(inimigo[0]);

    //Configurações iniciais do poder "IMUNE_19"
    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        imune_19[p].posicao = (Vector2){0,0};
        imune_19[p].raio = 10;
        imune_19[p].cor = BLACK;
    }

    //Configurações iniciais da animação dos inimigos
    Frame_inimigo frameInimigoT1 = {0};
    Texture2D spritesMinion = LoadTexture("sprites/minion.png"); //Carregamento da sprite sheet
    frameInimigoT1.currentFrame = 0;
    frameInimigoT1.framesCounter = 0;
    frameInimigoT1.framesSpeed = 13;
    Frame_inimigo frameInimigoT2 = {0};
    Texture2D spritesGado = LoadTexture("sprites/gado.png"); //Carregamento da sprite sheet
    frameInimigoT2.currentFrame = 0;
    frameInimigoT2.framesCounter = 0;
    frameInimigoT2.framesSpeed = 13;

    //Configurações Iniciais dos Elementos do Cenário
    EnvItem envItems[] = {
        {{0, 0, TAMANHO_X_CENARIO, TAMANHO_Y_CENARIO}, 0, SKYBLUE}, //Background
        {{0, 400, 3000, 200}, 1, GRAY},
        {{300, 200, 400, 10}, 1, GRAY},
        {{250, 300, 100, 10}, 1, GRAY},
        {{650, 300, 100, 10}, 1, GRAY},
        {{900, 350,  50, 50}, 1, PURPLE},
        {{1050, 311,  50, 50}, 1, PURPLE},
        {{1200, 308,  50, 50}, 1, PURPLE},
        {{1350, 330,  50, 50}, 1, PURPLE},
        {{1450, 340,  30, 60}, 1, GREEN},
        {{1970, 340,  30, 60}, 1, GREEN},
        {{2490, 340,  30, 60}, 1, GREEN}
    };
    int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

    //Configurações Iniciais de Câmera
    Camera2D camera = {0};
    camera.target = jogador.posicao; //Câmera centralizada inicialmente no jogador
    camera.offset = (Vector2){screenWidth / 2, screenHeight / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    time(&sc); //Tempo no começo do jogo

    //--------------------------------------------------------------------------------------
    //O Jogo
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();
        t = clock(); //Armazena o tempo da frame
        time(&s); //Tempo enquanto o jogo está acontecendo

        jogador.posicaoAnterior = jogador.posicao; //Atualiza a posição anterior do jogador

        //Atualiza os dados do jogador
        if(updateplayer == 1)
        {
            UpdatePlayer(&jogador, envItems, envItemsLength, deltaTime);
        }
        
        //Atualiza os dados dos inimigos
        for (int i = 0; i < tamanhoInimigo; i++)
        {
            UpdateInimigo(&(inimigo[i]), envItems, &jogador, tamanhoInimigo, envItemsLength, deltaTime);
            AnimacaoInimigo(&(inimigo[i]), &frameInimigoT1, &frameInimigoT2, spritesMinion, spritesGado, tamanhoInimigo, deltaTime); //Atualiza a animação do inimigo
        }

        //Atualiza a animação do jogador quando o jogador está em movimento
        AnimacaoJogadorMovimento(&jogador, &personagem, deltaTime);

        //Atualiza os dados do poder
        UpdatePoder(imune_19, &jogador, envItems, envItemsLength, deltaTime);      

        //Atualiza a Câmera focada no jogador
        UpdateCameraCenter(&camera, &jogador, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------

        Draw(camera, envItems, envItemsLength, tamanhoInimigo, inimigo, &jogador, &personagem, &frameInimigoT1, &frameInimigoT2, spritesMinion, spritesGado);

        //----------------------------------------------------------------------------------
      
        //Atualiza a animação quando o jogador está parado
        AnimacaoJogadorParado(&jogador, &personagem, deltaTime);
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------

    //Descarregamento da sprite sheet do jogador
    UnloadTexture(personagem.texture); 
    UnloadTexture(spritesMinion);
    UnloadTexture(spritesGado);

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


void UpdatePlayer(Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta)
{
    if (IsKeyDown(KEY_LEFT) && jogador->vida > 0) //Movimentação para a Esquerda
    {
        jogador->posicao.x -= JOGADOR_MOVIMENTO_VELOCIDADE * delta; //Decrementa o valor da posição do player
        jogador->direcao_movimento = 0;
    }
    if (IsKeyDown(KEY_RIGHT)) //Movimentação para a Direita
    {
        jogador->posicao.x += JOGADOR_MOVIMENTO_VELOCIDADE * delta; //Incrementa o valor da posição do player
        jogador->direcao_movimento = 1;
    }
    if (IsKeyDown(KEY_UP) && jogador->podePular  && jogador->vida > 0)
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
        if (objeto->colisao)
        {
            if (VerificaColisaoBordaS(jogador->posicao, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR, objeto->retangulo))
            {
                jogador->posicao.y = objeto->retangulo.y + objeto->retangulo.height + TAMANHO_Y_JOGADOR + 1;
                jogador->velocidade = GRAVIDADE * delta;
            } 
            else if (VerificaColisaoBordasED(jogador->posicao, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR, objeto->retangulo) == 1)
            {
                jogador->posicao.x = objeto->retangulo.x + objeto->retangulo.width + TAMANHO_X_JOGADOR / 2;
            }
            else if (VerificaColisaoBordasED(jogador->posicao, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR, objeto->retangulo) == 2)
            {
                jogador->posicao.x = objeto->retangulo.x - TAMANHO_X_JOGADOR / 2;
            }
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

void UpdateInimigo(Inimigo *inimigo, EnvItem *envItems, Jogador *jogador, int tamanhoInimigos, int envItemsLength, float delta)
{
    Rectangle ret_jogador = {jogador->posicao.x - (TAMANHO_X_JOGADOR / 2),jogador->posicao.y - TAMANHO_Y_JOGADOR,TAMANHO_X_JOGADOR,TAMANHO_Y_JOGADOR};
    Rectangle ret_inimigo = {inimigo->posicao.x - (TAMANHO_MINION_X / 2), inimigo->posicao.y - TAMANHO_MINION_Y, TAMANHO_MINION_X, TAMANHO_MINION_Y};

    //Verifica se o inimigo é do tipo: minion
    if (inimigo->tipo == 1)
    {
        if (inimigo->direcao_movimento == 0)
            inimigo->posicao.x -= VELOCIDADE_INIMIGO_MINION * delta;
        else if (inimigo->direcao_movimento == 1)
            inimigo->posicao.x += VELOCIDADE_INIMIGO_MINION * delta;
    }

    //Verifica se o inimigo é do tipo: gado
    if (inimigo->tipo == 2)
    {
        Rectangle ret_jogador = {jogador->posicao.x, jogador->posicao.y, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR};
        //Verifica se o inimigo está andando para a esquerda
        if (inimigo->direcao_movimento == 0) {
            if (VerificaRangeGado(inimigo->posicao,TAMANHO_GADO_X,TAMANHO_GADO_Y,ret_jogador,RANGE_GADO) == 1) //Verifica o range do gado a esquerda
            {
                inimigo->posicao.x -= VELOCIDADE_INIMIGO_GADO_STRESS * delta; //Velocidade do gado sob Stress
            } 
            else //Se não há nenhum inimigo no range a esquerda
            {
                inimigo->posicao.x -= VELOCIDADE_INIMIGO_GADO_NORMAL * delta; //Velocidade normal do gado
            }
        }
        else if (inimigo->direcao_movimento == 1) //Verifica se o inimigo está andando para a esquerda
        {
            if (VerificaRangeGado(inimigo->posicao,TAMANHO_GADO_X,TAMANHO_GADO_Y,ret_jogador,RANGE_GADO) == 2) //Verifica o range do gado a direita
            {
                inimigo->posicao.x += VELOCIDADE_INIMIGO_GADO_STRESS * delta; //Velocidade do gado sob Stress
            } 
            else //Se não há nenhum inimigo no range a direita
            {
                inimigo->posicao.x += VELOCIDADE_INIMIGO_GADO_NORMAL * delta; //Velocidade normal do gado
            }    
        }
    }

    //Limites da area de movimentação do inimigo no cenário
    if ((inimigo->posicao.x + TAMANHO_MINION_X / 2) > TAMANHO_X_CENARIO)
    {
        inimigo->posicao.x = TAMANHO_X_CENARIO - TAMANHO_MINION_X / 2; //Limites para direita
        inimigo->direcao_movimento = !inimigo->direcao_movimento;
    }
    else if (inimigo->posicao.x < TAMANHO_MINION_X / 2)
    {
        inimigo->posicao.x = TAMANHO_MINION_X / 2; //Limites para a esquerda
        inimigo->direcao_movimento = !inimigo->direcao_movimento;
    }

    int colisaoObjeto = 0;
    for (int i = 0; i < envItemsLength; i++) //Preechimento da área dos pixels dos objetos colidiveis
    {
        EnvItem *objeto = envItems + i;
        Vector2 *j = &(inimigo->posicao);

        //Condição de colisão para andar encima de plataformas
        if (objeto->colisao &&
            objeto->retangulo.x - TAMANHO_MINION_X / 2 <= j->x &&                           
            objeto->retangulo.x + objeto->retangulo.width + TAMANHO_MINION_X / 2 >= j->x && // Definindo a invasão da área do inimigo com a área do objeto(área de colisão)
            objeto->retangulo.y >= j->y &&
            objeto->retangulo.y < j->y + inimigo->velocidade * delta)
        {
            colisaoObjeto = 1;
            inimigo->velocidade = 0.0f; //Reduzindo a velocidade do player para 0, para freiar ele
            j->y = objeto->retangulo.y; //Atualiza a variável do movimento
        }

        //Condição de colisão em objetos Universais
        if (objeto->colisao)
        {
            if (VerificaColisaoBordasED(inimigo->posicao, TAMANHO_MINION_X, TAMANHO_MINION_Y, objeto->retangulo) == 1)
            {
                inimigo->direcao_movimento = 1;
            }
            else if (VerificaColisaoBordasED(inimigo->posicao, TAMANHO_MINION_X, TAMANHO_MINION_Y, objeto->retangulo) == 2)
            {
                inimigo->direcao_movimento = 0;
            }
        }
    }

    //Verifica a colisão entre o Poder e o Inimigo
    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        if (inimigo->tipo > 0)
        {
            //Desenho do inimigo
            Rectangle inimigoRect = {inimigo->posicao.x - TAMANHO_MINION_X / 2, inimigo->posicao.y - TAMANHO_MINION_Y, TAMANHO_MINION_X, TAMANHO_MINION_Y};

            if (CheckCollisionCircleRec(imune_19[p].posicao, imune_19[p].raio, inimigoRect) && imune_19[p].poder_ativo)
            {
                imune_19[p].poder_ativo = false; //Poder é desativado quando colide
                inimigo->tipo = 0;               //Inimigo morre
            }
        }
    }

    //Verifica colisão entre inimigo e jogador
    if (inimigo->tipo > 0)
    {
        //Verifica se jogador encosta nas bordas do objeto inimigo
        if (VerificaColisaoBordasED(jogador->posicao, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR, ret_inimigo) != 0)
        {
            jogador->vida -= 1; //Jogador encosta em inimigo e perde vida
        }
        //Verifica se borda superior do inimigo encosta em objeto jogador
        else if (VerificaColisaoBordaS(inimigo->posicao, TAMANHO_MINION_X, TAMANHO_MINION_Y, ret_jogador))
        {
            inimigo->tipo = 0; //Jogador mata o inimigo
        }
    }

    if (!colisaoObjeto) //Se não há colisão com objeto
    {
        inimigo->posicao.y += inimigo->velocidade * delta; //Aumentar a posição do Y do inimigo
        inimigo->velocidade += GRAVIDADE * delta;          //Vai sofrer com a Gravidade
    }
}

void AnimacaoJogadorMovimento(Jogador *jogador, Personagem *personagem, float deltaTime)
{
    personagem->framesCounter++; //Atualiza o valor da frame do jogo

    if (personagem->framesCounter % 2 == 0) personagem->currentFrame = 1;
    else personagem->currentFrame = 2; //Controle da alternância dos passos

    if ((personagem->framesCounter >= (t/personagem->framesSpeed)) && personagem->framesCounter % 2 == 1) //Altera as FPS do jogo para a desejada para a movimentação do jogador
    {
        personagem->framesCounter = 0;
        personagem->framesSpeed += 0.4;
        if((float)s > (float)sc + 60) personagem->framesSpeed += 0.1;
        if((float)s > (float)sc + 5*60) personagem->framesSpeed += 0.1;
        
        //Jogador
        if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && personagem->currentFrame == 1 && jogador->vida > 0) //Passo 1 esquerda
        {
            personagem->posicao.x = 140 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 2*personagem->frameWidth;
            personagem->frameRect.y = 2*personagem->frameHeight;
        }
        if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && personagem->currentFrame == 2 && jogador->vida > 0) //Passo 2 esquerda
        {
            personagem->posicao.x = 140 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 0.0f;
            personagem->frameRect.y = 3*personagem->frameHeight;
        }
        if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && personagem->currentFrame == 1 && jogador->vida > 0) //Passo 1 direita
        {
            personagem->posicao.x = 116 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 0.0f;
            personagem->frameRect.y = personagem->frameHeight;
        }
        if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && personagem->currentFrame == 2 && jogador->vida > 0) //Passo 2 direita
        {
            personagem->posicao.x = 116 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 2*personagem->frameWidth;
            personagem->frameRect.y = personagem->frameHeight;
        }

        if ((IsKeyDown(KEY_UP) && jogador->direcao_movimento == 0) ||
        (jogador->podePular == false && IsKeyDown(KEY_LEFT) && jogador->vida > 0)) //Pulo esquerda
        {
            personagem->posicao.x = 140 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 2*personagem->frameWidth;
            personagem->frameRect.y = 2*personagem->frameHeight;
        }
        if ((IsKeyDown(KEY_UP) && jogador->direcao_movimento == 1) ||
        (jogador->podePular == false && IsKeyDown(KEY_RIGHT) && jogador->vida > 0)) //Pulo direita
        {
            personagem->posicao.x = 116 - TAMANHO_X_JOGADOR;
            personagem->frameRect.x = 0.0f;
            personagem->frameRect.y = personagem->frameHeight;
        }
    }
}

void AnimacaoInimigo(Inimigo *inimigo, Frame_inimigo *frameInimigoT1, Frame_inimigo *frameInimigoT2, Texture2D spritesMinion, Texture2D spritesGado, int tamanhoInimigos, float deltaTime)
{
    if (inimigo->tipo == 1)
    {
        frameInimigoT1->texture = (Texture2D)spritesMinion;
        frameInimigoT1->frameWidth = frameInimigoT1->texture.width / 2; //Largura da sprite
        frameInimigoT1->frameHeight = frameInimigoT1->texture.height / 2; //Altura da sprite
        frameInimigoT1->frameRect = (Rectangle){0.0f, 0.0f, frameInimigoT1->frameWidth, frameInimigoT1->frameHeight}; //Sprite inicial
        frameInimigoT1->posicao.x = 146 - TAMANHO_MINION_X; //Posição x do personagem em relação à posição x do inimigo tipo 1
        frameInimigoT1->posicao.y = 265 - TAMANHO_MINION_Y; //Posição y do personagem em relação à posição y do inimigo tipo 1
    }

    if (inimigo->tipo == 2)
    {
        frameInimigoT2->texture = (Texture2D)spritesGado;
        frameInimigoT2->frameWidth = frameInimigoT2->texture.width / 2; //Largura da sprite
        frameInimigoT2->frameHeight = frameInimigoT2->texture.height / 2; //Altura da sprite
        frameInimigoT2->frameRect = (Rectangle){0.0f, frameInimigoT2->frameHeight, frameInimigoT2->frameWidth, frameInimigoT2->frameHeight}; //Sprite inicial
        frameInimigoT2->posicao.x = 283 - TAMANHO_GADO_X; //Posição x do personagem em relação à posição x do inimigo tipo 2
        frameInimigoT2->posicao.y = 253 - TAMANHO_GADO_Y; //Posição y do personagem em relação à posição y do inimigo tipo 2
    }
    
    frameInimigoT1->framesCounter++;

    if (frameInimigoT1->framesCounter % 2 == 0) frameInimigoT1->currentFrame = 1;
    else frameInimigoT1->currentFrame = 2; //Controle da alternância dos passos

    if (frameInimigoT1->framesCounter >= (t/frameInimigoT1->framesSpeed)) //Altera as FPSs do jogo para a desejada para a movimentação do inimigo
    {
        //Minions
        if (inimigo->tipo == 1)
        {
            if (inimigo->direcao_movimento == 0 && frameInimigoT1->currentFrame == 1) //Passo 1 esquerda
            {
                frameInimigoT1->posicao.x = 146 - TAMANHO_MINION_X;
                frameInimigoT1->frameRect.x = 0.0f;
                frameInimigoT1->frameRect.y = 0.0f;
            }
            if (inimigo->direcao_movimento == 0 && frameInimigoT1->currentFrame == 2) //Passo 2 esquerda
            {
                frameInimigoT1->posicao.x = 146 - TAMANHO_MINION_X;
                frameInimigoT1->frameRect.x = frameInimigoT1->frameWidth;
                frameInimigoT1->frameRect.y = 0.0f;
            }
            if (inimigo->direcao_movimento == 1 && frameInimigoT1->currentFrame == 1) //Passo 1 direita
            {
                frameInimigoT1->posicao.x = 159 - TAMANHO_MINION_X;
                frameInimigoT1->frameRect.x = 0.0f;
                frameInimigoT1->frameRect.y = frameInimigoT1->frameHeight;
            }
            if (inimigo->direcao_movimento == 1 && frameInimigoT1->currentFrame == 2) //Passo 2 direita
            {
                frameInimigoT1->posicao.x = 159 - TAMANHO_MINION_X;
                frameInimigoT1->frameRect.x = frameInimigoT1->frameWidth;
                frameInimigoT1->frameRect.y = frameInimigoT1->frameHeight;
            }
        }
    }

    frameInimigoT2->framesCounter++;

    if (frameInimigoT2->framesCounter % 2 == 0) frameInimigoT2->currentFrame = 1;
    else frameInimigoT2->currentFrame = 2; //Controle da alternância dos passos

    if (frameInimigoT2->framesCounter >= (t/frameInimigoT2->framesSpeed)) //Altera as FPSs do jogo para a desejada para a movimentação do inimigo
    {
        if (inimigo->tipo == 2)
        {
            if (inimigo->direcao_movimento == 0 && frameInimigoT2->currentFrame == 1) //Passo 1 esquerda
            {
                frameInimigoT2->frameRect.x = frameInimigoT2->frameWidth;
                frameInimigoT2->frameRect.y = frameInimigoT2->frameHeight;
            }
            if (inimigo->direcao_movimento == 0 && frameInimigoT2->currentFrame == 2) //Passo 2 esquerda
            {
                frameInimigoT2->frameRect.x = 0.0f;
                frameInimigoT2->frameRect.y = frameInimigoT2->frameHeight;
            }
            if (inimigo->direcao_movimento == 1 && frameInimigoT2->currentFrame == 1) //Passo 1 direita
            {
                frameInimigoT2->frameRect.x = frameInimigoT2->frameWidth;
                frameInimigoT2->frameRect.y = 0.0f;                    
            }
            if (inimigo->direcao_movimento == 1 && frameInimigoT2->currentFrame == 2) //Passo 2 direita
            {                    
                frameInimigoT2->frameRect.x = 0.0f;
                frameInimigoT2->frameRect.y = 0.0f;
            }
        }
    }
}

void AnimacaoJogadorParado(Jogador *jogador, Personagem *personagem, float delta)
{
    if (jogador->direcao_movimento == 0 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && jogador->vida > 0) //Parado esquerda
    {
        personagem->posicao.x = 140 - TAMANHO_X_JOGADOR;
        personagem->frameRect.x = 0.0f;
        personagem->frameRect.y = 2*personagem->frameHeight;
    }
    if (jogador->direcao_movimento == 1 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && jogador->vida > 0) //Parado direita
    {
        personagem->posicao.x = 116 - TAMANHO_X_JOGADOR;
        personagem->frameRect.x = 2*personagem->frameWidth;
        personagem->frameRect.y = 0.0f;
    }
    
    if (jogador->vida <= 0 && jogador->vida >= -3) //Pulo depois da morte
    {
        personagem->posicao.x = 120 - TAMANHO_X_JOGADOR;
        personagem->frameRect.x = personagem->frameHeight;
        personagem->frameRect.y = 0.0f;
        jogador->velocidade = -JOGADOR_PULO_VELOCIDADE;
        jogador->posicao.y += jogador->velocidade * delta; //Aumentar a posição do Y do jogador
    } 
    if (jogador->vida < -3 && jogador->posicao.y == TAMANHO_Y_CENARIO/2) //Caída
    {
        updateplayer = 0;
        jogador->posicao.y += 2* jogador->velocidade * delta;
        jogador->velocidade += GRAVIDADE * delta; //Vai sofrer com a Gravidade
    }
}

void Draw(Camera2D camera, EnvItem *envItems, int envItemsLength, int tamanhoInimigo, Inimigo *inimigo, Jogador *jogador, Personagem *personagem, Frame_inimigo *framesInimigoT1, Frame_inimigo *framesInimigoT2, Texture2D spritesMinion, Texture2D spritesGado)
{
    BeginDrawing();

    //Desenho do Background do restante da Janela que não é objeto
    ClearBackground(LIGHTGRAY);

    BeginMode2D(camera);

    //Desenho dos Retângulos referentes aos obstáculos de EnvItems
    for (int i = 0; i < envItemsLength; i++)
        DrawRectangleRec(envItems[i].retangulo, envItems[i].cor);

    for (int i = 0; i < tamanhoInimigo; i++)
    {
        if (inimigo[i].tipo == 1)
        {
            //Desenho da hitbox do inimigo
            DrawRectangleLines(inimigo[i].posicao.x - TAMANHO_MINION_X / 2, inimigo[i].posicao.y - TAMANHO_MINION_Y, TAMANHO_MINION_X, TAMANHO_MINION_Y, YELLOW);

            //Desenho da textura dos minions
            DrawTextureRec(framesInimigoT1->texture, framesInimigoT1->frameRect, (Vector2){inimigo[i].posicao.x - (framesInimigoT1->posicao.x - TAMANHO_MINION_X), inimigo->posicao.y - (framesInimigoT1->posicao.y - TAMANHO_MINION_Y)}, RAYWHITE);
        }
        if (inimigo[i].tipo == 2)
        {
            //Desenho da hitbox do inimigo
            DrawRectangleLines(inimigo[i].posicao.x - TAMANHO_GADO_X / 2, inimigo[i].posicao.y - TAMANHO_GADO_Y, TAMANHO_GADO_X, TAMANHO_GADO_Y, ORANGE);

            //Desenho da textura dos gados
            DrawTextureRec(framesInimigoT2->texture, framesInimigoT2->frameRect, (Vector2){inimigo[i].posicao.x - (framesInimigoT2->posicao.x - TAMANHO_GADO_X), inimigo[i].posicao.y - (framesInimigoT2->posicao.y - TAMANHO_GADO_Y)}, RAYWHITE);
        }
    }

    //Desenho circular do poder "IMUNE_19"
    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        if (imune_19[p].poder_ativo)
        {
            DrawCircleV(imune_19[p].posicao, imune_19[p].raio, BLACK);
        }
    }

    //Criação e Desenho do jogador

    //Desenho da hitbox do jogador
    DrawRectangleLines(jogador->posicao.x - TAMANHO_X_JOGADOR / 2, jogador->posicao.y - TAMANHO_Y_JOGADOR, TAMANHO_X_JOGADOR, TAMANHO_Y_JOGADOR, RED);
    //Desenho da textura do jogador
    DrawTextureRec(personagem->texture, personagem->frameRect, (Vector2){jogador->posicao.x - (personagem->posicao.x + TAMANHO_X_JOGADOR), jogador->posicao.y - (personagem->posicao.y + TAMANHO_Y_JOGADOR)}, RAYWHITE);

    DrawText(FormatText("Colisão : %01i", colisaoJogador), 1000, 450, 20, BLACK);

    DrawText(FormatText("Exemplo de Inimigo"), 1650, 450, 20, BLACK);
    DrawText(FormatText("Vida Jogador: %01i",jogador->vida), 1650, 475, 20, BLACK);

    DrawText(FormatText("Exemplo de Gado"), 2050, 450, 20, BLACK);
    DrawText(FormatText("Vida Jogador: %01i",jogador->vida), 2050, 475, 20, BLACK);

    EndMode2D();

    EndDrawing();
}

void UpdatePoder(Poder *imune_19, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta){


    //Acionamento do poder IMUNE_19
    if (IsKeyPressed(KEY_SPACE)) {                                                               
        for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)  //Configuração do "imune_19" quando desativado
        {
            if (!imune_19[p].poder_ativo && jogador->direcao_movimento == 1) //Caso jogador esteja indo para a DIREITA
            {
                imune_19[p].posicao = (Vector2){jogador->posicao.x + (TAMANHO_X_JOGADOR/2), jogador->posicao.y - (TAMANHO_Y_JOGADOR/2)}; //Posição inicial do poder é o centro do jogador
                imune_19[p].direcao_movimento = jogador->direcao_movimento; //Poder tem direção baseada na do jogador, porém fica independente quando emitido
                imune_19[p].poder_ativo = true;                             
                break;   
            }
            else if (!imune_19[p].poder_ativo && jogador->direcao_movimento == 0) //Caso jogador esteja indo para a ESQUERDA
            {
                imune_19[p].posicao = (Vector2){jogador->posicao.x - (TAMANHO_X_JOGADOR/2), jogador->posicao.y - (TAMANHO_Y_JOGADOR/2)}; //Centro do jogador
                imune_19[p].direcao_movimento = jogador->direcao_movimento;
                imune_19[p].poder_ativo = true;                             
                break;   
            }
        }
    }
    
    //Movimentação do poder
    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        if (imune_19[p].direcao_movimento == 1)  //Considerando a direção do poder para a DIREITA:
        {
            imune_19[p].posicao.x += PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na DIREITA
        } else if (imune_19[p].direcao_movimento == 0) //Considerando a direção do poder para a ESQUERDA:
        {
            imune_19[p].posicao.x -= PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na ESQUERDA
        }

        //Colisão do poder com os objetos do cenário (inimigos não contam aqui)
        for (int o = 0; o < envItemsLength; o++)
        {
            if (envItems[o].colisao //Se houver um objeto colidível
                && CheckCollisionCircleRec(imune_19[p].posicao, imune_19[p].raio, envItems[o].retangulo))  //e a colisão for entre o poder
            {                                                                                               // (círculo) e um retângulo
                imune_19[p].poder_ativo = false; //O poder é dasativado ("desaparece" do cenário)                                                 
            }
        }
        
        //Limite da área de movimento do poder
        if (imune_19[p].posicao.x < imune_19[p].raio) //Limite até o fim do cenário (lado ESQUERDO)
        {
            imune_19[p].poder_ativo = false; //Poder é desativado
        } else if (imune_19[p].posicao.x + imune_19[p].raio > TAMANHO_X_CENARIO) //Limite até o fim do cenário (lado DIREITO)
        {
            imune_19[p].poder_ativo = false; //Poder é desativado
        }
        
    }
}

void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    camera->offset = (Vector2){width / 2, height / 2};
    camera->target = jogador->posicao;
}

/*
Verifica se há colisão com as bordas Esquerda e Direita de uma Entidade com um objeto
Retorna 0 se não há colisão
Retorna 1 se há colisão com borda esquerda
Retorna 2 se há colisão com borda direita
*/
int VerificaColisaoBordasED(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto) {
    const float ponto_superior_esquerda = entidade.x - (tamanho_entidade_x / 2);
    const float ponto_superior_direita = entidade.x + (tamanho_entidade_x / 2);
    const float ponto_superior = entidade.y - tamanho_entidade_y + 1;
    const float ponto_inferior = entidade.y - 1;

    //Verifica a reta (conjunto de ~infinitos~ pontos) do ponto superior ao inferior
    for (float ponto = ponto_superior; ponto <= ponto_inferior; ponto++)
    {
        //Verifica se há colisão com borda/reta esquerda
        if (CheckCollisionPointRec((Vector2){ponto_superior_esquerda,ponto},objeto))
        {
            return 1;
        }
        //Verifica se há colisão com borda/reta direita
        if (CheckCollisionPointRec((Vector2){ponto_superior_direita,ponto},objeto))
        {
            return 2;
        }
    }

    return 0;
}

/*
Verifica se há colisão com a borda superior de uma Entidade com um objeto
Retorna 0 se não há colisão
Retorna 1 se há colisão com borda superior
*/
bool VerificaColisaoBordaS(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto) {
    Vector2 ponto_superior_esquerda = (Vector2){entidade.x - (tamanho_entidade_x / 2) + 5, entidade.y - tamanho_entidade_y - 1};
    Vector2 ponto_superior_direita = (Vector2){entidade.x + (tamanho_entidade_x / 2) - 5, entidade.y - tamanho_entidade_y - 1};

    //Verifica a colisão entre 2 pontos superiores da entidade
    if (CheckCollisionPointRec(ponto_superior_esquerda,objeto) ||
        CheckCollisionPointRec(ponto_superior_direita,objeto))
    {
        return 1;
    } 
    else
    {
        return 0;
    }
}

/*
Verifica se o jogador entra no range superior do Gado
Retorna 0 se não há jogador no range
Retorna 1 se há jogador no range a esquerda
Retorna 2 se há jogador no range a direita
*/
int VerificaRangeGado(Vector2 posicao_inicial, float tamanho_gado_x, float tamanho_gado_y, Rectangle jogador, float range) {
    const float ponto_inicial_range_y = posicao_inicial.y + (tamanho_gado_y / 2); //Pega a posição central do retângulo do gadinho
    const float ponto_inicial_range_x_esquerda = posicao_inicial.x - (tamanho_gado_x / 2);
    const float ponto_inicial_range_x_direita = posicao_inicial.x + (tamanho_gado_x / 2);

    //Verifica a reta que sai do ponto até o range
    for (float ponto = 0; ponto <= range; ponto++)
    {
        //Verifica se há colisão no range a esquerda do jogador
        if (CheckCollisionPointRec((Vector2){ponto_inicial_range_x_esquerda - ponto, ponto_inicial_range_y},jogador))
        {
            return 1;
        }
        //Verifica se há colisão no range a direita do jogador
        if (CheckCollisionPointRec((Vector2){ponto_inicial_range_x_direita + ponto, ponto_inicial_range_y},jogador))
        {
            return 2;
        }
    }

    return 0;
}
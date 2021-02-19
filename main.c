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
    Vector2 tamanho;
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
} Personagem;

typedef struct Minions
{
    Vector2 posicao;
    Texture2D texture;
    float frameWidth;
    float frameHeight;
    Rectangle frameRect;
} Minions;

typedef struct Gados
{
    Vector2 posicao;
    Texture2D texture;
    float frameWidth;
    float frameHeight;
    Rectangle frameRect;
} Gados;

typedef struct FPS_Animacao
{
    int counter;
    float speed;
    int currentFrame;
} FPS_Animacao;

int updateplayer;
clock_t t;
time_t s;
time_t sc;

static Poder imune_19[PODER_MAX_PERSONAGEM] = {0}; //A variável inicializa zerada em suas posições

//Protótipo das funções

void UpdatePlayer(Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void UpdateInimigo(Inimigo *inimigo, EnvItem *envItems, Jogador *jogador, int tamanhoInimigos, int envItemsLength, float delta);
void UpdatePoder(Poder *imune_19, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void AnimacaoJogadorMovimento(FPS_Animacao *frames, Jogador *jogador,Personagem *personagem, Inimigo *inimigo, Minions *minions, int tamanhoInimigos, float deltaTime);
void AnimacaoInimigo(FPS_Animacao *frames, Inimigo *inimigo, Minions *minions, Gados *gados, int tamanhoInimigos, float deltaTime);
void AnimacaoJogadorParado(Jogador *jogador, Personagem *personagem, float delta);
void Draw(Camera2D camera, EnvItem *envItems, int envItemsLength, int tamanhoInimigo, Inimigo *inimigo, Minions *minions, Gados *gados, Jogador *jogador, Personagem *personagem);
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
    jogador.tamanho = (Vector2){TAMANHO_JOGADOR_X,TAMANHO_JOGADOR_Y};
    jogador.posicao = (Vector2){400, 280}; //Posição Inicial
    jogador.velocidade = 0; //Velocidade Inicial
    jogador.podePular = false; //Habilitação de pulo
    jogador.vida = 1;
    jogador.direcao_movimento = 1;


    
    //Configurações Iniciais da animação do joagdor
    FPS_Animacao frames;
    frames.counter = 0; //Conta as FPS
    frames.speed = 12;  //FPS da animação
    frames.currentFrame = 0; //Controla a passagem de frames
    Personagem personagem;
    Texture2D spritesPersonagem = LoadTexture("sprites/companheiro-da-silva.png"); //Carregamento da sprite sheet
    personagem.texture = (Texture2D)spritesPersonagem;
    personagem.frameWidth = personagem.texture.width / 4; //Largura da sprite
    personagem.frameHeight = personagem.texture.height / 4; //Altura da sprite
    personagem.frameRect = (Rectangle){2*personagem.frameWidth, 0.0f, personagem.frameWidth, personagem.frameHeight}; //Sprite inicial
    personagem.posicao.x = 116 - jogador.tamanho.x; //Posiçâo x do personagem em relação à posição x do jogador
    personagem.posicao.y = 190 - jogador.tamanho.y; //Posiçâo y do personagem em relação à posição y do jogador

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

    //Configurações iniciais da animação dos minions
    Minions minions;
    Texture2D spritesMinion = LoadTexture("sprites/minion.png"); //Carregamento da sprite sheet
    minions.texture = (Texture2D)spritesMinion;
    minions.frameWidth = minions.texture.width / 2; //Largura da sprite
    minions.frameHeight = minions.texture.height / 2; //Altura da sprite
    minions.frameRect = (Rectangle){0.0f, 0.0f, minions.frameWidth, minions.frameHeight}; //Sprite inicial
    minions.posicao.x = 146 - TAMANHO_MINION_X; //Posição x do personagem em relação à posição x do inimigo tipo 1
    minions.posicao.y = 241 - TAMANHO_MINION_Y; //Posição y do personagem em relação à posição y do inimigo tipo 1

    //Configurações iniciais da animação dos gados
    Gados gados;
    Texture2D spritesGado = LoadTexture("sprites/gado.png"); //Carregamento da sprite sheet
    gados.texture = (Texture2D)spritesGado;
    gados.frameWidth = gados.texture.width / 2; //Largura da sprite
    gados.frameHeight = gados.texture.height / 2; //Altura da sprite
    gados.frameRect = (Rectangle){0.0f, gados.frameHeight, gados.frameWidth, gados.frameHeight}; //Sprite inicial
    gados.posicao.x = 283 - TAMANHO_GADO_X; //Posição x do personagem em relação à posição x do inimigo tipo 2
    gados.posicao.y = 287 - TAMANHO_GADO_Y; //Posição y do personagem em relação à posição y do inimigo tipo 2

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
        }

        //Atualiza a animação do jogador quando o jogador está em movimento
        AnimacaoJogadorMovimento(&frames, &jogador, &personagem, inimigo, &minions, tamanhoInimigo, deltaTime);

        //Atualiza a animação do inimigo
        AnimacaoInimigo(&frames, inimigo, &minions, &gados, tamanhoInimigo, deltaTime);
      
        //Atualiza os dados do poder
        UpdatePoder(imune_19, &jogador, envItems, envItemsLength, deltaTime);      

        //Atualiza a Câmera focada no jogador
        UpdateCameraCenter(&camera, &jogador, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------

        //Desenho circular do poder "IMUNE_19"
        Draw(camera, envItems, envItemsLength, tamanhoInimigo, inimigo, &minions, &gados, &jogador, &personagem);

        //----------------------------------------------------------------------------------
      
        //Atualiza a animação quando o jogador está parado
        AnimacaoJogadorParado(&jogador, &personagem, deltaTime);
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------

    //Descarregamento da sprite sheet do jogador
    UnloadTexture(personagem.texture); 
    UnloadTexture(minions.texture);
    UnloadTexture(gados.texture);

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
    if ((jogador->posicao.x + jogador->tamanho.x / 2) > TAMANHO_X_CENARIO)
    {
        jogador->posicao.x = TAMANHO_X_CENARIO - jogador->tamanho.x / 2; //Limites para direita
    } else if (jogador->posicao.x < jogador->tamanho.x / 2)
    {
        jogador->posicao.x = jogador->tamanho.x / 2; //Limites para a esquerda
    }
    
    if ((jogador->posicao.y) > TAMANHO_Y_CENARIO)  //Limites na vertical
    {
        jogador->posicao.y = TAMANHO_Y_CENARIO; 
    } else if (jogador->posicao.y < jogador->tamanho.y)
    {
        jogador->posicao.y = jogador->tamanho.y;
    }

    colisaoJogador = 0;
    int colisaoObjeto = 0;
    for (int i = 0; i < envItemsLength; i++) //Preechimento da área dos pixels dos objetos colidiveis
    {
        EnvItem *objeto = envItems + i;
        Vector2 *j = &(jogador->posicao);
        
        //Condição de colisão para pulo e andar encima de plataformas
        if (objeto->colisao &&                                             
            objeto->retangulo.x - jogador->tamanho.x/2 <= j->x &&                                      //
            objeto->retangulo.x + objeto->retangulo.width + jogador->tamanho.x/2 >= j->x &&            // Definindo a invasão da área do player com a área do objeto(área de colisão)   
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
            if (VerificaColisaoBordaS(jogador->posicao, jogador->tamanho.x, jogador->tamanho.y, objeto->retangulo))
            {
                jogador->posicao.y = objeto->retangulo.y + objeto->retangulo.height + jogador->tamanho.y + 1;
                jogador->velocidade = GRAVIDADE * delta;
            } 
            else if (VerificaColisaoBordasED(jogador->posicao, jogador->tamanho.x, jogador->tamanho.y, objeto->retangulo) == 1)
            {
                jogador->posicao.x = objeto->retangulo.x + objeto->retangulo.width + jogador->tamanho.x / 2;
            }
            else if (VerificaColisaoBordasED(jogador->posicao, jogador->tamanho.x, jogador->tamanho.y, objeto->retangulo) == 2)
            {
                jogador->posicao.x = objeto->retangulo.x - jogador->tamanho.x / 2;
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
    Rectangle ret_jogador = {jogador->posicao.x - (jogador->tamanho.x / 2),jogador->posicao.y - jogador->tamanho.y,jogador->tamanho.x,jogador->tamanho.y};
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
        Rectangle ret_jogador = {jogador->posicao.x, jogador->posicao.y, jogador->tamanho.x, jogador->tamanho.y};
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
        if (VerificaColisaoBordasED(jogador->posicao, jogador->tamanho.x, jogador->tamanho.y, ret_inimigo) != 0)
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

void AnimacaoJogadorMovimento(FPS_Animacao *frames, Jogador *jogador, Personagem *personagem, Inimigo *inimigo, Minions *minions, int tamanhoInimigos, float deltaTime)
{
    frames->counter++; //Atualiza o valor da frame do jogo

    if (frames->counter % 2 == 0) frames->currentFrame = 1;
    else frames->currentFrame = 2; //Controle da alternância dos passos

    if ((frames->counter >= (t/frames->speed)) && frames ->counter % 2 == 1) //Altera as FPS do jogo para a desejada para a movimentação do jogador
    {
        frames->counter = 0;
        frames->speed += 0.5;
        if((float)s > (float)sc + 60) frames->speed += 0.1;
        if((float)s > (float)sc + 4*60) frames->speed += 0.1;
        
        //Jogador
        if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && frames->currentFrame == 1 && jogador->vida > 0) //Passo 1 esquerda
        {
            personagem->posicao.x = 140 - jogador->tamanho.x;
            personagem->frameRect.x = 2*personagem->frameWidth;
            personagem->frameRect.y = 2*personagem->frameHeight;
        }
        if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && frames->currentFrame == 2 && jogador->vida > 0) //Passo 2 esquerda
        {
            personagem->posicao.x = 140 - jogador->tamanho.x;
            personagem->frameRect.x = 0.0f;
            personagem->frameRect.y = 3*personagem->frameHeight;
        }
        if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && frames->currentFrame == 1 && jogador->vida > 0) //Passo 1 direita
        {
            personagem->posicao.x = 116 - jogador->tamanho.x;
            personagem->frameRect.x = 0.0f;
            personagem->frameRect.y = personagem->frameHeight;
        }
        if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && frames->currentFrame == 2 && jogador->vida > 0) //Passo 2 direita
        {
            personagem->posicao.x = 116 - jogador->tamanho.x;
            personagem->frameRect.x = 2*personagem->frameWidth;
            personagem->frameRect.y = personagem->frameHeight;
        }

        if ((IsKeyDown(KEY_UP) && jogador->direcao_movimento == 0) ||
        (jogador->podePular == false && IsKeyDown(KEY_LEFT) && jogador->vida > 0)) //Pulo esquerda
        {
            personagem->posicao.x = 140 - jogador->tamanho.x;
            personagem->frameRect.x = 2*personagem->frameWidth;
            personagem->frameRect.y = 2*personagem->frameHeight;
        }
        if ((IsKeyDown(KEY_UP) && jogador->direcao_movimento == 1) ||
        (jogador->podePular == false && IsKeyDown(KEY_RIGHT) && jogador->vida > 0)) //Pulo direita
        {
            personagem->posicao.x = 116 - jogador->tamanho.x;
            personagem->frameRect.x = 0.0f;
            personagem->frameRect.y = personagem->frameHeight;
        }
    }
}

void AnimacaoInimigo(FPS_Animacao *frames, Inimigo *inimigo, Minions *minions, Gados *gados, int tamanhoInimigos, float deltaTime)
{
    if ((frames->counter >= (t/frames->speed)) && frames ->counter % 2 == 1) //Altera as FPSs do jogo para a desejada para a movimentação do inimigo
    {
        //Minions
        for (int i = 0; i < tamanhoInimigos; i++)
        {
            inimigo += i;
            if (inimigo->tipo == 1)
            {
                if (inimigo->direcao_movimento == 0 && frames->currentFrame == 1) //Passo 1 esquerda
                {
                    minions->posicao.x = 146 - TAMANHO_MINION_X;
                    minions->frameRect.x = 0.0f;
                    minions->frameRect.y = 0.0f;
                }
                if (inimigo->direcao_movimento == 0 && frames->currentFrame == 2) //Passo 2 esquerda
                {
                    minions->posicao.x = 146 - TAMANHO_MINION_X;
                    minions->frameRect.x = minions->frameWidth;
                    minions->frameRect.y = 0.0f;
                }
                if (inimigo->direcao_movimento == 1 && frames->currentFrame == 1) //Passo 1 direita
                {
                    minions->posicao.x = 159 - TAMANHO_MINION_X;
                    minions->frameRect.x = 0.0f;
                    minions->frameRect.y = minions->frameHeight;
                }
                if (inimigo->direcao_movimento == 1 && frames->currentFrame == 2) //Passo 2 direita
                {
                    minions->posicao.x = 159 - TAMANHO_MINION_X;
                    minions->frameRect.x = minions->frameWidth;
                    minions->frameRect.y = minions->frameHeight;
                }
            }
            if (inimigo->tipo == 2)
            {
                if (inimigo->direcao_movimento == 0 && frames->currentFrame == 1) //Passo 1 esquerda
                {
                    gados->frameRect.x = gados->frameWidth;
                    gados->frameRect.y = gados->frameHeight;
                }
                if (inimigo->direcao_movimento == 0 && frames->currentFrame == 2) //Passo 2 esquerda
                {
                    gados->frameRect.x = 0.0f;
                    gados->frameRect.y = gados->frameHeight;
                }
                if (inimigo->direcao_movimento == 1 && frames->currentFrame == 1) //Passo 1 direita
                {
                    gados->frameRect.x = gados->frameWidth;
                    gados->frameRect.y = 0.0f;                    
                }
                if (inimigo->direcao_movimento == 1 && frames->currentFrame == 2) //Passo 2 direita
                {                    
                    gados->frameRect.x = 0.0f;
                    gados->frameRect.y = 0.0f;
                }
            }
        }
    }
}

void AnimacaoJogadorParado(Jogador *jogador, Personagem *personagem, float delta)
{
    if (jogador->direcao_movimento == 0 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && jogador->vida > 0) //Parado esquerda
    {
        personagem->posicao.x = 140 - jogador->tamanho.x;
        personagem->frameRect.x = 0.0f;
        personagem->frameRect.y = 2*personagem->frameHeight;
    }
    if (jogador->direcao_movimento == 1 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && jogador->vida > 0) //Parado direita
    {
        personagem->posicao.x = 116 - jogador->tamanho.x;
        personagem->frameRect.x = 2*personagem->frameWidth;
        personagem->frameRect.y = 0.0f;
    }
    
    if (jogador->vida <= 0 && jogador->vida >= -3) //Pulo depois da morte
    {
        personagem->posicao.x = 120 - jogador->tamanho.x;
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

void Draw(Camera2D camera, EnvItem *envItems, int envItemsLength, int tamanhoInimigo, Inimigo *inimigo, Minions *minions, Gados *gados, Jogador *jogador, Personagem *personagem)
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
            DrawTextureRec(minions->texture, minions->frameRect, (Vector2){inimigo[i].posicao.x - (minions->posicao.x - TAMANHO_MINION_X), inimigo[i].posicao.y - (minions->posicao.y - TAMANHO_MINION_X)}, RAYWHITE);
        }
        if (inimigo[i].tipo == 2)
        {
            //Desenho da hitbox do inimigo
            DrawRectangleLines(inimigo[i].posicao.x - TAMANHO_GADO_X / 2, inimigo[i].posicao.y - TAMANHO_GADO_Y, TAMANHO_GADO_X, TAMANHO_GADO_Y, ORANGE);

            //Desenho da textura dos gados
            DrawTextureRec(gados->texture, gados->frameRect, (Vector2){inimigo[i].posicao.x - (gados->posicao.x - TAMANHO_GADO_X), inimigo[i].posicao.y - (gados->posicao.y - TAMANHO_GADO_X)}, RAYWHITE);
        }
    }

    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        if (imune_19[p].poder_ativo)
        {
            DrawCircleV(imune_19[p].posicao, imune_19[p].raio, BLACK);
        }
    }

    //Criação e Desenho do jogador

    //Desenho da hitbox do jogador
    DrawRectangleLines(jogador->posicao.x - jogador->tamanho.x / 2, jogador->posicao.y - jogador->tamanho.y, jogador->tamanho.x, jogador->tamanho.y, RED);
    //Desenho da textura do jogador
    DrawTextureRec(personagem->texture, personagem->frameRect, (Vector2){jogador->posicao.x - (personagem->posicao.x + jogador->tamanho.x), jogador->posicao.y - (personagem->posicao.y + jogador->tamanho.y)}, RAYWHITE);

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
                imune_19[p].posicao = (Vector2){jogador->posicao.x + (jogador->tamanho.x/2), jogador->posicao.y - (jogador->tamanho.y/2)}; //Posição inicial do poder é o centro do jogador
                imune_19[p].direcao_movimento = jogador->direcao_movimento; //Poder tem direção baseada na do jogador, porém fica independente quando emitido
                imune_19[p].poder_ativo = true;                             
                break;   
            }
            else if (!imune_19[p].poder_ativo && jogador->direcao_movimento == 0) //Caso jogador esteja indo para a ESQUERDA
            {
                imune_19[p].posicao = (Vector2){jogador->posicao.x - (jogador->tamanho.x/2), jogador->posicao.y - (jogador->tamanho.y/2)}; //Centro do jogador
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
#include "raylib.h"
#include "libraries/defines.c"
#include "time.h"
#include "cenario.h"
#include "inimigo.h"

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
    int direcao_movimento;
    int vida;
    int poder;
} Jogador;

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
    int direcao_movimento;
    Color cor;
} Poder;

/* colisao: tipo de bloco de colisão
colisao = 1 = bloco normal com colisão
colisao = 2 = bloco com poder imune-19 */
typedef struct IMUNE_19
{
    Vector2 posicao;
    Texture2D texture;
    Rectangle frameRect;
} IMUNE_19;

typedef struct Animacao
{
    Vector2 posicao;
    Texture2D texture;
    float frameWidth;
    float frameHeight;
    Rectangle frameRect;
    int framesCounter;
    float framesSpeed;
    int currentFrame;
    int poderCounter;
    int poderCurrentFrame;
} Animacao;

typedef enum 
{
    LOGO,
    INICIO,
    SELECT,
    INGAME
} GameScreen; // As diferentes telas do jogo

int updateplayer;
clock_t t;
time_t s;
time_t sc;

static Poder imune_19[PODER_MAX_PERSONAGEM] = {0}; //A variável inicializa zerada em suas posições
static Poder laranja[PODER_MAX_FABIO] = {0}; //A variável inicializa zerada em suas posições
static Poder dinheiro[PODER_MAX_TCHUTCHUCA] = {0}; //A variável inicializa zerada em suas posições
static Poder pocao[PODER_MAX_REI] = {0}; //A variável inicializa zerada em suas posições

unsigned int inimigo_cooldown_poder = 0;
unsigned int inimigo_cooldown_pulo = 0;
//Protótipo das funções

unsigned int jogador_tempo_poder_pocao52 = 0; //Variável inicializa zerada

//Protótipo das funções
void UpdatePlayer(Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta);
void UpdateInimigo(Inimigo *inimigo, EnvItem *envItems, Jogador *jogador, int tamanhoInimigos, int envItemsLength, float delta);
void UpdateBoss(Inimigo *inimigo, EnvItem *envItems, Jogador *jogador, int envItemsLength, float delta);
void UpdatePoder(Poder *imune_19, IMUNE_19 *imune, Jogador *jogador, Inimigo *boss, EnvItem *envItems, int envItemsLength, float delta, Texture2D spriteImune);
void AnimacaoJogadorMovimento(Jogador *jogador, Animacao *personagem, float deltaTime);
void AnimacaoInimigo(Inimigo *inimigo, Animacao *frameInimigoT1, Animacao *frameInimigoT2, Texture2D spritesMinion, Texture2D spritesGado, int tamanhoInimigos, float deltaTime);
void AnimacaoJogadorParado(Jogador *jogador, Animacao *personagem, float delta);
void Draw(Camera2D camera, EnvItem *envItems, int envItemsLength, int tamanhoInimigo, Inimigo *inimigo, Jogador *jogador, Animacao *personagem, Animacao *frameInimigoT1, Animacao *frameInimigoT2, Texture2D spritesMinion, Texture2D spritesGado, IMUNE_19 *imune, Inimigo *boss, float delta);
void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
int VerificaColisaoBordasED(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto);
bool VerificaColisaoBordaS(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto, int range);
int VerificaRangeGado(Vector2 posicao_inicial, float tamanho_gado_x, float tamanho_gado_y, Rectangle jogador, float range);
bool VerificaRangeDudu(Vector2 posicao_inicial, Vector2 tamanho, Rectangle ret_jogador, float range);
bool VerificaColisaoPoderPoder(Poder *poder1, Poder *poder2);
void IniciaCenario(Jogador *jogador, int cenario);
void CarregaObjetos(EnvItem *cenario, EnvItem *loadCenario, int tamanhoLoadCenario);
void CarregaInimigos(Inimigo *inimigo, InimigoData *loadInimigo);

int bossAtivo = 0; //Define qual o tipo de boss que deve estar ativo

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
    jogador.poder = 0;

    //Configurações Iniciais da animação do joagdor
    Animacao personagem;
    Texture2D spritesPersonagem = LoadTexture("sprites/companheiro-da-silva.png"); //Carregamento da sprite sheet
    personagem.texture = (Texture2D)spritesPersonagem;
    personagem.frameWidth = personagem.texture.width / 4; //Largura da sprite
    personagem.frameHeight = personagem.texture.height / 4; //Altura da sprite
    personagem.frameRect = (Rectangle){2*personagem.frameWidth, 0.0f, personagem.frameWidth, personagem.frameHeight}; //Sprite inicial
    personagem.posicao.x = 116 - jogador.tamanho.x; //Posiçâo x do personagem em relação à posição x do jogador
    personagem.posicao.y = 190 - jogador.tamanho.y; //Posiçâo y do personagem em relação à posição y do jogador
    
    //Configurações iniciais Boss
    Inimigo boss[] = {
        {},
        {1, {0}, {5800, 450}, 0, 0, 0, 0, 0},
        {2, {0}, {4800, 450}, 0, 0, 0, 0, 0},
        {3, {0}, {6000, 440}, 0, 0, 0, 0, 0},
        {4, {0}, {2450, 350}, 0, 0, 0, 0, 0}
    };
    const int tamanhoBoss = sizeof(boss) / sizeof(boss[0]);

    personagem.poderCounter = 0;
    personagem.poderCurrentFrame = 0;
    personagem.framesSpeed = 12;
    personagem.framesCounter = 0;
    personagem.currentFrame = 0;
    int tela_personagem = 1;
    
    Texture2D spritesPersonagem_john = LoadTexture("sprites/john-dorivac.png");
    Texture2D spritesPersonagem_cake = LoadTexture("sprites/dr-cake.png");
    Texture2D spritesPersonagem_comp = LoadTexture("sprites/companheiro-da-silva.png"); //Carregamento da sprite sheet 

    //Preenchimento dos valores do inimigo
    for (int i = 0; i < tamanhoBoss; i++)
    {
        if (boss[i].tipo == 1)
        {
            boss[i].tamanho = (Vector2){TAMANHO_DUDU_X,TAMANHO_DUDU_Y};
            boss[i].vida = 1;
            boss[i].cor = BLUE;
        }
        if (boss[i].tipo == 2)
        {
            boss[i].tamanho = (Vector2){TAMANHO_FABIO_X,TAMANHO_FABIO_Y};
            boss[i].vida = 1;
            boss[i].cor = BLUE;
        }
        if (boss[i].tipo == 3)
        {
            boss[i].tamanho = (Vector2){TAMANHO_TCHUTCHUCA_X,TAMANHO_TCHUTCHUCA_Y};
            boss[i].vida = 1;
            boss[i].cor = BLUE;
        }
        if (boss[i].tipo == 3)
        {
            boss[i].tamanho = (Vector2){TAMANHO_TCHUTCHUCA_X,TAMANHO_TCHUTCHUCA_Y};
            boss[i].vida = 1;
            boss[i].cor = BLUE;
        }
        if (boss[i].tipo == 4)
        {
            boss[i].tamanho = (Vector2){TAMANHO_REI_X,TAMANHO_REI_Y};
            boss[i].vida = 1;
            boss[i].cor = BLUE;
        }
    }

    //Configurações iniciais da animação dos inimigos
    Animacao frameInimigoT1[tamanhoInimigo];
    Texture2D spritesMinion = LoadTexture("sprites/minion.png"); //Carregamento da sprite sheet
    for (int i = 0; i < tamanhoInimigo; i++)
    {
        if (inimigo[i].tipo == 1)
        {
            frameInimigoT1[i].currentFrame = 0;
            frameInimigoT1[i].framesCounter = 0;
            frameInimigoT1[i].framesSpeed = 13;
        }
    } 
    Animacao frameInimigoT2[tamanhoInimigo];
    Texture2D spritesGado = LoadTexture("sprites/gado.png"); //Carregamento da sprite sheet
    for (int i = 0; i < tamanhoInimigo; i++)
    {
        if (inimigo[i].tipo == 2)
        {
            frameInimigoT2[i].currentFrame = 0;
            frameInimigoT2[i].framesCounter = 0;
            frameInimigoT2[i].framesSpeed = 13;
        }
    }
//Configurações iniciais da animação do poder "IMUNE_19"
    IMUNE_19 imune;
    Texture2D spriteImune = LoadTexture("sprites/seringas.png"); //Carregamento da sprite sheet
    imune.texture = spriteImune;
    imune.frameRect = (Rectangle){0.0f, imune.texture.height, imune.texture.width, imune.texture.height/2}; //Sprite
    //Configurações iniciais do poder "IMUNE_19"
    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        imune_19[p].posicao = (Vector2){0,0};
        imune_19[p].raio = 10;
        imune_19[p].cor = BLACK;
    }

    //Configurações iniciais do poder "LARANJA!"
    for (int p = 0; p < PODER_MAX_FABIO; p++)
    {
        laranja[p].posicao = (Vector2){0,0};
        laranja[p].raio = 10;
        laranja[p].cor = ORANGE;
    }

    //Configurações iniciais do poder "Dinheiro"
    for (int p = 0; p < PODER_MAX_FABIO; p++)
    {
        dinheiro[p].posicao = (Vector2){0,0};
        dinheiro[p].raio = 10;
        dinheiro[p].cor = ORANGE;
    }

    //Configurações iniciais do poder "Poção"
    for (int p = 0; p < PODER_MAX_REI; p++)
    {
        pocao[p].posicao = (Vector2){0,0};
        pocao[p].raio = 10;
        pocao[p].cor = BLUE;
    }

    //Configuração Inicial de Cenário
    int cenarioAtual = 0;
    IniciaCenario(&jogador,cenarioAtual);

    //Configurações Iniciais de Câmera
    Camera2D camera = {0};
    camera.target = jogador.posicao; //Câmera centralizada inicialmente no jogador
    camera.offset = (Vector2){screenWidth / 2, screenHeight / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    time(&sc); //Tempo no começo do jogo

    GameScreen gamescreen = LOGO; // A primeira tela é sempre a tela de LOGO da "empresa"
    int state = 0; //serve para diferenciar se o jogo está carregando a textura preta de transição o jogo ou outros estados
    float alpha = 0.0; //serve para mudar da textura preta de transicao para as outras texturas do jogo
    int framesCounter = 0;

    //TECLAS para as telas

    /*Carregando texturas*/
    Texture2D LOGO_img = LoadTexture("imagens_tela/LOGO_img.png");
    Texture2D INICIO_img = LoadTexture("imagens_tela/INICIO_img.png");
    Texture2D tela_companheiro = LoadTexture("imagens_tela/tela_companheiro.png");
    Texture2D tela_cake = LoadTexture("imagens_tela/tela_cake.png");
    Texture2D tela_dorivac = LoadTexture("imagens_tela/tela_dorivac.png");

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
        framesCounter++;

        switch (gamescreen)
        {
        case LOGO:
            // o logo tem uma transição com uma tela preta depois
            if (state == 0)
            {
                if (alpha < 1.0)
                    alpha += 0.05;
                else
                    state = 1;
            }
            else if (state == 1)
            {
                alpha = 1.0;
                if (framesCounter % 800 == 0)
                { // tempo que o logo aparece
                    state = 2;
                }
            }
            else if (state == 2)
            {
                if (alpha > 0.0)
                    alpha -= 0.05;
                else
                {
                   gamescreen = INICIO; //Mudar para tela de inicio
                    state = 0;           // setando o estado para 0 para ser usado depois em outra tela
                }
            }
            break;
            case INICIO:
            if (state == 0)
            { //transição
                if (alpha < 1.0)
                    alpha += 0.05;
                else
                    state = 1;
            }
            else if (state == 1)
            { //transição feita esperando o usuario apertar a tecla de inicio
                alpha = 1.0;

                if (IsKeyPressed(KEY_SPACE))
                {
                    gamescreen = SELECT;
                    state = 0;
                }
            }
            break;
        case SELECT:
            if (IsKeyPressed(KEY_RIGHT))
            {
                tela_personagem += 1;
            }
            else if (IsKeyPressed(KEY_LEFT))
            {
                tela_personagem -= 1;
            }
            if (tela_personagem > 2) 
            {
                tela_personagem = 0;
            }else if(tela_personagem < 0)
            {
                tela_personagem = 2;
            }   

            if (IsKeyPressed(KEY_SPACE))
            {
                switch (tela_personagem)
                {
                case 1:
                    personagem.texture = (Texture2D)spritesPersonagem_comp;
                    break;
                case 2:
                    personagem.texture = (Texture2D)spritesPersonagem_john;
                    break;
                case 0:
                    personagem.texture = (Texture2D)spritesPersonagem_cake;
                    break;
                }
                personagem.frameWidth = personagem.texture.width / 4;                                                               //Largura da sprite
                personagem.frameHeight = personagem.texture.height / 4;                                                             //Altura da sprite
                personagem.frameRect = (Rectangle){2 * personagem.frameWidth, 0.0f, personagem.frameWidth, personagem.frameHeight}; //Sprite inicial
                personagem.posicao.x = 116 - jogador.tamanho.x;                                                                     //Posiçâo x do personagem em relação à posição x do jogador
                personagem.posicao.y = 190 - jogador.tamanho.y;                                                                     //Posiçâo y do personagem em relação à posição y do jogador

                gamescreen = INGAME;
                state = 0;
            }
            break;
        case INGAME:
        
        jogador.posicaoAnterior = jogador.posicao; //Atualiza a posição anterior do jogador

        //Faz Transição de cenário de testes para Cenário 1
        if (jogador.posicao.x > 2000 && boss[bossAtivo].tipo == 0)
        {
            cenarioAtual += 1;
            IniciaCenario(&jogador, cenarioAtual);
        }

        //Atualiza os dados do jogador
        if(updateplayer == 1)
        {
            UpdatePlayer(&jogador, envItems, envItemsLength, deltaTime);
        }
        
        //Atualiza os dados dos inimigos
        for (int i = 0; i < tamanhoInimigo; i++)
        {
            UpdateInimigo(&(inimigo[i]), envItems, &jogador, tamanhoInimigo, envItemsLength, deltaTime);
            AnimacaoInimigo(&(inimigo[i]), &(frameInimigoT1[i]), &(frameInimigoT2[i]), spritesMinion, spritesGado, tamanhoInimigo, deltaTime); //Atualiza a animação do inimigo
        }

        //Atualiza os dados do Boss
        UpdateBoss(&(boss[bossAtivo]), envItems, &jogador, envItemsLength, deltaTime);

        //Atualiza a animação do jogador quando o jogador está em movimento
        AnimacaoJogadorMovimento(&jogador, &personagem, deltaTime);

        //Atualiza os dados do poder
        UpdatePoder(imune_19, &imune, &jogador, &(boss[bossAtivo]), envItems, envItemsLength, deltaTime, spriteImune);

        //Atualiza a Câmera focada no jogador
        UpdateCameraCenter(&camera, &jogador, envItems, envItemsLength, deltaTime, screenWidth, screenHeight);
        }
        // Draw
        BeginDrawing();


        //Desenho do Background do restante da Janela que não é objeto
        ClearBackground(LIGHTGRAY);
        BeginMode2D(camera);
        switch (gamescreen)
        {
            
            case LOGO:
            switch (state)
            {
            case 0:
                DrawTexture(LOGO_img, -110.0f, -105.0f, Fade(BLACK,alpha)); // "Fade()" é usado para fazer a transição entre as telas
                break;
            case 1:
                DrawTexture(LOGO_img, -110.0f, -105.0f, WHITE); //
                break;
            case 2:
                DrawTexture(LOGO_img, -110.0f, -105.0f, Fade(BLACK, alpha));
                break;
            }
            break;
        case INICIO:
            ClearBackground(LIME);
            switch (state)
            {
            case 1:
                DrawTexture(INICIO_img,-110.0f, -105.0f, WHITE);
                break;
            case 2:
                DrawTexture(INICIO_img, -110.0f, -105.0f, Fade(BLACK, alpha));
                break;
            }
            break;
        case SELECT:
            switch (tela_personagem)
            {
            case 1:
                DrawTexture(tela_companheiro, -110.0f, -105.0f, WHITE);
                break;
            case 0:
                DrawTexture(tela_cake, -110.0f, -105.0f, WHITE);
                break;
            case 2:
                DrawTexture(tela_dorivac, -110.0f, -105.0f, WHITE);
                break;
            }
        break;
        case INGAME:

        // Draw
        //----------------------------------------------------------------------------------

        Draw(camera, envItems, envItemsLength, tamanhoInimigo, inimigo, &jogador, &personagem, frameInimigoT1, frameInimigoT2, spritesMinion, spritesGado, &imune, &(boss[bossAtivo]), deltaTime);

        //----------------------------------------------------------------------------------
      
        //Atualiza a animação quando o jogador está parado
        AnimacaoJogadorParado(&jogador, &personagem, deltaTime);
            break;

        }
        EndMode2D();

        EndDrawing();

    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    //Descarregamento da sprite sheet do jogador
    UnloadTexture(personagem.texture); 
    UnloadTexture(spritesMinion);
    UnloadTexture(spritesGado);
    UnloadTexture(spriteImune);

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdatePlayer(Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta)
{
    if (jogador->vida > 0)
    {
        if (IsKeyDown(KEY_LEFT)) //Movimentação para a Esquerda
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
    }

    //Limites da area de movimentação do jogador
    if ((jogador->posicao.x + jogador->tamanho.x / 2) > TAMANHO_X_CENARIO)
    {
        jogador->posicao.x = TAMANHO_X_CENARIO - jogador->tamanho.x / 2; //Limites para direita
    } else if (jogador->posicao.x < jogador->tamanho.x / 2)
    {
        jogador->posicao.x = jogador->tamanho.x / 2; //Limites para a esquerda
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
        if (objeto->colisao > 0)
        {
            if (VerificaColisaoBordaS(jogador->posicao, jogador->tamanho.x, jogador->tamanho.y, objeto->retangulo, 5))
            {
                if (objeto->colisao == 1)
                {
                    jogador->posicao.y = objeto->retangulo.y + objeto->retangulo.height + jogador->tamanho.y + 1;
                    jogador->velocidade = GRAVIDADE * delta;
                }
                else if (objeto->colisao == 2) //Verifica se é bloco de poder: imune-19
                {
                    jogador->poder = 1; //Define o poder imune-19 ao jogador
                    jogador->posicao.y = objeto->retangulo.y + objeto->retangulo.height + jogador->tamanho.y + 1;
                    jogador->velocidade = GRAVIDADE * delta;
                    objeto->retangulo.y -= 10; //Desloca o bloco 10 pixels para cima
                    objeto->colisao = 1;  //Define o bloco para um bloco normal neste momento
                }
                else if (objeto->colisao == 3) //Verifica se é bloco de poder: poção-52
                {
                    jogador->poder = 2; //Define o poder poção-52 ao jogador
                    jogador_tempo_poder_pocao52 = t; //Recebe o tempo decorrido do jogo
                    jogador->posicao.y = objeto->retangulo.y + objeto->retangulo.height + jogador->tamanho.y + 1;
                    jogador->velocidade = GRAVIDADE * delta;
                    objeto->retangulo.y -= 10; //Desloca o bloco 10 pixels para cima
                    objeto->colisao = 1;  //Define o bloco para um bloco normal neste momento
                }
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
    Rectangle ret_inimigo = {inimigo->posicao.x - (inimigo->tamanho.x / 2), inimigo->posicao.y - inimigo->tamanho.y, inimigo->tamanho.x, inimigo->tamanho.y};

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
        //Verifica se o inimigo está andando para a esquerda
        if (inimigo->direcao_movimento == 0) {
            if (VerificaRangeGado(inimigo->posicao,inimigo->tamanho.x,inimigo->tamanho.y,ret_jogador,RANGE_GADO) == 1) //Verifica o range do gado a esquerda
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
            if (VerificaRangeGado(inimigo->posicao,inimigo->tamanho.x,inimigo->tamanho.y,ret_jogador,RANGE_GADO) == 2) //Verifica o range do gado a direita
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
    if ((inimigo->posicao.x + inimigo->tamanho.x / 2) > TAMANHO_X_CENARIO)
    {
        inimigo->posicao.x = TAMANHO_X_CENARIO - inimigo->tamanho.x / 2; //Limites para direita
        inimigo->direcao_movimento = !inimigo->direcao_movimento;
    }
    else if (inimigo->posicao.x < inimigo->tamanho.x / 2)
    {
        inimigo->posicao.x = inimigo->tamanho.x / 2; //Limites para a esquerda
        inimigo->direcao_movimento = !inimigo->direcao_movimento;
    }

    int colisaoObjeto = 0;
    for (int i = 0; i < envItemsLength; i++) //Preechimento da área dos pixels dos objetos colidiveis
    {
        EnvItem *objeto = envItems + i;
        Vector2 *j = &(inimigo->posicao);

        //Condição de colisão para andar encima de plataformas
        if (objeto->colisao &&
            objeto->retangulo.x - inimigo->tamanho.x / 2 <= j->x &&                           
            objeto->retangulo.x + objeto->retangulo.width + inimigo->tamanho.x / 2 >= j->x && // Definindo a invasão da área do inimigo com a área do objeto(área de colisão)
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
            if (VerificaColisaoBordasED(inimigo->posicao, inimigo->tamanho.x, inimigo->tamanho.y, objeto->retangulo) == 1)
            {
                inimigo->direcao_movimento = 1;
            }
            else if (VerificaColisaoBordasED(inimigo->posicao, inimigo->tamanho.x, inimigo->tamanho.y, objeto->retangulo) == 2)
            {
                inimigo->direcao_movimento = 0;
            }
        }
    }

    //Verifica a colisão entre o Poder e o Inimigo
    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        if (inimigo->tipo > 0 && inimigo->tipo != 3)
        {
            //Desenho do inimigo
            Rectangle inimigoRect = {inimigo->posicao.x - inimigo->tamanho.x / 2, inimigo->posicao.y - inimigo->tamanho.y, inimigo->tamanho.x, inimigo->tamanho.y};

            if (CheckCollisionCircleRec(imune_19[p].posicao, imune_19[p].raio, inimigoRect) && imune_19[p].poder_ativo)
            {
                imune_19[p].poder_ativo = false; //Poder é desativado quando colide
                inimigo->tipo = 0;               //Inimigo morre
            }
        }
    }

    //Verifica colisão entre inimigo e jogador
    if (inimigo->tipo > 0 && jogador->vida > 0)
    {
        //Verifica se borda superior do inimigo encosta em objeto jogador
        if (VerificaColisaoBordaS(inimigo->posicao, inimigo->tamanho.x, inimigo->tamanho.y, ret_jogador, 5))
        {
            //Verifica se inimigo não é do tipo banana
            if (inimigo->tipo != 3)
            {
                inimigo->tipo = 0; //Jogador mata o inimigo
            }
        }

        if (jogador->poder != 2) // Caso o poder do jogador não seja a "poção-52"
        {
            //Verifica se jogador encosta nas bordas do objeto inimigo
            if (VerificaColisaoBordasED(jogador->posicao, jogador->tamanho.x, jogador->tamanho.y, ret_inimigo) != 0)
            {
                jogador->vida = 0; //Jogador encosta em inimigo e perde vida
            }
        }
    }

    if (!colisaoObjeto) //Se não há colisão com objeto
    {
        inimigo->posicao.y += inimigo->velocidade * delta; //Aumentar a posição do Y do inimigo
        inimigo->velocidade += GRAVIDADE * delta;          //Vai sofrer com a Gravidade
    }
}

void UpdateBoss(Inimigo *boss, EnvItem *envItems, Jogador *jogador, int envItemsLength, float delta)
{
    Rectangle ret_jogador = {jogador->posicao.x - (jogador->tamanho.x / 2),jogador->posicao.y - jogador->tamanho.y,jogador->tamanho.x,jogador->tamanho.y};
    Rectangle ret_boss = {boss->posicao.x - (boss->tamanho.x / 2), boss->posicao.y - boss->tamanho.y, boss->tamanho.x, boss->tamanho.y};

    if (boss->tipo == 1)
    {
        if (VerificaRangeDudu(boss->posicao, boss->tamanho, ret_jogador, RANGE_DUDU))
        {
            boss->posicao.x -= VELOCIDADE_DUDU_ATAQUE * delta;
        }
    }

    if (boss->tipo == 2)
    {
        //Condição de Fábio pular
        if (boss->podePular && (inimigo_cooldown_pulo + TEMPO_COOLDOWN_PULO_FABIO <= t))
        {
            boss->velocidade = -JOGADOR_PULO_VELOCIDADE;
            boss->podePular = false;
            inimigo_cooldown_pulo = t;
        }
    }
    
    int colisaoObjeto = 0;
    for (int i = 0; i < envItemsLength; i++) //Preechimento da área dos pixels dos objetos colidiveis
    {
        EnvItem *objeto = envItems + i;
        Vector2 *j = &(boss->posicao);

        //Condição de colisão para andar encima de plataformas
        if (objeto->colisao &&
            objeto->retangulo.x - boss->tamanho.x / 2 <= j->x &&                           
            objeto->retangulo.x + objeto->retangulo.width + boss->tamanho.x / 2 >= j->x && // Definindo a invasão da área do boss com a área do objeto(área de colisão)
            objeto->retangulo.y >= j->y &&
            objeto->retangulo.y < j->y + boss->velocidade * delta)
        {
            colisaoObjeto = 1;
            boss->velocidade = 0.0f; //Reduzindo a velocidade do player para 0, para freiar ele
            j->y = objeto->retangulo.y; //Atualiza a variável do movimento
        }

        //Condição de colisão em objetos Universais
        if (objeto->colisao)
        {
            if (VerificaColisaoBordasED(boss->posicao, boss->tamanho.x, boss->tamanho.y, objeto->retangulo) == 1)
            {
                boss->direcao_movimento = 1;
            }
            else if (VerificaColisaoBordasED(boss->posicao, boss->tamanho.x, boss->tamanho.y, objeto->retangulo) == 2)
            {
                boss->direcao_movimento = 0;
            }
        }
    }

    //Verifica a colisão entre o Poder e o Inimigo
    for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)
    {
        if (boss->tipo > 0)
        {           
            if (CheckCollisionCircleRec(imune_19[p].posicao, imune_19[p].raio, ret_boss) && imune_19[p].poder_ativo)
            {
                imune_19[p].poder_ativo = false; //Poder é desativado quando colide
            }
        }
    }

    //Verifica colisão entre boss e jogador
    if (boss->tipo > 0 && jogador->vida > 0)
    {
        //Verifica se jogador encosta nas bordas do objeto boss
        if (VerificaColisaoBordasED(jogador->posicao, jogador->tamanho.x, jogador->tamanho.y, ret_boss) != 0)
        {
            jogador->vida = 0; //Jogador encosta em boss e perde vida
        }
        //Verifica se borda superior do boss encosta em objeto jogador
        else if (VerificaColisaoBordaS(boss->posicao, boss->tamanho.x, boss->tamanho.y, ret_jogador, 5))
        {
            boss->tipo = 0; //Jogador mata o boss
            boss->vida = 0;
        }
    }

    if (!colisaoObjeto) //Se não há colisão com objeto
    {
        boss->posicao.y += boss->velocidade * delta; //Aumentar a posição do Y do boss
        boss->velocidade += GRAVIDADE * delta;          //Vai sofrer com a Gravidade
    }
    else
    {
        boss->podePular = true;
    }
}

void AnimacaoJogadorMovimento(Jogador *jogador, Animacao *personagem, float deltaTime)
{
    if(jogador->poder != 2)
    {
        personagem->framesCounter++; //Atualiza o valor da frame do jogo

        if (personagem->framesCounter % 2 == 0) personagem->currentFrame = 1;
        else personagem->currentFrame = 2; //Controle da alternância dos passos

        if ((personagem->framesCounter >= (t/personagem->framesSpeed)) && personagem ->framesCounter % 2 == 1) //Altera as FPS do jogo para a desejada para a movimentação do jogador
        {
            personagem->framesCounter = 0;
            personagem->framesSpeed += 0.5;
            if((float)s > (float)sc + 60) personagem->framesSpeed += 0.1;
            if((float)s > (float)sc + 4*60) personagem->framesSpeed += 0.1;
            
            //Jogador
            if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && personagem->currentFrame == 1 && jogador->vida > 0) //Passo 1 esquerda
            {
                personagem->posicao.x = 140 - jogador->tamanho.x;
                personagem->frameRect.x = 2*personagem->frameWidth;
                personagem->frameRect.y = 2*personagem->frameHeight;
            }
            if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && personagem->currentFrame == 2 && jogador->vida > 0) //Passo 2 esquerda
            {
                personagem->posicao.x = 140 - jogador->tamanho.x;
                personagem->frameRect.x = 0.0f;
                personagem->frameRect.y = 3*personagem->frameHeight;
            }
            if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && personagem->currentFrame == 1 && jogador->vida > 0) //Passo 1 direita
            {
                personagem->posicao.x = 116 - jogador->tamanho.x;
                personagem->frameRect.x = 0.0f;
                personagem->frameRect.y = personagem->frameHeight;
            }
            if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && personagem->currentFrame == 2 && jogador->vida > 0) //Passo 2 direita
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
    else
    {
        personagem->framesCounter++; //Atualiza o valor da frame do jogo

        if (personagem->framesCounter % 2 == 0) personagem->currentFrame = 1;
        else personagem->currentFrame = 2; //Controle da alternância dos passos

        if (personagem->poderCounter == 1)
        {
            personagem->poderCounter = 2;
            personagem->poderCurrentFrame = 1;
        }
        else if (personagem->poderCounter == 2) 
        {
            personagem->poderCounter = 3;
            personagem->poderCurrentFrame = 2;
        } 
        else if (personagem->poderCounter == 3)
        {
            personagem->poderCounter = 4;
            personagem->poderCurrentFrame = 1;
        }
        else
        {
            personagem->poderCounter = 1;
            personagem->poderCurrentFrame = 2;
        } //Controle da alternância dos passos especiais
        
        if ((personagem->framesCounter >= (t/personagem->framesSpeed)) && personagem->framesCounter % 2 == 1) //Altera as FPS do jogo para a desejada para a movimentação do jogador
        {
            personagem->framesCounter = 0;
            personagem->framesSpeed += 0.5;
            if((float)s > (float)sc + 60) personagem->framesSpeed += 0.1;
            if((float)s > (float)sc + 4*60) personagem->framesSpeed += 0.1;
            
            //Jogador
            if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && personagem->currentFrame == 1 && personagem->poderCurrentFrame == 1 && jogador->vida > 0) //Passo 1 esquerda
            {
                personagem->posicao.x = 140 - jogador->tamanho.x;
                personagem->frameRect.x = 2*personagem->frameWidth;
                personagem->frameRect.y = 2*personagem->frameHeight;
            }
            if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && personagem->currentFrame == 2 && personagem->poderCurrentFrame == 1 && jogador->vida > 0) //Passo 2 esquerda
            {
                personagem->posicao.x = 140 - jogador->tamanho.x;
                personagem->frameRect.x = 0.0f;
                personagem->frameRect.y = 3*personagem->frameHeight;
            }
            if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && personagem->currentFrame == 1 && personagem->poderCurrentFrame == 1 && jogador->vida > 0) //Passo 1 direita
            {
                personagem->posicao.x = 116 - jogador->tamanho.x;
                personagem->frameRect.x = 0.0f;
                personagem->frameRect.y = personagem->frameHeight;
            }
            if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && personagem->currentFrame == 2 && personagem->poderCurrentFrame == 1 && jogador->vida > 0) //Passo 2 direita
            {
                personagem->posicao.x = 116 - jogador->tamanho.x;
                personagem->frameRect.x = 2*personagem->frameWidth;
                personagem->frameRect.y = personagem->frameHeight;
            }

            if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && personagem->currentFrame == 1 && personagem->poderCurrentFrame == 2 && jogador->vida > 0) //Passo 1 esquerda especial
            {
                personagem->posicao.x = 140 - jogador->tamanho.x;
                personagem->frameRect.x = 3*personagem->frameWidth;
                personagem->frameRect.y = 2*personagem->frameHeight;
            }
            if (IsKeyDown(KEY_LEFT) && jogador->podePular == true && personagem->currentFrame == 2 && personagem->poderCurrentFrame == 2 && jogador->vida > 0) //Passo 2 esquerda especial
            {
                personagem->posicao.x = 140 - jogador->tamanho.x;
                personagem->frameRect.x = personagem->frameWidth;
                personagem->frameRect.y = 3*personagem->frameHeight;
            }
            if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && personagem->currentFrame == 1 && personagem->poderCurrentFrame == 2 && jogador->vida > 0) //Passo 1 direita especial
            {
                personagem->posicao.x = 116 - jogador->tamanho.x;
                personagem->frameRect.x = personagem->frameWidth;
                personagem->frameRect.y = personagem->frameHeight;
            }
            if (IsKeyDown(KEY_RIGHT) && jogador->podePular == true && personagem->currentFrame == 2 && personagem->poderCurrentFrame == 2 && jogador->vida > 0) //Passo 2 direita especial
            {
                personagem->posicao.x = 116 - jogador->tamanho.x;
                personagem->frameRect.x = 3*personagem->frameWidth;
                personagem->frameRect.y = personagem->frameHeight;
            }

            if (((IsKeyDown(KEY_UP) && jogador->direcao_movimento == 0) ||
            (jogador->podePular == false && IsKeyDown(KEY_LEFT) && jogador->vida > 0)) && personagem->poderCurrentFrame == 1) //Pulo esquerda
            {
                personagem->posicao.x = 140 - jogador->tamanho.x;
                personagem->frameRect.x = 2*personagem->frameWidth;
                personagem->frameRect.y = 2*personagem->frameHeight;
            }
            if (((IsKeyDown(KEY_UP) && jogador->direcao_movimento == 1) ||
            (jogador->podePular == false && IsKeyDown(KEY_RIGHT) && jogador->vida > 0)) && personagem->poderCurrentFrame == 1) //Pulo direita
            {
                personagem->posicao.x = 116 - jogador->tamanho.x;
                personagem->frameRect.x = 0.0f;
                personagem->frameRect.y = personagem->frameHeight;
            }

            if (((IsKeyDown(KEY_UP) && jogador->direcao_movimento == 0) ||
            (jogador->podePular == false && IsKeyDown(KEY_LEFT) && jogador->vida > 0)) && personagem->poderCurrentFrame == 2) //Pulo esquerda especial
            {
                personagem->posicao.x = 140 - jogador->tamanho.x;
                personagem->frameRect.x = 3*personagem->frameWidth;
                personagem->frameRect.y = 2*personagem->frameHeight;
            }
            if (((IsKeyDown(KEY_UP) && jogador->direcao_movimento == 1) ||
            (jogador->podePular == false && IsKeyDown(KEY_RIGHT) && jogador->vida > 0)) && personagem->poderCurrentFrame == 2) //Pulo direita especial
            {
                personagem->posicao.x = 116 - jogador->tamanho.x;
                personagem->frameRect.x = personagem->frameWidth;
                personagem->frameRect.y = personagem->frameHeight;
            }
            
            if (jogador->direcao_movimento == 0 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && personagem->poderCurrentFrame == 1 && jogador->vida > 0) //Parado esquerda
            {
                personagem->posicao.x = 140 - jogador->tamanho.x;
                personagem->frameRect.x = 0.0f;
                personagem->frameRect.y = 2*personagem->frameHeight;
            }
            if (jogador->direcao_movimento == 1 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && personagem->poderCurrentFrame == 1 && jogador->vida > 0) //Parado direita
            {
                personagem->posicao.x = 116 - jogador->tamanho.x;
                personagem->frameRect.x = 2*personagem->frameWidth;
                personagem->frameRect.y = 0.0f;
            }
            
            if (jogador->direcao_movimento == 0 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && personagem->poderCurrentFrame == 2 && jogador->vida > 0) //Parado esquerda especial
            {
                personagem->posicao.x = 140 - jogador->tamanho.x;
                personagem->frameRect.x = personagem->frameWidth;
                personagem->frameRect.y = 2*personagem->frameHeight;
            }
            if (jogador->direcao_movimento == 1 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && personagem->poderCurrentFrame == 2 && jogador->vida > 0) //Parado direita especial
            {
                personagem->posicao.x = 116 - jogador->tamanho.x;
                personagem->frameRect.x = 3*personagem->frameWidth;
                personagem->frameRect.y = 0.0f;
            }
        }
    } 
}

void AnimacaoInimigo(Inimigo *inimigo, Animacao *frameInimigoT1, Animacao *frameInimigoT2, Texture2D spritesMinion, Texture2D spritesGado, int tamanhoInimigos, float deltaTime)
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
                frameInimigoT1->posicao.x = 146 - inimigo->tamanho.x;
                frameInimigoT1->frameRect.x = 0.0f;
                frameInimigoT1->frameRect.y = 0.0f;
            }
            if (inimigo->direcao_movimento == 0 && frameInimigoT1->currentFrame == 2) //Passo 2 esquerda
            {
                frameInimigoT1->posicao.x = 146 - inimigo->tamanho.x;
                frameInimigoT1->frameRect.x = frameInimigoT1->frameWidth;
                frameInimigoT1->frameRect.y = 0.0f;
            }
            if (inimigo->direcao_movimento == 1 && frameInimigoT1->currentFrame == 1) //Passo 1 direita
            {
                frameInimigoT1->posicao.x = 159 - inimigo->tamanho.x;
                frameInimigoT1->frameRect.x = 0.0f;
                frameInimigoT1->frameRect.y = frameInimigoT1->frameHeight;
            }
            if (inimigo->direcao_movimento == 1 && frameInimigoT1->currentFrame == 2) //Passo 2 direita
            {
                frameInimigoT1->posicao.x = 159 - inimigo->tamanho.x;
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

void AnimacaoJogadorParado(Jogador *jogador, Animacao *personagem, float delta)
{
    if (jogador->poder != 2)
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
    }
    else
    {
        if (jogador->direcao_movimento == 0 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && personagem->currentFrame == 1 && jogador->vida > 0) //Parado esquerda especial
        {
            personagem->posicao.x = 140 - jogador->tamanho.x;
            personagem->frameRect.x = personagem->frameWidth;
            personagem->frameRect.y = 2*personagem->frameHeight;
        }
        if (jogador->direcao_movimento == 1 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && personagem->currentFrame == 1 && jogador->vida > 0) //Parado direita especial
        {
            personagem->posicao.x = 116 - jogador->tamanho.x;
            personagem->frameRect.x = 3*personagem->frameWidth;
            personagem->frameRect.y = 0.0f;
        }
        if (jogador->direcao_movimento == 0 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && personagem->currentFrame == 2 && jogador->vida > 0) //Parado esquerda
        {
            personagem->posicao.x = 140 - jogador->tamanho.x;
            personagem->frameRect.x = 0.0f;
            personagem->frameRect.y = 2*personagem->frameHeight;
        }
        if (jogador->direcao_movimento == 1 && jogador->podePular == true && jogador->posicao.x == jogador->posicaoAnterior.x && personagem->currentFrame == 2 && jogador->vida > 0) //Parado direita
        {
            personagem->posicao.x = 116 - jogador->tamanho.x;
            personagem->frameRect.x = 2*personagem->frameWidth;
            personagem->frameRect.y = 0.0f;
        }
    }
    

    if (jogador->vida == 0) //Pulo depois da morte
    {
        personagem->posicao.x = 120 - jogador->tamanho.x;
        personagem->frameRect.x = personagem->frameHeight;
        personagem->frameRect.y = 0.0f;
        jogador->velocidade = -JOGADOR_PULO_VELOCIDADE;
        jogador->posicao.y += jogador->velocidade * delta; //Aumentar a posição do Y do jogador
        if (jogador->posicao.y <= 400)
        {
            jogador->vida = -1;
        }
    } 
    else if (jogador->vida == -1) //Caída
    {
        updateplayer = 0;
        jogador->posicao.y += 2* jogador->velocidade * delta;
        jogador->velocidade += GRAVIDADE * delta; //Vai sofrer com a Gravidade
    }
}

void Draw(Camera2D camera, EnvItem *envItems, int envItemsLength, int tamanhoInimigo, Inimigo *inimigo, Jogador *jogador, Animacao *personagem, Animacao *framesInimigoT1, Animacao *framesInimigoT2, Texture2D spritesMinion, Texture2D spritesGado, IMUNE_19 *imune, Inimigo *boss, float delta)
{
    //Desenho dos Retângulos referentes aos obstáculos de EnvItems
    for (int i = 0; i < envItemsLength; i++)
        DrawRectangleRec(envItems[i].retangulo, envItems[i].cor);

    for (int i = 0; i < tamanhoInimigo; i++)
    {
        if (inimigo[i].tipo == 1)
        {
            //Desenho da hitbox do inimigo
            DrawRectangleLines(inimigo[i].posicao.x - inimigo[i].tamanho.x / 2, inimigo[i].posicao.y - inimigo[i].tamanho.y, inimigo[i].tamanho.x, inimigo[i].tamanho.y, YELLOW);

            //Desenho da textura dos minions
            DrawTextureRec(framesInimigoT1[i].texture, framesInimigoT1[i].frameRect, (Vector2){inimigo[i].posicao.x - (framesInimigoT1[i].posicao.x - inimigo[i].tamanho.x), inimigo[i].posicao.y - (framesInimigoT1[i].posicao.y - inimigo[i].tamanho.y)}, RAYWHITE);
        }
        if (inimigo[i].tipo == 2)
        {
            //Desenho da hitbox do inimigo
            DrawRectangleLines(inimigo[i].posicao.x - inimigo[i].tamanho.x / 2, inimigo[i].posicao.y - inimigo[i].tamanho.y, inimigo[i].tamanho.x, inimigo[i].tamanho.y, ORANGE);

            //Desenho da textura dos gados
            DrawTextureRec(framesInimigoT2[i].texture, framesInimigoT2[i].frameRect, (Vector2){inimigo[i].posicao.x - (framesInimigoT2[i].posicao.x - inimigo[i].tamanho.x), inimigo[i].posicao.y - (framesInimigoT2[i].posicao.y - inimigo[i].tamanho.y)}, RAYWHITE);
        }
        if (inimigo[i].tipo == 3)
        {
            DrawRectangleLines(inimigo[i].posicao.x - inimigo[i].tamanho.x / 2, inimigo[i].posicao.y - inimigo[i].tamanho.y, inimigo[i].tamanho.x, inimigo[i].tamanho.y, inimigo[i].cor);
        }
        
    }

    //Desenho circular dos poderes
    for (int p = 0; p < PODER_MAX_FABIO; p++)
    {
        if (p < PODER_MAX_PERSONAGEM)
        {
            if (imune_19[p].poder_ativo)
            {
                DrawCircleV(imune_19[p].posicao, imune_19[p].raio, BLACK);
                DrawTextureRec(imune->texture, imune->frameRect, (Vector2){imune_19[p].posicao.x - 30, imune_19[p].posicao.y - 30}, RAYWHITE);
            }
        }
        if (p < PODER_MAX_FABIO)
        {
            if (laranja[p].poder_ativo)
            {
                DrawCircleV(laranja[p].posicao, laranja[p].raio, ORANGE);
            }
        }

        if (p < PODER_MAX_TCHUTCHUCA)
        {
            if (dinheiro[p].poder_ativo)
            {
                DrawCircleV(dinheiro[p].posicao, dinheiro[p].raio, DARKGREEN);
            }
        }

        if (p < PODER_MAX_REI)
        {
            if (pocao[p].poder_ativo)
            {
                DrawCircleV(pocao[p].posicao, pocao[p].raio, pocao[p].cor);
            }
        }

    }

    //Criação e Desenho

    //Desenho da hitbox do jogador
    DrawRectangleLines(jogador->posicao.x - jogador->tamanho.x / 2, jogador->posicao.y - jogador->tamanho.y, jogador->tamanho.x, jogador->tamanho.y, RED);


    //Desenho da textura do jogador
    DrawTextureRec(personagem->texture, personagem->frameRect, (Vector2){jogador->posicao.x - (personagem->posicao.x + jogador->tamanho.x), jogador->posicao.y - (personagem->posicao.y + jogador->tamanho.y)}, RAYWHITE);
    
    //Desenho da hitbox do boss
    if (boss->vida > 0)
    {
        DrawRectangleLines(boss->posicao.x - (boss->tamanho.x / 2), boss->posicao.y - (boss->tamanho.y), boss->tamanho.x, boss->tamanho.y, boss->cor);
    }

    DrawText(FormatText("Colisão : %01i", colisaoJogador), 1000, 450, 20, BLACK);

    DrawText(FormatText("Exemplo de Inimigo"), 1650, 450, 20, BLACK);
    DrawText(FormatText("Vida Jogador: %01i",jogador->vida), 1650, 475, 20, BLACK);

    DrawText(FormatText("Exemplo de Gado"), 2050, 450, 20, BLACK);
    DrawText(FormatText("Vida Jogador: %01i",jogador->vida), 2050, 475, 20, BLACK);

    for (int i = 0; i < envItemsLength; i++)
    {
        //Desenho da interrogação dentro do bloco (imune-19)
        if (envItems[i].colisao == 2)
        {
            DrawText(FormatText("?"),envItems[i].retangulo.x + 10, envItems[i].retangulo.y + 5, 50, WHITE);
        } 
        else if (envItems[i].colisao == 3)  //Desenho da exclamação dentro do bloco (poção-52)
        {
            DrawText(FormatText("!"),envItems[i].retangulo.x + 23, envItems[i].retangulo.y + 5, 50, WHITE);
        }
    }
    DrawText(FormatText("Exemplo de Bloco de Poder"), 2550, 450, 20, BLACK);
    DrawText(FormatText("Poder do Jogador: %01i",jogador->poder), 2550, 475, 20, BLACK);

}

void UpdatePoder(Poder *imune_19, IMUNE_19 *imune, Jogador *jogador, Inimigo *boss, EnvItem *envItems, int envItemsLength, float delta, Texture2D spriteImune)
{
    Rectangle ret_jogador = {jogador->posicao.x - (jogador->tamanho.x / 2), jogador->posicao.y - jogador->tamanho.y, jogador->tamanho.x, jogador->tamanho.y};

    //Acionamento do poder IMUNE_19
    if (IsKeyPressed(KEY_SPACE) && jogador->poder == 1) {                                                               
        for (int p = 0; p < PODER_MAX_PERSONAGEM; p++)  //Configuração do "imune_19" quando desativado
        {
            if (!imune_19[p].poder_ativo && jogador->direcao_movimento == 1) //Caso jogador esteja indo para a DIREITA
            {
                imune_19[p].posicao = (Vector2){jogador->posicao.x + (jogador->tamanho.x/2), jogador->posicao.y - (jogador->tamanho.y/2)}; //Posição inicial do poder é o centro do jogador
                imune_19[p].direcao_movimento = jogador->direcao_movimento; //Poder tem direção baseada na do jogador, porém fica independente quando emitido
                imune_19[p].poder_ativo = true;
                imune->frameRect.y = 0.0f;
                break;   
            }
            else if (!imune_19[p].poder_ativo && jogador->direcao_movimento == 0) //Caso jogador esteja indo para a ESQUERDA
            {
                imune_19[p].posicao = (Vector2){jogador->posicao.x - (jogador->tamanho.x/2), jogador->posicao.y - (jogador->tamanho.y/2)}; //Centro do jogador
                imune_19[p].direcao_movimento = jogador->direcao_movimento;
                imune_19[p].poder_ativo = true;                             
                imune->frameRect.y = imune->texture.width;
                break;   
            }
        }
    }

    //Condição de Fábio atirar LARANJAS!
    if ((boss->tipo == 2) && (inimigo_cooldown_poder + TEMPO_COOLDOWN_PODER_FABIO <= t))
    {
        for (int p = 0; p < PODER_MAX_FABIO; p++)
        {
            if (!laranja[p].poder_ativo)
            {
                laranja[p].posicao = (Vector2){boss->posicao.x - (boss->tamanho.x/2), boss->posicao.y - (boss->tamanho.y/2)};
                laranja[p].direcao_movimento = 0;
                laranja[p].poder_ativo = true;
                break;
            }
        }
        inimigo_cooldown_poder = t;
    }

    //Condição de Tchutchuca atirar dinheiro!
    if ((boss->tipo == 3) && (inimigo_cooldown_poder + TEMPO_COOLDOWN_PODER_TCHUTCHUCA <= t))
    {
        for (int i = 0; i < 5; i++)
        {
            for (int p = 0; p < PODER_MAX_TCHUTCHUCA; p++)
            {
                if (!dinheiro[p].poder_ativo)
                {   
                    switch (i)
                    {
                    case 0: 
                        dinheiro[p].posicao = (Vector2){boss->posicao.x - (boss->tamanho.x / 2), boss->posicao.y - (boss->tamanho.y / 2)};
                        break;
                    case 1: 
                        dinheiro[p].posicao = (Vector2){boss->posicao.x - (boss->tamanho.x / 2), boss->posicao.y - boss->tamanho.y};
                        break;
                    case 2: 
                        dinheiro[p].posicao = (Vector2){boss->posicao.x, boss->posicao.y - boss->tamanho.y};
                        break;
                    case 3: 
                        dinheiro[p].posicao = (Vector2){boss->posicao.x + (boss->tamanho.x / 2), boss->posicao.y - boss->tamanho.y};
                        break;
                    case 4: 
                        dinheiro[p].posicao = (Vector2){boss->posicao.x + (boss->tamanho.x / 2), boss->posicao.y - (boss->tamanho.y / 2)};
                        break;
                    
                    default:
                        break;
                    }
                    dinheiro[p].direcao_movimento = i;
                    dinheiro[p].poder_ativo = true;
                    break;
                }
            }
        }
        inimigo_cooldown_poder = t;
    }
    
    //Condição do rei de atirar poções
    if ((boss->tipo == 4) && (inimigo_cooldown_poder + TEMPO_COOLDOWN_PODER_REI <= t))
    {
        for (int p = 0; p < PODER_MAX_REI; p++)
        {
            if (!pocao[p].poder_ativo)
            {
                pocao[p].posicao = (Vector2){boss->posicao.x - (boss->tamanho.x/2), boss->posicao.y - (boss->tamanho.y/4)};
                pocao[p].direcao_movimento = 0;
                pocao[p].poder_ativo = true;
                break;
            }
        }
        inimigo_cooldown_poder = t;
    }

    //Movimentação do poder
    /*Aqui há uma economia no for, veja que o 'for' é utilizado para o maior valor de p 
     *e existe um 'if' que separa o loop para o personagem e o boss fábio
     */
    for (int p = 0; p < PODER_MAX_FABIO; p++)
    {
        if (p < PODER_MAX_PERSONAGEM)
        {
            if (imune_19[p].direcao_movimento == 1)  //Considerando a direção do poder para a DIREITA:
            {
                imune_19[p].posicao.x += PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na DIREITA
            } else if (imune_19[p].direcao_movimento == 0) //Considerando a direção do poder para a ESQUERDA:
            {
                imune_19[p].posicao.x -= PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na ESQUERDA
            }
          
            imune->posicao = imune_19[p].posicao; //Posição y do personagem em relação à posição y do inimigo tipo 2
          
            //Colisão do poder com os objetos do cenário (inimigos não contam aqui)
            for (int o = 0; o < envItemsLength; o++)
            {
                if (envItems[o].colisao //Se houver um objeto colidível
                    && CheckCollisionCircleRec(imune_19[p].posicao, imune_19[p].raio, envItems[o].retangulo))  //e a colisão for entre o poder
                {                                                                                               // (círculo) e um retângulo
                    imune_19[p].poder_ativo = false; //O poder é desativado ("desaparece" do cenário)                                                                                               
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

      
        if (p < PODER_MAX_FABIO)
        {
            if (laranja[p].direcao_movimento == 0) //Considerando a direção do poder para a ESQUERDA:
            {
                laranja[p].posicao.x -= PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na ESQUERDA
            }

            //Colisão do poder com os objetos do cenário (inimigos não contam aqui)
            for (int o = 0; o < envItemsLength; o++)
            {
                if (envItems[o].colisao                                                                       //Se houver um objeto colidível
                    && CheckCollisionCircleRec(laranja[p].posicao, laranja[p].raio, envItems[o].retangulo)) //e a colisão for entre o poder
                {                                                                                             // (círculo) e um retângulo
                    laranja[p].poder_ativo = false;                                                          //O poder é dasativado ("desaparece" do cenário)
                }
            }

            //Limite da área de movimento do poder
            if (laranja[p].posicao.x < laranja[p].raio) //Limite até o fim do cenário (lado ESQUERDO)
            {
                laranja[p].poder_ativo = false; //Poder é desativado
            }
            else if (laranja[p].posicao.x + laranja[p].raio > TAMANHO_X_CENARIO) //Limite até o fim do cenário (lado DIREITO)
            {
                laranja[p].poder_ativo = false; //Poder é desativado
            }
        }

        if (p < PODER_MAX_TCHUTCHUCA)
        {
            switch (dinheiro[p].direcao_movimento)
            {
            case 0:
                dinheiro[p].posicao.x -= PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na ESQUERDA
                break;
            case 1:
                dinheiro[p].posicao.x -= PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na ESQUERDA
                dinheiro[p].posicao.y -= PODER_MOVIMENTO_VELOCIDADE * delta; 
                break;
            case 2:
                dinheiro[p].posicao.y -= PODER_MOVIMENTO_VELOCIDADE * delta; 
                break;
            case 3:
                dinheiro[p].posicao.x += PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na ESQUERDA
                dinheiro[p].posicao.y -= PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na ESQUERDA
                break;
            case 4:
                dinheiro[p].posicao.x += PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na ESQUERDA
                break;
            
            default:
                break;
            }

            //Colisão do poder com os objetos do cenário (inimigos não contam aqui)
            for (int o = 0; o < envItemsLength; o++)
            {
                if (envItems[o].colisao                                                                     //Se houver um objeto colidível
                    && CheckCollisionCircleRec(dinheiro[p].posicao, dinheiro[p].raio, envItems[o].retangulo)) //e a colisão for entre o poder
                {                                                                                           // (círculo) e um retângulo
                    dinheiro[p].poder_ativo = false;                                                         //O poder é dasativado ("desaparece" do cenário)
                }
            }

            //Limite da área de movimento do poder
            if (dinheiro[p].posicao.x < dinheiro[p].raio) //Limite até o fim do cenário (lado ESQUERDO)
            {
                dinheiro[p].poder_ativo = false; //Poder é desativado
            }
            else if (dinheiro[p].posicao.x + dinheiro[p].raio > TAMANHO_X_CENARIO) //Limite até o fim do cenário (lado DIREITO)
            {
                dinheiro[p].poder_ativo = false; //Poder é desativado
            }
          
            if (dinheiro[p].posicao.y < 0) //Limite até o teto do cenário
            {
                dinheiro[p].poder_ativo = false;
            }
        } 

        if (p < PODER_MAX_REI)
        {
            if (pocao[p].direcao_movimento == 0) //Considerando a direção do poder para a ESQUERDA:
            {
                pocao[p].posicao.x -= PODER_MOVIMENTO_VELOCIDADE * delta; //Ele permanece na ESQUERDA
            }

            //Colisão do poder com os objetos do cenário (inimigos não contam aqui)
            for (int o = 0; o < envItemsLength; o++)
            {
                if (envItems[o].colisao                                                                     //Se houver um objeto colidível
                    && CheckCollisionCircleRec(pocao[p].posicao, pocao[p].raio, envItems[o].retangulo)) //e a colisão for entre o poder
                {                                                                                           // (círculo) e um retângulo
                    pocao[p].poder_ativo = false;                                                         //O poder é dasativado ("desaparece" do cenário)
                }
            }

            //Limite da área de movimento do poder
            if (pocao[p].posicao.x < pocao[p].raio) //Limite até o fim do cenário (lado ESQUERDO)
            {
                pocao[p].poder_ativo = false; //Poder é desativado
            }
            else if (pocao[p].posicao.x + pocao[p].raio > TAMANHO_X_CENARIO) //Limite até o fim do cenário (lado DIREITO)
            {
                pocao[p].poder_ativo = false; //Poder é desativado
            }
        }

        for (int i = 0; i < PODER_MAX_PERSONAGEM; i++)
        {
            switch (boss->tipo)
            {
            case 2:
                //Verificação se há colisão de poder com poder
                if (VerificaColisaoPoderPoder(&(imune_19[i]), &(laranja[p])))
                {
                    imune_19[i].poder_ativo = false;
                    laranja[p].poder_ativo = false;
                }

                //Verifica a colisão entre laranja e jogador
                if (CheckCollisionCircleRec(laranja[p].posicao,laranja[p].raio,ret_jogador) && laranja[p].poder_ativo)
                    jogador->vida = 0;
                
                break;
            case 3:
                if (VerificaColisaoPoderPoder(&(imune_19[i]), &(dinheiro[p])))
                {
                    imune_19[i].poder_ativo = false;
                    dinheiro[p].poder_ativo = false;
                }

                //Verifica a colisão entre dinheiro e jogador
                if (CheckCollisionCircleRec(dinheiro[p].posicao,dinheiro[p].raio,ret_jogador) && dinheiro[p].poder_ativo)
                    jogador->vida = 0;
                
                break;
            case 4:
                if (VerificaColisaoPoderPoder(&(imune_19[i]), &(pocao[p])))
                {
                    imune_19[i].poder_ativo = false;
                    pocao[p].poder_ativo = false;
                }

                //Verifica a colisão entre poção e jogador
                if (CheckCollisionCircleRec(pocao[p].posicao,pocao[p].raio,ret_jogador) && pocao[p].poder_ativo)
                    jogador->vida = 0;
                
                break;
            default:
                break;
            }
        }
    }

    // Referente ao poder "Poção-52"
    if ((jogador->poder == 2) && (jogador_tempo_poder_pocao52 + DURACAO_PODER_POCAO52 <= t)) // Caso o power-up esteja ativo e o tempo de uso
    {                                                                                        //seja menor ou igual ao tempo decorrido de jogo
        jogador->poder = 0; // Poder é desativado

    }
}

void UpdateCameraCenter(Camera2D *camera, Jogador *jogador, EnvItem *envItems, int envItemsLength, float delta, int width, int height)
{
    if (jogador->vida > 0)
    {
        camera->offset = (Vector2){width / 2, height / 2};
        camera->target = jogador->posicao;
    }
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
bool VerificaColisaoBordaS(Vector2 entidade, float tamanho_entidade_x, float tamanho_entidade_y, Rectangle objeto, int range) {
    const float ponto_superior = entidade.y - tamanho_entidade_y - 1;
    const float ponto_esquerda = entidade.x - (tamanho_entidade_x / 2) + range;
    const float ponto_direita = entidade.x + (tamanho_entidade_x / 2) - range;

    //Verifica a colisão entre 2 pontos superiores da entidade
    for (float ponto = ponto_esquerda; ponto <= ponto_direita; ponto++)
    {
        if (CheckCollisionPointRec((Vector2){ponto,ponto_superior},objeto))
        {
            return 1;
        }
    }

    return 0;
}

/*
Verifica se o jogador entra no range superior do Gado
Retorna 0 se não há jogador no range
Retorna 1 se há jogador no range a esquerda
Retorna 2 se há jogador no range a direita
*/
int VerificaRangeGado(Vector2 posicao_inicial, float tamanho_gado_x, float tamanho_gado_y, Rectangle jogador, float range) {
    const float ponto_inicial_range_y = posicao_inicial.y - (tamanho_gado_y / 2); //Pega a posição central do retângulo do gadinho
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

bool VerificaRangeDudu(Vector2 posicao_inicial, Vector2 tamanho, Rectangle ret_jogador, float range) {
    const float ponto_inicial_range_y = posicao_inicial.y - 1; //Pega a posição inferior do retângulo do gadinho
    const float ponto_inicial_range_x_esquerda = posicao_inicial.x - (tamanho.x / 2);
    
    //Verifica a reta que sai do ponto até o range
    for (float ponto = 0; ponto <= range; ponto++)
    {
        //Verifica se há colisão no range a esquerda do boss
        if (CheckCollisionPointRec((Vector2){ponto_inicial_range_x_esquerda - ponto, ponto_inicial_range_y}, ret_jogador))
        {
            return 1;
        }
    }

    return 0;
}

bool VerificaColisaoPoderPoder(Poder * poder1, Poder * poder2)
{
    if (poder1->poder_ativo && poder2->poder_ativo)
    {
        if (CheckCollisionCircles(poder1->posicao, poder1->raio, poder2->posicao, poder2->raio))
        {
            return 1;
        }
    }

    return 0;
}

//Funções referentes ao cenário

void IniciaCenario(Jogador *jogador, int cenario)
{
    switch (cenario)
    {
    case 0:
        //Carrega a nova posição do jogador
        jogador->posicao = (Vector2){400, 280}; //Posição Inicial Cenário de Testes

        CarregaObjetos(envItems, cenarioTeste, tamanhoCenarioTeste);
        envItemsLength = tamanhoCenarioTeste;

        for (int i = 0; i < tamanho_inimigoTeste; i++)
        {
            CarregaInimigos(&(inimigo[i]),&(inimigoTeste[i]));
        }
        tamanhoInimigo = tamanho_inimigoTeste;

        //Carrega o Boss do cenário
        bossAtivo = 0;

        break;
    case 1:
        //Carrega a nova posição do jogador
        jogador->posicao = (Vector2){0, 280}; //Posição Inicial Cenário 1

        //Carrega os objetos do cenário
        CarregaObjetos(envItems, cenario1, tamanhoCenario1);
        envItemsLength = tamanhoCenario1;

        for (int i = 0; i < tamanho_inimigoCenario1; i++)
        {
            CarregaInimigos(&(inimigo[i]),&(inimigoCenario1[i]));
        }
        tamanhoInimigo = tamanho_inimigoCenario1;

        //Carrega o Boss do cenário
        bossAtivo = 0;
        
        break;
    case 2:
        //Carrega a nova posição do jogador
        jogador->posicao = (Vector2){0, 280}; //Posição Inicial Cenário 2

        //Carrega os objetos do cenário
        CarregaObjetos(envItems, cenario2, tamanhoCenario2);
        envItemsLength = tamanhoCenario2;

        for (int i = 0; i < tamanhoCenario2; i++)
        {
            CarregaInimigos(&(inimigo[i]),&(inimigoCenario2[i]));
        }
        tamanhoInimigo = tamanho_inimigoCenario2;

        //Carrega o Boss do cenário
        bossAtivo = 1;
        
        break;
    case 3:
        //Carrega a nova posição do jogador
        jogador->posicao = (Vector2){0, 280}; //Posição Inicial Cenário 3

        //Carrega os objetos do cenário
        CarregaObjetos(envItems, cenario3, tamanhoCenario3);
        envItemsLength = tamanhoCenario3;

        for (int i = 0; i < tamanhoCenario3; i++)
        {
            CarregaInimigos(&(inimigo[i]),&(inimigoCenario3[i]));
        }
        tamanhoInimigo = tamanho_inimigoCenario3;

        //Carrega o Boss do cenário
        bossAtivo = 2;

        break;
    case 4:
        //Carrega a nova posição do jogador
        jogador->posicao = (Vector2){0, 120}; //Posição Inicial Cenário 4

        //Carrega os objetos do cenário
        CarregaObjetos(envItems, cenario4, tamanhoCenario4);
        envItemsLength = tamanhoCenario4;

        for (int i = 0; i < tamanhoCenario4; i++)
        {
            CarregaInimigos(&(inimigo[i]),&(inimigoCenario4[i]));
        }
        tamanhoInimigo = tamanho_inimigoCenario4;

        //Carrega o Boss do cenário
        bossAtivo = 3;

        break;
    case 5:
        //Carrega a nova posição do jogador
        jogador->posicao = (Vector2){0, 1150}; //Posição Inicial Cenário 4

        //Carrega os objetos do cenário
        CarregaObjetos(envItems, cenario5, tamanhoCenario5);
        envItemsLength = tamanhoCenario5;

        for (int i = 0; i < tamanhoCenario5; i++)
        {
            CarregaInimigos(&(inimigo[i]),&(inimigoCenario5[i]));
        }
        tamanhoInimigo = tamanho_inimigoCenario5;

        //Carrega o Boss do cenário
        bossAtivo = 4;

        break;

    default:
        break;
    }
}

void CarregaObjetos(EnvItem *cenario, EnvItem *loadCenario, int tamanhoLoadCenario)
{
    for (int i = 0; i < tamanhoLoadCenario; i++)
    {
        cenario[i].colisao = loadCenario[i].colisao;
        cenario[i].cor = loadCenario[i].cor;
        cenario[i].retangulo = loadCenario[i].retangulo;
    }
}

void CarregaInimigos(Inimigo *inimigo, InimigoData *loadInimigo) {
    //Carrega os dados de loadInimigo para inimigo
    inimigo->tipo = loadInimigo->tipo;
    inimigo->posicao = loadInimigo->posicao;
    inimigo->direcao_movimento = loadInimigo->direcao_movimento;

    //Preenche os valores de inimigo que valem de acordo com seu tipo
    if (inimigo->tipo == 1)
    {
        inimigo->tamanho = (Vector2){TAMANHO_MINION_X, TAMANHO_MINION_Y};
        inimigo->vida = 1;
        inimigo->cor = YELLOW;
    }
    else if (inimigo->tipo == 2)
    {
        inimigo->tamanho = (Vector2){TAMANHO_GADO_X, TAMANHO_GADO_Y};
        inimigo->vida = 1;
        inimigo->cor = ORANGE;
    }
    else if (inimigo->tipo == 3)
    {
        inimigo->tamanho = (Vector2){TAMANHO_BANANA_X, TAMANHO_BANANA_Y};
        inimigo->vida = 1;
        inimigo->cor = DARKBLUE;
    }  
}
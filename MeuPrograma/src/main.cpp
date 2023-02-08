/*********************************************************************
// Jogo Tetris recriado utilizando a "Canvas", criada pelo Professor Cesar Tadeu Pozzer, sobre a API OpenGl.
//
// Comandos:

// Seta esquerda e direita: mover a peça para a direção da seta
// Seta para baixo: mover a peça para baixo
// Seta para cima: girar a peça (sentido horário)
//
//
// Funcionamento do código:
//
// O tabuleiro do jogo é uma matriz 20x10 que recebe os seguintes valores:
//      0: peças vazias (nada é desenhado na tela)
//      Valores negativos: representam as peças em movimento, ao encostarem a base em algo são multiplicadas por -1 e se tornam peças fixas
//              -1 a -5: são as 5 peças do jogo
//              -12 a -15: são o eixo rotacional (quadrado não possui) das peças de suas respectivas unidades (possuem a mesma cor destas)
//      Valores positivos: representam peças fixas
//              1 a 5: são as 5 peças do jogo porém fixas
//              12 a 15: eram o eixo rotacional das peças de suas respectivas unidades quando estavam em
// Cada peça possui sua cor própria, que é a mesma para seus valores negativos, positivos e seu eixo de rotação
//
// A rotação é feita gerando e rotacionando uma matriz de menor tamanho em volta do eixo de rotação da peça em movimento
// Existem diferentes padrões de rotação:
//      Quadrado: não rotaciona pois não é necessário
//      Peça reta: é gerada uma matriz 5x5 ao redor de seu eixo
//      Demais peças: é gerada uma matriz 3x3 ao redor
//
//
// Quesitos implementados:
//
// Cenário do jogo
// Movimento e rotação da peça
// Contador de pontos
// Remoção de linhas completas
// Peças acima de uma linha removida são baixadas em um nível.
// Menu principal de início do jogo.
// Ajuste da velocidade que a peça cai (aumentando conforme o jogo avança)
// Tamanho de tela variável e responsivo
// Game over e reset
// Próxima peça mostrada no canto da tela
//
//
// *********************************************************************/


#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gl_canvas2d.h"
#include "auxiliar.h"


///Constantes
    //linhas e colunas
#define linhas_tabuleiro 20
#define colunas_tabuleiro 10
    //tamanho do bloco
#define tam_bloco 25
    //posicoes do tabuleiro
#define x_i_t screenWidth/2-125  //x_inicial_tabuleiro
#define x_f_t screenWidth/2+125//x_finall_tabuleiro
#define y_i_t screenHeight/2+250 //y_inicial tabuleiro
#define y_f_t screenHeight/2-250 //y_final tabuleiro
    //tamanho das matrizes de rotação
#define tam_matriz_rotacao_reta 5
#define tam_matriz_rotacao_padrao 3

///Variáveis globais
int g_jogar=0;//controle do menu
int g_reset=0;//controle do reset
int g_mexendo=0; //controle da geração de novas peças
int g_congelar = 0; //controle do congelamento o jogo após o término

int g_peca;//peça gerada
int g_pontuacao = 0; //pontuação
float g_segundos = 1; //dificuldade inicia com 1 segundo para cada queda da peça

int g_encostapeca=0; //variavel para caso esteja encostando em uma peca
int g_encostandoborda=0;//variavel para caso esteja encostando na borda

int g_pecaativa =0;//saber qual o tipo da peça ativa para rotacionar
int g_matrizreta[5][5];//matriz de rotação da reta
int g_matrizoutraspecas[3][3];//matriz de rotação das peças além da reta e cubo
int g_rotacaocopia3[3][3];//copia que vai ser rotacionada
int g_rotacaocopia5[5][5];//copia que vai ser rotacionada

int g_troca=0; //variavel para realizar as trocas nos movimentos das peças

int g_tabuleiro[20][10];//tabuleiro principal do jogo
int g_copiatabuleiro[20][10];//copia do tabuleiro para realizar os testes de colisão

int screenWidth = 800, screenHeight = 600;
int mouseX, mouseY, cliquemouse;



///Funções

///desenha a próxima peça no canto da tela
void desenhaproximapeca()
{
    int proxima[4][6]; //matriz que recebera a proxima peça p ser impressa

    for(int i=0; i<4; i++){ //preencher ela com 0 primeiro
        for(int j=0; j<6; j++){
            proxima[i][j]=0;
        }
    }

    switch(g_peca){ //receber os blocos do numero gerado aleatoriamente
        case 1: //quadrado
            proxima[0][4]=-1, proxima[0][5]=-1, proxima[1][4]=-1, proxima[1][5]=-1;
            break;
        case 2://reta
            proxima[0][4]=-2, proxima[1][4]=-2, proxima[2][4]=-2, proxima[3][4]=-2;
            break;
        case 3://+ pela metade
            proxima[0][4]=-3, proxima[1][3]=-3, proxima[1][4]=-3, proxima[2][4]=-3;
            break;
        case 4:// L inverso
            proxima[0][3]=-4, proxima[1][3]=-4, proxima[1][4]=-4, proxima[1][5]=-4;
            break;
        case 5:// S
            proxima[0][4]=-5, proxima[0][5]=-5, proxima[1][4]=-5, proxima[1][3]=-5;
            break;
    }

    for(int i=0; i<4; i++){ //cores da peça e impressão dos blocos dela
        for(int j=0; j<6; j++){
            if(proxima[i][j] != 0){//se estiver preenchido
                switch (proxima[i][j]){
                    case -1:
                    case 1:
                        CV::color(0, 1, 0);//verde
                        break;
                    case -2:
                    case 2:
                        CV::color(1, 0, 1);//magenta
                        break;
                    case -3:
                    case 3:
                        CV::color(0, 0, 1);//azul
                        break;
                    case -4:
                    case 4:
                        CV::color(1, 0, 0);//vermelho
                        break;
                    case -5:
                    case 5:
                        CV::color(1, 1, 0);//amarelo
                        break;
                }
                CV::rectFill(x_i_t +250+ (j*tam_bloco)+1, y_i_t - (i*tam_bloco)-1, x_i_t+250+ (j*tam_bloco)-1 + tam_bloco, y_i_t - (i*tam_bloco) - tam_bloco+1);
            }
        }
    }

    CV::color(0, 1, 0);//verde
    CV::text(screenWidth/2+175, screenHeight/2+250+5,"Proxima peca:"); //imprime texto
}


///desenha o tabuleiro
void desenhatabuleiro()
{
    CV::color(0.8, 0.8, 0.8);
    //linhas
    for(int i=0; i<=linhas_tabuleiro; i++){
        CV::line(x_i_t, y_i_t - (tam_bloco*i), x_f_t, y_i_t - (tam_bloco*i));
    }
    //colunas
    for(int i=0; i<=colunas_tabuleiro; i++){
        CV::line(x_i_t + (tam_bloco*i), y_i_t, x_i_t + (tam_bloco*i), y_f_t);
    }
}


///desenhar os blocos
void desenhablocos()
{
    for(int i=0; i<linhas_tabuleiro; i++){
        for(int j=0; j<colunas_tabuleiro; j++){
            if(g_tabuleiro[i][j] != 0){//se estiver preenchido
                switch (g_tabuleiro[i][j]){
                    case -1:
                    case 1:
                        CV::color(0, 1, 0);//verde
                        break;
                    case -2:
                    case 2:
                    case -12:
                    case 12:
                        CV::color(1, 0, 1);//magenta
                        break;
                    case -3:
                    case 3:
                    case -13:
                    case 13:
                        CV::color(0, 0, 1);//azul
                        break;
                    case -4:
                    case 4:
                    case -14:
                    case 14:
                        CV::color(1, 0, 0);//vermelho
                        break;
                    case -5:
                    case 5:
                    case -15:
                    case 15:
                        CV::color(1, 1, 0);//amarelo
                        break;
                }
                CV::rectFill(x_i_t + (j*tam_bloco)+1, y_i_t - (i*tam_bloco)-1, x_i_t + (j*tam_bloco)-1 + tam_bloco, y_i_t - (i*tam_bloco) - tam_bloco+1);
            }
        }
    }
}


///gerar o nº da proxima peça
void proximapeca()
{
    g_peca = rand()%5 + 1;
}


///gerar nova peça no tabuleiro
void novapeca()
{
    switch(g_peca){
        case 1: //quadrado
            g_tabuleiro[0][4]=-1, g_tabuleiro[0][5]=-1, g_tabuleiro[1][4]=-1, g_tabuleiro[1][5]=-1;
            g_pecaativa=1;//nao precisa rotacionar
            break;
        case 2://reta
            g_tabuleiro[0][4]=-2, g_tabuleiro[1][4]=-2, g_tabuleiro[2][4]=-12, g_tabuleiro[3][4]=-2;
            g_pecaativa=2;
            break;
        case 3://+ pela metade
            g_tabuleiro[0][4]=-3, g_tabuleiro[1][3]=-3, g_tabuleiro[1][4]=-13, g_tabuleiro[2][4]=-3;
            g_pecaativa=3;
            break;
        case 4:// L inverso
            g_tabuleiro[0][3]=-4, g_tabuleiro[1][3]=-4, g_tabuleiro[1][4]=-14, g_tabuleiro[1][5]=-4;
            g_pecaativa=3;
            break;
        case 5:// S
            g_tabuleiro[0][4]=-5, g_tabuleiro[0][5]=-5, g_tabuleiro[1][4]=-15, g_tabuleiro[1][3]=-5;
            g_pecaativa=3;
            break;
    }
    proximapeca();

}


///verificar se o jogo acabou
void congelajogo()
{
    switch(g_peca){
        case 1:
            if(g_tabuleiro[0][4]>0 || g_tabuleiro[0][5]>0 || g_tabuleiro[1][4]>0 ||  g_tabuleiro[1][5]>0){
                g_congelar=1; // quando a nova peça não puder se mover pelo tabuleiro
            }
        case 2:
            if(g_tabuleiro[0][4]>0 || g_tabuleiro[1][4]>0 || g_tabuleiro[2][4]>0 || g_tabuleiro[3][4]>0){
                g_congelar=1; // quando a nova peça não puder se mover pelo tabuleiro
            }
        case 3:
            if(g_tabuleiro[0][4]>0 ||  g_tabuleiro[1][3]>0 || g_tabuleiro[1][4]>0 || g_tabuleiro[2][4]>0){
                g_congelar=1; // quando a nova peça não puder se mover pelo tabuleiro
            }
        case 4:
            if(g_tabuleiro[0][3]>0 || g_tabuleiro[1][3]>0 || g_tabuleiro[1][4]>0 ||  g_tabuleiro[1][5]>0){
                g_congelar=1; // quando a nova peça não puder se mover pelo tabuleiro
            }
        case 5:
            if(g_tabuleiro[0][4]>0 || g_tabuleiro[0][5]>0 || g_tabuleiro[1][4]>0 || g_tabuleiro[1][3]>0){
                g_congelar=1; // quando a nova peça não puder se mover pelo tabuleiro
            }
    }

    if(g_congelar==1){//se o jogo terminou
        CV::color(0, 1, 0);//verde
        CV::text(screenWidth/2 - 56, screenHeight/2 - 275, "FIM DE JOGO!");

        //botao reset
        CV::color(0, 0.8, 0.2);//caixa do título
        int xbotao = (screenWidth/2-360);
        int ybotao = (screenHeight/2-40);
        if(mouseX>xbotao && mouseX<xbotao+195 && mouseY>ybotao && mouseY<ybotao+80){
            CV::color(0, 0.6 , 0.2);//muda cor caso o mouse passe no botao
            if(cliquemouse==0){//se clicar no botao: resetar variaveis e tabuleiro
                g_mexendo=0;
                g_congelar = 0;
                g_pontuacao = 0;
                g_segundos=1;
                for(int i=0; i<linhas_tabuleiro; i++){
                    for(int j=0; j<colunas_tabuleiro; j++){
                        g_tabuleiro[i][j]=0;
                    }
                }
            }
        }
        CV::rectFill(xbotao, ybotao, xbotao+195, ybotao+80);
        CV::color(1,1,1);//cor branca - texto jogar
        CV::text(screenWidth/2 - 286, screenHeight/2-4,"RESET");
    }
}


///verificar se as linhas estão completas e precisam ser eliminadas
void linhascompletas()
{
    int cont;
    for(int i=0; i<20; i++){
        cont = 0;
        for(int j=0; j<10; j++){
            if(g_tabuleiro[i][j] != 0){//se estiver preenchido
                cont++;
            }
        }
        if(cont==10){//se todos blocos da linha estiver preenchidos
            for(int j=0; j<10; j++){//remover a linha
                g_tabuleiro[i][j]=0;
                g_pontuacao+=50;
            }
            for(int k=19; k>=0; k--){  //baixar todas as peças de cima
                if(k<i){  //se a linha estiver acima da que foi removida
                    for(int l=9; l>=0; l--){//baixar as peças
                        int copiaremocao=g_tabuleiro[k][l];
                        g_tabuleiro[k][l]=g_tabuleiro[k+1][l];
                        g_tabuleiro[k+1][l]=copiaremocao;
                    }
                }
            }
        }
    }
}


void keyboard(int key); //prototipo da função keyboard para poder fazer descer a peça sozinha
clock_t tempo; //tempo inicial de quando a peça desceu

///função do jogo
void tetris()
{
    CV::color(0, 1, 0);//verde
    int xpontuacao = screenWidth/2-125, ypontuacao = screenHeight/2+250+5;
    CV::text(xpontuacao, ypontuacao,"Pontuacao:"); //imprime texto
    CV::text(xpontuacao+110, ypontuacao,  g_pontuacao); //imprime numeros inteiros

    desenhatabuleiro();

    if(g_mexendo==0){ //se terminou de mover a última peça ou começou o jogo
        linhascompletas();//verificar se tem linhas pra remover
        novapeca();
        g_mexendo=1;
    }

    desenhablocos();
    desenhaproximapeca();

    //Fazer a peça cair sozinha
    clock_t tempoatual = clock();
    double diferenca = (double)(tempoatual-tempo)/CLOCKS_PER_SEC;

    if(diferenca>=g_segundos){//se passou o tempo para cair a próxima peça
        tempo=tempoatual;
        g_segundos = g_segundos*0.99; //delay entre as quedas diminui gradativamente
        keyboard(203);//aperta a seta pra baixo (faz a peça descer)
    }

    congelajogo();//chamar continuamente pra printar a mensagem de fim de jogo
}


///menu inicial
void menu()
{
        CV::clear(0, 0.54, 0.44);//fundo azul
        //botao jogar
        CV::color(0, 0.35, 0.35);//caixa do título
        int xbotao = (screenWidth/2)-150;
        int ybotao = (screenHeight/2 - 40);
        if(mouseX>xbotao && mouseX<xbotao+300 && mouseY>ybotao && mouseY<ybotao+80){
            CV::color(0, 0.21 , 0.25);//muda cor caso o mouse passe no botao
            if(cliquemouse==0){//se clicar no botao
                g_jogar=1;
            }
        }
        CV::rectFill(xbotao, ybotao, xbotao+300, ybotao+80);
        CV::color(1,1,1);//cor branca - texto jogar
        CV::text(screenWidth/2 - 26, screenHeight/2-4,"JOGAR");

        //título
        int xtitulo = (screenWidth/2)-320;
        int ytitulo = (screenHeight/2)+160;
        CV::color(0, 0.21 , 0.25);//borda da caixa
        CV::rectFill(xtitulo, ytitulo-10, xtitulo+640+10, ytitulo+80);
        CV::color(0, 0.35, 0.35);//caixa do título
        CV::rectFill(xtitulo, ytitulo, xtitulo+640, ytitulo+80);
        CV::color(1,1,1);//cor branca - textos
        CV::text(screenWidth/2 - 300, screenHeight/2+205,"TRABALHO 4 DE LOGICA E ALGORITMO - JORDANO XAVIER DOS SANTOS");
        CV::text(screenWidth/2 - 32, screenHeight/2+185,"TETRIS");
}



///função principal
void render()
{
    CV::clear(0,0,0);//fundo

    if(g_jogar==0){
        menu();
    }
    if (g_jogar==1){
        tetris();
    }
}



///funções auxiliares  para mover e rotacionar as peças

    ///movimento (horizontal e vertical)
void congelapeca()//congelar a peça e gerar outra quando encostar a base
{
    g_pontuacao+= 50;
    for(int i=linhas_tabuleiro-1; i>=0; i--){
        for(int j=colunas_tabuleiro-1; j>=0; j--){
            if(g_tabuleiro[i][j] <0){
                g_tabuleiro[i][j] = g_tabuleiro[i][j] * -1; //tornar a peça imóvel caso tenha encostado
                congelajogo(); //verificar se há bloco impedindo a próxima peça
                if(g_congelar==1){
                    g_mexendo=2;//para de gerar novas peças
                }
                else{
                    g_mexendo=0; //gerar nova peça
                }
            }
        }
    }
}


void copiamatriz()//copiar a matriz para prever a mudança e ver se vai ter colisão
{
    for(int i=0; i<20; i++){
        for(int j=0; j<10; j++){
            g_copiatabuleiro[i][j] = g_tabuleiro[i][j];
        }
    }
}


void verificacolisao()
{
    for(int i=0; i<20; i++){
        for(int j=0; j<10; j++){
            if(g_copiatabuleiro[i][j] <0 && g_tabuleiro[i][j] >0){//se a peça móvel for parar no mesmo espaço que uma fixa
                g_encostapeca=1;
            }
        }
    }
}


    ///rotação
void zeramatrizrotacaocopia()//zera a copia da matriz de rotação
{
    if(g_pecaativa==3){
        for(int i=0; i<tam_matriz_rotacao_padrao; i++){
            for(int j=0; j<tam_matriz_rotacao_padrao; j++){
                g_rotacaocopia3[i][j] = 0;
            }
        }
    }
    if(g_pecaativa==2){
        for(int i=0; i<tam_matriz_rotacao_reta; i++){
            for(int j=0; j<tam_matriz_rotacao_reta; j++){
                g_rotacaocopia5[i][j] = 0;
            }
        }
    }
}


void geramatrizrotacao()//gera a matriz que vai ser rotacionada a partir do centro de rotação da peça
{
    if(g_pecaativa==3){//se não for a reta
        for(int i=0; i<20; i++){
            for(int j=0; j<10; j++){
                if(g_tabuleiro[i][j]<-10){//se achar o centro de rotação da peça
                    for(int k=0; k<tam_matriz_rotacao_padrao; k++){
                        for(int l=0; l<tam_matriz_rotacao_padrao; l++){
                            g_matrizoutraspecas[k][l]=g_tabuleiro[i+k-1][j+l-1];
                        }
                    }
                }
            }
        }
    }
    if(g_pecaativa==2){ //se for a reta
        for(int i=0; i<20; i++){
            for(int j=0; j<10; j++){
                if(g_tabuleiro[i][j]<-10){//se achar o centro de rotação da peça
                    for(int k=0; k<tam_matriz_rotacao_reta; k++){
                        for(int l=0; l<tam_matriz_rotacao_reta; l++){
                            g_matrizreta[k][l]=g_tabuleiro[i+k-2][j+l-2];
                        }
                    }
                }
            }
        }
    }

}


void rotacionapadrao() //rotaciona as peças que podem ser rotacionadas com uma matriz 3x3
{
    //verificar se a rotação pode ser feita nas bordas do tabuleiro
    for(int i=0; i<20; i++){
        for(int j=0; j<10; j++){
            if(g_tabuleiro[i][j]<-10){//se achar o centro de rotação da peça
                if(i==19 || j==0 || j==9){//se não for possível rotacionar na base ou nas laterais (matriz de rotação para fora do tabuleiro)
                    g_encostandoborda=1;
                }
            }
        }
    }

    //verificar se a rotação não resultará em colisão
    copiamatriz();
    for(int i=0; i<tam_matriz_rotacao_padrao; i++){
        for(int j=0; j<tam_matriz_rotacao_padrao; j++){
            g_rotacaocopia3[j][tam_matriz_rotacao_padrao-1-i]=g_matrizoutraspecas[i][j];//girar a cópia da matriz de rotacao
        }
    }
    for(int i=0; i<20; i++){
        for(int j=0; j<10; j++){
            if(g_copiatabuleiro[i][j]<-10){//se achar o centro de rotação da peça
                for(int k=0; k<tam_matriz_rotacao_padrao; k++){
                    for(int l=0; l<tam_matriz_rotacao_padrao; l++){
                        if(g_copiatabuleiro[i+k-1][j+l-1]<0){
                            g_copiatabuleiro[i+k-1][j+l-1]=0;//apaga a peça antiga
                        }
                        if(g_rotacaocopia3[k][l]<0){ //passa só o que está em movimento
                            g_copiatabuleiro[i+k-1][j+l-1] = g_rotacaocopia3[k][l]; //passar as peças rotacionadas pro tabuleiro original
                        }
                    }
                }
            }
        }
    }
    verificacolisao();


    //quando possível rotacionar
    if(g_encostandoborda==0 && g_encostapeca==0){
        for(int i=0; i<tam_matriz_rotacao_padrao; i++){
            for(int j=0; j<tam_matriz_rotacao_padrao; j++){
                        g_rotacaocopia3[j][tam_matriz_rotacao_padrao-1-i]=g_matrizoutraspecas[i][j];//girar a cópia da matriz de rotacao
            }
        }
        for(int i=0; i<20; i++){
            for(int j=0; j<10; j++){
                if(g_tabuleiro[i][j]<-10){//se achar o centro de rotação da peça
                    for(int k=0; k<tam_matriz_rotacao_padrao; k++){
                        for(int l=0; l<tam_matriz_rotacao_padrao; l++){
                            if(g_tabuleiro[i+k-1][j+l-1]<0){
                                g_tabuleiro[i+k-1][j+l-1]=0;//apaga a peça antiga
                            }
                            if(g_rotacaocopia3[k][l]<0){ //passa só o que está em movimento
                                g_tabuleiro[i+k-1][j+l-1] = g_rotacaocopia3[k][l]; //passar as peças rotacionadas pro tabuleiro original
                            }
                        }
                    }
                }
            }
        }
    }

}


void rotacionareta() //rotaciona a reta com uma matriz 5x5
{
    //verificar se a rotação pode ser feita nas bordas do tabuleiro
    for(int i=0; i<20; i++){
        for(int j=0; j<10; j++){
            if(g_tabuleiro[i][j]<-10){//se achar o centro de rotação da peça
                if(i>=18 || j<=1 || j>=8){//se não for possível rotacionar na base ou nas laterais (matriz de rotação para fora do tabuleiro)
                    g_encostandoborda=1;
                }
            }
        }
    }

    //verificar se a rotação não resultará em colisão
    copiamatriz();
    for(int i=0; i<tam_matriz_rotacao_reta; i++){
        for(int j=0; j<tam_matriz_rotacao_reta; j++){
                g_rotacaocopia5[j][tam_matriz_rotacao_reta-1-i]=g_matrizreta[i][j];//girar a cópia da matriz de rotacao
        }
    }
    for(int i=0; i<20; i++){
        for(int j=0; j<10; j++){
            if(g_copiatabuleiro[i][j]<-10){//se achar o centro de rotação da peça
                for(int k=0; k<tam_matriz_rotacao_reta; k++){
                    for(int l=0; l<tam_matriz_rotacao_reta; l++){
                            if(g_copiatabuleiro[i+k-2][j+l-2] <0){
                                g_copiatabuleiro[i+k-2][j+l-2]=0;//apaga a peça antiga
                            }
                            if(g_rotacaocopia5[k][l]<0)  { //passa só o que está em movimento
                                g_copiatabuleiro[i+k-2][j+l-2] = g_rotacaocopia5[k][l]; //passar as peças rotacionadas pro tabuleiro original
                            }
                    }
                }
            }
        }
    }
    verificacolisao();

    //quando possível rotacionar
    if(g_encostandoborda==0 && g_encostapeca==0){
        for(int i=0; i<tam_matriz_rotacao_reta; i++){
            for(int j=0; j<tam_matriz_rotacao_reta; j++){
                    g_rotacaocopia5[j][tam_matriz_rotacao_reta-1-i]=g_matrizreta[i][j];//girar a cópia da matriz de rotacao
            }
        }
        for(int i=0; i<20; i++){
            for(int j=0; j<10; j++){
                if(g_tabuleiro[i][j]<-10){//se achar o centro de rotação da peça
                    for(int k=0; k<tam_matriz_rotacao_reta; k++){
                        for(int l=0; l<tam_matriz_rotacao_reta; l++){
                                if(g_tabuleiro[i+k-2][j+l-2] <0){
                                    g_tabuleiro[i+k-2][j+l-2]=0;//apaga a peça antiga
                                }
                                if(g_rotacaocopia5[k][l]<0)  { //passa só o que está em movimento
                                    g_tabuleiro[i+k-2][j+l-2] = g_rotacaocopia5[k][l]; //passar as peças rotacionadas pro tabuleiro original
                                }
                        }
                    }
                }
            }
        }
    }

}


///Funções principais para mover e rotacionar as peças

///girar a peça
void Girar()
{
    g_encostandoborda=0;
    g_encostapeca=0;

    geramatrizrotacao();
    zeramatrizrotacaocopia();

    if(g_pecaativa==3){//se nao for quadrado ou reta
        rotacionapadrao();
    }

    if(g_pecaativa==2){//reta
        rotacionareta();
    }

}


///mover para baixo
void MoveBaixo()
{
    g_encostandoborda=0;
    g_encostapeca=0;

    //verificar colisão borda inferior
    for(int i=linhas_tabuleiro-1; i>=0; i--){
        for(int j=colunas_tabuleiro-1; j>=0; j--){
            if(g_tabuleiro[i][j] <0){
                if(i==19){
                    g_encostandoborda=1;
                }
            }
        }
    }
    if(g_encostandoborda==1){
        congelapeca();
    }

    //verificar colisão com outras peças
    copiamatriz();
    for(int i=linhas_tabuleiro-1; i>=0; i--){
        for(int j=colunas_tabuleiro-1; j>=0; j--){
                if(g_copiatabuleiro[i][j]<0){
                g_troca = g_copiatabuleiro[i][j];
                g_copiatabuleiro[i][j] = g_copiatabuleiro[i+1][j];
                g_copiatabuleiro[i+1][j] = g_troca;
                }
        }
    }
    verificacolisao();
    if(g_encostapeca==1){
        congelapeca();
    }

    //movimentação da peça (quando possível)
    for(int i=linhas_tabuleiro-1; i>=0; i--){
        for(int j=colunas_tabuleiro-1; j>=0; j--){
            if(g_tabuleiro[i][j] <0 && g_encostandoborda==0 && g_encostapeca==0){//se estiver sendo movido e não estiver encostando na borda e em outra peça
                g_troca = g_tabuleiro[i][j];
                g_tabuleiro[i][j] = g_tabuleiro[i+1][j];
                g_tabuleiro[i+1][j] = g_troca;
            }
        }
    }

}


///mover para direita
void MoveDireita()
{
    g_encostandoborda=0;
    g_encostapeca=0;

    //verificar se algum bloco da peça está encostando na borda direita
    for(int i=linhas_tabuleiro-1; i>=0; i--){
        for(int j=colunas_tabuleiro-1; j>=0; j--){
            if(g_tabuleiro[i][j] <0){
                if(j==9){
                    g_encostandoborda=1;
                }
            }
        }
    }

    //verificar colisão com outras peças
    copiamatriz();
    for(int i=linhas_tabuleiro-1; i>=0; i--){
        for(int j=colunas_tabuleiro-1; j>=0; j--){
            if(g_tabuleiro[i][j] <0){
                g_troca = g_copiatabuleiro[i][j];
                g_copiatabuleiro[i][j] = g_copiatabuleiro[i][j+1];
                g_copiatabuleiro[i][j+1] = g_troca;
            }
        }
    }
    verificacolisao();

    //movimentação da peça (quando possível)
    for(int i=linhas_tabuleiro-1; i>=0; i--){
        for(int j=colunas_tabuleiro-1; j>=0; j--){
            if(g_tabuleiro[i][j] <0 && g_encostandoborda==0 && g_encostapeca==0){//se estiver sendo movido e não estiver encostando na borda
                g_troca = g_tabuleiro[i][j];
                g_tabuleiro[i][j] = g_tabuleiro[i][j+1];
                g_tabuleiro[i][j+1] = g_troca;
            }
        }
    }

}


///mover para esquerda
void MoveEsquerda()
{
    g_encostandoborda=0;
    g_encostapeca=0;

    //verificar se algum bloco da peça está encostando na borda esquerda
    for(int i=linhas_tabuleiro-1; i>=0; i--){
        for(int j=colunas_tabuleiro-1; j>=0; j--){
            if(g_tabuleiro[i][j] <0){
                if(j==0){
                    g_encostandoborda=1;
                }
            }
        }
    }

    //verificar colisão com outras peças
    copiamatriz();
    for(int i=0; i<linhas_tabuleiro; i++){
        for(int j=0; j<colunas_tabuleiro; j++){
            if(g_tabuleiro[i][j] <0){
                g_troca = g_copiatabuleiro[i][j];
                g_copiatabuleiro[i][j] = g_copiatabuleiro[i][j-1];
                g_copiatabuleiro[i][j-1] = g_troca;
            }
        }
    }
    verificacolisao();

    //movimentação da peça (quando possível)
    for(int i=0; i<linhas_tabuleiro; i++){
        for(int j=0; j<colunas_tabuleiro; j++){
            if(g_tabuleiro[i][j] <0 && g_encostandoborda==0 && g_encostapeca==0){//se estiver sendo movido e não estiver encostando na borda
                g_troca = g_tabuleiro[i][j];
                g_tabuleiro[i][j] = g_tabuleiro[i][j-1];
                g_tabuleiro[i][j-1] = g_troca;
            }
        }
    }

}


//funcao chamada toda vez que uma tecla for pressionada
void keyboard(int key)
{
   switch(key)
   {
      case 27: //finaliza programa
	     exit(0);
        break;
	  case 201: //seta para cima
        if(g_pecaativa!=1){//se não for o quadrado porque ele não precisa ser rotacionado
            Girar();
        }
        break;
	  case 203: //seta para a baixo
        MoveBaixo();
        break;
	  case 202: //seta para direita
        MoveDireita();
        break;
	  case 200: //seta para esquerda
        MoveEsquerda();
        break;
   }
}


//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
}


//funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   mouseX = x; //guarda as coordenadas do mouse para exibir dentro da render()
   mouseY = y;

   cliquemouse=button;
}


int main(void)
{
   CV::init(&screenWidth, &screenHeight, "Tetris");

    for(int i=0; i<linhas_tabuleiro; i++){//preencher o tabuleiro com 0
        for(int j=0; j<colunas_tabuleiro; j++){
            g_tabuleiro[i][j]=0;
        }
    }

    srand(time(NULL));
    proximapeca();//gerar a peça inicial

    tempo = clock();//começar a contar o tempo pra peça cair pela primeira vez

   CV::run();
}

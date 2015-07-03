	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#include<time.h>
	#include<math.h>


	//variavel global usada. O ID e um valor passado pelo programa principal que vai identificar se o jogador e o player 1 ou player 2
int id;
	//ID do jogo
int ident;

int modificacao;

int move[5];

char range_symbol[3];
char e_range_symbol[3];

int plantedBomb = 0; //If it planted a bomb it looks into running a away from it.

	//esta estrutura guarda as posicoes em i e j num tabuleiro
typedef struct pos
{
	int i;
	int j;
}pos;

typedef struct info_bomba
{
	int i;
	int j;
	int range;
}info_bomba;

pos posIam;
pos oldPos; //a Posicao anterior do usuario

int jogarbomba; //Verifica se vai jogar bomba nesta posicao ou nao;
int willexplode; //Indica se ira jogar bomba nesta posicao ou nao;

//no maximmo pode ter 5 bombas(TMB)
info_bomba bombas[5];// guarda as posicoes da bomba;
int qtd_bombas = 0;
int qtd_bombas_max = 2;
int range = 2; //Alcance das bombas;

info_bomba bombas_enemy[5];
int qtd_bombas_enemy = 0;
int range_enemy = 2;
int qtd_bombas_max_enemy = 2;

// nome das nossas bombas
char bomb[3];
// nome das bombas do inimigo
char enemyBomb[3];

pos matinhos[100];
int qtdMatinhos = 0;
int rodada = 1;

pos bonus_range[100];
int qtdBonusRange = 0;
pos bonus_bombas[100];
int qtdBonusBomba = 0;

//vetores de deslocamento. {parado, sobe, esquerda, desce, direita}
int dx[] = {0,-1,0,1,0};
int dy[] = {0,0,-1,0,1};


FILE *fp = NULL; //objeto que possui metodos para escrevermos nos objetos;

//estrutura do tabuleiro. Como e sabido, os dois primeiros caracteres guardam as informacoes sobre o jogadore e os dois ultimos
//sobre a presenca de bombas, bonus, etc
//logo, usa-se duas strings

typedef struct tabela
{
	char str1[3],str2[3];
}tabela;

//mapa a ser lido.
tabela tab[11][13];
//mapa secundario
tabela tab2[11][13];
// tabela com os pesos das posições do mapa
int tabPeso[11][13];

// Nome do nosso player
char s[3];
// Nome do Player inimigo
char enemyS[3];

//funcao que realiza a leitura do mapa. O mapa e passado como entrada padrao pelo programa principal, portanto, nesses casos, usem scanf normalmente.
//usem fscanf para trabalhar com arquivos criados por voces.

void leitura()
{
	int i, j;
	for(i = 0; i<11; i++)
	{
		for(j = 0; j<13; j++)
		{
			char temp[5];
			scanf("%s", temp);
			tab[i][j].str1[0] = temp[0];
			tab[i][j].str1[1] = temp[1];
			tab[i][j].str2[0] = temp[2];
			tab[i][j].str2[1] = temp[3];
		}
	}
}

// verifica se o mapa secundario eh igual ao mapa principal
void verificarMapa2()
{
	int i, j;
	for(i = 0; i<11; i++)
	{
		for(j = 0; j<13; j++)
		{
			//verifica se a primeira string eh igual
			if(strcmp(tab2[i][j].str1, tab[i][j].str1) != 0)
			{
				//se for diferente verifica se a segunda string é diferente de F1
				//CASO
				//tab  - MMMM
				//tab2 - --F1
				//(pq a gente quer representar que o matinho explodiu)
				if(strcmp(tab2[i][j].str2, range_symbol) != 0){
					strcpy(tab2[i][j].str1, tab[i][j].str1);
				}
			//explodir o matinho
			}else if(strcmp(tab2[i][j].str2, range_symbol) == 0)
			{
				strcpy(tab2[i][j].str1, "--");
			}
			//verificar a segunda string
			if(strcmp(tab2[i][j].str2, tab[i][j].str2) != 0)
			{
				//verifica se no mapa secundario tem o char 'F' o qual é normal que exista
				if(strcmp(tab2[i][j].str2, range_symbol) == 0) continue;
				strcpy(tab2[i][j].str2, tab[i][j].str2);
			}
		}
	}
}

// cria mapa com pesesos para cada posição
void criarMapaPeso(){
	/*
		-------------------
		| POSIÇÃO  | PESO |
		-------------------
		| Pnosso   |    0 |
		-------------------
		| pInimigo |    0 |
		-------------------
		| --       |    1 |
		-------------------
		| MM       |    3 |
		-------------------
		| XX       |  999 |
		-------------------
		| Fnosso   |    5 |
		-------------------
		| Finimigo |   98 |
		-------------------
		| Bnosso   |  999 |
		-------------------
		| Binimigo |  999 |
		-------------------
	*/
	int i, j;
	for (i = 0; i < 11; ++i){
		for (j = 0; j < 13; ++j){
			if(strcmp(tab2[i][j].str1, s) == 0) tabPeso[i][j] = 2;
			else if(strcmp(tab2[i][j].str1, enemyS) == 0) tabPeso[i][j] = 0;
			else if(strcmp(tab2[i][j].str2, "--") == 0) tabPeso[i][j] = 1;
			else if(strcmp(tab2[i][j].str2, "MM") == 0) tabPeso[i][j] = 3;
			else if(strcmp(tab2[i][j].str2, "XX") == 0) tabPeso[i][j] = 999;
			else if(strcmp(tab2[i][j].str2, range_symbol) == 0) tabPeso[i][j] = 5;
			else if(strcmp(tab2[i][j].str2, e_range_symbol) == 0) tabPeso[i][j] = 98;
			else if(strcmp(tab2[i][j].str2, bomb) == 0) tabPeso[i][j] = 999;
			else if(strcmp(tab2[i][j].str2, enemyBomb) == 0) tabPeso[i][j] = 999;
			
		}
	}
}

// cria um .txt com os pesos do mapa
void escreverMapaPeso(){

	fp = fopen("mapaPeso.txt","w+");
	int i, j;
	for(i = 0; i  < 11; i++){
		for(j = 0; j  < 13; j++){
			// imprime só o utimo valor do peso para ficar uma matriz arrumada
			fprintf(fp,"%d ", tabPeso[i][j]%10);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

//funcao retorna a posicao corrente de determinado jogador, cuja string (P1 ou P2) e passada como parametro.
pos cur_pos(char* player)
{
	pos posi;
	int i,j;
	for(i = 0; i < 11; i++){
		for(j = 0; j < 13; j++)
		{
			if(strcmp(tab[i][j].str1, player) == 0 || strcmp(tab[i][j].str1, "B3") == 0 )
			{
				posi.i = i;
				posi.j = j;
				return posi;
			}
		}
	}
}

int theyAreTogether(){
	int i, j;
	for(i = 0; i < 11; i++){
		for(j = 0; j < 13; j++)
		{
			if(strcmp(tab[i][j].str1, "P3") == 0 )
			{
				return 1;
			}
		}
	}
	return 0;
}


/*
Returns the distance between two objects;
*/
double distance(int x1,int x2,int y1,int y2){
	double dist = sqrt((y2-y1)^2 + (x2-x1)^2);
	return dist;
}

//funcao que checa se o movimento e valido em determinada posicao
int check(int x, int y)
{
	if(x>=0 && x<11 && y>=0 && y<13 && (strcmp(tab[x][y].str2,"--")==0  || strcmp(tab[x][y].str2,"+F")==0 || strcmp(tab[x][y].str2,"+B")==0) && strcmp(tab[x][y].str2,"MM") != 0 && strcmp(tab[x][y].str2,"XX") != 0)
		return 1;
	else
		return 0;
}

int checkPos(int x, int y){
	if(x >= 0 && x < 11 && y >=0 && y <13){
		return 1;
	}else
		return 0;
}


int manhattanDistance(int playerX,int enemyX,int playerY, int enemyY){

	int distanceX = abs(playerX - enemyX);
	int distanceY = abs(playerY - enemyY);
	return distanceX+distanceY;
}


//Caso seja uma posicao 100% segura (ou seja, na diagonal) da minha bomba inicial, então é uma boa posição para ir.
void checkSafety(int x, int y){

	if(qtd_bombas > 0){
		if(x-1 != bombas[0].i && y != bombas[0].j){
			move[1] += 3;
		}

		if(x != bombas[0].i && y-1 != bombas[0].j){
			move[2] += 3;
		}

		if(x+1 != bombas[0].i && y != bombas[0].j){
			move[3] += 3;
		}

		if(x != bombas[0].i && y+1 != bombas[0].j){
			move[4] += 3;
		}
	}
}

// coloca os FF no mapa2 
void bombaColocouMapa2(int x, int y){
	strcpy(tab2[x][y].str2, bomb);
	int i, j, matoAntes, paredeAntes;
	for(j = 1; j<5; j++){
		matoAntes = 0;
		paredeAntes = 0;
		for(i = 1; i<=range; i++){
			// verifica se teve mato em alguma posição anterios
			if(checkPos(x+(dx[j]*i), y+(dy[j]*i)) && strcmp(tab2[x+(dx[j]*i)][y+(dy[j]*i)].str2, "MM") == 0) 
				matoAntes = 1;
			// verifica se teve parede em alguma posição anterior
			else if(checkPos(x+(dx[j]*i), y+(dy[j]*i)) && strcmp(tab2[x+(dx[j]*i)][y+(dy[j]*i)].str2, "XX") == 0) 
				paredeAntes = 1;
			
			// coloca FF no mapa se não tive passado anteriormente por uma parece ou mato
			if(check(x+(dx[j]*i), y+(dy[j]*i)) && !matoAntes && !paredeAntes)	
				strcpy(tab2[x+(dx[j]*i)][y+(dy[j]*i)].str2, range_symbol);			
		}
	}
}

	//funcao se determina se devo soltar uma bomba ou nao
int soltarbomba(int x, int y,int enemyX,int enemyY){
	if(qtd_bombas < 1){
		int impossibleWays = 0;
		if(!check(x,y+1)){
			impossibleWays++;
		}

		if(!check(x+1,y)){
			impossibleWays++;
		}

		if(!check(x-1,y)){
			impossibleWays++;
		}

		if(!check(x,y-1)){
			impossibleWays++;
		}

		if(impossibleWays == 3 || ( x == enemyX  && (distance(x,enemyX,y,enemyY) < 4))|| (y == enemyY && (distance(x,enemyX,y,enemyY) < 4))){ //If all my surroundings are gone then it is better to get rid off it.
			bombas[qtd_bombas].i = x;
			bombas[qtd_bombas].j = y;
			bombas[qtd_bombas].range = range;
			qtd_bombas++;
			bombaColocouMapa2(x, y);
			return 1;
		}			


		if(checkPos(x,y+1)){

			/*Checo se à minha direita existe mato/ 
			y < enemyY is because if I am the left of him, going is probably going to make me closer to him
			*/
			if(strcmp(tab[x][y+1].str2,"MM") == 0 && (y < enemyY)){ 
				bombas[qtd_bombas].i = x;
				bombas[qtd_bombas].j = y;
				bombas[qtd_bombas].range = range;
				qtd_bombas++;
				bombaColocouMapa2(x, y);
				return 1;

			}
		}

		if(checkPos(x+1,y)){

			/*Checo se abaixo de mim existe mato/ 
			x < enemyX is because if I am the up to him, going is probably going to make me closer to him
			*/
			if(strcmp(tab[x+1][y].str2,"MM") == 0 && (x < enemyX)){ 
				bombas[qtd_bombas].i = x;
				bombas[qtd_bombas].j = y;
				bombas[qtd_bombas].range = range;
				qtd_bombas++;
				bombaColocouMapa2(x, y);
				return 1;

			}
		}

		if(checkPos(x,y-1)){

			/*Checo se à minha direita existe mato/ 
			y < enemyY is because if I am the left of him, going is probably going to make me closer to him
			*/
			if(strcmp(tab[x][y-1].str2,"MM") == 0 && (y > enemyY)){ 
				bombas[qtd_bombas].i = x;
				bombas[qtd_bombas].j = y;
				bombas[qtd_bombas].range = range;
				qtd_bombas++;
				bombaColocouMapa2(x, y);
				return 1;

			}
		}


		if(checkPos(x-1,y)){

			/*Checo se à minha direita existe mato/ 
			y < enemyY is because if I am the left of him, going is probably going to make me closer to him
			*/
			if(strcmp(tab[x-1][y].str2,"MM") == 0 && (x >  enemyX)){ 
				bombas[qtd_bombas].i = x;
				bombas[qtd_bombas].j = y;
				bombas[qtd_bombas].range = range;
				qtd_bombas++;
				bombaColocouMapa2(x, y);
				return 1;

			}
		}

	}
	return 0;
}





	//funcao: determina se caso voce exploda a ultima bomba, voce continue vivo ou qual lugar possui a maior distancia em relacao a bomba;
void bealive(int x, int y, int jogarbomba){
	if(qtd_bombas > 0){

		if(jogarbomba == 0){
			if(check(x-1,y)){
				if(x < bombas[0].i){
					move[1] += 3;
				}
			}

			if(check(x,y-1)){
				if((y < bombas[0].j)){
					move[2] += 3;
				}
			}

			if(check(x+1,y)){
				if(x > bombas[0].i){
					move[3] += 3;
				}
			}

			if(check(x,y+1)){
				if(y > bombas[0].j){
					move[4] += 3;
				}
			}

		}else{ //jogarbomba == 1

			if(check(x-1,y)){
				if((distance(x-1,bombas[0].i,y,bombas[0].j) >  distance(x,bombas[0].i,y,bombas[0].j)) && bombas[0].i != x-1){
					move[1] += 1;

					if(distance(x-1,bombas[0].i,y,bombas[0].j) < 4){
						bealive(x-1,y,jogarbomba);	
					}
				}
			}

			if(check(x,y-1)){
				if((distance(x,bombas[0].i,y-1,bombas[0].j) >  distance(x,bombas[0].i,y,bombas[0].j)) &&
					bombas[0].j != y-1){
					move[2] += 1;


					if(distance(x,bombas[0].i,y-1,bombas[0].j) < 4){
						bealive(x,y-1,jogarbomba);	
					}
				}
			}

			if(check(x+1,y)){
				if((distance(x+1,bombas[0].i,y,bombas[0].j) >  distance(x,bombas[0].i,y,bombas[0].j))
					&& bombas[0].i != x+1){
					move[3] +=1;


				if(distance(x+1,bombas[0].i,y,bombas[0].j) < 4){
					bealive(x+1,y,jogarbomba);	
				}
			}
	}

		if(check(x,y+1)){
			if((distance(x,bombas[0].i,y+1,bombas[0].j) >  distance(x,bombas[0].i,y,bombas[0].j)) &&
				bombas[0].j != y+1){
				move[4] += 1;


				if(distance(x,bombas[0].i,y+1,bombas[0].j) < 4){
					bealive(x,y+1,jogarbomba);	
				}
			}
		}
		}
	}	
}

//Arquivo de teste para escrever os dados que possuo;
void escreverValores(){
	fp = fopen("valores.txt","a");
	int i = 0;
	for(; i  < 5; i++){
		fprintf(fp," %d ", move[i]);
	}
	fprintf(fp,"\n");

	fclose(fp);
}

	//Escreve os dados das bombas atualizadas no arquivo;
void escreverBombas(){

	fp = fopen("bombas.txt","w+");
	int i = 0;
	//nossas bombas
	fprintf(fp, "%d %d %d\n",qtd_bombas,qtd_bombas_max,range); //tem que guardar o range atual
	for(; i  < qtd_bombas; i++){
		fprintf(fp,"%d %d %d ", bombas[i].range, bombas[i].i,bombas[i].j);
	}
	//bombas do inimigo
	fprintf(fp, "\n%d %d %d\n", qtd_bombas_enemy,qtd_bombas_max_enemy, range_enemy);
	for(i = 0; i < qtd_bombas_enemy; i++){
		fprintf(fp, "%d %d %d ", bombas_enemy[i].range, bombas_enemy[i].i, bombas_enemy[i].j);
	}
	fclose(fp);
}

	//Le as coordenadas atuais das bombas;
void lerBombas(){

	fp = fopen("bombas.txt","r+");
	if(fp != NULL){
		int i = 0;
		//nossas bombas
		fscanf(fp," %d %d %d", &qtd_bombas,&qtd_bombas_max, &range);
		for(; i  < qtd_bombas; i++){
			fscanf(fp,"%d %d %d ", &bombas[i].range, &bombas[i].i, &bombas[i].j);
		}
		//bombas do inimigo
		fscanf(fp, " %d %d %d", &qtd_bombas_enemy,&qtd_bombas_max_enemy, &range_enemy);
		for(i = 0; i < qtd_bombas_enemy; i++){
			fscanf(fp, "%d %d %d ", &bombas_enemy[i].range, &bombas_enemy[i].i, &bombas_enemy[i].j);
		}
		fclose(fp);
	}
}


void modifybombs(){
	int i  = 0;
	for(;i < qtd_bombas;i++){
		bombas[i] = bombas[i+1];
	}
}

// metodo para retirar B# e FF da bomba que explodiu
void bombaExplodiuMapa2()
{
	int i = bombas[0].i;
	int j = bombas[0].j;
	int range = bombas[0].range;
	strcpy(tab2[i][j].str2, "--");
	int k, l;
	for(l = 1; l<5; l++)
	{
		for(k = 1; k<=range; k++)
		{
			if(check(i+(dx[l]*k), j+(dy[l]*k)))
			{
				strcpy(tab2[i+(dx[l]*k)][j+(dy[l]*k)].str1, "--");
				strcpy(tab2[i+(dx[l]*k)][j+(dy[l]*k)].str2, "--");
			}
		}
	}
}


//x is the player position in the x-axis
//y is the player position in the y-axis
int explodirbomba(int x,int y,int where){
	int retorno = 0;
	if(qtd_bombas > 0){
		int i = 0;
		int j = 0;//size of the map
		
		switch(where){
			case 0:
				if(check(x,y)){ //Parece desnecessário , contudo, estou utilizando para fins de teste.
					if(strcmp(tab2[x][y].str2,range_symbol) != 0 &&  strcmp(tab2[x][y].str2,bomb) != 0){
						retorno = 1;
					}
				}
			break;
			case 1:
				if(check(x-1,y)){
					if(strcmp(tab2[x-1][y].str2,range_symbol) != 0 &&  strcmp(tab2[x-1][y].str2,bomb) != 0){
						retorno = 1;
					}
				}
			break;
			case 2:
				if(check(x,y-1)){
					if(strcmp(tab2[x][y-1].str2,range_symbol) != 0 &&  strcmp(tab2[x][y-1].str2,bomb) != 0){
						retorno = 1;
					}
				}
			break;
			case 3:
				if(check(x+1,y)){
					if(strcmp(tab2[x+1][y].str2,range_symbol) != 0 &&  strcmp(tab2[x+1][y].str2,bomb) != 0){
						retorno = 1;
					}
				}
			break;
			case 4:
				if(check(x,y+1)){
					if(strcmp(tab2[x][y+1].str2,range_symbol) != 0 &&  strcmp(tab2[x][y+1].str2,bomb) != 0){
						retorno = 1;
					}
				}
			break;
		}
		if(retorno == 1){
			qtd_bombas--;
			bombaExplodiuMapa2();
			modifybombs();

			if(qtd_bombas > 0) bombaExplodiuMapa2(bombas[0].i,bombas[0].j); //Caso a quantidade de bombas ainda seja maior do que 0, teremos de colocar os novos F's no mapa pois agora bomba[1] pasosu a ser bomba[0].

		}
	}
	return retorno;
}

	

void lerPosicao(){

	fp = fopen("posicao.txt","r+");
	if(fp != NULL){
		fscanf(fp,"%d %d ",&oldPos.i, &oldPos.j);
		fclose(fp);
	}


}

void escreverPosicao(int x , int y){

	fp = fopen("posicao.txt","w+");
	fprintf(fp," %d %d ",x,y);

	fclose(fp);

}

void checkWays(int playerX,int playerY){
		if(check(playerX,playerY) == 0){ //parado
			move[0] -= 1;
		}

		if(check(playerX-1,playerY) == 0){ //sobe
			move[1] -= 1;
		}

		if(check(playerX,playerY-1) == 0){ //esquerda
			move[2] -= 1;
		}

		if(check(playerX+1,playerY) == 0){ //desce
			move[3] -= 1;
		}

		if(check(playerX,playerY+1) == 0){ //direta
			move[4] -= 1;
		}

}

	/*
		O lado que conseguir a maior quantidade de "pontos" sera o lado escolhido
	*/
int MAIOR(int *vec)
{
	int i, maior = 0;
	for(i = 0; i<5; i++)
	{
		if(vec[i]>vec[maior])
		maior = i;
	}
	return maior;
}



//verifica se as bombas ainda estão no lugar ou se foram explodidas
//verifiquem se esta certo
void verificar_bombas(){
	int i, j;
	for(i = 0; i < qtd_bombas_enemy; i++){
		if(strcmp(tab[bombas_enemy[i].i][bombas_enemy[i].j].str2, enemyBomb) != 0){
			//se for o ultimo do array
			if(i == qtd_bombas_enemy-1){
				qtd_bombas_enemy--;
			}else{
				//igual ao modifyBombs
				for(j = i+1;j < qtd_bombas_enemy; j++){
					bombas_enemy[i-1] = bombas_enemy[i];
				}
				qtd_bombas_enemy--;
			}
		}
	}
}

//recebe a pos do inimigo e verifica se ha bombas por perto
void bombas_inimigo(int x, int y){
	if(checkPos(x+1,y)){
		if(strcmp(tab[x+1][y].str2,enemyBomb) == 0){
			if(verificar_bombas_inimigo(x+1,y)){
				bombas_enemy[qtd_bombas_enemy].i = x+1;
				bombas_enemy[qtd_bombas_enemy].j = y;
				bombas_enemy[qtd_bombas_enemy].range = range_enemy;
				qtd_bombas_enemy++;
			}
		}
	}
	if(checkPos(x,y+1)){
		if(strcmp(tab[x][y+1].str2,enemyBomb) == 0){
			if(verificar_bombas_inimigo(x,y+1)){
				bombas_enemy[qtd_bombas_enemy].i = x;
				bombas_enemy[qtd_bombas_enemy].j = y+1;
				bombas_enemy[qtd_bombas_enemy].range = range_enemy;
				qtd_bombas_enemy++;
			}
		}
	}
	if(checkPos(x-1,y)){
		if(strcmp(tab[x-1][y].str2,enemyBomb) == 0){
			if(verificar_bombas_inimigo(x-1,y)){
				bombas_enemy[qtd_bombas_enemy].i = x-1;
				bombas_enemy[qtd_bombas_enemy].j = y;
				bombas_enemy[qtd_bombas_enemy].range = range_enemy;
				qtd_bombas_enemy++;
			}
		}
	}
	if(checkPos(x,y-1)){
		if(strcmp(tab[x][y-1].str2,enemyBomb) == 0){
			if(verificar_bombas_inimigo(x,y-1)){
				bombas_enemy[qtd_bombas_enemy].i = x;
				bombas_enemy[qtd_bombas_enemy].j = y-1;
				bombas_enemy[qtd_bombas_enemy].range = range_enemy;
				qtd_bombas_enemy++;
			}
		}
	}
	if(checkPos(x,y)){
		if(strcmp(tab[x][y].str2,enemyBomb) == 0){
			if(verificar_bombas_inimigo(x,y)){
				bombas_enemy[qtd_bombas_enemy].i = x;
				bombas_enemy[qtd_bombas_enemy].j = y;
				bombas_enemy[qtd_bombas_enemy].range = range_enemy;
				qtd_bombas_enemy++;
			}
		}
	}
}

int verificar_bombas_inimigo(int x, int y){
	int i;
	for(i = 0; i < qtd_bombas_enemy; i++){
		if(bombas_enemy[i].i == x && bombas_enemy[i].j == y){
			return 0;
		}
	}
	return 1;
}

void verificarBonus()
{
	int i;
	// Bonus Range
	for(i = 0; i < qtdBonusRange; i++)
	{	
		//se o bonus nao estiver mais la
		if(strcmp(tab[bonus_range[i].i][bonus_range[i].j].str2, "+F") != 0) 
		{
			if(strcmp(tab[bonus_range[i].i][bonus_range[i].j].str1, s) == 0){ 
				range++;
			}else if(strcmp(tab[bonus_range[i].i][bonus_range[i].j].str1, enemyS) == 0){
				 range_enemy++;
			}
		
		
			//caso seja o ultimo bonus do array
			if(i == qtdBonusRange-1) qtdBonusRange--;
			//caso NAO seja o ultimo bonus do array
			else 
			{
				bonus_range[i] = bonus_range[qtdBonusRange-1];
				qtdBonusRange--;
			} 
		}
	}
	// Bonus Bomba
	for(i = 0; i < qtdBonusBomba; i++)
	{
		//se o bonus nao estiver mais la
		if(strcmp(tab[bonus_bombas[i].i][bonus_bombas[i].j].str2, "+B") != 0) 
 		{
			if(strcmp(tab[bonus_bombas[i].i][bonus_bombas[i].j].str1, s) == 0) qtd_bombas_max++;
			else if(strcmp(tab[bonus_range[i].i][bonus_range[i].j].str1, enemyS) == 0) qtd_bombas_max_enemy++;

			//caso seja o ultimo bonus do array
			if(i == qtdBonusBomba-1) qtdBonusBomba--;
			//caso NAO seja o ultimo bonus do array
			else 
			{
				bonus_bombas[i] = bonus_bombas[qtdBonusBomba-1];
				qtdBonusBomba--;
			} 
		}
	}
}

void escreverMapa2(){
	/* Coloca as bombas nas posições */
	int l, k = 0;
	for(; l < qtd_bombas;l++){
		bombaColocouMapa2(bombas[l].i,bombas[l].j);
	}

	for(; k < qtd_bombas_enemy;k++){
		bombaColocouMapa2(bombas_enemy[k].i,bombas_enemy[k].j);
	}


	fp = fopen("mapa2.txt","w+");
	int i, j;
	for(i = 0; i  < 11; i++){
		for(j = 0; j  < 13; j++){
			fprintf(fp,"%s%s ", tab2[i][j].str1, tab2[i][j].str2);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

	//Le as coordenadas atuais das bombas;
void lerMapa2(){

	fp = fopen("mapa2.txt","r+");
	if(fp != NULL){
		int i, j;
		for(i = 0; i<11; i++)
		{
			for(j = 0; j<13; j++)
			{
				char temp[5];
				fscanf(fp,"%s", temp);
				tab2[i][j].str1[0] = temp[0];
				tab2[i][j].str1[1] = temp[1];
				tab2[i][j].str2[0] = temp[2];
				tab2[i][j].str2[1] = temp[3];
			}
		}
		fclose(fp);
	}
}



int menorCaminho2;
int menorCaminho;


void criarMapa2(){
	int i, j;
	if(rodada == 1){
		
		for (i = 0; i < 11; i++)
		{
			for (j = 0; j < 13; j++)
			{
				strcpy(tab2[i][j].str1, tab[i][j].str1);
				strcpy(tab2[i][j].str2, tab[i][j].str2);
			}
		}
	}
}

void lerMatinhos(){
	int i;
	fp = fopen("matinhos.txt","r+");
	if(fp != NULL){
		fscanf(fp,"%d %d ",&rodada,&qtdMatinhos);
		for(i = 0; i < qtdMatinhos; i++){
			fscanf(fp," %d %d", &matinhos[i].i, &matinhos[i].j);
		}
		fclose(fp);
	}
}

void escreverMatinhos(){
	int i;
	fp = fopen("matinhos.txt","w+");
	fprintf(fp,"%d %d \n",rodada,qtdMatinhos);
	for(i = 0; i < qtdMatinhos; i++){
		fprintf(fp, "%d %d ", matinhos[i].i, matinhos[i].j);
	}
	fclose(fp);

}

//eu vou pegar todas as posições dos matinhos no comeco do jogo e vou guardar num arquivo
//ai só ficar vendo se algum matinho desse array foi quebrado e verificar se no lugar dele tem um bonus
//acho que tem um custo melhor do que ficar percorrendo o mapa o tempo inteiro
void posMatinhos(){
	int i, j;
	if(rodada == 1){
		for(i = 0; i < 11; i++){
			for(j = 0; j < 13; j++){
				if(strcmp(tab[i][j].str2,"MM") == 0){
					matinhos[qtdMatinhos].i = i;
					matinhos[qtdMatinhos].j = j;
					qtdMatinhos++;
				}
			}
		}
	}
}

void lerBonus(){
	int i;
	fp = fopen("bonus.txt","r+");
	if(fp != NULL){
		fscanf(fp,"%d ",&qtdBonusRange);
		for(i = 0; i < qtdBonusRange; i++){
			fscanf(fp," %d %d", &bonus_range[i].i, &bonus_range[i].j);
		}
		fscanf(fp," %d",&qtdBonusBomba);
		for(i = 0; i < qtdBonusBomba; i++){
			fscanf(fp," %d %d", &bonus_bombas[i].i, &bonus_bombas[i].j);
		}
		fclose(fp);
	}
}

void escreverBonus(){
	int i;
	fp = fopen("bonus.txt","w+");
	fprintf(fp,"%d \n",qtdBonusRange);
	for(i = 0; i < qtdBonusRange; i++){
		fprintf(fp, "%d %d ", bonus_range[i].i, bonus_range[i].j);
	}
	fprintf(fp,"\n%d \n",qtdBonusBomba);
	for(i = 0; i < qtdBonusBomba; i++){
		fprintf(fp, "%d %d ", bonus_bombas[i].i, bonus_bombas[i].j);
	}
	fclose(fp);

}

void adicionar_bonus(){
	int i;
	for(i = 0; i < qtdMatinhos; i++){
		//verificar se tem um bonus de range na posicao checada
		if(strcmp(tab[matinhos[i].i][matinhos[i].j].str2,"+F") == 0){
			bonus_range[qtdBonusRange].i = matinhos[i].i;
			bonus_range[qtdBonusRange].j = matinhos[i].j;
			if(i < qtdMatinhos-1){
				matinhos[i] = matinhos[qtdMatinhos-1];
			}
			qtdBonusRange++;
			qtdMatinhos--;
		}
		//verifica se tem um bonus de bomba na posicao checada
		if(strcmp(tab[matinhos[i].i][matinhos[i].j].str2,"+B") == 0){
			bonus_bombas[qtdBonusBomba].i = matinhos[i].i;
			bonus_bombas[qtdBonusBomba].j = matinhos[i].j;
			if(i < qtdMatinhos-1){
				matinhos[i] = matinhos[qtdMatinhos-1];
			}
			qtdBonusBomba++;
			qtdMatinhos--;
		}
	}
}


//menorCaminho e menorCaminho2 estao declarados em cima do debug
//int menorCaminho = 2147483647;
int dir = 0, dir_fixo = 10;
//pos proximo[2];
int marcacao[11][13];

void pathfinding(int x, int y, int caminho, int contador){
	if(checkPos(x, y) /*&& tabPeso[x][y] != -10*/ && marcacao[x][y] != 1){
		marcacao[x][y] = 1;
		if(strcmp(tab2[x][y].str1, enemyS) == 0){
			if(caminho < menorCaminho){
				menorCaminho = caminho;
				/*
				if(proximo[1].i != 100){
					//lixoooo, eu acho
					proximo[0].i = proximo[1].i;
					proximo[0].j = proximo[1].j;
					proximo[1].i = 100;
					//lixooo acabou
					//dir_fixo = dir;
				}*/
				dir_fixo = dir;
				//debug(x, y, 1, 0);
			}else{
				//debug(x, y, 2, 0);
			}
			return;
		}
		int /*temp,*/ peso = tabPeso[x][y];
		int contador_temp = contador;
		//ordem: {direita, baixo, esquerda, cima}
		//dx[] = {0,1,0,-1}
		//dy[] = {1,0,-1,0}
		//if(contador == 1){

		//debug(x, y, 0, peso);

		//otimizando
		if(caminho+peso >= menorCaminho){
			//tabPeso[x][y] = peso;
			return;
		}

		//debug(x, y, 4, caminho+peso);

		//direita
		if(contador_temp == 1){
			//lixo, talvez
			//proximo[1].i = x;
			//proximo[1].j = y+1;
			//lixo acabou
			dir = 4;
		}
		pathfinding(x, y+1, caminho+peso, contador + 1);
		//if(temp == 15){ dir = 4; return 4;}

		//esquerda
		if(contador_temp == 1){
			//proximo[1].i = x;
			//proximo[1].j = y-1;
			dir = 2;
		}
		pathfinding(x, y-1, caminho+peso, contador + 1);

		//cima
		if(contador_temp == 1){
			//proximo[1].i = x-1;
			//proximo[1].j = y;
			dir = 1;
		}
		pathfinding(x-1, y, caminho+peso, contador + 1);
		//if(temp == 15){ dir = 1; return 1;}
		
		//baixo
		if(contador_temp == 1){
			//proximo[1].i = x+1;
			//proximo[1].j = y;
			dir = 3;
		}
		pathfinding(x+1, y, caminho+peso, contador + 1);
		//if(temp == 15){ dir = 3; return 3;}
				
		//desmarcando a pos
		/*tabPeso[x][y] = peso;*/
		//}
	}
	return;
}

//menorCaminho2 = 2147483647;
int dir2 = 0, dir_fixo2 = 10;
pos proximo2[2];

//pra quando ele lançar uma bomba ele procurar um pos com "--", é basicamente o mesmo metodo de cima
void pathfinding_bomb_escape(int x, int y, int caminho, int contador){
	if(checkPos(x, y) && marcacao[x][y] != 1){
		marcacao[x][y] = 1;
		if(strcmp(tab2[x][y].str2, "--") == 0){
			if(caminho < menorCaminho2){
				menorCaminho2 = caminho;
				if(proximo2[1].i != 100){
					//proximo2[0].i = proximo2[1].i;
					//proximo2[0].j = proximo2[1].j;
					//proximo2[1].i = 100;
					//dir_fixo = dir;
				}
				dir_fixo2 = dir2;
				//debug(x, y, 1);
			}else{
				//debug(x, y, 2);
			}
			return;
		}

		if(strcmp(tab2[x][y].str2, "XX") == 0) return;
		if(strcmp(tab2[x][y].str2, enemyBomb) == 0) return;

		int peso, entrou = 0;
		int contador_temp = contador;

		//eu ponho pra que se ele encontrar o range da nossa bomba ele considerar peso 0, pq eu tinha
		//feito alguns testes e se nao diminuisse o peso ele tentava um caminho com um mato
		if(strcmp(tab2[x][y].str2, range_symbol) == 0){
			peso = 0;
		//	entrou = 1;
		}else{
			peso = tabPeso[x][y];
		}

		

		//otimizando
		if(caminho+peso >= menorCaminho2) return;

		//direita
		if(contador_temp == 1){
			//proximo2[1].i = x;
			//proximo2[1].j = y+1;
			dir2 = 4;
		}
		pathfinding_bomb_escape(x, y+1, caminho+peso, contador + 1);
		//if(temp == 15){ dir = 4; return 4;}

		//baixo
		if(contador_temp == 1){
			//proximo2[1].i = x+1;
			//proximo2[1].j = y;
			dir2 = 3;
		}
		pathfinding_bomb_escape(x+1, y, caminho+peso, contador + 1);
		//if(temp == 15){ dir = 3; return 3;}

		//esquerda
		if(contador_temp == 1){
			//proximo2[1].i = x;
			//proximo2[1].j = y-1;
			dir2 = 2;
		}
		pathfinding_bomb_escape(x, y-1, caminho+peso, contador + 1);

		//cima
		if(contador_temp == 1){
			//proximo2[1].i = x-1;
			//proximo2[1].j = y;
			dir2 = 1;
		}
		pathfinding_bomb_escape(x-1, y, caminho+peso, contador + 1);
		//if(temp == 15){ dir = 1; return 1;}

		//desmarcando a pos
		//if(entrou) tabPeso[x][y] = 5;
		//else tabPeso[x][y] = peso;
		//}
	}
	return;
}


typedef struct pospf{
	int i, j, pa;
}pospf;

typedef struct peso_cnt{
	int peso, cnt;
}peso_cnt;

peso_cnt pf2[11][13];
pospf all_pos[4*11*13];

void pathfinding2(int x, int y, int pesoAnt){
	pospf init;
	init.i = x;
	init.j = y;
	init.pa = pesoAnt;
	int num_pos = 0, k, w;
	all_pos[num_pos++] = init;
	for(k = 0; k < num_pos; k++){
		int i = all_pos[k].i;
		int j = all_pos[k].j;
		int pa = all_pos[k].pa;
		int temp;
		if(i < 0 || i >= 11) continue;
		if(j < 0 || j >= 13) continue;
		if(tabPeso[i][j] == 999) continue;
		//if(pf2[i][j].cnt == 4) continue;
		if(pf2[i][j].peso != 0) continue;
		if(strcmp(tab2[i][j].str1, enemyS) == 0 || strcmp(tab2[i][j].str1, s) == 0) temp = 1;
		else temp = tabPeso[i][j];
		/*if(temp+pa < pf2[i][j].peso)*/ pf2[i][j].peso = temp+pa;
		//pf2[i][j].cnt++;
		for(w = 1; w < 5; w++){
			pospf novo;
			novo.i = i+dx[w];
			novo.j = j+dy[w];
			novo.pa = pf2[i][j].peso;
			all_pos[num_pos++] = novo;
		}
	}
	/*if(checkPos(x, y) == 0) return;
	else if(pf2[x][y] != 0) return;
	int temp;
	if(strcmp(tab2[x][y].str1, enemyS) == 0 || strcmp(tab2[x][y].str1, s) == 0) temp = 1;
	else temp = tabPeso[x][y];
	pf2[x][y] = pesoAnt+temp;
	pathfinding2(x, y+1, pf2[x][y]);
	pathfinding2(x+1, y, pf2[x][y]);
	pathfinding2(x, y-1, pf2[x][y]);
	pathfinding2(x-1, y, pf2[x][y]);*/
}

peso_cnt pf3[11][13];
pospf all_pos_bombas[4*11*13];

void pathfinding_bombas(int x, int y, int pesoAnt){
	pospf init;
	init.i = x;
	init.j = y;
	init.pa = pesoAnt;
	int num_pos = 0, k, w;
	all_pos_bombas[num_pos++] = init;
	for(k = 0; k < num_pos; k++){
		int i = all_pos_bombas[k].i;
		int j = all_pos_bombas[k].j;
		int pa = all_pos_bombas[k].pa;
		int temp;
		if(i < 0 || i >= 11) continue;
		if(j < 0 || j >= 13) continue;
		if(tabPeso[i][j] == 999) continue;
		if(tabPeso[i][j] == 3) continue;
		//if(pf2[i][j].cnt == 4) continue;
		if(pf3[i][j].peso != 0) continue;
		if(strcmp(tab2[i][j].str1, enemyS) == 0 || strcmp(tab2[i][j].str1, s) == 0) temp = 1;
		else if(strcmp(tab2[i][j].str2, range_symbol) == 0) temp = 100;
		else temp = tabPeso[i][j];
		/*if(temp+pa < pf2[i][j].peso)*/ pf3[i][j].peso = temp+pa;
		//pf2[i][j].cnt++;
		for(w = 1; w < 5; w++){
			pospf novo;
			novo.i = i+dx[w];
			novo.j = j+dy[w];
			novo.pa = pf3[i][j].peso;
			all_pos_bombas[num_pos++] = novo;
		}
	}
	/*if(checkPos(x, y) == 0) return;
	else if(pf2[x][y] != 0) return;
	int temp;
	if(strcmp(tab2[x][y].str1, enemyS) == 0 || strcmp(tab2[x][y].str1, s) == 0) temp = 1;
	else temp = tabPeso[x][y];
	pf2[x][y] = pesoAnt+temp;
	pathfinding2(x, y+1, pf2[x][y]);
	pathfinding2(x+1, y, pf2[x][y]);
	pathfinding2(x, y-1, pf2[x][y]);
	pathfinding2(x-1, y, pf2[x][y]);*/
}

int pf_andar(int x, int y){
	int k, i, j, dir, temp = 999;
	for(k = 1; k < 5; k++){
		i = x+dx[k];
		j = y+dy[k];
		if(checkPos(i, j)){
			if(pf2[i][j].peso == 0) continue;
			if(pf2[i][j].peso < temp){
				temp = pf2[i][j].peso;
				dir = k;
			}
		}
	}
	return dir;
}

int pf_fugir_bomba(int x, int y){
	int k, i, j, dir, temp = 999;
	for(k = 1; k < 5; k++){
		i = x+dx[k];
		j = y+dy[k];
		if(checkPos(i, j)){
			if(pf3[i][j].peso == 0) continue;
			if(pf3[i][j].peso < temp){
				temp = pf3[i][j].peso;
				dir = k;
			}
		}
	}
	return dir;
}

void debug(){
	fp = fopen("debug.txt","a+");
	int i, j;
	fprintf(fp, "PF2\n");
	for(i = 0; i < 11; i++){
		for(j = 0; j < 13; j++){
			fprintf(fp, "%d ", pf2[i][j].peso);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "PF3\n");
	for(i = 0; i < 11; i++){
		for(j = 0; j < 13; j++){
			fprintf(fp, "%d ", pf3[i][j].peso);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n\n");
	fclose(fp);
}

int main(int argc, char *argv[])//a assinatura da funcao principal deve ser dessa forma
{
	int i;
	int where;

	//convercao dos identificadores
    id = atoi(argv[1]);	//identificador do Jogador
    ident = atoi(argv[2]); //identificador da partida

    pos cur;
    pos enemyPos;
    srand(time(NULL));

    leitura();
    lerMatinhos();
    	

    if(rodada != 1){
    	lerMapa2();
    }else{
    	criarMapa2();
    }
    	

    	verificarMapa2();
    
    if(id==1){
    	strcpy(s,"P1");
    	strcpy(enemyS,"P2");
    }
    else{
    	strcpy(enemyS,"P1");
    	strcpy(s,"P2");

   	}
    	//pra saber qual a bomba do cara
    if(strcmp(enemyS,"P1") == 0){
    	strcpy(enemyBomb,"B1");
    	strcpy(bomb, "B2");
    	strcpy(range_symbol, "F2");
    	strcpy(e_range_symbol, "F1");
    }else{
    	strcpy(bomb, "B1");
    	strcpy(enemyBomb,"B2");
    	strcpy(range_symbol, "F1");
    	strcpy(e_range_symbol, "F2");
   	}

   	if(!theyAreTogether()){
    	enemyPos = cur_pos(enemyS);
    	lerPosicao();

		posIam = cur_pos(s);
		cur = cur_pos(s); // o parametro a ser passado depende se o jogador atual e 1 o 2
	}else{
		posIam = cur_pos("P3");
		cur.i = posIam.i;
		cur.j = posIam.j;
		enemyPos.i = posIam.i;
		enemyPos.j = posIam.j;
 	}

 	
	lerBombas(); //Antes de inicializar qualquer decisao
	lerBonus();

	posMatinhos();
	adicionar_bonus();

	criarMapaPeso();
	escreverMapaPeso(); // debug mapa pesso



	jogarbomba = soltarbomba(cur.i,cur.j,enemyPos.i,enemyPos.j);
	int w = 0;
	for(;w <qtd_bombas;w++){
		bombaColocouMapa2(bombas[w].i,bombas[w].j);
	}

	checkSafety(cur.i,cur.j);
	//ver as bombas do inimigo
	verificar_bombas();
	bombas_inimigo(enemyPos.i,enemyPos.j);
	
	int l, m;

	for(l = 0; l < 11; l++){
		for(m = 0; m < 13; m++){
			pf2[l][m].peso = 0;
			pf3[l][m].peso = 0;
		}
	}

	menorCaminho2 = 2147483647;
	//menorCaminho2 = 0;

	//ou seja, se tiver com uma bomba nossa, ele vai se afastar dela
	if(qtd_bombas > 0){
		pathfinding_bomb_escape(cur.i, cur.j, 0, 1);
		//where = pf_fugir_bomba(cur.i, cur.j);
		where = dir_fixo2;
	}else{
		pathfinding2(enemyPos.i, enemyPos.j, 0);
		where = pf_andar(cur.i, cur.j);
	}
	debug();

	escreverPosicao(cur.i,cur.j);


	willexplode = explodirbomba(cur.i,cur.j,where);
	
	rodada++;
	verificarBonus();
	escreverBombas(); //Apos finalizar as decisoes
	escreverBonus();
	escreverMatinhos();

	escreverValores();

	escreverMapa2();
	
	

	//impressao da saida
	printf("%d %d %d %d \n",ident,jogarbomba, where,willexplode);


	return 0;
}

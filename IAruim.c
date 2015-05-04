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
int explodirbomba; //Indica se ira jogar bomba nesta posicao ou nao;

//no maximmo pode ter 5 bombas(TMB)
info_bomba bombas[5];// guarda as posicoes da bomba;
int qtd_bombas = 0;
int range = 2; //Alcance das bombas;

info_bomba bombas_enemy[5];
int qtd_bombas_enemy = 0;
int range_enemy = 2;

char enemyBomb[3];

pos bonus_range[100];
int qtdBonusRange = 0;
pos bonus_bombas[100];
int qtdBonusBomba = 0;

//vetores de deslocamento. {parado, sobe, esquerda, desce, direita}
// int dx[] = {0,-1,0,1,0};
// int dy[] = {0,0,-1,0,1};


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


/*
Returns the distance between two objects;
*/
double distance(int x1,int x2,int y1,int y2){
	double dist = sqrt((x1-x2)^2 + (y1- y2)^2);
	return dist;
}

//funcao que checa se o movimento e valido em determinada posicao
int check(int x, int y)
{
	if(x>=0 && x<11 && y>=0 && y<13 && (strcmp(tab[x][y].str2,"--")==0  || strcmp(tab[x][y].str2,"+F")==0 || strcmp(tab[x][y].str2,"+B")==0))
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
	fprintf(fp, "%d %d\n",qtd_bombas,range); //tem que guardar o range atual
	for(; i  < qtd_bombas; i++){
		fprintf(fp,"%d %d %d ", bombas[i].range, bombas[i].i,bombas[i].j);
	}
	//bombas do inimigo
	fprintf(fp, "\n%d %d\n", qtd_bombas_enemy, range_enemy);
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
		fscanf(fp," %d %d", &qtd_bombas, &range);
		for(; i  < qtd_bombas; i++){
			fscanf(fp,"%d %d %d ", &bombas[i].range, &bombas[i].i, &bombas[i].j);
		}
		//bombas do inimigo
		fscanf(fp, " %d %d", &qtd_bombas_enemy, &range_enemy);
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

	//funcao se determina se devo explodir uma bomba ou nao
int goexplodirbomba(int x, int y, int where,int enemyX,int enemyY){
	int retorno = 0;
	if(qtd_bombas > 0){


			switch(where){ //Because what it matters is my next move, not the position I'm at.
			case 1:
			if(bombas[0].i != x-1 && bombas[0].j != y ){
				qtd_bombas--;
				modifybombs();
				retorno =  1;
			}

			if(bombas[0].i == y && distance(bombas[0].i,x-1,bombas[0].j,y) > 3){
				qtd_bombas--;
				modifybombs();
				retorno =  1;
			}


			break;
			case 2:

			if(bombas[0].i != x && bombas[0].j != y-1){
				qtd_bombas--;
				modifybombs();
				retorno =  1;
			}

			if(bombas[0].i == x && distance(bombas[0].i,x,bombas[0].j,y-1) > 3){
				qtd_bombas--;
				modifybombs();
				retorno =  1;
			}


			break;
			case 3:
			if(bombas[0].i != x+1 && bombas[0].j != y){
				qtd_bombas--;
				modifybombs();
				retorno =  1;
			}

			if(bombas[0].j == y && distance(bombas[0].i,x+1,bombas[0].j,y) > 3){
				qtd_bombas--;
				modifybombs();
				retorno =  1;
			}
			break;
			case 4:
			if(bombas[0].i != x && bombas[0].j != y+1){
				qtd_bombas--;
				modifybombs();
				retorno =  1;
			}

			if(bombas[0].i == x && distance(bombas[0].i,x-1,bombas[0].j,y+1) > 3){
				qtd_bombas--;
				modifybombs();
				retorno =  1;
			}

			break;
		}
	}
	return retorno;
		//Checar se o usuario esta distante da ultima bomba ou nao;
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
	Funcao feita para que ele nao pense em retornar a uma posicao antiga(desnecessario)
	*/
void avoidOldPosition(int x, int y,int jogarbomba){
	if(jogarbomba == 0){ //Sem soltar bomba
		if(oldPos.i != x-1 && check(x-1,y)){
			move[1] += 1;
		}
		if(oldPos.j != y-1 && check(x,y-1)){
			move[2] += 1;
		}

		if(oldPos.i != x+1 && check(x+1,y)){
			move[3] += 1;
		}

		if(oldPos.j != y+1 && check(x,y+1)){
			move[4] += 1;
		}
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

/*
	Metodo responsavel pela medida de distancia;
*/
void getcloser(int enemyX,int enemyY, int playerX, int playerY){

	if(qtd_bombas == 0 && jogarbomba == 0){
		if(check(playerX-1,playerY)){ //sobe
				if(playerX > enemyX){
					move[1] += 1; //It holds more weight
				}
		}

		if(check(playerX,playerY-1)){ //esquerda
				if(playerY > enemyY ){
					move[2] += 1; //It holds  more weight
				}
		}


		if(check(playerX+1,playerY)){ //desce
				if(playerX < enemyX ){ // This bombas[0].i /or j != playerX or Y is because it's starting a loop
					move[3] += 1;
				}
		}

		if(check(playerX,playerY+1)){ //direita
				if(playerY < enemyY  && bombas[0].j != playerY){
					move[4] += 1;
				}
		}
	}
}

//It returns if in that direction there is a path with the minimum size required.
/*
Parameters(x,y) : Position of our player (x,y)
Size: The size of the path we want
sizeFound: The size of the path
Paramerters(oldx,oldy) : To avoid going back to the previous tile or position (avoiding a huge loop), it's default is the inital position x and y
*/
int findSize(int x, int y, int size,int sizeFound,int oldx,int oldy){
	int ways = 0;

	if(size == sizeFound){
		return sizeFound;
	}

	if(check(x-1,y) && posIam.i != x-1 && (sizeFound < size) && oldx != x-1){
		ways++;
		sizeFound += findSize(x-1,y,size,sizeFound+1,x,y);
	}

	if(check(x,y-1) && posIam.j != y-1 && (sizeFound < size) && oldy != y-1){
		sizeFound += findSize(x,y-1,size,sizeFound+1,x,y);
		
	}

	if(check(x+1,y) && posIam.i != x+1 && (sizeFound < size) && oldx != x+1){
		ways++;
		sizeFound += findSize(x+1,y,size,sizeFound+1,x,y);
	}

	if(check(x,y+1) && posIam.j != y+1 && (sizeFound < size) && oldy != y-1){
		ways++;
		sizeFound += findSize(x,y+1,size,sizeFound+1,x,y);
	}


	if(ways == 0 && sizeFound < size){
		return 0;
	}

	if(sizeFound >= size){
		return 1;
	}else{
		return 0;
	}
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
		if(strcmp(tab[bonus_range[i].i][bonus_range[i].j].str2, "+F") != 0) 
		{
			if(i == qtdBonusRange-1) qtdBonusRange--;
			else 
			{
				bonus_range[i] == bonus_range[qtdBonusRange-1];
				qtdBonusRange--;
			} 
		}
	}
	// Bonus Bomba
	for(i = 0; i < qtdBonusBomba; i++)
	{
		if(strcmp(tab[bonus_bombas[i].i][bonus_bombas[i].j].str2, "+F") != 0) 
		{
			if(i == qtdBonusBomba-1) qtdBonusBomba--;
			else 
			{
				bonus_bombas[i] == bonus_bombas[qtdBonusBomba-1];
				qtdBonusBomba--;
			} 
		}
	}
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


    	char s[3];
    	char enemyS[3];
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
    	}else{
    		strcpy(enemyBomb,"B2");
    	}

    	enemyPos = cur_pos(enemyS);
    	lerPosicao();
	posIam = cur_pos(s);
	cur = cur_pos(s); // o parametro a ser passado depende se o jogador atual e 1 o 2
	lerBombas(); //Antes de inicializar qualquer decisao

	jogarbomba = soltarbomba(cur.i,cur.j,enemyPos.i,enemyPos.j);
	if(jogarbomba){
		if(check(cur.i-1,cur.j)){
			if(findSize(cur.i-1,cur.j,5,0,cur.i,cur.j) == 0 ){
				move[1] -= 10; 
			}
		}
		if(check(cur.i,cur.j-1)){
			if(findSize(cur.i,cur.j-1,5,0,cur.i,cur.j) == 0){
				move[2] -= 10;
			}
		}

		if(check(cur.i+1,cur.j)){
			if(findSize(cur.i+1,cur.j,5,0,cur.i,cur.j) == 0){
				move[3] -= 10; 
			}
		}

		if(check(cur.i,cur.j+1)){
			if(findSize(cur.i,cur.j+1,5,0,cur.i,cur.j) == 0){
				move[4] -= 10; 
			}
		}
	}
	//ver as bombas do inimigo
	verificar_bombas();
	bombas_inimigo(enemyPos.i,enemyPos.j);

	bealive(cur.i,cur.j,jogarbomba);
	checkWays(cur.i,cur.j);
	getcloser(enemyPos.i,enemyPos.j,cur.i,cur.j);

	avoidOldPosition(cur.i,cur.j,jogarbomba); //Solucao Temporatoria 
	where = MAIOR(move);
	escreverPosicao(cur.i,cur.j);
	explodirbomba = goexplodirbomba(cur.i,cur.j,where,enemyPos.i,enemyPos.j);
	escreverBombas(); //Apos finalizar as decisoes

	
	escreverValores();
	//impressao da saida
	printf("%d %d %d %d \n",ident,jogarbomba, where,explodirbomba);


	return 0;
}

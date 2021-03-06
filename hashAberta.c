//
//  hashFechada.c
//  LibLista
//
//  Created by Mikael Botassi de Oliveira on 07/10/21.
//

#include "hashAberta.h"
#include <stdio.h>
#include <stdlib.h>

hashAberto *expandeHash(hashAberto *hash, int (*pegaChave)(void *, char));

void inicializaHashAberto(hashAberto *hash, int tamanhoInicial, float fatorCarga)
{
	hash->quant = 0;
	hash->tamanho = tamanhoInicial;
	hash->fatorCarga = fatorCarga;
	hash->tabela = (elemento *)malloc(tamanhoInicial * sizeof(elemento));

	int i;
	for (i = 0; i < tamanhoInicial; i++)
	{
		hash->tabela[i].situacao = 0;
		hash->tabela[i].tipo = '\0';
		hash->tabela[i].valor = NULL;
	}
}

int hashCode(int matricula, int tam)
{
	return matricula % tam;
}

hashAberto *inserirNaHashAberta(char chave, hashAberto *hash, void *a, int (*pegaChave)(void *, char))
{
	//verifico se é necessário expandir
	if (((float)hash->quant / (float)hash->tamanho) > hash->fatorCarga)
	{
		hash = expandeHash(hash, pegaChave);
	}
	int code = hashCode(pegaChave(a, chave), hash->tamanho); //Pega a chave com a função na biblioteca aluno
	//Achar primeira posiço não ocupada a partir do codigo calculado (note a circularidade)
	while (hash->tabela[code].situacao == 1)
		code = (code + 1) % hash->tamanho;
	hash->tabela[code].valor = a;
	hash->tabela[code].situacao = 1;
	hash->tabela[code].tipo = chave;
	hash->quant++;
	return hash;
}

hashAberto *expandeHash(hashAberto *hash, int (*pegaChave)(void *, char))
{
	int i;
	//Aloco uma nova hash com o dobro do tamanho da orginal
	hashAberto *novaHash;
	novaHash = (hashAberto *)malloc(sizeof(hashAberto));
	inicializaHashAberto(novaHash, hash->tamanho + 200000, hash->fatorCarga);

	//Transfiro os elementos da hash antiga para a nova
	for (i = 0; i < hash->tamanho; i++)
	{
		if (hash->tabela[i].situacao == 1)
		{
			inserirNaHashAberta(hash->tabela[i].tipo, novaHash, hash->tabela[i].valor, pegaChave);
		}
	}
	//desaloco a tabela da hash antiga e depois a hash em si.
	free(hash->tabela);
	free(hash);
	return novaHash;
}

void imprimeHash(hashAberto *hash, void (*print)(elemento *, int))
{
	int i;
	for (i = 0; i < hash->tamanho; i++)
	{
		if (hash->tabela[i].situacao == 1)
		{
			print(&(hash->tabela[i]), i);
		}
	}
}

elemento *pesquisaNaHash(hashAberto *h, int chave, int (*cmp)(int, elemento *))
{
	int code = hashCode(chave, h->tamanho);
	int fim = code;

	do
	{
		if (&(h->tabela[code]) == NULL)
		{
			return NULL;
		}
		else
		{
			if (h->tabela[code].situacao == 1)
			{
				if (cmp(chave, &(h->tabela[code])))
				{
					printf("\nIndice: %d", code);
					return &(h->tabela[code]);
				}
				else
				{
					code = (code + 1) % h->tamanho;
				}
			}
			else
			{
				if (h->tabela[code].situacao == -1)
				{
					code = (code + 1) % h->tamanho;
				}
				else
				{
					return NULL;
				}
			}
		}
	} while (code != fim);
	return NULL;
}

void freeHash(hashAberto *h){
	int i;
	for(i = 0; i < h->tamanho; i++){
		if(h->tabela[i].situacao == 1){
			free(h->tabela[i].valor);
		}
	}
	free(h->tabela);
	h->tabela = NULL;
	free(h);
	h  = NULL;
}

//###################### FUNÇÕES DE ESTATATÍSTICAS ##########################################

bloco insereNoBloco(int quant, int inicio, int fim)
{
	bloco b;
	b.indiceInicial = inicio;
	b.indiceFinal = fim;
	b.quant = quant;

	return b;
}

void todasEstatisticas(hashAberto *h)
{ //Printa as estatisticas
	totalElementos(h);
	printf("\nA media de elementos em cada indice eh: %.1f", pegaMediaElementosBloco(h));
	descobreBlocoMaiorMenor(h);
}

void totalElementos(hashAberto *h)
{ //quantidade total de elementos
	printf("\nO total de elementos da Hash eh: %d", h->quant);
}

float pegaMediaElementosBloco(hashAberto *h)
{					 //quantidade média de elementos por índice
	int i;			 //Contador do primeiro for que percorre toda Hash
	int totalNo = 0; //guarda o total de nós que tem situação igual a 1(possui algum aluno) ou -1(Possuia aluno, porém foi retirado)
	int blocos = 0;	 //guarda a quantidade de blocos totais na hash
	float media;	 //guarda a media de elementos por bloco
	for (i = 0; i < h->tamanho; i++)
	{
		if (h->tabela[i].situacao == 1 || h->tabela[i].situacao == -1)
		{ //Se ainda estiver dentro de um bloco
			totalNo++;
		}
		else
		{			  //Se for uma posicão de situação 0, ou seja não tem blocos
			blocos++; //Fecha o bloco anterior e soma mais um no número total de blocos
			while (h->tabela[i].situacao == 0 && i < h->tamanho)
			{ //Vai descartar os indices com situação igual 0;
				i++;
			}
		}
	}
	media = ((float)h->quant) / blocos;
	return media;
}

void descobreBlocoMaiorMenor(hashAberto *h)
{
	bloco b;
	bloco maior = insereNoBloco(-1, 0, 0);
	bloco menor = insereNoBloco(99999, 0, 0);

	int i = 0; //Contador do primeiro for que percorre toda Hash
	int reseta = 0;

	for (i = 0; i < h->tamanho; i++)
	{
		if (&(h->tabela[i]) == NULL)
		{
			break;
		}
		else
		{

			if (h->tabela[i].situacao == 1 || h->tabela[i].situacao == -1)
			{ //Se ainda estiver dentro de um bloco
				if (reseta == 0)
				{
					b = insereNoBloco(0, 0, 0); //Se for igual a zero quer dizer que vai começar a contar o bloco novo agora, então reseta tudo
					b.indiceInicial = i;
					reseta = 1;
				}

				b.quant++;
			}
			else
			{ //Se for uma posicão de situação 0, ou seja não tem blocos
				reseta = 0;
				b.indiceFinal = i - 1; //Este indice final é só por curiosidade
				if (b.quant > maior.quant)
				{	//Se a quantidade de elementos no bloco atual for maior que o bloco com maior número de elementos
					maior = b; //Vai inserir os valores do bloco no bloco maior
				}
				if (b.quant < menor.quant && b.quant>0)
				{ //Se for menor
					menor = b;
				}
				while (h->tabela[i + 1].situacao == 0 && i < h->tamanho)
				{//Vai descartar os indices com situação igual 0;
					i++; //Percorre o "i" até sair do 0;
				}
			}
		}
	}
	printf("\nO indice com o maior numero de elementos tem o indice inicial de: %d, e ele possui %d elementos", maior.indiceInicial, maior.quant);
	printf("\nO indice com o menor numero de elementos eh: %d, e ele possui %d elementos", menor.indiceInicial, menor.quant);
}
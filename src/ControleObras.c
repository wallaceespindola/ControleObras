#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NOME 	 	150
#define MAX_ENG_RESP  	20
#define MAX_ENDERECO 	50
#define MAX_PREV_ENTREG 12
#define MAX_TAM_LINHA	250
#define MAX_LINHAS		1000

typedef struct registro_obra {
	int codigo;
	char nome[MAX_NOME];
	char engResponsavel[MAX_ENG_RESP];
	char endereco[MAX_ENDERECO];
	float valor;
	char prevEntrega[MAX_PREV_ENTREG];
} registro_obra;

/**
 * VARIAVEIS GLOBAIS
 */
FILE *arquivo;
FILE *arquivoTemp;
// Pode usar caminho completo aqui. (Ex: C:/xxxx/yyyy.txt)
char caminhoArquivo[] = "controle_obras.txt";
char caminhoArquivoTemp[] = "controle_obras_temp.txt";

// Abrir arquivo - leitura
void abrirArquivoLeitura(FILE **pArquivo, char *caminhoArquivo) {
	if (!(*pArquivo = fopen(caminhoArquivo, "r"))) {
		printf(" ##### Erro ao abrir arquivo!!! #####\n");
	} else {
		printf("\n Aberto arquivo modo leitura. \n");
	}
}

// Abrir arquivo - escrita
void abrirArquivoEscrita(FILE **pArquivo, char *caminhoArquivo) {
	if (!(*pArquivo = fopen(caminhoArquivo, "a"))) {
		printf(" ##### Erro ao abrir arquivo!!! #####\n");
	} else {
		printf("\n Aberto arquivo modo escrita. \n");
	}
}

// Escrever no arquivo
void gravarArquivo(FILE **pArquivo, char *dados) {

	fprintf(*pArquivo, "%s", dados);

	printf("\n Gravacao em arquivo realizada. \n");
}

// Ler do arquivo
int lerArquivo(FILE **pArquivo, char stringSaida[MAX_LINHAS][MAX_TAM_LINHA]) {

	printf("\n Lendo arquivo. \n");

	int contRegistros = 0;
	while (!feof(*pArquivo)) {
		fscanf(*pArquivo, "%s", stringSaida[contRegistros]);
		contRegistros++;
	}

	printf("\n Leitura de arquivo realizada - %d obras. \n", contRegistros);

	return contRegistros;
}

// Fechar arquivo
void fecharArquivo(FILE **pArquivo) {
	fclose(*pArquivo);
	printf("\n Arquivo fechado. \n");
}

void carregarRegistros(FILE **pArquivo, registro_obra *obras, int qtdObras) {

	printf("\n Carregando registros. \n");

	for (int i = 0; i < qtdObras; i++) {

		fscanf(*pArquivo, "%d;%s;%s;%s;%f;%s;", &obras[i].codigo, obras[i].nome,
				obras[i].engResponsavel, obras[i].endereco, &obras[i].valor, obras[i].prevEntrega);

		printf("\n Para i: %d - codigo da obra lido: %d \n", i, obras[i].codigo);
	}
}

registro_obra buscarRegistro(int codigoBuscado) {

	char stringSaida[MAX_LINHAS][MAX_TAM_LINHA];
	int qtdObras = lerArquivo(&arquivo, stringSaida);
	registro_obra *obras = malloc(sizeof(registro_obra) * qtdObras);

	carregarRegistros(&arquivo, obras, qtdObras);

	int achou = 0;
	int indice = 0;
	for (int i = 0; i < qtdObras; i++) {

		if (obras[i].codigo == codigoBuscado) {
			achou = 1; // verdadeiro
			indice = i; // indice (posicao) do item buscado
			break; // interrompe a busca
		}
	}

	if (achou == 0) { // falso
		printf("\n\n Obra de codigo %d nao encontrada. \n\n", codigoBuscado);
	} else {
		printf("\n\n Obra de codigo %d encontrada. \n\n", codigoBuscado);
	}

	return obras[indice];
}

// Montar linha arquivo
void montarLinhaArquivo(char *stringEntrada, registro_obra obra) {

	int len;
	char aux[20];
	stringEntrada[0] = '\0';

	sprintf(aux, "%d;", obra.codigo);
	strcat(stringEntrada, aux);

	strcat(stringEntrada, obra.nome);
	len = strlen(stringEntrada);
	stringEntrada[len - 1] = ';';
	stringEntrada[len] = '\0';

	strcat(stringEntrada, obra.engResponsavel);
	len = strlen(stringEntrada);
	stringEntrada[len - 1] = ';';
	stringEntrada[len] = '\0';

	strcat(stringEntrada, obra.endereco);
	len = strlen(stringEntrada);
	stringEntrada[len - 1] = ';';
	stringEntrada[len] = '\0';

	sprintf(aux, "%.2f;", obra.valor);
	strcat(stringEntrada, aux);

	strcat(stringEntrada, obra.prevEntrega);
	len = strlen(stringEntrada);
	stringEntrada[len - 1] = ';';
	stringEntrada[len] = '\0';

	strcat(stringEntrada, "\n");

	printf("\n Linha de arquivo montada: %s \n", stringEntrada); // mostra a linha
}

int excluirRegistro(int codigo) {

	abrirArquivoLeitura(&arquivo, caminhoArquivo); // Arquivo original
	abrirArquivoEscrita(&arquivoTemp, caminhoArquivoTemp); // Novo arquivo

	char stringSaida[MAX_LINHAS][MAX_TAM_LINHA];
	int qtdObras = lerArquivo(&arquivo, stringSaida);
	registro_obra *obras = malloc(sizeof(registro_obra) * qtdObras);

	carregarRegistros(&arquivo, obras, qtdObras);

	registro_obra obra = buscarRegistro(codigo);

	printf("\n ============= Dados da obra ============= \n\n");

	printf(" Codigo da obra: %d\n", obra.codigo);
	printf(" Nome da obra: %s\n", obra.nome);

	printf("\n ============ Fim da consulta ============ \n");

	printf("\n Excluindo... \n");

	for (int i = 0; i < qtdObras; i++) {

		// Adicionar na lista somente se diferente aqui, pois se igual ao codigo queremos excluir da lista
		if (obras[i].codigo != codigo) {
			char stringEntrada[MAX_TAM_LINHA];
			montarLinhaArquivo(stringEntrada, obras[i]);
			gravarArquivo(&arquivoTemp, stringEntrada);
		}
	}

	// Fecha ambos os arquivos
	fecharArquivo(&arquivo);
	fecharArquivo(&arquivoTemp);

	remove(caminhoArquivo); // Remove original
	rename(caminhoArquivoTemp, caminhoArquivo); // Renomeia novo para nome original

	return 0;
}

int alterarRegistro(int codigo, int opcao) {

	abrirArquivoLeitura(&arquivo, caminhoArquivo); // Arquivo original
	abrirArquivoEscrita(&arquivoTemp, caminhoArquivoTemp); // Novo arquivo

	char stringSaida[MAX_LINHAS][MAX_TAM_LINHA];
	int qtdObras = lerArquivo(&arquivo, stringSaida);
	registro_obra *obras = malloc(sizeof(registro_obra) * qtdObras);

	carregarRegistros(&arquivo, obras, qtdObras);

	registro_obra obra = buscarRegistro(codigo);

	printf("\n ============= Dados da obra ============= \n\n");

	printf(" Codigo da obra: %d\n", obra.codigo);
	printf(" Nome da obra: %s\n", obra.nome);

	printf("\n ============ Fim da consulta ============ \n");

	printf("\n Alterando... \n");

	for (int i = 0; i < (qtdObras - 1); i++) {

		// Alterar somente o que tem codigo igual aqui, pois se diferente simplesmente copia para a lista
		if (obras[i].codigo == codigo) {

			printf("\n Por favor informe: \n");
			switch (opcao) {
				case 1: {
					printf(" Nome da obra (max 150 caract.): ");
					fflush(stdin);
					fgets(obras[i].nome, MAX_NOME, stdin);
					break;
				}
				case 2: {
					printf(" Engenheiro responsavel (max 20 caract.): ");
					fflush(stdin);
					fgets(obras[i].engResponsavel, MAX_ENG_RESP, stdin);
					break;
				}
				case 3: {
					printf(" Endereco (max 50 caract.): ");
					fflush(stdin);
					fgets(obras[i].endereco, MAX_ENDERECO, stdin);
					break;
				}
				case 4: {
					printf(" Valor (numerico 999999.99): ");
					scanf("%f", &obras[i].valor);
					break;
				}

				case 5: {
					printf(" Previsao de entrega (dd/mm/aaaa): ");
					fflush(stdin);
					fgets(obras[i].prevEntrega, MAX_PREV_ENTREG, stdin);
					break;
				}
				default: {
					// Nao faz nada, essa opcao nao existe
					break;
				}
			}
			char stringEntrada[MAX_TAM_LINHA];
			montarLinhaArquivo(stringEntrada, obras[i]);
			gravarArquivo(&arquivoTemp, stringEntrada);

			printf("\n ========== Confimando dados atualizados ========== \n\n");

			printf(" Codigo da obra: %d\n", obras[i].codigo);
			printf(" Nome da obra: %s", obras[i].nome);
			printf(" Engenheiro responsavel: %s", obras[i].engResponsavel);
			printf(" Endereco: %s", obras[i].endereco);
			printf(" Valor: %.2f\n", obras[i].valor);
			printf(" Previsao de entrega: %s", obras[i].prevEntrega);

			printf("\n ================ Entradas lidas ================ \n");

			break; // Interrompe o for, já atualizou o que queria
		}
	}

	// Fecha ambos os arquivos
	fecharArquivo(&arquivo);
	fecharArquivo(&arquivoTemp);

	remove(caminhoArquivo); // Remove original
	rename(caminhoArquivoTemp, caminhoArquivo); // Renomeia novo para nome original

	return 0;
}

void menuAlteracao(int codigo) {

	int opcao;

	do {
		printf("\n ############ ALTERAR DADOS DA OBRA ############ ");
		printf("\n 1 - Alterar nome ");
		printf("\n 2 - Alterar engenheiro responsavel ");
		printf("\n 3 - Alterar endereco ");
		printf("\n 4 - Alterar valor ");
		printf("\n 5 - Alterar previsao de entrega ");
		printf("\n ############################################### ");
		printf("\n\n ==> Escolha a opcao (sair = 6): ");
		scanf("%d", &opcao);

		// estrutura switch-case
		switch (opcao) {
			case 1: {
				printf("\n Opcao: 1 - Alterar nome \n");
				alterarRegistro(codigo, opcao);
				break;
			}
			case 2: {
				printf("\n Opcao: 2 - Alterar engenheiro responsavel \n");
				alterarRegistro(codigo, opcao);
				break;
			}
			case 3: {
				printf("\n Opcao: 3 - Alterar endereco \n");
				alterarRegistro(codigo, opcao);
				break;
			}
			case 4: {
				printf("\n Opcao: 4 - Alterar valor \n");
				alterarRegistro(codigo, opcao);
				break;
			}
			case 5: {
				printf("\n Opcao: 5 - Alterar previsao de entrega \n");
				alterarRegistro(codigo, opcao);
				break;
			}
			default: {
				if (opcao == 6) {
					break;
				}
				// Se o usuario digitar um valor diferente dos do menu, informa que a opcao nao existe
				printf("\n\n Nenhuma opcao valida foi escolhida. \n");
				break;
			}
		}
	} while (opcao != 6);

	if (opcao == 6) {
		printf("\n\n Alteracao encerrada. \n");
	}
}

void exibirRegistro(registro_obra obra) {

	printf("\n ============= Dados da obra ============= \n\n");

	printf(" Codigo da obra: %d\n", obra.codigo);
	printf(" Nome da obra: %s\n", obra.nome);
	printf(" Engenheiro responsavel: %s\n", obra.engResponsavel);
	printf(" Endereco: %s\n", obra.endereco);
	printf(" Valor: %.2f\n", obra.valor);
	printf(" Previsao de entrega: %s\n", obra.prevEntrega);

	printf("\n ============ Fim da consulta ============ \n");
}

void exibirListaRegistros(registro_obra *obras, int qtdObras) {

	for (int i = 0; i < qtdObras; i++) {
		exibirRegistro(obras[i]);
	}
}

// Ler entradas
void lerEntradas(registro_obra *obra) {

	printf(" Codigo da obra (numerico): ");
	scanf("%d", &obra->codigo);

	printf(" Nome da obra (max 150 caract.): ");
	fflush(stdin);
	fgets(obra->nome, MAX_NOME, stdin);

	printf(" Engenheiro responsavel (max 20 caract.): ");
	fflush(stdin);
	fgets(obra->engResponsavel, MAX_ENG_RESP, stdin);

	printf(" Endereco (max 50 caract.): ");
	fflush(stdin);
	fgets(obra->endereco, MAX_ENDERECO, stdin);

	printf(" Valor (numerico 999999.99): ");
	scanf("%f", &obra->valor);

	printf(" Previsao de entrega (dd/mm/aaaa): ");
	fflush(stdin);
	fgets(obra->prevEntrega, MAX_PREV_ENTREG, stdin);

	printf("\n ========== Confimando dados inseridos ========== \n\n");

	printf(" Codigo da obra: %d\n", obra->codigo);
	printf(" Nome da obra: %s", obra->nome);
	printf(" Engenheiro responsavel: %s", obra->engResponsavel);
	printf(" Endereco: %s", obra->endereco);
	printf(" Valor: %.2f\n", obra->valor);
	printf(" Previsao de entrega: %s", obra->prevEntrega);

	printf("\n ================ Entradas lidas ================ \n");
}

void ordenarPorValorInsertionSort(struct registro_obra *V_obras, int N_obras) {
	int i, j;
	struct registro_obra aux;
	for (i = 1; i < N_obras; i++) {
		aux = V_obras[i];
		for (j = i; (j > 0) && (aux.valor < V_obras[j - 1].valor); j--) {
			V_obras[j] = V_obras[j - 1];
		}
		V_obras[j] = aux;
	}
}

void ordenarPorNomeInsertionSort(struct registro_obra *V_obras, int N_obras) {
	int i, j;
	struct registro_obra aux;
	for (i = 1; i < N_obras; i++) {
		aux = V_obras[i];
		for (j = i; (j > 0) && (strcmp(aux.nome, V_obras[j - 1].nome) < 0); j--) {
			V_obras[j] = V_obras[j - 1];
		}
		V_obras[j] = aux;
	}
}

void relatorioObraMaiorValor() {

	printf("\n\n ------ Exibindo relatorio da obra de maior valor ------ \n\n");

	abrirArquivoLeitura(&arquivo, caminhoArquivo);

	char stringSaida[MAX_LINHAS][MAX_TAM_LINHA];
	int qtdObras = lerArquivo(&arquivo, stringSaida);
	registro_obra *obras = malloc(sizeof(registro_obra) * qtdObras);

	carregarRegistros(&arquivo, obras, qtdObras);

	ordenarPorValorInsertionSort(obras, qtdObras);

	// Como foi ordenado por valor, o ultimo item tem o maior valor
	int ultimoItem = qtdObras - 1;
	exibirRegistro(obras[ultimoItem]);

	fecharArquivo(&arquivo);

	printf("\n\n ------------ Fim do relatorio ------------ \n\n");

	printf("\n Pressione qualquer tecla para voltar ao menu... \n");

	getch();
}

void relatorioSomatorioObras() {

	printf("\n\n ------ Exibindo relatorio de somatorio de obras ------ \n\n");

	abrirArquivoLeitura(&arquivo, caminhoArquivo);

	char stringSaida[MAX_LINHAS][MAX_TAM_LINHA];
	int qtdObras = lerArquivo(&arquivo, stringSaida);
	registro_obra *obras = malloc(sizeof(registro_obra) * qtdObras);

	carregarRegistros(&arquivo, obras, qtdObras);

	double somatorio = 0.0;

	for (int i = 0; i < qtdObras; i++) {
		somatorio += obras[i].valor;
	}
	printf("\n Somatorio total das %d obras: R$ %.2lf \n", qtdObras, somatorio);

	fecharArquivo(&arquivo);

	printf("\n\n ------------ Fim do relatorio ------------ \n\n");

	printf("\n Pressione qualquer tecla para voltar ao menu... \n");

	getch();
}

void relatorioOrdemAlfabetica() {

	printf("\n\n ------ Exibindo relatorio por ordem alfabetica ------ \n\n");

	abrirArquivoLeitura(&arquivo, caminhoArquivo);

	char stringSaida[MAX_LINHAS][MAX_TAM_LINHA];
	int qtdObras = lerArquivo(&arquivo, stringSaida);
	registro_obra *obras = malloc(sizeof(registro_obra) * qtdObras);

	carregarRegistros(&arquivo, obras, qtdObras);

	ordenarPorNomeInsertionSort(obras, qtdObras);

	exibirListaRegistros(obras, qtdObras);

	fecharArquivo(&arquivo);

	printf("\n\n ------------ Fim do relatorio ------------ \n\n");

	printf("\n Pressione qualquer tecla para voltar ao menu... \n");

	getch();
}

void excluirObra() {

	printf("\n\n Excluindo obra - informe os dados abaixo: \n\n");

	int codigoBuscado;

	printf(" Codigo da obra (numerico): ");
	scanf("%d", &codigoBuscado);

	printf("\n Buscar codigo lido: %d \n", codigoBuscado);

	excluirRegistro(codigoBuscado);

	printf("\n Pressione qualquer tecla para voltar ao menu... \n");

	getch();
}

void alterarObra() {

	printf("\n\n Alterando obra - informe os dados abaixo: \n\n");

	int codigoBuscado;

	printf(" Codigo da obra (numerico): ");
	scanf("%d", &codigoBuscado);

	printf("\n Buscar codigo lido: %d \n", codigoBuscado);

	menuAlteracao(codigoBuscado);

	printf("\n Pressione qualquer tecla para voltar ao menu principal... \n");

	getch();
}

void consultarObra() {

	printf("\n\n Consultando obra - informe os dados abaixo: \n\n");

	int codigoBuscado;

	printf(" Codigo da obra (numerico): ");
	scanf("%d", &codigoBuscado);

	printf("\n Buscar codigo lido: %d \n", codigoBuscado);

	abrirArquivoLeitura(&arquivo, caminhoArquivo);

	registro_obra obra = buscarRegistro(codigoBuscado);

	exibirRegistro(obra);

	fecharArquivo(&arquivo);

	printf("\n Pressione qualquer tecla para voltar ao menu... \n");

	getch();
}

void adicionarObra() {

	printf("\n\n Cadastrando obra - informe os dados abaixo: \n\n");

	registro_obra obra;
	char stringEntrada[MAX_TAM_LINHA];

	lerEntradas(&obra);

	montarLinhaArquivo(stringEntrada, obra);

	abrirArquivoEscrita(&arquivo, caminhoArquivo);

	gravarArquivo(&arquivo, stringEntrada);

	fecharArquivo(&arquivo);

	printf("\n Pressione qualquer tecla para voltar ao menu... \n");

	getch();
}

void lerTodosRegistros() {

	abrirArquivoLeitura(&arquivo, caminhoArquivo);

	char stringSaida[MAX_LINHAS][MAX_TAM_LINHA];
	int qtdObras = lerArquivo(&arquivo, stringSaida);

	printf("\n\n Registros encontrados: %d\n\n", qtdObras);

	for (int cont = 0; cont < qtdObras; cont++) {
		printf(" Registro %d: %s\n", (cont + 1), stringSaida[cont]);
	}

	fecharArquivo(&arquivo);

	printf("\n Pressione qualquer tecla para voltar ao menu... \n");

	getch();
}

void menuPrincipal() {

	int opcao;

	do {
		printf("\n ############ SISTEMA DE CONTROLE DE OBRAS ############ ");
		printf("\n 1 - Adicionar obra ");
		printf("\n 2 - Consultar obra ");
		printf("\n 3 - Alterar obra ");
		printf("\n 4 - Excluir obra ");
		printf("\n 5 - Ler todos os registros de obras ");
		printf("\n 6 - Relatorio das obras por ordem alfabetica ");
		printf("\n 7 - Relatorio de somatorio das obras ");
		printf("\n 8 - Relatorio dos dados da obra de maior valor ");
		printf("\n ###################################################### ");
		printf("\n\n ==> Escolha a opcao (sair = 9): ");
		scanf("%d", &opcao);

		// estrutura switch-case
		switch (opcao) {
			case 1: {
				printf("\n Opcao: 1 - Adicionar obra \n");
				adicionarObra();
				break;
			}
			case 2: {
				printf("\n Opcao: 2 - Consultar obra \n");
				consultarObra();
				break;
			}
			case 3: {
				printf("\n Opcao: 3 - Alterar obra \n");
				alterarObra();
				break;
			}
			case 4: {
				printf("\n Opcao: 4 - Excluir obra \n");
				excluirObra();
				break;
			}
			case 5: {
				printf("\n Opcao: 5 - Ler todos os registros de obras \n");
				lerTodosRegistros();
				break;
			}
			case 6: {
				printf("\n Opcao: 6 - Relatorio das obras por ordem alfabetica \n");
				relatorioOrdemAlfabetica();
				break;
			}
			case 7: {
				printf("\n Opcao: 7 - Relatorio de somatorio das obras \n");
				relatorioSomatorioObras();
				break;
			}
			case 8: {
				printf("\n Opcao: 8 - Relatorio dos dados da obra de maior valor \n");
				relatorioObraMaiorValor();
				break;
			}
			default: {
				if (opcao == 9) {
					break;
				}
				// Se o usuario digitar um valor diferente dos do menu, informa que a opcao nao existe
				printf("\n\n Nenhuma opcao valida foi escolhida. \n");
				break;
			}
		}
	} while (opcao != 9);

	if (opcao == 9) {
		printf("\n\n Programa encerrado. \n");
	}
}

int main() {

	menuPrincipal();

	getch();

	return 0;
}

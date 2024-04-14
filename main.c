// importando bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32 /*Lib necessária para usar o sleep()*/
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define MAX_LINHA 1000
#define MAX_CAMPO 100
#define BUFFER_SIZE 1024

struct Produto {
    char nome[MAX_CAMPO];
    char categoria[MAX_CAMPO];
    int quantidade;
    float preco;
};

void criarNovoArquivo(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao criar o arquivo %s.\n", nomeArquivo);
        exit(EXIT_FAILURE);
    }

    fclose(arquivo);
}

FILE *abrirArquivo(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r+");

    if (arquivo == NULL) {
        char resposta;
        printf("O arquivo %s não existe. Deseja criar um novo arquivo com esse nome? (S/N): ", nomeArquivo);
        scanf(" %c", &resposta);

        if (resposta == 'S' || resposta == 's') {
            criarNovoArquivo(nomeArquivo);
            arquivo = fopen(nomeArquivo, "r+");
            if (arquivo == NULL) {
                fprintf(stderr, "Erro ao abrir o arquivo %s.\n", nomeArquivo);
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Digite o nome do arquivo novamente: ");
            char novoNomeArquivo[20];
            scanf("%19s", novoNomeArquivo);
            arquivo = abrirArquivo(novoNomeArquivo);
        }
    }

    return arquivo;
}

// Função adicionar produto
void adicionarProduto(FILE *arquivo, const struct Produto *produto) {
    fseek(arquivo, 0, SEEK_END);

    if (ftell(arquivo) == 0 || fgetc(arquivo) == '\n') {
        fprintf(arquivo, "%s,%s,%d,%.2f\n", produto->nome, produto->categoria, produto->quantidade, produto->preco);
    } else {
        fprintf(arquivo, "\n%s,%s,%d,%.2f\n", produto->nome, produto->categoria, produto->quantidade, produto->preco);
    }

    fflush(arquivo);
}

// Função alterar produto 
void alterarProduto(const char *nomeArquivo, const char *nomeAntigo, const struct Produto *novoProduto) {
    FILE *arquivo = fopen(nomeArquivo, "r+");
    
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(EXIT_FAILURE);
    }

    FILE *temp = fopen("temp.csv", "w");

    if (temp == NULL) {
        fprintf(stderr, "Erro ao criar arquivo temporário.\n");
        fclose(arquivo);
        exit(EXIT_FAILURE);
    }

    char linha[MAX_LINHA];
    int encontrou = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char nomeProduto[MAX_CAMPO];

        // Extrai o nome do produto diretamente
        sscanf(linha, "%[^,]", nomeProduto);

        if (strcmp(nomeProduto, nomeAntigo) == 0) {
            // Substitui a linha com o novo produto
            fprintf(temp, "%s,%s,%d,%.2f\n", novoProduto->nome, novoProduto->categoria, novoProduto->quantidade, novoProduto->preco);
            encontrou = 1;
        } else {
            // Copia a linha original
            fprintf(temp, "%s", linha);
        }
    }

    fclose(temp);
    fclose(arquivo);

    // Remove o arquivo original
    remove(nomeArquivo);

    // Renomeia o arquivo temporário para o nome original
    if (rename("temp.csv", nomeArquivo) != 0) {
        fprintf(stderr, "Erro ao renomear o arquivo temporário.\n");
        exit(EXIT_FAILURE);
    }

    // Reabre o arquivo para continuar usando-o
    arquivo = fopen(nomeArquivo, "r+");
    
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao reabrir o arquivo.\n");
        exit(EXIT_FAILURE);
    }

    if (encontrou) {
        printf("Produto alterado com sucesso.\n");
    } else {
        printf("Produto não encontrado para alteração. Nome digitado: %s\n", nomeAntigo);
    }
}

// Função excluir produto
void excluirProduto(const char *nomeArquivo, const char *nome) {
    FILE *arquivo = fopen(nomeArquivo, "r+");

    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", nomeArquivo);
        exit(EXIT_FAILURE);
    }

    FILE *temp = fopen("temp.csv", "w");

    if (temp == NULL) {
        fprintf(stderr, "Erro ao criar arquivo temporário.\n");
        fclose(arquivo);
        exit(EXIT_FAILURE);
    }

    char linha[MAX_LINHA];
    int encontrou = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char nomeProduto[MAX_CAMPO];

        // Extrai o nome do produto diretamente
        sscanf(linha, "%[^,]", nomeProduto);

        if (strcmp(nomeProduto, nome) == 0) {
            encontrou = 1;
        } else {
            // Se o nome não coincide, copia a linha original para o arquivo temporário
            fprintf(temp, "%s", linha);
        }
    }

    fclose(temp);
    fclose(arquivo);

    // Remove o arquivo original
    remove(nomeArquivo);

    // Renomeia o arquivo temporário para o nome original
    if (rename("temp.csv", nomeArquivo) != 0) {
        fprintf(stderr, "Erro ao renomear o arquivo temporário.\n");
        exit(EXIT_FAILURE);
    }

    // Reabre o arquivo para continuar usando-o
    arquivo = fopen(nomeArquivo, "r+");
    
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao reabrir o arquivo.\n");
        exit(EXIT_FAILURE);
    }

    if (encontrou) {
        printf("Produto excluído com sucesso.\n");
    } else {
        printf("Produto não encontrado para exclusão. Nome digitado: %s\n", nome);
    }
}

// Função para consultar arquivos
void consultarArquivo(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
        fprintf(stderr, "O arquivo %s não existe \n", nomeArquivo);
    }

    printf("\n Tabela de produtos: \n");

    char linha[MAX_LINHA];

    int vazio = 1;  // Flag para indicar se o arquivo está vazio

     while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        printf("%s", linha);
        vazio = 0;  // Marca que o arquivo não está vazio
    }

    if (vazio) {
        printf("Não há produtos cadastrados.\n");
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        printf("%s", linha);
    }

    fclose(arquivo);
}

int main() {
    char fileName[20];

    printf("Digite o nome do arquivo: ");
    scanf("%19s", fileName);

    FILE *arquivo = abrirArquivo(fileName);

    printf("\nGerenciador de Produtos (v0.6.2)\n");
    printf("Nome da base de dados: %s\n", fileName);

    int choice;
    do {
        printf("\nSelecione uma opção:\n");
        printf("1. Adicionar Produto\n");
        printf("2. Alterar Produto\n");
        printf("3. Excluir Produto\n");
        printf("4. Consultar Arquivos \n");
        printf("5. Sair\n");

        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                struct Produto novoProduto;

                printf("Digite o nome do produto: ");
                scanf("%s", novoProduto.nome);

                printf("Digite a categoria do produto: ");
                scanf("%s", novoProduto.categoria);

                printf("Digite a quantidade do produto: ");
                scanf("%d", &novoProduto.quantidade);

                printf("Digite o preço do produto: ");
                scanf("%f", &novoProduto.preco);

                adicionarProduto(arquivo, &novoProduto);
                printf("Produto adicionado com sucesso.\n");
                break;
            }
            case 2: {
                char nomeAlterar[MAX_CAMPO];
                printf("Digite o nome do produto a ser alterado: ");
                scanf("%s", nomeAlterar);

                // Cria temporariamente uma nova estrutura para a alteração dos produtos
                struct Produto novoProduto;

                char buffer[BUFFER_SIZE];

                // Entrada do usuário para alterar os dados
                printf("Digite o novo nome do produto: ");
                scanf("%s", novoProduto.nome);
                fgets(buffer, sizeof(buffer), stdin);
                if (buffer[0] != '\n') {
                    sscanf(buffer, "%s", novoProduto.nome);
                } 

                printf("Digite a nova categoria do produto: ");
                scanf("%s", novoProduto.categoria);

                printf("Digite a nova quantidade do produto: ");
                scanf("%d", &novoProduto.quantidade);

                printf("Digite o novo preço do produto: ");
                scanf("%f", &novoProduto.preco);

                // Alterar o produto
                alterarProduto(fileName, nomeAlterar, &novoProduto);
                printf("Produto alterado com sucesso.\n");
                break;
        }
            case 3: {
                char nomeExcluir[MAX_CAMPO];
                printf("Digite o nome do produto a ser excluído: ");
                scanf("%s", nomeExcluir);

                excluirProduto(fileName, nomeExcluir);
                printf("Produto excluído com sucesso.\n");
                break;
            }
            case 4: {
                consultarArquivo(fileName);
                break;
            }
                
            case 5: {
                printf("Saindo do programa...\n");
                break;
            }
            default:
                printf("Opção inválida. Tente novamente.\n");
        }

    } while (choice != 5);

    return 0;
}
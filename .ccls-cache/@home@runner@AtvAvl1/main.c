#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char nome[100];
    char genero[100];
    char local[100];
    char **albums;
    int num_albums;
} Artista;

typedef struct Node {
    Artista artista;
    struct Node *next;
} Node;

Node *criarNo(Artista artista) {
    Node *novo = (Node *)malloc(sizeof(Node));
    if (novo == NULL) {
        printf("Erro: não foi possível alocar memória.\n");
        exit(1);
    }
    novo->artista = artista;
    novo->next = NULL;
    return novo;
}

void inserirArtistaOrdenado(Node **lista, Artista artista) {
    Node *novo = criarNo(artista);

    if (*lista == NULL || strcmp(artista.nome, (*lista)->artista.nome) < 0) {
        novo->next = *lista;
        *lista = novo;
        return;
    }

    Node *atual = *lista;
    Node *anterior = NULL;

    while (atual != NULL && strcmp(artista.nome, atual->artista.nome) > 0) {
        anterior = atual;
        atual = atual->next;
    }

    novo->next = atual;
    anterior->next = novo;
}

void removerArtista(Node **lista, char *nome) {
    Node *atual = *lista;
    Node *anterior = NULL;

    while (atual != NULL && strcmp(nome, atual->artista.nome) != 0) {
        anterior = atual;
        atual = atual->next;
    }

    if (atual != NULL) {
        
        if (anterior == NULL) {
            *lista = atual->next;
        } else {
            anterior->next = atual->next;
        }

        free(atual->artista.albums);
        free(atual);
    }
}

Node *buscarArtistaBinario(Node *lista, char *nome) {
    Node *inicio = lista;
    Node *fim = NULL;

    while (inicio != fim) {
        Node *meio = inicio;
        int count = 0;

        while (meio != fim) {
            meio = meio->next;
            count++;
        }

        meio = lista;
        for (int i = 0; i < count / 2; i++) {
            meio = meio->next;
        }

        int cmp = strcmp(meio->artista.nome, nome);

        if (cmp == 0) {
            return meio;
        } else if (cmp < 0) {
            inicio = meio->next;
        } else {
            fim = meio;
        }
    }

    return NULL; 
}

void buscarAlbum(Node *lista, char *album) {
    Node *atual = lista;

    while (atual != NULL) {
        for (int i = 0; i < atual->artista.num_albums; i++) {
            if (strcmp(atual->artista.albums[i], album) == 0) {
                printf("Artista: %s\n", atual->artista.nome);
                return; 
            }
        }
        atual = atual->next;
    }

    printf("Álbum não encontrado.\n");
}

void liberarLista(Node *lista) {
    while (lista != NULL) {
        Node *temp = lista;
        lista = lista->next;
        free(temp->artista.albums);
        free(temp);
    }
}

int main() {
    FILE *arquivo;
    arquivo = fopen("artistas.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    Node *lista = NULL;
    char linha[100];
    char delim[] = "\n";

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        
        if (strcmp(linha, "==========\n") == 0) {
            Artista artista;
            fgets(artista.nome, sizeof(artista.nome), arquivo);
            fgets(artista.genero, sizeof(artista.genero), arquivo);
            fgets(artista.local, sizeof(artista.local), arquivo);

            char *nl = strchr(artista.local, '\n');
            if (nl != NULL) {
                *nl = '\0';
            }

            artista.num_albums = 0;
            artista.albums = NULL;

            while (fgets(linha, sizeof(linha), arquivo) != NULL) {
                if (strcmp(linha, "==========\n") == 0) {
                    break;
                }
                char **temp = realloc(artista.albums, (artista.num_albums + 1) * sizeof(char *));
                if (temp == NULL) {
                    printf("Erro: não foi possível alocar memória.\n");
                    fclose(arquivo);
                    liberarLista(lista);
                    return 1;
                }
                artista.albums = temp;
                artista.albums[artista.num_albums] = strdup(linha);
                artista.num_albums++;
            }

            inserirArtistaOrdenado(&lista, artista);
        }
    }

    fclose(arquivo);

    int opcao;
    while (1) {
        printf("\nMenu:\n");
        printf("1. Inserir artista\n");
        printf("2. Remover artista\n");
        printf("3. Editar artista\n");
        printf("4. Buscar artista por nome\n");
        printf("5. Buscar álbum por nome\n");
        printf("6. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: {
                printf("Digite o nome do novo artista: ");
                char novo_nome[100];
                scanf(" %[^\n]", novo_nome);

                if (buscarArtistaBinario(lista, novo_nome) != NULL) {
                    printf("O artista já existe na lista.\n");
                    break;
                }

                Artista novo_artista;
                strcpy(novo_artista.nome, novo_nome);
                printf("Digite o gênero musical: ");
                scanf(" %[^\n]", novo_artista.genero);
                printf("Digite o local de origem: ");
                scanf(" %[^\n]", novo_artista.local);
                novo_artista.num_albums = 0;
                novo_artista.albums = NULL;

                printf("Digite os álbuns (digite 'fim' para parar):\n");
                while (1) {
                    char album[100];
                    scanf(" %[^\n]", album);
                    if (strcmp(album, "fim") == 0) {
                        break;
                    }
                    char **temp = realloc(novo_artista.albums, (novo_artista.num_albums + 1) * sizeof(char *));
                    if (temp == NULL) {
                        printf("Erro: não foi possível alocar memória.\n");
                        liberarLista(lista);
                        return 1;
                    }
                    novo_artista.albums = temp;
                    novo_artista.albums[novo_artista.num_albums] = strdup(album);
                    novo_artista.num_albums++;
                }

                inserirArtistaOrdenado(&lista, novo_artista);

                arquivo = fopen("artistas.txt", "a");
                if (arquivo == NULL) {
                    printf("Erro ao abrir o arquivo para escrita.\n");
                    liberarLista(lista);
                    return 1;
                }
                fprintf(arquivo, "==========\n");
                fprintf(arquivo, "%s\n", novo_artista.nome);
                fprintf(arquivo, "%s\n", novo_artista.genero);
                fprintf(arquivo, "%s\n", novo_artista.local);
                for (int i = 0; i < novo_artista.num_albums; i++) {
                    fprintf(arquivo, "%s\n", novo_artista.albums[i]);
                }
                fclose(arquivo);

                printf("Artista inserido com sucesso.\n");
                break;
            }
            case 2: {
                printf("Digite o nome do artista a ser removido: ");
                char nome_remover[100];
                scanf(" %[^\n]", nome_remover);

                removerArtista(&lista, nome_remover);

                arquivo = fopen("artistas.txt", "w");
                if (arquivo == NULL) {
                    printf("Erro ao abrir o arquivo para escrita.\n");
                    liberarLista(lista);
                    return 1;
                }

                Node *atual = lista;
                while (atual != NULL) {
                    fprintf(arquivo, "==========\n");
                    fprintf(arquivo, "%s\n", atual->artista.nome);
                    fprintf(arquivo, "%s\n", atual->artista.genero);
                    fprintf(arquivo, "%s\n", atual->artista.local);
                    for (int i = 0; i < atual->artista.num_albums; i++) {
                        fprintf(arquivo, "%s\n", atual->artista.albums[i]);
                    }
                    atual = atual->next;
                }

                fclose(arquivo);

                printf("Artista removido com sucesso.\n");
                break;
            }
            case 3: {
                printf("Digite o nome do artista a ser editado: ");
                char nome_editar[100];
                scanf(" %[^\n]", nome_editar);
           
                Node *artista_editar = buscarArtistaBinario(lista, nome_editar);
                if (artista_editar == NULL) {
                    printf("Artista não encontrado.\n");
                    break;
                }

                printf("Digite o novo nome do artista: ");
                char novo_nome[100];
                scanf(" %[^\n]", novo_nome);
                
                if (strcmp(novo_nome, artista_editar->artista.nome) != 0 &&
                    buscarArtistaBinario(lista, novo_nome) != NULL) {
                    printf("O novo nome já existe na lista.\n");
                    break;
                }
              
                removerArtista(&lista, artista_editar->artista.nome);

                strcpy(artista_editar->artista.nome, novo_nome);
                printf("Digite o novo gênero musical: ");
                scanf(" %[^\n]", artista_editar->artista.genero);
                printf("Digite o novo local de origem: ");
                scanf(" %[^\n]", artista_editar->artista.local);
                printf("Digite os novos álbuns (digite 'fim' para parar):\n");
                for (int i = 0; i < artista_editar->artista.num_albums; i++) {
                    free(artista_editar->artista.albums[i]);
                }
                free(artista_editar->artista.albums);
                artista_editar->artista.num_albums = 0;
                artista_editar->artista.albums = NULL;
                while (1) {
                    char album[100];
                    scanf(" %[^\n]", album);
                    if (strcmp(album, "fim") == 0) {
                        break;
                    }
                    char **temp = realloc(artista_editar->artista.albums, (artista_editar->artista.num_albums + 1) * sizeof(char *));
                    if (temp == NULL) {
                        printf("Erro: não foi possível alocar memória.\n");
                        liberarLista(lista);
                        return 1;
                    }
                    artista_editar->artista.albums = temp;
                    artista_editar->artista.albums[artista_editar->artista.num_albums] = strdup(album);
                    artista_editar->artista.num_albums++;
                }

                inserirArtistaOrdenado(&lista, artista_editar->artista);

                arquivo = fopen("artistas.txt", "w");
                if (arquivo == NULL) {
                    printf("Erro ao abrir o arquivo para escrita.\n");
                    liberarLista(lista);
                    return 1;
                }

                Node *atual = lista;
                while (atual != NULL) {
                    fprintf(arquivo, "==========\n");
                    fprintf(arquivo, "%s\n", atual->artista.nome);
                    fprintf(arquivo, "%s\n", atual->artista.genero);
                    fprintf(arquivo, "%s\n", atual->artista.local);
                    for (int i = 0; i < atual->artista.num_albums; i++) {
                        fprintf(arquivo, "%s\n", atual->artista.albums[i]);
                    }
                    atual = atual->next;
                }

                fclose(arquivo);

                printf("Artista editado com sucesso.\n");
                break;
            }
            case 4: {
                printf("Digite o nome do artista a ser buscado: ");
                char nome_buscar[100];
                scanf(" %[^\n]", nome_buscar);

                Node *artista_encontrado = buscarArtistaBinario(lista, nome_buscar);
                if (artista_encontrado != NULL) {
                    printf("Artista encontrado:\n");
                    printf("Nome: %s\n", artista_encontrado->artista.nome);
                    printf("Gênero: %s\n", artista_encontrado->artista.genero);
                    printf("Local de origem: %s\n", artista_encontrado->artista.local);
                    printf("Álbuns:\n");
                    for (int i = 0; i < artista_encontrado->artista.num_albums; i++) {
                        printf("%s\n", artista_encontrado->artista.albums[i]);
                    }
                } else {
                    printf("Artista não encontrado.\n");
                }
                break;
            }
            case 5: {
                printf("Digite o nome do álbum a ser buscado: ");
                char nome_album[100];
                scanf(" %[^\n]", nome_album);
                buscarAlbum(lista, nome_album);
                break;
            }
            case 6: {
                liberarLista(lista);
                return 0;
            }
            default:
                printf("Opção inválida.\n");
        }
    }

    return 0;
}
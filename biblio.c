#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// protótipos das funções
// funções de inicialização, arquivos e menu
void inicializar_structs();
void ler_arquivo(char tipo, FILE *arq, void *dados, int *num_elementos);
void atualizar_arquivo(char tipo, FILE *arq, int num_elementos);
void backup();
void menu();
void retornar_menu();

// funções de cada opção
void cadastrar_livro();
void cadastrar_usuario();
void realizar_emprestimo();
void realizar_devolucao();
void pesquisar_livro();
void pesquisar_usuario();
void listar_emprestimos();

// funções avançadas
void relatorio_livros();
void relatorio_usuarios();
void pesquisa_avancada();
void renovar_emprestimo();

// funções adicionais (necessárias para o funcionamento de outras partes do código)
int existe(char tipo, int info);
int cmplwr(const char s1[101], const char s2[101]);
int get_int();
int is_empty(const char str[101]);


// structs
struct livro
{
    int codigo;
    char titulo[101];
    char autor[81];
    char editora[61];
    int ano;
    int exemplares; // número total de exemplares disponíveis
    bool disponivel; // true = disponível, false = indisponível
};

struct usuario
{
    int matricula;
    char nome[101];
    char curso[51];
    char telefone[16];
    struct tm data_cadastro;
};

struct emprestimo
{
    int codigo;
    int matricula_usuario;
    int codigo_livro;
    struct tm data_emprestimo;
    struct tm data_devolucao;
    bool ativo; // true = ativo, false = inativo (já devolvido)
};

// definição de valores máximos e variáveis globais
#define MAX_LIVROS 100
#define MAX_USUARIOS 100
#define MAX_EMPRESTIMOS 100

// vetores para armazenar os dados
struct livro livros[MAX_LIVROS];
struct usuario usuarios[MAX_USUARIOS];
struct emprestimo emprestimos[MAX_EMPRESTIMOS];

// contadores para número de livros, usuários ou empréstimos cadastrados
int num_livros = 0;
int num_usuarios = 0;
int num_emprestimos = 0;


int main(void) {
    // inicializa structs
    inicializar_structs();


    // tenta abrir os arquivos e se não existirem ainda, os cria e abre em r+ em seguida
    FILE *arq_l = fopen("livros.txt", "r+");
    if (arq_l == NULL) {
        arq_l = fopen("livros.txt", "w");
        if (arq_l == NULL) {
            printf("Erro! Não foi possível criar o arquivo livros.txt.\n");
            exit(1);
        }
        fclose(arq_l);
        arq_l = fopen("livros.txt", "r+");
        if (arq_l == NULL) {
            printf("Erro! Arquivo livros.txt não encontrado\n");
            exit(1);
        }
    }

    FILE *arq_u = fopen("usuarios.txt", "r+");
    if (arq_u == NULL) {
        arq_u = fopen("usuarios.txt", "w");
        if (arq_u == NULL) {
            printf("Erro! Não foi possível criar o arquivo usuarios.txt.\n");
            exit(1);
        }
        fclose(arq_u);
        arq_u = fopen("usuarios.txt", "r+");
        if (arq_u == NULL) {
            printf("Erro! Arquivo usuarios.txt não encontrado\n");
            fclose(arq_l);
            exit(1);
        }
    }

    FILE *arq_e = fopen("emprestimos.txt", "r+");
    if (arq_e == NULL) {
        arq_e = fopen("emprestimos.txt", "w");
        if (arq_e == NULL) {
            printf("Erro! Não foi possível criar o arquivo emprestimos.txt.\n");
            exit(1);
        }
        fclose(arq_e);
        arq_e = fopen("emprestimos.txt", "r+");
        if (arq_e == NULL) {
            printf("Erro! Arquivo emprestimos.txt não encontrado\n");
            fclose(arq_l);
            fclose(arq_u);
            exit(1);
        }
    }

    // adiciona as informações dos arquivos nas structs
    ler_arquivo('L', arq_l, livros, &num_livros);
    ler_arquivo('U', arq_u, usuarios, &num_usuarios);
    ler_arquivo('E', arq_e, emprestimos, &num_emprestimos);

    // chama o menu principal
    menu();
    printf("Saindo do programa...\n");


    // atualiza as informações dos arquivos
    atualizar_arquivo('L', arq_l, num_livros);
    atualizar_arquivo('U', arq_u, num_usuarios);
    atualizar_arquivo('E', arq_e, num_emprestimos);

    // faz backup uma última vez
    backup();

    // fecha os arquivos
    fclose(arq_l);
    fclose(arq_u);
    fclose(arq_e);

    return 0;
}




// funções
void inicializar_structs() {
    // inicializa todos os campos com zero
    for (int i = 0; i < MAX_LIVROS; i++) {
        livros[i] = (struct livro){0};
    }

    for (int i = 0; i < MAX_USUARIOS; i++) {
        usuarios[i] = (struct usuario){0};
    }

    for (int i = 0; i < MAX_EMPRESTIMOS; i++) {
        emprestimos[i] = (struct emprestimo){0};
    }
}

void ler_arquivo(char tipo, FILE *arq, void *dados, int *num_elementos) {
    // adiciona os livros, usuários e empréstimos dos arquivos .txt nas structs
    if (tipo == 'L') {
        struct livro temp = {0};
        int bool_int; // variável temporária para ler o bool como int
        while (fscanf(arq, "%d,%99[^,],%79[^,],%59[^,],%d,%d,%d\n",
                    &temp.codigo, temp.titulo, temp.autor, temp.editora,
                    &temp.ano, &temp.exemplares, &bool_int) != EOF) {
            temp.disponivel = (bool_int != 0); // se for 1 = true / se for 0 = false
            // adiciona livro ao array
            ((struct livro *)dados)[*num_elementos] = temp;
            (*num_elementos)++;  // incrementa o contador
        }
    } else if (tipo == 'U') {
        struct usuario temp = {0};
        while (fscanf(arq, "%d,%99[^,],%49[^,],%14[^,],%d-%d-%d\n",
                      &temp.matricula, temp.nome, temp.curso, temp.telefone,
                      &temp.data_cadastro.tm_year, &temp.data_cadastro.tm_mon,
                      &temp.data_cadastro.tm_mday) != EOF) {
            // adiciona usuário ao array
            ((struct usuario *)dados)[*num_elementos] = temp;
            (*num_elementos)++;
        }
    } else if (tipo == 'E') {
        struct emprestimo temp = {0};
        int bool_int; // variável temporária para ler o bool como int
        while (fscanf(arq, "%d,%d,%d,%d-%d-%d,%d-%d-%d,%d\n",
                      &temp.codigo, &temp.matricula_usuario, &temp.codigo_livro,
                      &temp.data_emprestimo.tm_year, &temp.data_emprestimo.tm_mon,
                      &temp.data_emprestimo.tm_mday, &temp.data_devolucao.tm_year,
                      &temp.data_devolucao.tm_mon, &temp.data_devolucao.tm_mday,
                      &bool_int) != EOF) {
            temp.ativo = (bool_int != 0); // se for 1 = true / se for 0 = false
            // adiciona empréstimo ao array
            ((struct emprestimo *)dados)[*num_elementos] = temp;
            (*num_elementos)++;
        }
    }
}


void atualizar_arquivo(char tipo, FILE *arq, int num_elementos) {
    // volta para o início do arquivo
    fseek(arq, 0, SEEK_SET);

    // sobrescreve o arquivo e atualiza os dados
    if (tipo == 'L') {
        for (int i = 0; i < num_elementos; i++) {
        fprintf(arq, "%d,%s,%s,%s,%d,%d,%d\n",
                livros[i].codigo, livros[i].titulo, livros[i].autor, livros[i].editora,
                livros[i].ano, livros[i].exemplares, livros[i].disponivel);
        }
    } else if (tipo == 'U') {
        for (int i = 0; i < num_elementos; i++) {
            fprintf(arq, "%d,%s,%s,%s,%d-%d-%d\n", usuarios[i].matricula, usuarios[i].nome,
                    usuarios[i].curso, usuarios[i].telefone, usuarios[i].data_cadastro.tm_year,
                    usuarios[i].data_cadastro.tm_mon, usuarios[i].data_cadastro.tm_mday);
        }
    } else if (tipo == 'E') {
        for (int i = 0; i < num_elementos; i++) {
            fprintf(arq, "%d,%d,%d,%d-%d-%d,%d-%d-%d,%d\n", emprestimos[i].codigo,
                    emprestimos[i].matricula_usuario, emprestimos[i].codigo_livro,
                    emprestimos[i].data_emprestimo.tm_year, emprestimos[i].data_emprestimo.tm_mon,
                    emprestimos[i].data_emprestimo.tm_mday, emprestimos[i].data_devolucao.tm_year,
                    emprestimos[i].data_devolucao.tm_mon, emprestimos[i].data_devolucao.tm_mday,
                    emprestimos[i].ativo);
        }
    }
}


void backup() {
    // abre os arquivos
    FILE *backup_l = fopen("livros_backup.txt", "w");

    FILE *backup_u = fopen("usuarios_backup.txt", "w");

    FILE *backup_e = fopen("emprestimos_backup.txt", "w");

    // atualiza os arquivos
    for (int i = 0; i < num_livros; i++) {
        fprintf(backup_l, "%d,%s,%s,%s,%d,%d,%d\n",
                livros[i].codigo, livros[i].titulo, livros[i].autor, livros[i].editora,
                livros[i].ano, livros[i].exemplares, livros[i].disponivel);
    }
    for (int i = 0; i < num_usuarios; i++) {
        fprintf(backup_u, "%d,%s,%s,%s,%d-%d-%d\n", usuarios[i].matricula, usuarios[i].nome,
                usuarios[i].curso, usuarios[i].telefone, usuarios[i].data_cadastro.tm_year,
                usuarios[i].data_cadastro.tm_mon, usuarios[i].data_cadastro.tm_mday);
    }
    for (int i = 0; i < num_emprestimos; i++) {
        fprintf(backup_e, "%d,%d,%d,%d-%d-%d,%d-%d-%d,%d\n", emprestimos[i].codigo,
                emprestimos[i].matricula_usuario, emprestimos[i].codigo_livro,
                emprestimos[i].data_emprestimo.tm_year, emprestimos[i].data_emprestimo.tm_mon,
                emprestimos[i].data_emprestimo.tm_mday, emprestimos[i].data_devolucao.tm_year,
                emprestimos[i].data_devolucao.tm_mon, emprestimos[i].data_devolucao.tm_mday,
                emprestimos[i].ativo);
    }

    // fecha os arquivos
    fclose(backup_l);
    fclose(backup_u);
    fclose(backup_e);
}


void menu() {
    printf("\n== Menu de opções ==\n"
        "1. Cadastrar livro\n"
        "2. Cadastrar usuário\n"
        "3. Realizar empréstimo\n"
        "4. Realizar devolução\n"
        "5. Pesquisar livro\n"
        "6. Pesquisar usuário\n"
        "7. Listar empréstimos ativos\n"
        "8. Funções avançadas\n"
        "-1. Sair do programa\n\n");

    printf("Pressione a opção desejada: ");
    int opcao = get_int();

    switch (opcao) {
        case 1:
            cadastrar_livro();
            break;
        case 2:
            cadastrar_usuario();
            break;
        case 3:
            realizar_emprestimo();
            break;
        case 4:
            realizar_devolucao();
            break;
        case 5:
            pesquisar_livro();
            break;
        case 6:
            pesquisar_usuario();
            break;
        case 7:
            listar_emprestimos();
            break;
        case 8:
            printf("\n== Opções avançadas ==\n"
                "1. Relatório de livros mais emprestados\n"
                "2. Relatório de usuários com empréstimos em atraso\n"
                "3. Pesquisa avançada com múltiplos critérios\n"
                "4. Sistema de renovação de empréstimo\n"
                "-1. Voltar\n");
            printf("\nPressione a opção desejada: ");
            opcao = get_int();
            switch (opcao) {
                case 1:
                    relatorio_livros();
                    break;
                case 2:
                    relatorio_usuarios();
                    break;
                case 3:
                    pesquisa_avancada();
                    break;
                case 4:
                    renovar_emprestimo();
                    break;
                case -1:
                    printf("Voltando ao menu.\n");
                    menu();
                    break;
                default:
                    printf("Opção inválida. Voltando ao menu.\n");
                    menu();
                    break;
            }
        case -1:
            break;
        default:
            printf("Opção inválida. Tente novamente.\n");
            menu();
            break;
    }
}


void retornar_menu() {
    printf("\nDeseja retornar ao menu? (1 - Sim / 0 - Sair do Programa): ");
    int retornar;
    do {
        retornar = get_int();
        if (retornar == 1) {
            printf("\n");
            menu();
            break;
        } else if (retornar == 0) {
            break;
        } else {
            printf("\nOpção inválida. Tente novamente: ");
        }
    } while (true);
}


void cadastrar_livro() {
    printf("\n== Cadastro de livro ==\n");

    // verifica se é possível adicionar mais livros e para o programa se o máximo foi atingido
    if (num_livros < MAX_LIVROS) {
        printf("Insira as informações do livro que deseja cadastrar:\n");
        struct livro livro;

        printf("Código do livro: ");
        livro.codigo = get_int();

        // verifica se já existe um livro com esse código e só continua se não existir
        if (existe('L', livro.codigo) == 0) {
            do {
                printf("Título: ");
                fgets(livro.titulo, sizeof(livro.titulo), stdin);
                livro.titulo[strcspn(livro.titulo, "\n")] = 0;
            } while (is_empty(livro.titulo) == 1);

            do {
                printf("Autor: ");
                fgets(livro.autor, sizeof(livro.autor), stdin);
                livro.autor[strcspn(livro.autor, "\n")] = 0;
            } while (is_empty(livro.titulo) == 1);

            do {
                printf("Editora: ");
                fgets(livro.editora, sizeof(livro.editora), stdin);
                livro.editora[strcspn(livro.editora, "\n")] = 0;
            } while (is_empty(livro.editora) == 1);

            printf("Ano de lançamento: ");
            livro.ano = get_int();

            printf("Número de exemplares disponíveis: ");
            livro.exemplares = get_int();

            // coloca a disponibilidade como "true" se existir pelo menos 1 exemplar disponível
            livro.disponivel = (livro.exemplares > 0);

            // adiciona informações ao array
            livros[num_livros] = livro;

            // incrementa o contador de livros
            num_livros++;

            backup();
        } else {
            printf("Erro! Já existe um livro com o código %d.\n", livro.codigo);
        }
    } else {
        printf("Erro! Não há mais espaço para registrar livros.\n");
    }
    retornar_menu();
}


void cadastrar_usuario() {
    printf("\n== Cadastro de usuário ==\n");

    // verifica se é possível adicionar mais usuários e para o programa se o máximo foi atingido
    if (num_usuarios < MAX_USUARIOS) {
        printf("Insira as informações do usuário que deseja cadastrar:\n");
        struct usuario usuario;

        printf("Número de matrícula: ");
        usuario.matricula = get_int();

        // verifica se já existe um usuário com a mesma matrícula e só continua se não existir
        if (existe('U', usuario.matricula) == 0) {
            do {
                printf("Nome completo: ");
                fgets(usuario.nome, sizeof(usuario.nome), stdin); // fgets para permitir espaços
                usuario.nome[strcspn(usuario.nome, "\n")] = 0;
            } while (is_empty(usuario.nome) == 1);

            do {
                printf("Curso: ");
                fgets(usuario.curso, sizeof(usuario.curso), stdin);
                usuario.curso[strcspn(usuario.curso, "\n")] = 0;
            } while (is_empty(usuario.curso) == 1);

            do {
                printf("Número de telefone: ");
                fgets(usuario.telefone, sizeof(usuario.telefone), stdin);
                usuario.telefone[strcspn(usuario.telefone, "\n")] = 0;
            } while (is_empty(usuario.telefone) == 1);

            time_t date = time(NULL);
            struct tm tm = *localtime(&date);

            usuario.data_cadastro = tm;

            // adiciona informações ao array
            usuarios[num_usuarios] = usuario;

            // incrementa o contador de usuários
            num_usuarios++;

            backup();
        } else {
            printf("Erro! Já existe um usuário com a matrícula %d.\n", usuario.matricula);
        }
    } else {
        printf("Erro! Não há mais espaço para registrar usuários.\n");
    }
    retornar_menu();
}


void realizar_emprestimo() {
    printf("\n== Empréstimo de livro ==\n");

    // verifica se é possível adicionar mais empréstimos e não executa o empréstimo se o máximo foi atingido
    if (num_emprestimos < MAX_EMPRESTIMOS) {
        printf("Insira as informações do empréstimo:\n");
        struct emprestimo emprestimo;

        printf("Código do empréstimo: ");
        emprestimo.codigo = get_int();

        // verifica se já existe um empréstimo com o mesmo código e só continua se não existir
        if (existe('E', emprestimo.codigo) == 0) {
            printf("Matrícula do usuário: ");
            emprestimo.matricula_usuario = get_int();

            // verifica se o usuário existe e só continua se existir
            if (existe('U', emprestimo.matricula_usuario) == 1) {
                printf("Código do livro: ");
                emprestimo.codigo_livro = get_int();

                // verifica se o livro existe e só continua se existir
                if (existe('L', emprestimo.codigo_livro) == 1) {
                    // empresta o livro apenas se existem exemplares disponíveis
                    for (int i = 0; i < num_livros; i++) {
                        if (emprestimo.codigo_livro == livros[i].codigo) {
                            // exibe o título do livro e o nome do autor
                            printf("'%s' de %s ", livros[i].titulo, livros[i].autor);
                            // verifica se o livro está disponível (se existem exemplares)
                            if (livros[i].disponivel == true) {
                                char usuario[101];
                                for (int u = 0; u < num_usuarios; u++) {
                                    if (usuarios[u].matricula == emprestimo.matricula_usuario) {
                                        strcpy(usuario, usuarios[u].nome);
                                    }
                                }
                                printf("emprestado pelo usuário %s com sucesso!\n", usuario);
                                livros[i].exemplares--;
                                livros[i].disponivel = (livros[i].exemplares > 0);

                                // data de empréstimo e devolução
                                time_t date = time(NULL);
                                struct tm tm = *localtime(&date);

                                emprestimo.data_emprestimo = tm;

                                date = date + 7 * 24 * 60 * 60; // data atual + 7 dias x 24 horas x 60 minutos x 60 segundos (time_t é representado em segundos)
                                tm = *localtime(&date);
                                emprestimo.data_devolucao = tm;

                                emprestimo.ativo = true;

                                // adiciona informações ao array
                                emprestimos[num_emprestimos] = emprestimo;

                                // incrementa o contador de empréstimos
                                num_emprestimos++;

                                backup();
                            } else {
                                printf("\nNão há exemplares desse livro disponíveis no momento\n");
                            }
                        }
                    }
                } else {
                    printf("Erro! Não existe nenhum livro com o código %d", emprestimo.codigo_livro);
                }
            } else {
                printf("Erro! Não existe nenhum usuário com a matrícula %d", emprestimo.matricula_usuario);
            }
        } else {
            printf("Erro! Já existe um empréstimo com o código %d.\n", emprestimo.codigo);
        }
    } else {
        printf("Erro! Não há mais espaço para registrar empréstimos.\n");
    }


    retornar_menu();
}


void realizar_devolucao() {
    printf("\n== Devolução de livro ==\n");

    printf("Insira o código do empréstimo: ");
    int codigo = get_int();

    bool devolvido = false;

    for (int i = 0; i < num_emprestimos; i++) {
        if (emprestimos[i].codigo == codigo) {
            for (int l = 0; l < num_livros; l++) {
                if (emprestimos[i].codigo_livro == livros[l].codigo) {
                    char usuario[101];
                    for (int u = 0; u < num_usuarios; u++) {
                        if (usuarios[u].matricula == emprestimos[i].matricula_usuario) {
                            strcpy(usuario, usuarios[u].nome);
                        }
                    }

                    printf("'%s' de %s emprestado pelo usuário %s ", livros[l].titulo, livros[l].autor, usuario);
                    livros[l].exemplares++;
                    livros[l].disponivel = (livros[l].exemplares > 0);
                }
            }
            printf("devolvido com sucesso!\n");
            emprestimos[i].ativo = false;
            devolvido = true;
        }
    }

    if (devolvido == false) {
        printf("Empréstimo não encontrado. Deseja inserir o código novamente? (1 - Sim / 0 - Não): ");
        int novamente;
        do {
            novamente = get_int();
            if (novamente == 1) {
                printf("\n");
                realizar_devolucao();
                break;
            } else if (novamente == 0) {
                retornar_menu();
                break;
            } else {
                printf("\nOpção inválida. Tente novamente. ");
            }
        } while (true);
    } else {
        backup();
        retornar_menu();
    }
}


void pesquisar_livro() {
    printf("\n== Pesquisa de livros ==\n");

    bool encontrado = false;
    printf("1. Código\n");
    printf("2. Título\n");
    printf("3. Autor\n");
    printf("\nEscolha o tipo de informação que deseja utilizar na pesquisa: ");
    int tipo = get_int();

    switch (tipo) {
        case 1: { // pesquisa por código
            printf("Insira o código: ");
            int codigo = get_int();

            // verifica todos os livros registrados
            for (int i = 0; i < num_livros; i++)
            {
                // compara o código inserido com o código do livro i
                if (codigo == livros[i].codigo) {

                    // informações do livro
                    printf("\nLivro de código %d encontrado\n", codigo);
                    printf("Título: %s\n", livros[i].titulo);
                    printf("Autor: %s\n", livros[i].autor);
                    printf("Editora: %s\n", livros[i].editora);
                    printf("Ano: %d\n", livros[i].ano);
                    if (livros[i].exemplares > 0) {
                        printf("%d exemplares disponíveis no momento\n", livros[i].exemplares);
                    } else if (livros[i].exemplares == 1) {
                        printf("1 exemplar disponível no momento\n");
                    } else {
                        printf("Não há exemplares disponíveis para este livro\n");
                    }

                    // define que a pesquisa foi bem sucedida
                    encontrado = true;
                }
            }
            break;
        }
        case 2: { // pesquisa por título
            char titulo[101];
            do {
                printf("Insira o título do livro: ");
                fgets(titulo, sizeof(titulo), stdin); // fgets para permitir espaços
                titulo[strcspn(titulo, "\n")] = 0; // limpa o \n
            } while (is_empty(titulo) == 1);

            // verifica todos os livros registrados
            for (int i = 0; i < num_livros; i++) {

                // compara o título inserido com o título do livro i (case insensitive por causa de cmplwr)
                if (cmplwr(titulo, livros[i].titulo) == 0) {

                    if (encontrado == false) { // como encontrado = false apenas para o primeiro livro encontrado, exibe essa mensagem apenas uma vez
                        printf("\n= Livro(s) com o título %s =\n", livros[i].titulo); // pode haver mais de um livro com o mesmo título
                        encontrado = true; // atualiza o valor de "encontrado"
                    }

                    // informações do livro
                    printf("\n- Código: %d -\n", livros[i].codigo);
                    printf("Autor: %s\n", livros[i].autor);
                    printf("Editora: %s\n", livros[i].editora);
                    printf("Ano: %d\n", livros[i].ano);
                    if (livros[i].exemplares > 0) {
                        printf("%d exemplares disponíveis no momento\n", livros[i].exemplares);
                    } else if (livros[i].exemplares == 1) {
                        printf("1 exemplar disponível no momento\n");
                    } else {
                        printf("Não há exemplares disponíveis para este livro\n");
                    }
                }
            }
            break;
        }
        case 3: { // pesquisa por autor
            char autor[51];
            do {
                printf("Insira o autor: ");
                fgets(autor, sizeof(autor), stdin);
                autor[strcspn(autor, "\n")] = 0;
            } while (is_empty(autor) == 1);


            // verifica todos os livros registrados
            for (int i = 0; i < num_livros; i++) {

                // compara o autor inserido com o autor do livro i (case insensitive por causa de cmplwr)
                if (cmplwr(autor, livros[i].autor) == 0) {

                    if (encontrado == false) { // como encontrado = false apenas para o primeiro livro encontrado, exibe essa mensagem apenas uma vez
                        printf("\n= Livro(s) do autor %s =\n", livros[i].autor); // pode haver mais de um livro do mesmo autor
                        encontrado = true; // atualiza o valor de "encontrado"
                    }

                    // informações do livro
                    printf("\n- Título: %s -\n", livros[i].titulo);
                    printf("Código: %d\n", livros[i].codigo);
                    printf("Editora: %s\n", livros[i].editora);
                    printf("Ano: %d\n", livros[i].ano);
                    if (livros[i].exemplares > 1) {
                        printf("%d exemplares disponíveis no momento\n", livros[i].exemplares);
                    } else if (livros[i].exemplares == 1) {
                        printf("1 exemplar disponível no momento\n");
                    } else {
                        printf("Não há exemplares disponíveis para este livro\n");
                    }
                }
            }
            break;
        }
        default:
            printf("Opção inválida! Tente novamente.\n");
            pesquisar_livro();
    }

    if (encontrado == false) { // se não encontrou nada na pesquisa
        printf("Nenhum livro encontrado. Deseja tentar novamente? (1 - Sim / 0 - Não): ");
        int novamente;
        do {
            novamente = get_int();
            if (novamente == 1) {
                printf("\n");
                pesquisar_livro();
                break;
            } else if (novamente == 0) {
                retornar_menu();
                break;
            } else {
                printf("\nOpção inválida. Tente novamente: ");
            }
        } while (true);
    } else {
        retornar_menu();
    }
}


void pesquisar_usuario() {
    printf("\n== Pesquisa de usuários ==\n");

    char data_formatada[11];

    bool encontrado = false;
    printf("1. Matrícula\n");
    printf("2. Nome completo\n");
    printf("\nEscolha o tipo de informação que deseja utilizar na pesquisa: ");
    int tipo = get_int();

    switch (tipo) {
        case 1: { // pesquisa por matrícula
            printf("Insira a matrícula: ");
            int matricula = get_int();

            // verifica todos os usuários registrados
            for (int i = 0; i < num_usuarios; i++)
            {
                // compara a matrícula inserida com a matrícula do usuário i
                if (matricula == usuarios[i].matricula) {

                    // informações do usuário
                    printf("\nUsuário de matrícula %d encontrado\n", matricula);
                    printf("Nome completo: %s\n", usuarios[i].nome);
                    printf("Curso: %s\n", usuarios[i].curso);
                    printf("Telefone: %s\n", usuarios[i].telefone);

                    // formata e exibe a data de cadastro
                    strftime(data_formatada, sizeof(data_formatada), "%d/%m/%Y", &usuarios[i].data_cadastro);
                    printf("Cadastrado em: %s\n", data_formatada);

                    // define que a pesquisa foi bem sucedida
                    encontrado = true;
                }
            }
            break;
        }
        case 2: {
            // pesquisa por nome
            char nome[101];
            do {
                printf("Insira o nome completo do usuário: ");
                fgets(nome, sizeof(nome), stdin); // fgets para permitir espaços
                nome[strcspn(nome, "\n")] = 0; // limpa o \n
            } while (is_empty(nome) == 1);


            // verifica todos os usuários registrados
            for (int i = 0; i < num_usuarios; i++) {

                // compara o nome inserido com o nome do usuário i (case insensitive por causa de cmplwr)
                if (cmplwr(nome, usuarios[i].nome) == 0) {

                    if (encontrado == false) { // como encontrado = false apenas para o primeiro usuário encontrado, exibe essa mensagem apenas uma vez
                        printf("\n= Usuário(s) com o nome %s =\n", usuarios[i].nome); // pode haver mais de um usuário com o mesmo nome
                    }

                    // informações do usuário
                    printf("\n- Matrícula: %d -\n", usuarios[i].matricula);
                    printf("Curso: %s\n", usuarios[i].curso);
                    printf("Telefone: %s\n", usuarios[i].telefone);

                    // formata e exibe a data de cadastro
                    strftime(data_formatada, sizeof(data_formatada), "%d/%m/%Y", &usuarios[i].data_cadastro);
                    printf("Cadastrado em: %s\n", data_formatada);

                    // define que a pesquisa foi bem sucedida
                    encontrado = true;
                }
            }
            break;
        }
        default:
            printf("Opção inválida! Tente novamente.\n");
            pesquisar_usuario();
    }

    if (encontrado == false) { // se não encontrou nada na pesquisa
        printf("Nenhum usuário encontrado. Deseja tentar novamente? (1 - Sim / 0 - Não): ");
        int novamente;
        do {
            novamente = get_int();
            if (novamente == 1) {
                printf("\n");
                pesquisar_usuario();
                break;
            } else if (novamente == 0) {
                retornar_menu();
                break;
            } else {
                printf("\nOpção inválida. Tente novamente: ");
            }
        } while (true);
    } else {
        retornar_menu();
    }
}


void listar_emprestimos() {
    printf("\n== Empréstimos ativos ==\n");

    char data_formatada[11];

    for (int i = 0; i < num_emprestimos; i++) {

        // apenas empréstimos ativos
        if (emprestimos[i].ativo == true) {
            printf("\n- Empréstimo %d -\n", emprestimos[i].codigo);

            // exibe o título e o código do livro
            for (int l = 0; l < num_livros; l++) {
                if (emprestimos[i].codigo_livro == livros[l].codigo) {
                    printf("Livro: %s \n", livros[l].titulo);
                    printf("Código do livro: %d \n", livros[l].codigo);
                    break;
                }
            }

            // exibe o nome do usuário que fez o empréstimo
            for (int u = 0; u < num_usuarios; u++) {
                if (emprestimos[i].matricula_usuario == usuarios[u].matricula) {
                    printf("Emprestado por: %s \n", usuarios[u].nome);
                    break;
                }
            }

            // formata e exibe a data de empréstimo
            strftime(data_formatada, sizeof(data_formatada), "%d/%m/%Y", &emprestimos[i].data_emprestimo);
            printf("Data do empréstimo: %s \n", data_formatada);

            // formata e exibe a data de devolução
            strftime(data_formatada, sizeof(data_formatada), "%d/%m/%Y", &emprestimos[i].data_devolucao);
            printf("Devolução prevista para: %s \n", data_formatada);
        }
    }

    retornar_menu();
}


void relatorio_livros() {
    printf("\n== Livros mais emprestados ==\n");

    // vetores de int referentes ao índice dos livros [0] e número de empréstimos [1]
    // top 3
    // valores iniciais de -1 já que não existe um índice -1. impede que haja um ranking com livros sem empréstimos
    int primeiro[2] = {-1, -1};
    int segundo[2] = {-1, -1};
    int terceiro[2] = {-1, -1};

    for (int i = 0; i < num_livros; i++) {
        int emp = 0; // número de empréstimos do livro i
        for (int e = 0; e < num_emprestimos; e++) {
            // adiciona 1 ao contador de empréstimos se o empréstimo for referente ao livro i
            if (emprestimos[e].codigo_livro == livros[i].codigo) {
                emp++;
            }
        }
        // apenas considera entrada no ranking se houver pelo menos 1 empréstimo
        if (emp >= 1) {
            if (emp >= primeiro[1]) { // verifica se é melhor do que o atual primeiro lugar
                // rebaixa o atual segundo lugar para terceiro
                terceiro[0] = segundo[0];
                terceiro[1] = segundo[1];

                // rebaixa o atual primeiro lugar para segundo
                segundo[0] = primeiro[0];
                segundo[1] = primeiro[1];

                // entra no topo do ranking
                primeiro[0] = i;
                primeiro[1] = emp;
            } else if (emp >= segundo[1]) { // verifica se é melhor do que o atual segundo lugar
                // rebaixa o atual segundo lugar para terceiro
                terceiro[0] = segundo[0];
                terceiro[1] = segundo[1];

                // entra no segundo lugar
                segundo[0] = i;
                segundo[1] = emp;
            } else if (emp >= terceiro[1]) { // verifica se é melhor do que o atual terceiro lugar
                // entra no terceiro lugar
                terceiro[0] = i;
                terceiro[1] = emp;
            }

        }
    }

    if (primeiro[0] == -1) { // se não há primeiro lugar, considera que não houve nenhum empréstimo
        printf("Nenhum livro foi emprestado até o momento\n");
    } else {
        printf("1°. '%s' de %s (código: %d), %d empréstimos\n", livros[primeiro[0]].titulo, livros[primeiro[0]].autor, livros[primeiro[0]].codigo, primeiro[1]);
        if (segundo[0] != -1) {
            printf("2°. '%s' de %s (código: %d), %d empréstimos\n", livros[segundo[0]].titulo, livros[segundo[0]].autor, livros[segundo[0]].codigo, segundo[1]);
            if (terceiro[0] != -1) {
                printf("3°. '%s' de %s (código: %d), %d empréstimos\n", livros[terceiro[0]].titulo, livros[terceiro[0]].autor, livros[terceiro[0]].codigo, terceiro[1]);
            }
        }
    }

    retornar_menu();
}


void relatorio_usuarios() {
    printf("\n== Usuários com empréstimos atrasados ==\n");

    char data_formatada[15];

    bool atraso = false; // assume que nenhum empréstimo está atrasado antes de começar a iteração

    // data atual
    time_t date = time(NULL);

    for (int i = 0; i < num_usuarios; i++) {
        bool tem_atraso = false; // reseta o bool (antes de verificar os empréstimos, usuário não tem atrasos)
        for (int e = 0; e < num_emprestimos; e++) {
            if (emprestimos[e].matricula_usuario == usuarios[i].matricula && emprestimos[e].ativo == true) {

                // converte struct tm para time_t (para usar difftime)
                time_t devolucao = mktime(&emprestimos[e].data_devolucao);

                if (difftime(date, devolucao) > 0) { // se a data atual é DEPOIS da data de devolução (está atrasado)
                    if (tem_atraso == false) {
                        printf("= Usuário %s, matrícula %d =\n", usuarios[i].nome, usuarios[i].matricula); // nome e matrícula do usuário com atraso
                        tem_atraso = true; // atualiza o valor de "tem_atraso" (se refere à existência de atrasos desse usuário)
                        atraso = true; // atualiza o valor de "atraso" (se refere à existência de atrasos de qualquer usuário)
                    }

                    // formata a data e exibe junto com o código do empréstimo
                    strftime(data_formatada, sizeof(data_formatada), "%d/%m/%Y", &emprestimos[e].data_devolucao);
                    printf("Empréstimo %d, vencido em %s\n", emprestimos[e].codigo, data_formatada);
                }
            }
        }
    }

    if (atraso == false) { // não há nenhum atraso
        printf("Não há nenhum usuário com empréstimos atrasados\n");
    }

    retornar_menu();
}


void pesquisa_avancada() {
    printf("\n== Pesquisa avançada ==\n");

    printf("O que deseja pesquisar?\n"
        "1. Livro\n"
        "2. Usuário\n"
        "3. Empréstimo\n");
    printf("\nPressione a opção desejada: ");
    int tipo = get_int();

    bool encontrado = false;


    switch (tipo) {
        case 1: { // livro
            struct livro temp = {0}; // struct para armazenar as informações de pesquisa

            bool ignorado[5]; // 0 = código / 1 = título / 2 = autor / 3 = editora / 4 = ano
            int ignorados = 0; // conta quantos campos foram ignorados (para impedir o usuário de ignorar todos os campos)

            printf("\nInsira as informações de pesquisa ou pressione 'Enter' para ignorar.\n");

            char codigo[11];
            printf("Código: ");
            fgets(codigo, sizeof(codigo), stdin);
            if (codigo[0] == '\n') {
                ignorado[0] = true;
                ignorados++;
            } else {
                if (isdigit(codigo[0]) != 0) {
                    temp.codigo = atoi(codigo);
                    ignorado[0] = false;
                } else {
                    printf("Input inválido. Este campo será ignorado.");
                    ignorado[0] = true;
                    ignorados++;
                }
            }

            printf("Título: ");
            fgets(temp.titulo, sizeof(temp.titulo), stdin);
            if (temp.titulo[0] == '\n') {
                ignorado[1] = true;
                ignorados++;
            } else {
                temp.titulo[strcspn(temp.titulo, "\n")] = 0;
                ignorado[1] = false;
            }

            printf("Autor: ");
            fgets(temp.autor, sizeof(temp.autor), stdin);
            if (temp.autor[0] == '\n') {
                ignorado[2] = true;
                ignorados++;
            } else {
                temp.autor[strcspn(temp.autor, "\n")] = 0;
                ignorado[2] = false;
            }

            printf("Editora: ");
            fgets(temp.editora, sizeof(temp.editora), stdin);
            if (temp.editora[0] == '\n') {
                ignorado[3] = true;
                ignorados++;
            } else {
                temp.titulo[strcspn(temp.titulo, "\n")] = 0;
                ignorado[3] = false;
            }

            char ano[5];
            printf("Ano: ");
            fgets(ano, sizeof(ano), stdin);
            if (ano[0] == '\n') {
                ignorado[4] = true;
                ignorados++;
            } else {
                if (isdigit(ano[0]) != 0) {
                    temp.ano = atoi(ano);
                    ignorado[4] = false;
                } else {
                    printf("Input inválido. Este campo será ignorado.");
                    ignorado[4] = true;
                    ignorados++;
                }
            }

            if (ignorados == 5) { // se todos os campos foram ignorados, sai do loop (sem fazer a pesquisa)
                printf("Pelo menos um campo deve ser preenchido.\n");
                break;
            }

            for (int i = 0; i < num_livros; i++) {
                // se o campo foi ignorado ou se a informação é compatível com o livro i, continua e faz o mesmo com os próximos campos
                if (ignorado[0] == true || temp.codigo == livros[i].codigo) {
                    if (ignorado[1] == true || cmplwr(temp.titulo, livros[i].titulo) == 0) {
                        if (ignorado[2] == true || cmplwr(temp.autor, livros[i].autor) == 0) {
                            if (ignorado[3] == true || cmplwr(temp.editora, livros[i].editora) == 0) {
                                if (ignorado[4] == true || temp.ano == livros[i].ano) {
                                    encontrado = true;
                                    printf("\nLivro encontrado!\n");
                                    printf("'%s', de %s, publicado pela editora %s em %d - código %d\n", livros[i].titulo, livros[i].autor, livros[i].editora, livros[i].ano, livros[i].codigo);
                                    if (livros[i].disponivel == true) {
                                        printf("Existe(m) %d exemplar(es) disponíveis no momento.\n", livros[i].exemplares);
                                    } else {
                                        printf("Não existem exemplares disponíveis no momento.\n");
                                    }
                                }
                            }
                        }
                    }
                }
            }

            break;
        }
        case 2: { // usuário
            struct usuario temp = {0}; // struct para armazenar as informações de pesquisa

            bool ignorado[4]; // 0 = matrícula / 1 = nome / 2 = curso / 3 = telefone
            int ignorados = 0; // conta quantos campos foram ignorados (para impedir o usuário de ignorar todos os campos)

            printf("Insira as informações de pesquisa ou pressione 'Enter' para ignorar.\n");

            char matricula[11];
            printf("Matrícula: ");
            fgets(matricula, sizeof(matricula), stdin); // fgets para permitir input vazio
            if (matricula[0] == '\n') {
                ignorado[0] = true;
                ignorados++;
            } else {
                if (isdigit(matricula[0]) != 0) {
                    temp.matricula = atoi(matricula);
                    ignorado[0] = false;
                } else {
                    printf("Input inválido. Este campo será ignorado.");
                    ignorado[0] = true;
                    ignorados++;
                }
            }

            printf("Nome completo: ");
            fgets(temp.nome, sizeof(temp.nome), stdin); // fgets para permitir espaços
            if (temp.nome[0] == '\n') {
                ignorado[1] = true;
                ignorados++;
            } else {
                temp.nome[strcspn(temp.nome, "\n")] = 0;
                ignorado[1] = false;
            }

            printf("Curso: ");
            fgets(temp.curso, sizeof(temp.curso), stdin); // fgets para permitir espaços
            if (temp.curso[0] == '\n') {
                ignorado[2] = true;
                ignorados++;
            } else {
                temp.curso[strcspn(temp.curso, "\n")] = 0;
                ignorado[2] = false;
            }

            printf("Telefone: ");
            fgets(temp.telefone, sizeof(temp.telefone), stdin); // fgets para permitir espaços
            if (temp.telefone[0] == '\n') {
                ignorado[3] = true;
                ignorados++;
            } else {
                temp.telefone[strcspn(temp.telefone, "\n")] = 0;
                ignorado[3] = false;
            }

            if (ignorados == 4) { // se todos os campos foram ignorados, sai do loop (sem fazer a pesquisa)
                printf("Pelo menos um campo deve ser preenchido.\n");
                break;
            }

            for (int i = 0; i < num_usuarios; i++) {
                // se o campo foi ignorado ou se a informação é compatível com o livro i, continua e faz o mesmo com os próximos campos
                if (ignorado[0] == true || temp.matricula == usuarios[i].matricula) {
                    if (ignorado[1] == true || cmplwr(temp.nome, usuarios[i].nome) == 0) {
                        if (ignorado[2] == true || cmplwr(temp.curso, usuarios[i].curso) == 0) {
                            if (ignorado[3] == true || cmplwr(temp.telefone, usuarios[i].telefone) == 0) {
                                encontrado = true;
                                printf("\nUsuário encontrado!\n");
                                printf("%s, do curso de %s. Número de matrícula %d e telefone %s\n", usuarios[i].nome, usuarios[i].curso, usuarios[i].matricula, usuarios[i].telefone);
                            }
                        }
                    }
                }
            }

            break;
        }
        case 3: { // empréstimo
            struct emprestimo temp = {0}; // struct para armazenar as informações de pesquisa


            bool ignorado[4]; // 0 = código / 1 = matrícula do usuário / 2 = código do livro / 3 = status
            int ignorados = 0; // conta quantos campos foram ignorados (para impedir o usuário de ignorar todos os campos)

            printf("Insira as informações de pesquisa ou pressione 'Enter' para ignorar.\n");

            char codigo[11];
            printf("Código: ");
            fgets(codigo, sizeof(codigo), stdin); // fgets para permitir input vazio
            if (codigo[0] == '\n') {
                ignorado[0] = true;
                ignorados++;
            } else {
                if (isdigit(codigo[0]) != 0) {
                    temp.codigo = atoi(codigo);
                    ignorado[0] = false;
                } else {
                    printf("Input inválido. Este campo será ignorado.");
                    ignorado[0] = true;
                    ignorados++;
                }
            }

            char matricula_usuario[11];
            printf("Matrícula do usuário: ");
            fgets(matricula_usuario, sizeof(matricula_usuario), stdin); // fgets para permitir input vazio
            if (matricula_usuario[0] == '\n') {
                ignorado[1] = true;
                ignorados++;
            } else {
                if (isdigit(matricula_usuario[0]) != 0) {
                    temp.matricula_usuario = atoi(matricula_usuario);
                    ignorado[1] = false;
                } else {
                    printf("Input inválido. Este campo será ignorado.");
                    ignorado[1] = true;
                    ignorados++;
                }
            }

            char codigo_livro[11];
            printf("Código do livro: ");
            fgets(codigo_livro, sizeof(codigo_livro), stdin); // fgets para permitir input vazio
            if (codigo_livro[0] == '\n') {
                ignorado[2] = true;
                ignorados++;
            } else {
                if (isdigit(codigo_livro[0]) != 0) {
                    temp.codigo_livro = atoi(codigo_livro);
                    ignorado[2] = false;
                } else {
                    printf("Input inválido. Este campo será ignorado.");
                    ignorado[2] = true;
                    ignorados++;
                }
            }

            char status[2];
            printf("Status (1 - ativo / 0 - inativo): ");
            fgets(status, sizeof(status), stdin); // fgets para permitir input vazio
            if (status[0] == '\n') {
                ignorado[3] = true;
                ignorados++;
            } else {
                if (isdigit(status[0]) != 0) {
                    int ativo = atoi(status);
                    if (ativo == 1) {
                        temp.ativo = true;
                        ignorado[3] = false;
                    } else if (ativo == 0) {
                        temp.ativo = false;
                        ignorado[3] = false;
                    } else {
                        printf("Input inválido. Este campo será ignorado.");
                        ignorado[3] = true;
                        ignorados++;
                    }
                } else {
                    printf("Input inválido. Este campo será ignorado.");
                    ignorado[3] = true;
                    ignorados++;
                }
            }

            if (ignorados == 4) { // se todos os campos foram ignorados, sai do loop (sem fazer a pesquisa)
                printf("Pelo menos um campo deve ser preenchido.\n");
                break;
            }

            for (int i = 0; i < num_emprestimos; i++) {
                // se o campo foi ignorado ou se a informação é compatível com o livro i, continua e faz o mesmo com os próximos campos
                if (ignorado[0] == true || temp.codigo == emprestimos[i].codigo) {
                    if (ignorado[1] == true || temp.matricula_usuario == emprestimos[i].matricula_usuario) {
                        if (ignorado[2] == true || temp.codigo_livro == emprestimos[i].codigo_livro) {
                            if (ignorado[3] == true || temp.ativo == emprestimos[i].ativo) {
                                encontrado = true;
                                printf("\nEmpréstimo encontrado!\n");

                                char livro[101];
                                char usuario[101];

                                for (int l = 0; l < num_livros; l++) {
                                    if (livros[l].codigo == emprestimos[i].codigo_livro) {
                                        strcpy(livro, livros[i].titulo);
                                    }
                                }

                                for (int u = 0; u < num_usuarios; u++) {
                                    if (usuarios[u].matricula == emprestimos[i].matricula_usuario) {
                                        strcpy(usuario, usuarios[i].nome);
                                    }
                                }

                                printf("Empréstimo código %d do livro %s (código: %d) pelo usuário %s (matrícula: %d)\n", emprestimos[i].codigo, livro, emprestimos[i].codigo_livro, usuario, emprestimos[i].matricula_usuario);
                                if (emprestimos[i].ativo == true) {
                                    printf("No momento, este empréstimo está ativo\n");
                                } else {
                                    printf("No momento, este empréstimo está inativo\n");
                                }
                            }
                        }
                    }
                }
            }

            break;
        }
        default:
            printf("Opção inválida.\n");
            break;
    }

    if (encontrado == false) {
        printf("Não foi possível encontrar o ");
        switch (tipo) {
            case 1:
                printf("livro");
                break;
            case 2:
                printf("usuário");
                break;
            case 3:
                printf("empréstimo");
                break;
            default:
                break;
        }
        printf(" desejado. Deseja tentar novamente? (1 - Sim / 0 - Não): ");
        int novamente;
        do {
            novamente = get_int();
            if (novamente == 1) {
                printf("\n");
                pesquisa_avancada();
                break;
            } else if (novamente == 0) {
                retornar_menu();
                break;
            } else {
                printf("\nOpção inválida. Tente novamente: ");
            }
        } while (true);
    } else {
        retornar_menu();
    }
}


void renovar_emprestimo() {
    printf("\n== Renovar empréstimo ==\n");

    printf("Insira o código do empréstimo que deseja renovar: ");
    int codigo = get_int();

    bool renovado = false;

    for (int i = 0; i < num_emprestimos; i++) {
        if (emprestimos[i].codigo == codigo) {
            // atualiza a data de devolução com base na data atual
            time_t date = time(NULL);
            struct tm tm = {0};

            date = date + 7 * 24 * 60 * 60; // data atual + 7 dias x 24 horas x 60 minutos x 60 segundos (time_t é representado em segundos)
            tm = *localtime(&date);
            emprestimos[i].data_devolucao = tm;

            printf("\nEmpréstimo renovado!\n");
            renovado = true;
        }
    }

    if (renovado == false) {
        printf("Empréstimo não encontrado. Deseja inserir o código novamente? (1 - Sim / 0 - Não): ");
        int novamente;
        do {
            novamente = get_int();
            if (novamente == 1) {
                printf("\n");
                renovar_emprestimo();
                break;
            } else if (novamente == 0) {
                retornar_menu();
                break;
            } else {
                printf("\nOpção inválida. Tente novamente: ");
            }
        } while (true);
    } else {
        backup();
        retornar_menu();
    }
}


int existe(char tipo, int info) {
    // tipo = livro (L), usuário (U) ou empréstimo (E)
    if (tipo == 'L') {
        for (int i = 0; i < num_livros; i++) {
            if (livros[i].codigo == info){
                return 1;
            }
        }
    } else if (tipo == 'U') {
        for (int i = 0; i < num_usuarios; i++) {
            if (usuarios[i].matricula == info) {
                return 1;
            }
        }
    } else if (tipo == 'E') {
        for (int i = 0; i < num_emprestimos; i++) {
            if (emprestimos[i].codigo == info) {
                return 1;
            }
        }
    }
    return 0;
}


int cmplwr(const char s1[], const char s2[]) { // compara duas strings case insensitive
    char s1copia[101], s2copia[101];
    strcpy(s1copia, s1);
    strcpy(s2copia, s2);
    for (int i = 0; i < strlen(s1); i++) {
        s1copia[i] = (char) tolower(s1[i]);
    }
    for (int i = 0; i < strlen(s2); i++) {
        s2copia[i] = (char) tolower(s2[i]);
    }

    if (strcmp(s1copia, s2copia) == 0) {
        return 0;
    } else {
        return 1;
    }
}

int get_int() { // garante que o input é do tipo int
    char integer[11];

    do {
        fgets(integer, sizeof(integer), stdin);
        if (integer[0] == '\n') {
            printf("Input vazio. Tente novamente: ");
        } else {
            if (isdigit(integer[0]) != 0 || (integer[0] == '-' && isdigit(integer[1]) != 0)) {
                int final = atoi(integer);
                return final;
            } else {
                printf("Input inválido. Tente novamente: ");
            }
        }
    } while (true);
}

int is_empty(const char str[101]) { // verifica se uma string é vazia
    if (str[0] == '\n' || str[0] == '\0') {
        printf("Input vazio. Tente novamente: ");
        return 1;
    } else {
        return 0;
    }
}

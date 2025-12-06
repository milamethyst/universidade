# Manual do Usuário

Ao executar, todas as funcionalidades do programa podem ser acessadas através da função menu(), que mostra ao usuário as opções que pode utilizar. Para escolher a opção, basta digitar o número correspondente à desejada e será redirecionado para a função correspondente.
As funções de opção chamam backup() se fazem qualquer tipo de manipulação de arquivos (para garantir que as alterações fiquem salvas em um arquivo secundário em caso de erro) e, ao fim de sua execução, retornar_menu(), que pergunta ao usuário se deseja, como o nome implica, retornar ao menu.

Cada livro deve ter um código único, assim como cada usuário deve ter uma matrícula única. O programa impede o registro de livros ou usuários com essa informação repetida. O mesmo serve para códigos de empréstimo, que também não podem se repetir.
Além disso, ao registrar um empréstimo, o código de livro e a matrícula de usuário devem ambos ser válidos e existentes no sistema. Caso o programa não encontre o livro ou usuário na base de dados, o empréstimo não é realizado.
Já para a devolução de livro ou renovação de um empréstimo, basta inserir o código de empréstimo. Se este não existir, o programa não executará a função e dará ao usuário a opção de tentar novamente ou retornar ao menu.

Para as pesquisas, sempre deve ser inserida a informação COMPLETA. Se, por exemplo, deseja verificar se o livro "Harry Potter" existe no sistema, não basta escrever "harry" para que o programa execute a pesquisa. O mesmo vale para nomes de usuários e autores, que devem ser inseridos por completo. Também é essencial se atentar à acentuação. Entretanto, a pesquisa é case insensitive, ou seja, não diferencia maiúsculas de minúsculas.

As funções de listagem funcionam sem qualquer input do usuário, não sendo necessária nenhuma instrução para o uso destas.



## Informações técnicas do código como um todo e de cada função (para manutenções futuras)
Todo o código está comentado e a grande maioria das informações relevantes se encontram lá. Entretanto, aqui será feito um sumário para facilitar em caso de necessidade de manutenção.


### Os arquivos .h incluídos neste programa são:
- ctype: para manipulação de tipos.
- stdbool: para o uso de booleanos.
- stdio e stdlib: para input/output e outras utilidades.
- string: para manipulação de strings.
- time: para utilização de data local e calculos de data.


### Existem alguns valores fixos e variáveis globais:
- Máximos de livros, usuários e empréstimos.
- 3 vetores de structs, um de livros, um de usuarios e um de emprestimos, cada um com a quantidade de elementos igual ao valor máximo definido anteriormente.
- 3 contadores para cada um dos tipos de dado (livros, usuarios, emprestimos) inicializados em 0.


### Os structs utilizados pelo programa são:
#### - Struct de livro:
- int codigo: é o código único do livro, utilizado para pesquisas e empréstimos. Não pode se repetir.
- char titulo[101]: título do livro. Não precisa ser único e tem tamanho máximo de 100 caracteres (mais o null terminator '\0').
- char autor[81]: nome do autor. Também não precisa ser único e tem tamanho máximo de 80 caracteres (mais o null terminator '\0').
- char editora[61]: nome da editora. Também não precisa ser único e tem tamanho máximo de 60 caracteres (mais o null terminator '\0').
- int ano: tipo int para simplicidade de pesquisa e controle, referente ao ano de publicação do livro.
- int exemplares: representa o número total de exemplares disponíveis.
- bool disponivel: booleano que tem valor true se o livro estiver disponível (ou seja, se houver ao menos 1 exemplar) e false se indisponível (0 exemplares).

#### - Struct de usuário:
- int matricula: é a matrícula única do usuário, utilizado para pesquisas e empréstimos. Não pode se repetir.
- char nome[101]: nome completo do usuário. Não precisa ser único e tem tamanho máximo de 100 caracteres (mais o null terminator '\0').
- char curso[51]: curso do usuário. Não precisa ser único e tem tamanho máximo de 50 caracteres (mais o null terminator '\0').
- char telefone[16]: telefone do usuário. Não precisa ser único e tem tamanho máximo de 15 caracteres (mais o null terminator '\0').
- struct tm data_cadastro: armazena as informações de data em um struct tm. Esse tipo de estrutura armazena o número de anos a partir de 1900, o mês -1 e o dia. Então, o dia 1 de dezembro de 2025 seria armazenado como 125-11-1.

#### - Struct de empréstimo:
- int codigo: é o código único do empréstimo, utilizado para pesquisas e empréstimos. Não pode se repetir.
- int matricula_usuario: correspondente à matrícula do usuário que realizou o empréstimo (quem está com o livro no momento).
- int codigo_livro: correspondente ao código do livro que foi emprestado.
- struct tm data_emprestimo: armazena a data em que ocorreu o empréstimo em um struct tm. Esse tipo de estrutura armazena o número de anos a partir de 1900, o mês -1 e o dia. Então, o dia 1 de dezembro de 2025 seria armazenado como 125-11-1.
- struct tm data_devolucao: armazena a data em que deverá ser feita a devolução, 7 dias após o empréstimo.
- bool ativo: booleano que tem valor true se o empréstimo estiver ativo e false se inativo (ou seja, se já foi devolvido).


### As funções do projeto incluem, em grupos:

#### Funções de inicialização, arquivos e menu
- void inicializar_structs(): inicializa todos os structs que serão necessários para o funcionamento, garantindo que estejam vazios para evitar conteúdos inesperados.
- void ler_arquivo(char tipo, FILE *arq, void *dados, int *num_elementos): lê os arquivos e passa as informações presentes neles para os vetores de structs. O char tipo define se é referente aos livros ('L'), usuários ('U') ou empréstimos ('e'). Um exemplo de chamada desta função é "ler_arquivo('L', arq_l, livros, &num_livros)". Aqui, o tipo é 'L', logo, todas as informações passadas são referentes a livros.
- void atualizar_arquivo(char tipo, FILE *arq, int num_elementos): similar à função anterior, atualiza os arquivos com as informações presentes nos vetores, com o char tipo definindo ao que se refere.
- void backup(): para garantir que nenhuma informação seja perdida caso o programa apresente algum erro, toda vez que uma informação é adicionada ou atualizada em qualquer uma das funções relacionadas aos vetores, "backup()" é chamada para armazenar as informações em um arquivo. A tarefa especificava que os arquivos principais deveriam ser abertos no início do programa e fechados apenas no final. Entretanto, os arquivos de backup são abertos, atualizados e fechados dentro dessa função.
- void menu(): mostra as opções do menu principal e permite ao usuário escolher o que deseja executar.
- void retornar_menu(): pergunta ao usuário se deseja voltar ao menu ou encerrar o programa. É executada ao final de todas as funções de opção.

#### Funções de opções
- void cadastrar_livro(): cadastra um livro.
- void cadastrar_usuario(): cadastra um usuário.
- void realizar_emprestimo(): realiza um empréstimo.
As duas opções de cadastro e a opção de empréstimo seguem uma lógica bem semelhante. As três executam somente se ainda há espaço no vetor (ou seja, se o contador é menor que o máximo permitido) e, do contrário, exibem uma mensagem de erro e não permitem um código/matrícula repetido.
As informações de tipo string são todas contidas dentro de loops do/while que impedem o usuário de seguir em frente se não inserir nenhuma informação. Assim, garante que os campos não fiquem vazios.
As informações de tipo int também não podem ser vazias e são solicidadas e devolvidas pela função get_int() (explicada mais abaixo), que executa essa lógica.
Todas as datas (cadastro, empréstimo e devolução) são aplicadas automaticamente com o uso de funções da biblioteca time.h. Além disso, os status (disponível e ativo) também são aplicados de forma lógica automaticamente, sem necessidade de intervenção do usuário.
A realização de empréstimo para caso o usuário ou o livro inseridos não existam.
- void realizar_devolucao(): realiza a devolução. Em resumo, compara o código inserido pelo usuário com os códigos de empréstimos e muda o status de "ativo" para false.
- void pesquisar_livro(): pesquisa um ou mais livros.
- void pesquisar_usuario(): pesquisa um ou mais usuários.
Ambas as pesquisas podem ser feitas com a informação única (código/matrícula) ou por uma mais genérica (título/autor/nome).
Com o código ou matrícula, a pesquisa terá apenas um resultado. Entretanto, como pode haver mais de um livro com mesmo título ou do mesmo autor, assim como mais de um usuário com mesmo nome, esta parte da pesquisa pode ou não ter mais de um resultado.
Independentemente, sempre serão exibidas todas as informações corretamente.
- void listar_emprestimos(): lista todos os empréstimos ativos no momento. Verifica se o campo "ativo" de cada empréstimo é "true" e exibe as informações apenas se for. Assim, não exibe empréstimos inativos.

#### Funções de opções avançadas
- void relatorio_livros(): exibe um ranking com os três livros mais emprestados e o número de empréstimos que foi feito para cada. O ranking não aparece caso nenhum empréstimo tenha sido feito, apenas exibe uma mensagem informando.
- void relatorio_usuarios(): compara a data atual com a data de devolução de todos os empréstimos ativos no momento e, se a data de devolução já tiver passado, exibe as informações do usuário e do empréstimo.
- void pesquisa_avancada(): aceita mais de um parâmetro para a pesquisa. Cada campo pode ser preenchido pelo usuário ou deixado vazio para ignorar. A pesquisa é feita apenas a partir dos campos preenchidos e todos devem ser compatíveis com o livro/usuário/empréstimo para ser considerado como resultado.
- void renovar_emprestimo(): atualiza a data de devolução de um empréstimo com base na data atual.

#### Funções adicionais (necessárias para o funcionamento de outras partes do código)
- int existe(char tipo, int info): utilizada para garantir que a informação inserida é única. Retorna 1 caso for encontrada em um struct e 0 se não for. O char tipo permite que seja mais específico na busca (código para livro, matrícula para usuário, código para empréstimo).
- int cmplwr(const char s1[101], const char s2[101]): compara duas strings sem diferenciar maiúsculas e minúsculas. Faz isso criando cópias em lowercase de ambas strings e comparando-as ao fim. Retorna 0 se forem iguais e 1 se forem diferentes.
- int get_int(): garante que a informação inserida por um usuário é do tipo int sem utilizar scanf (que é mais vulnerável a erros). Retorna o input como um int.
- int is_empty(const char str[101]): verifica se uma string está vazia. Se estiver, exibe uma mensagem de erro e retorna 1. Do contrário, retorna 0.

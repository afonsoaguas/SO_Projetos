/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 3 de Sistemas Operativos
 **
 ** Aluno: Nº: 99114  Nome: Afonso Valente Mareco Ferreira Águas
 ** Nome do Módulo: servidor.c v3
 ** Descrição/Explicação do Módulo: 
 ** A explicação está em comentários ao longo do código
 **
 ******************************************************************************/
#include "common.h"
#include "utils.h"
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>

/* Variáveis globais */
int msg_id;             // ID da Fila de Mensagens IPC usada
int sem_id;             // ID do array de Semáforos IPC usado
int shm_id;             // ID da Memória Partilhada IPC usada
Database* db;           // Database utilizada, que estará em Memória Partilhada
MsgCliente mensagem;    // Variável que tem a mensagem enviada do Cidadao para o Servidor
MsgServidor resposta;   // Variável que tem a mensagem de resposta enviadas do Servidor para o Cidadao
int vaga_ativa;         // Índice da BD de Vagas que foi reservado pela função reserva_vaga()
int n_cid, n_enf;

/* Protótipos de funções */
void init_ipc();                    // Função a ser implementada pelos alunos
void init_database();               // Função a ser implementada pelos alunos
void espera_mensagem_cidadao();     // Função a ser implementada pelos alunos
void trata_mensagem_cidadao();      // Função a ser implementada pelos alunos
void envia_resposta_cidadao();      // Função a ser implementada pelos alunos
void processa_pedido();             // Função a ser implementada pelos alunos
void vacina();                      // Função a ser implementada pelos alunos
void cancela_pedido();              // Função a ser implementada pelos alunos
void servidor_dedicado();           // Função a ser implementada pelos alunos
int reserva_vaga(int, int);         // Função a ser implementada pelos alunos
void liberta_vaga(int);             // Função a ser implementada pelos alunos
void termina_servidor(int);         // Função a ser implementada pelos alunos
void termina_servidor_dedicado(int);// Função a ser implementada pelos alunos
void sem_mutex_up();
void sem_mutex_down();

int main() {    // Não é suposto que os alunos alterem nada na função main()
    signal(SIGINT, termina_servidor);   // Arma o sinal SIGINT para que se receber <CTRL+C>, chama a função que termina o Servidor
    signal(SIGCHLD, SIG_IGN);
    // S1) Chama a função init_ipc(), que tenta criar uma fila de mensagens que tem a KEY IPC_KEY definida em common.h (alterar esta KEY para ter o valor do nº do aluno, como indicado nas aulas). Deve assumir que a fila de mensagens já foi criada. Se tal não aconteceu, dá erro e termina com exit status 1. Esta função, em caso de sucesso, preenche a variável global msg_id;
    init_ipc();
    // S2) Chama a função init_database(), que inicia a base de dados
    init_database();
    while (TRUE) {
        // S3) Chama a função espera_mensagem_cidadao(), que espera uma mensagem (na fila de mensagens com o tipo = 1) e preenche a mensagem enviada pelo processo Cidadão na variável global mensagem; em caso de erro, termina com erro e exit status 1;
        espera_mensagem_cidadao();
        // S4) O comportamento do processo Servidor agora irá depender da mensagem enviada pelo processo Cidadão no campo pedido:
        trata_mensagem_cidadao();
    }
}

/**
 * S1) Chama a função init_ipc(), que tenta criar:
 *     • uma fila de mensagens IPC;
 *     • um array de semáforos IPC de dimensão 1;
 *     • uma memória partilhada IPC de dimensão suficiente para conter um elemento Database.
 *     Todos estes elementos têm em comum serem criados com a KEY IPC_KEY definida em common.h (alterar esta KEY para ter o valor do nº do aluno, como indicado nas aulas), e com permissões 0600. Se qualquer um destes elementos IPC já existia anteriormente, dá erro e termina com exit status 1. Esta função, em caso de sucesso, preenche as variáveis globais respetivas msg_id, sem_id, e shm_id;
 *     O semáforo em questão será usado com o padrão “Mutex”, pelo que será iniciado com o valor 1;
 */
void init_ipc() {
    debug("<");

    msg_id = msgget(IPC_KEY, IPC_CREAT | IPC_EXCL | 0600); //cria uma fila de mensagens IPC com a KEY IPC_KEY
    exit_on_error(msg_id, "S1) Fila de Mensagens com a Key definida já existe ou não pode ser criada"); //Sai em caso de erro
    sem_id = semget(IPC_KEY, 1, IPC_CREAT | IPC_EXCL | 0600); //cria um array de semaforos IPC de dimensão 1 com a KEY IPC_KEY
    exit_on_error(sem_id, "S1) Semáforo com a Key definida já existe ou não pode ser criado"); //Sai em caso de erro
    int ini_sem = semctl(sem_id, 0, SETVAL, 1); //Inicializa o semaforo com o valor 1
    exit_on_error(ini_sem, "S1) Semáforo com a Key definida não pode ser iniciado com o valor 1"); //Sai em caso de erro
    shm_id = shmget(IPC_KEY, sizeof(Database), IPC_CREAT | IPC_EXCL | 0600); //cria uma zona de memoria partilhada IPC com a dimensão da DataBase
    exit_on_error(shm_id, "S1) Memória Partilhada com a Key definida já existe ou não pode ser criada"); //Sai em caso de erro

    sucesso("S1) Criados elementos IPC com a Key 0x%x: MSGid %d, SEMid %d, SHMid %d", IPC_KEY, msg_id, sem_id, shm_id); //Mostra no terminal a mensagem de sucesso com as Keys de todos os elementos criados

    // S1) Tenta criar:
    // Todos estes elementos têm em comum serem criados com a KEY IPC_KEY definida em common.h (alterar esta KEY para ter o valor do nº do aluno, como indicado nas aulas), e com permissões 0600. Se qualquer um destes elementos IPC já existia anteriormente, dá erro e termina com exit status 1. Esta função, em caso de sucesso, preenche as variáveis globais respetivas msg_id, sem_id, e shm_id;
    // • uma fila de mensagens IPC;
    // exit_on_error(<var>, "S1) Fila de Mensagens com a Key definida já existe ou não pode ser criada");

    // • um array de semáforos IPC de dimensão 1;
    // exit_on_error(<var>, "S1) Semáforo com a Key definida já existe ou não pode ser criada");

    // O semáforo em questão será usado com o padrão “Mutex”, pelo que será iniciado com o valor 1;
    // exit_on_error(<var>, "S1) Semáforo com a Key definida não pode ser iniciado com o valor 1");

    // • uma memória partilhada IPC de dimensão suficiente para conter um elemento Database.
    // exit_on_error(<var>, "S1) Memória Partilhada com a Key definida já existe ou não pode ser criada");

    // sucesso("S1) Criados elementos IPC com a Key 0x%x: MSGid %d, SEMid %d, SHMid %d", IPC_KEY, msg_id, sem_id, shm_id);

    debug(">");
}

/**
 * Lê um ficheiro binário
 * @param   filename    Nome do ficheiro a ler
 * @param   buffer      Ponteiro para o buffer onde armazenar os dados
 * @param   maxsize     Tamanho máximo do ficheiro a ler
 * @return              Número de bytes lidos, ou 0 em caso de erro
 */
int read_binary(char* filename, void* buffer, const size_t maxsize) {
    struct stat st;
    // A função stat() preenche uma estrutura com dados do ficheiro, incluindo o tamanho do ficheiro.
    // stat() retorna -1 se erro
    exit_on_error(stat(filename, &st), "Erro no cálculo do tamanho do ficheiro");
    // O tamanho do ficheiro é maior do que o tamanho do buffer alocado?
    if (st.st_size > maxsize)
        exit_on_error(-1, "O buffer não tem espaço para o ficheiro");

    FILE* f = fopen(filename, "r");
    // fopen retorna NULL se erro
    exit_on_null(f, "Erro na abertura do ficheiro");

    // fread está a ler st.st_size elementos, logo retorna um valor < st.st_size se erro
    if (fread(buffer, 1, st.st_size, f) < st.st_size)
        exit_on_error(-1, "Erro na leitura do ficheiro");

    fclose(f);
    return st.st_size; // retorna o tamanho do ficheiro
}

/**
 * Grava um ficheiro binário
 * @param   filename    Nome do ficheiro a escrever
 * @param   buffer      Ponteiro para o buffer que contém os dados
 * @param   size        Número de bytes a escrever
 * @return              Número de bytes escrever, ou 0 em caso de erro
 */
int save_binary(char* filename, void* buffer, const size_t size) {
    FILE* f = fopen(filename, "w");
    // fopen retorna NULL se erro
    exit_on_null(f, "Erro na abertura do ficheiro");
   
    // fwrite está a escrever size elementos, logo retorna um valor < size se erro
    if (fwrite(buffer, 1, size, f) < size)
        exit_on_error(-1, "Erro na escrita do ficheiro");

    fclose(f);
    return size;
}

/**
 * S2) Inicia a base de dados:
 *     • Associa a variável global db com o espaço de Memória Partilhada alocado para shm_id; se não o conseguir, dá erro e termina com exit status 1;
 *     • Lê o ficheiro FILE_CIDADAOS e armazena o seu conteúdo na base de dados usando a função read_binary(), assim preenchendo os campos db->cidadaos e db->num_cidadaos. Se não o conseguir, dá erro e termina com exit status 1;
 *     • Lê o ficheiro FILE_ENFERMEIROS e armazena o seu conteúdo na base de dados usando a função read_binary(), assim preenchendo os campos db->enfermeiros e db->num_enfermeiros. Se não o conseguir, dá erro e termina com exit status 1;
 *     • Inicia o array db->vagas, colocando todos os campos de todos os elementos com o valor -1.
 */
void init_database() {
    debug("<");

    db = (Database *) shmat(shm_id, NULL, 0); //Associa a variavel db á zona de memoria partilhada
    exit_on_null(db, "S2) Erro a ligar a Memória Dinâmica ao projeto"); //Sai em caso de erro

    // S2) Inicia a base de dados:
    // • Associa a variável global db com o espaço de Memória Partilhada alocado para shm_id; se não o conseguir, dá erro e termina com exit status 1;
    // exit_on_null(<var>, "S2) Erro a ligar a Memória Dinâmica ao projeto");

    int size_cid = read_binary(FILE_CIDADAOS, db->cidadaos, MAX_CIDADAOS*sizeof(Cidadao)); //Transfere as informações dos cidadaos do FILE_CIDADAOS para a zona de memoria partilhada
    db->num_cidadaos = size_cid/sizeof(Cidadao); //Divide o tamanho(em Bytes) do ficheiro FILE_CIDADAOS pelo tamanho de cada cidadao (em Bytes) para obter o numero de cidadaos, guardando o resultado na base de dados

    // • Lê o ficheiro FILE_CIDADAOS e armazena o seu conteúdo na base de dados usando a função read_binary(), assim preenchendo os campos db->cidadaos e db->num_cidadaos. Se não o conseguir, dá erro e termina com exit status 1;
 
    int size_enf = read_binary(FILE_ENFERMEIROS, db->enfermeiros, MAX_ENFERMEIROS*sizeof(Enfermeiro)); //Transfere as informações dos enfermeiros do FILE_ENFERMEIROS para a zona de memoria partilhada
    db->num_enfermeiros = size_enf/sizeof(Enfermeiro); //Divide o tamanho(em Bytes) do ficheiro FILE_ENFERMEIROS pelo tamanho de cada enfermeiro (em Bytes) para obter o numero de enfermeiros, guardando o resultado na base de dados

    // • Lê o ficheiro FILE_ENFERMEIROS e armazena o seu conteúdo na base de dados usando a função read_binary(), assim preenchendo os campos db->enfermeiros e db->num_enfermeiros. Se não o conseguir, dá erro e termina com exit status 1;
    
    for(int i=0; i<MAX_VAGAS; i++) //Percorre todo o array vagas
        db->vagas[i].index_cidadao = -1; //Inicializa todos os index_cidadao do array vagas a -1
    
    // • Inicia a Base de Dados de Vagas, db->vagas, colocando o campo index_cidadao de todos os elementos com o valor -1.

    sucesso("S2) Base de dados carregada com %d cidadãos e %d enfermeiros", db->num_cidadaos, db->num_enfermeiros); //Mostra no terminal a mensagem de sucesso com o numero de cidados e enfermeiros registados na base de dados

    // sucesso("S2) Base de dados carregada com %d cidadãos e %d enfermeiros", <num_cidadaos>, <num_enfermeiros>);

    debug(">");
}

/**
 * Espera uma mensagem (na fila de mensagens com o tipo = 1) e preenche a variável global mensagem, assim como preenche o tipo da resposta com o PID_cidadao recebido.
 * Em caso de erro, termina com erro e exit status 1;
 */
void espera_mensagem_cidadao() {
    debug("<");

    int stat_men = msgrcv(msg_id, &mensagem, sizeof(mensagem.dados), 1, 0); //O servidor fica a espera de receber uma mensagem com o tipo 1 do cidadao 
    exit_on_error(stat_men, "Não é possível ler a mensagem do Cidadao"); //Sai em caso de erro
    resposta.tipo = mensagem.dados.PID_cidadao; //Altera o tipo da resposta para o PID_cidadao do processo cidadao que fez o pedido
    sucesso("Cidadão enviou mensagem"); //Mostra no terminal a mensagem de sucesso

    // Espera uma mensagem (na fila de mensagens com o tipo = 1) e preenche a variável global mensagem,
    // assim como preenche o tipo da resposta com o PID_cidadao recebido.

    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // exit_on_error(<var>, "Não é possível ler a mensagem do Cidadao");
    // sucesso("Cidadão enviou mensagem");

    debug(">");
}

/**
 * Tratamento das mensagens do Cidadao
 */
void trata_mensagem_cidadao() {
    debug("<");

    // S4) O comportamento do processo Servidor agora irá depender da variável global mensagem enviada pelo processo Cidadão no campo pedido
    sem_mutex_down(); //diminui o valor do semaforo
    if(mensagem.dados.pedido == PEDIDO){ //Verifica se a variavel pedido da mensagem é igual a PEDIDO
        sucesso("S4.1) Novo pedido de vacinação de %d: %d, %s", mensagem.dados.PID_cidadao, mensagem.dados.num_utente, mensagem.dados.nome); //Mostra no terminal a mensagem de sucesso com o PID_cidadao, num_utente e nome do cidadao que fez o pedido
        processa_pedido(); //chama a função processa_pedido()

    }else if(mensagem.dados.pedido == CANCELAMENTO){ //Verifica se a variavel pedido da mensagem é igual a CANCELAMENTO
        sucesso("S4.2) Cancelamento de vacinação de %d: %d, %s", mensagem.dados.PID_cidadao, mensagem.dados.num_utente, mensagem.dados.nome); //Mostra no terminal a mensagem de erro com o PID_cidadao, num_utente e nome do cidadao que fez o pedido
        cancela_pedido(); //chama a função cancela_pedido()
        resposta.dados.status=OK; //mete o status da resposta a OK
        sem_mutex_up(); //aumenta o valor do semaforo
    }
    // if (...) {
        // S4.1) Se o pedido for PEDIDO, imprime uma mensagem e avança para o passo S5;
        // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        // sucesso("S4.1) Novo pedido de vacinação de %d: %d, %s", <PID_cidadao>, <num_utente>, <nome>);
        
    // } else if (...) {
        // S4.2) Se o estado for CANCELAMENTO, imprime uma mensagem, e avança para o passo S10;
        // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        // sucesso("S4.2) Cancelamento de vacinação de %d: %d, %s", <PID_cidadao>, <num_utente>, <nome>);
        
    // }

    debug(">");
}

/**
 * Estando a mensagem de resposta do processo Servidor na variável global resposta, envia essa mensagem para a fila de mensagens com o tipo = PID_Cidadao 
 */
void envia_resposta_cidadao() {
    debug("<");

    int stat_msg = msgsnd(msg_id, &resposta, sizeof(resposta.dados),0); //envia a resposta para o cidadao com as informaçoes do cidadao
    exit_on_error(stat_msg, "Não é possível enviar resposta para o cidadão"); //Sai em caso de erro
    sucesso("Resposta para o cidadão enviada"); //Mostra no terminal a mensagem de sucesso

    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // exit_on_error(<var>, "Não é possível enviar resposta para o cidadão");
    // sucesso("Resposta para o cidadão enviada");

    debug(">");
}

/**
 * S5) Processa um pedido de vacinação e envia uma resposta ao processo Cidadão. Para tal, essa função faz vários checks, atualizando o campo status da resposta:
 */
void processa_pedido() {
    debug("<");

    int found_cid = -1; //Variavel para saber se foi encontrado ou nao o cidadao
    int found_enf = -1; //Variavel para saber se foi encontrado ou nao o enfermeiro
   

    for(int i=0; i<MAX_CIDADAOS; i++){ //Percorre todos os cidadaos da base de dados
        if(mensagem.dados.num_utente == db->cidadaos[i].num_utente && strcmp(mensagem.dados.nome, db->cidadaos[i].nome) == 0){ //Verifica se o num_utente e o nome do cidadao na base de dados correspondem com os dados do cidadao na mensagem
            resposta.dados.cidadao = db->cidadaos[i]; //copia os dados do cidadao da base de dados para a variavel resposta
            n_cid = i; //guarda a posição do cidadao na base de dados
            found_cid = 1; //caso encontre o cidadao mete a variavel a 1
            break; //sai do for
        }
    }
    if(found_cid < 0){ //se não o encontrar cidadao
        resposta.dados.status = DESCONHECIDO; //mete o status da resposta a DESCONHECIDO
        erro("S5.1) Cidadão %d, %s  não foi encontrado na BD Cidadãos", mensagem.dados.num_utente, mensagem.dados.nome); //Mostra no terminal a mensagem de erro com o num_utente e nome do cidadao
    }else{ //se encontrar o cidadao
        if(db->cidadaos[n_cid].estado_vacinacao == 2){ //verifica se o cidadao ja tomou as duas vacinas
            resposta.dados.status = VACINADO; //mete o status da resposta a VACINADO
        }
        if(db->cidadaos[n_cid].PID_cidadao > 0){ //Verfica se o PID_cidadao é maior que zero, ou seja, esta a ser vacinado
            resposta.dados.status = EMCURSO; //mete o status da resposta a EMCURSO
        }else{ //caso o cidadao nao esteja a ser vacinado
            db->cidadaos[n_cid].PID_cidadao = mensagem.dados.PID_cidadao; //altera o PID_cidadao da base de dados para o PID_cidadao do processo que enviou a mensagem
        }
        sucesso("S5.1) Cidadão %d, %s encontrado, estado_vacinacao=%d, status=%d", mensagem.dados.num_utente, mensagem.dados.nome, resposta.dados.cidadao.estado_vacinacao, resposta.dados.status); //Mostra no terminal a mensagem de sucesso com o num_utente, nome e estado_vacinacao do cidadao e o status da resposta
    }

    // S5.1) Procura o num_utente e nome na base de dados (BD) de Cidadãos:
    //       • Se o utilizador (Cidadão) não for encontrado na BD Cidadãos => status = DESCONHECIDO;
    //       • Se o utilizador (Cidadão) for encontrado na BD Cidadãos, os dados do cidadão deverão ser copiados da BD Cidadãos para o campo cidadao da resposta;
    //       • Se o Cidadão na BD Cidadãos tiver estado_vacinacao = 2 => status = VACINADO;
    //       • Se o Cidadão na BD Cidadãos tiver PID_cidadao > 0 => status = EMCURSO; caso contrário, afeta o PID_cidadao da BD Cidadãos com o valor do PID_cidadao da mensagem;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // erro("S5.1) Cidadão %d, %s  não foi encontrado na BD Cidadãos", <num_utente>, <nome>);
    // sucesso("S5.1) Cidadão %d, %s encontrado, estado_vacinacao=%d, status=%d", <num_utente>, <nome>, <estado_vacinacao>, <status>);

    if(resposta.dados.status != DESCONHECIDO && resposta.dados.status != VACINADO && resposta.dados.status != EMCURSO){ //Verifica se o status da resposta não é DESCONHECIDO, VACINADO nem EMCURSO
        for(int j=0; j<MAX_ENFERMEIROS; j++){ //Percorre todos os enfermeiros da base de dados
            if(strcmp(db->cidadaos[n_cid].localidade, &db->enfermeiros[j].CS_enfermeiro[2]) == 0 ){ //Verifica se a localidade do cidadao corresponde com o Centro de Saude do enfermeiro
                n_enf = j; //guarda a posição do enfermeiro na base de dados
                found_enf = 1; //caso encontre o enfermeiro mete a variavel a 1
                sucesso("S5.2) Enfermeiro do CS %s encontrado, disponibilidade=%d, status=%d", db->cidadaos[n_cid].localidade, db->enfermeiros[j].disponibilidade, resposta.dados.status); //Mostra no terminal a mensagem de sucesso com a localidade e disponibilidade do enfermeiro e o status da resposta
                break; //sai do for
            }
        }
        if(found_enf < 0){ //se não encontrar o enfermeiro
            resposta.dados.status = NAOHAENFERMEIRO; //mete o status da resposta a NAOHAENFERMEIRO
            erro("S5.2) Enfermeiro do CS %s não foi encontrado na BD Enfermeiros", resposta.dados.cidadao.localidade); //Mostra no terminal a mensagem de erro com a localidade do cidadao
        }else{ //se encontrar o enfermeiro
            if(db->enfermeiros[n_enf].disponibilidade == 1){ //verifica se o enfermeiro esta disponivel
                if(reserva_vaga(n_cid, n_enf) == -1){ //verifica se a vaga esta ocupada
                    resposta.dados.status = AGUARDAR; //mete o status da resposta a AGUARDAR
                    erro("S5.3) Não foi encontrada nenhuma vaga livre para vacinação"); //Mostra no terminal a mensagem de erro
                }else{ //se a vaga estiver livre
                    resposta.dados.status = OK; //mete o status da resposta a OK
                    sucesso("S5.3) Foi reservada a vaga %d para vacinação, status=%d", vaga_ativa, resposta.dados.status); //Mostra no terminal a mensagem de sucesso com a vaga ativa e o status da resposta
                }
            }else if(db->enfermeiros[n_enf].disponibilidade == 0){ //se o enfermeiro não estiver disponivel
                resposta.dados.status = AGUARDAR; //mete o status da resposta a AGUARDAR
            }
        }

    }

    if(resposta.dados.status == OK){ //se o status da resposta estiver a OK
        vacina(); //chama a função vacina()
    }else{ //caso contrario
        db->cidadaos[n_cid].PID_cidadao=-1; //liberta o PID_cidadao
        envia_resposta_cidadao(); //chama a função envia_resposta_cidadao()
        resposta.dados.status=OK;//mete o status da resposta a OK para o proximo cidadao poder ser vacinado
        sem_mutex_up(); //aumenta o valor do semaforo
    }

    // S5.2) Caso o Cidadão esteja em condições de ser vacinado (i.e., se status não for DESCONHECIDO, VACINADO nem EMCURSO), procura o enfermeiro correspondente na BD Enfermeiros:
    //       • Se não houver centro de saúde, ou não houver nenhum enfermeiro no centro de saúde correspondente => status = NAOHAENFERMEIRO;
    //       • Se há enfermeiro, mas este não tiver disponibilidade => status = AGUARDAR.
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // erro("S5.2) Enfermeiro do CS %s não foi encontrado na BD Cidadãos", <localidade>);
    // sucesso("S5.2) Enfermeiro do CS %s encontrado, disponibilidade=%d, status=%d", <localidade>, <disponibilidade>, <status>);

    // S5.3) Caso o enfermeiro esteja disponível, procura uma vaga para vacinação na BD Vagas. Para tal, chama a função reserva_vaga(Index_Cidadao, Index_Enfermeiro) usando os índices do Cidadão e do Enfermeiro nas respetivas BDs:
    //      • Se essa função tiver encontrado e reservado uma vaga => status = OK;
    //      • Se essa função não conseguiu encontrar uma vaga livre => status = AGUARDAR.
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // erro("S5.3) Não foi encontrada nenhuma vaga livre para vacinação");
    // sucesso("S5.3) Foi reservada a vaga %d para vacinação, status=%d", <index_vaga>, <status>);

    // S5.4) Se no final de todos os checks, status for OK, chama a função vacina(),
    // if (OK == <status>)
    //    vacina();
    // S5.4) caso contrário, chama a função envia_resposta_cidadao(), que envia a resposta ao Cidadão;
    // else
    //     envia_resposta_cidadao();

    debug(">");
}

/**
 * S6) Processa a vacinação
 */
void vacina() {
    debug("<");

    int fork_stat = fork(); //cria um processo filho

    exit_on_error(fork_stat, "S6.1) Não foi possível criar um novo processo"); //Sai em caso de erro
    if(fork_stat == 0){ //processo filho
        servidor_dedicado(); //chama a função servidor_dedicado()
    }else{ //processo pai
        db->vagas[vaga_ativa].PID_filho = fork_stat; //altera o PID_filho da vaga_ativa no array vagas para o pid do processo filho criado
        sucesso("S6.1) Criado um processo filho com PID_filho=%d", fork_stat); //Mostra no terminal a mensagem de sucesso com o pid do processo filho
    }

    // S6.1) Cria um processo filho através da função fork();
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // exit_on_error(<var>, "S6.1) Não foi possível criar um novo processo");
    // sucesso("S6.1) Criado um processo filho com PID_filho=%d", <PID_filho>);

    // if (...) {   // Processo FILHO
        // S6.2) O processo filho chama a função servidor_dedicado();
        // servidor_dedicado();
    // } else {     // Processo PAI
        // S6.3) O processo pai regista o process ID do processo filho no campo PID_filho na BD de Vagas com o índice da variável global vaga_ativa;
    //}

    debug(">");
}

/**
 * S7) Servidor Dedicado
 */
void servidor_dedicado() {
    debug("<");

    // S7.1) Arma o sinal SIGTERM;
    signal(SIGTERM, termina_servidor_dedicado); //arma o sinal SIGTERM

    // S7.2) Envia a resposta para o Cidadao, chamando a função envia_resposta_cidadao(). Implemente também esta função, que envia a mensagem resposta para o cidadao, contendo os dados do Cidadao preenchidos em S5.1 e o campo status = OK;
    resposta.dados.status = OK; //mete o status da resposta a OK
    envia_resposta_cidadao(); //chama a função envia_resposta_cidadao()

    db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].disponibilidade = 0; //mete a disponibilidade do enfermeiro a 0
    sucesso("S7.3) Enfermeiro associado à vaga %d indisponível", vaga_ativa); //Mostra no terminal a mensagem de sucesso com numero da vaga_ativa

    // S7.3) Coloca a disponibilidade do enfermeiro afeto à vaga_ativa com o valor 0 (Indisponível);
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // sucesso("S7.3) Enfermeiro associado à vaga %d indisponível", <vaga_ativa>);

    sucesso("S7.4) Vacina em curso para o cidadão %d, %s, e com o enfermeiro %d, %s na vaga %d", db->cidadaos[db->vagas[vaga_ativa].index_cidadao].num_utente, db->cidadaos[db->vagas[vaga_ativa].index_cidadao].nome, db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].ced_profissional, db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].nome, vaga_ativa); //Mostra no terminal a mensagem de sucesso com o num_utente e nome do cidadao, a ced_profissional e o nome do enfermeiro e o numero da vaga_ativa
    sem_mutex_up(); //aumenta o valor do semaforo
    sleep(TEMPO_CONSULTA); //o servidor fica em espera TEMPO_CONCULTA
    sem_mutex_down(); //Diminui o valor do semaforo
    
    // S7.4) Imprime uma mensagem;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // sucesso("S7.4) Vacina em curso para o cidadão %d, %s, e com o enfermeiro %d, %s na vaga %d", <num_utente>, <nome cidadao>, <ced_profissional>, <nome enfermeiro>, <vaga_ativa>);
    // S7.4) Aguarda (em espera passiva!) TEMPO_CONSULTA segundos;

    // S7.5) Envia nova resposta para o Cidadao, chamando a função envia_resposta_cidadao() contendo os dados do Cidadao preenchidos em S5.1 e o campo status = TERMINADA, para indicar que a consulta terminou com sucesso;
    resposta.dados.status = TERMINADA; //mete o status da resposta a TERMINADA
    envia_resposta_cidadao(); //chama a função envia_resposta_cidadao()

    db->cidadaos[db->vagas[vaga_ativa].index_cidadao].estado_vacinacao++; //aumenta o estado_vacinacao do cidadao
    db->cidadaos[db->vagas[vaga_ativa].index_cidadao].PID_cidadao = -1; //mete o PID_cidadao a -1
    db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].nr_vacinas_dadas++; //aumenta o numero de vacinas dadas pelo enfermeiro
    db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].disponibilidade = 1; //mete o enfermeiro como disponivel
    sucesso("S7.6) Cidadão atualizado na BD para estado_vacinacao=%d, Enfermeiro atualizado na BD para nr_vacinas_dadas=%d e disponibilidade=%d", db->cidadaos[db->vagas[vaga_ativa].index_cidadao].estado_vacinacao, db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].nr_vacinas_dadas, db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].disponibilidade); //Mostra no terminal a mensagem de sucesso com o estado_vacinacao do cidadao, o nr_vacinas_dadas e a disponibilidade do enfermeiro

    // S7.6) Atualiza os dados do cidadão (incrementa estado_vacinacao) na BD de Cidadãos
    // S7.6) Atualiza os dados do cidadão (PID_cidadao = -1) na BD de Cidadãos
    // S7.6) Atualiza os dados do enfermeiro (incrementa nr_vacinas_dadas) na BD de Enfermeiros;
    // S7.6) Atualiza os dados do enfermeiro (coloca disponibilidade=1) na BD de Enfermeiros;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // sucesso("S7.6) Cidadão atualizado na BD para estado_vacinacao=%d, Enfermeiro atualizado na BD para nr_vacinas_dadas=%d e disponibilidade=%d", <estado_vacinacao>, <nr_vacinas_dadas>, <disponibilidade>);

    // S7.7) Liberta a vaga vaga_ativa da BD de Vagas, invocando a função liberta_vaga(vaga_ativa);
    liberta_vaga(vaga_ativa); //chama a função liberta_vaga(vaga_ativa)
    sem_mutex_up();//aumenta o valor do semaforo

    sucesso("S7.8) Servidor dedicado Terminado"); //Mostra no terminal a mensagem de sucesso
    exit(0); //termina o processo servidor dedicado com exit status 0
    // S7.8) Termina o processo Servidor Dedicado (filho) com exit status 0.
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // sucesso("S7.8) Servidor dedicado Terminado"); 

    debug(">");
}

/**
 * S8) Tenta reservar uma vaga livre na BD de Vagas
 */
int reserva_vaga(int index_cidadao, int index_enfermeiro) {
    debug("<");

    vaga_ativa = -1;

    for(int i=0; i<MAX_VAGAS; i++){ //percorre o numero total de vagas
        if(db->vagas[i].index_cidadao < 0){ //se o index_cidadao é menor que 0
            vaga_ativa = i; //guarda o valor da vaga na variavel vaga_ativa
            db->vagas[vaga_ativa].index_cidadao = index_cidadao; //guarda o index_cidadao no index_cidadao da vaga_ativa no array vagas
            db->vagas[vaga_ativa].index_enfermeiro = index_enfermeiro; //guarda o index_enfermeiro no index_enfermeiro da vaga_ativa no array vagas
            sucesso("S8.1.1) Encontrou uma vaga livre com o index %d", vaga_ativa); //Mostra no terminal a mensagem de sucesso com o numero da vaga_ativa
            break; //sai do for
        }
    }
    if(vaga_ativa == -1){ //no caso do valor da variavel vaga_ativa ser -1
        erro("S8.1.1) Não foi encontrada nenhuma vaga livre"); //Mostra no terminal a mensagem de erro
    }
    // S8.1) Procura uma vaga livre (index_cidadao < 0) na BD de Vagas. Se encontrar uma entrada livre:

    // S8.1.1) Atualiza o valor da variável global vaga_ativa com o índice da vaga encontrada;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // sucesso("S8.1.1) Encontrou uma vaga livre com o index %d", <vaga_ativa>);

    // S8.1.2) Atualiza a entrada de Vagas vaga_ativa com o índice do cidadão e do enfermeiro

    // S8.1.3) Retorna o valor do índice de vagas vaga_ativa ou -1 se não encontrou nenhuma vaga
    return vaga_ativa; //Retorna o valor da variavel vaga_ativa 

    debug(">");
}

/**
 * S9) Liberta a vaga da BD de Vagas, colocando o campo index_cidadao dessa entrada da BD de Vagas com o valor -1
 */
void liberta_vaga(int index_vaga) {
    debug("<");

    db->cidadaos[db->vagas[index_vaga].index_cidadao].PID_cidadao = -1; //mete o PID_cidadao do cidadao correspondente ao index_vaga a -1
    db->vagas[index_vaga].index_cidadao = -1; //mete o index_cidadao da posição index_vaga no array vagas a -1
    db->enfermeiros[db->vagas[index_vaga].index_enfermeiro].disponibilidade = 1; //mete a disponibilidade do enfermeiro correspondente ao index_vaga a 1

    sucesso("S9) A vaga com o index %d foi libertada", index_vaga); //Mostra no terminal a mensagem de sucesso com o numero da index_vaga

    debug(">");
}

/**
 * Ações quando o servidor processa um pedido de cancelamento do Cidadao
 */
void cancela_pedido() {
    debug("<");

    int pid_kill;
    int f = -1; //Variavel para saber se foi encontrado ou nao o cidadao

    for(int i=0; i<MAX_VAGAS; i++){ //percorre todas as posições do array vagas
        if(mensagem.dados.num_utente == db->cidadaos[db->vagas[i].index_cidadao].num_utente && strcmp(mensagem.dados.nome, db->cidadaos[db->vagas[i].index_cidadao].nome)==0){ //Verifica se o num_utente e o nome do cidadao na base de dados correspondem com os dados do cidadao na mensagem
            pid_kill = db->vagas[i].PID_filho; //guarda o PID_filho na variavel pid_kill
            f = 1; //caso encontre o enfermeiro mete a variavel a 1
            sucesso("S10.1) Foi encontrada a sessão do cidadão %d, %s na sala com o index %d", db->cidadaos[db->vagas[i].index_cidadao].num_utente, db->cidadaos[db->vagas[i].index_cidadao].nome, i); //Mostra no terminal a mensagem de sucesso com o num_utente e nome do cidadao
            break; //sai do for
        }
    }

    if(f < 0){ //se não encontrar o cidadao
        erro("S10.1) Não foi encontrada nenhuma sessão do cidadão %d, %s", mensagem.dados.num_utente, mensagem.dados.nome); //Mostra no terminal a mensagem de erro com o num_utente e nome do cidadao
    }else{ //se encontrar o cidadao
        kill(pid_kill, SIGTERM); //manda um sinal SIGTERM para o processo com o pid pid_kill
        sucesso("S10.2) Enviado sinal SIGTERM ao Servidor Dedicado com PID=%d", pid_kill); //Mostra no terminal a mensagem de sucesso com o pid guardado na variavel pid_kill
    }

    // S10) Processa o cancelamento de um pedido de vacinação e envia uma resposta ao processo Cidadão. Para este efeito, a função:
    // S10.1) Procura na BD de Vagas a vaga correspondente ao Cidadao em questão (procura por index_cidadao). Se encontrar a entrada correspondente, obtém o PID_filho do Servidor Dedicado correspondente;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // erro("S10.1) Não foi encontrada nenhuma sessão do cidadão %d, %s", <num_utente>, <nome cidadao>);
    // sucesso("S10.1) Foi encontrada a sessão do cidadão %d, %s na sala com o index %d", <num_utente>, <nome cidadao>, <vaga_ativa>);

    // S10.2) Envia um sinal SIGTERM ao processo Servidor Dedicado (filho) que está a tratar da vacinação;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // sucesso("S10.2) Enviado sinal SIGTERM ao Servidor Dedicado com PID=%d", <PID_filho>);

    debug(">");
}

/**
 * Ações quando o servidor recebeu um <CTRL+C>
 */
void termina_servidor(int sinal) {
    debug("<");

    if(semctl(sem_id,0,GETVAL)>0){ //no caso do valor do semaforo ser maior que 0
        sem_mutex_down(); //diminui o valor do semaforo
    }

    // S11) Implemente a função termina_servidor(), que irá tratar do fecho do servidor, e que:
    for(int i=0; i<MAX_VAGAS; i++){ //percorre todas as vagas no array vagas
        if(db->vagas[i].index_cidadao >= 0){ //se o index_cidadao da vaga for maior que 0
            kill(db->vagas[i].PID_filho, SIGTERM); //madanda um sinal SIGTERM para o PID_filho associado a essa vaga
        }
    }

    // S11.1) Envia um sinal SIGTERM a todos os processos Servidor Dedicado (filhos) ativos;

    save_binary(FILE_ENFERMEIROS, db->enfermeiros, db->num_enfermeiros*sizeof(Enfermeiro)); //atualiza o ficheiro FILE_ENFERMEIROS com a informacao da base de dados

    // S11.2) Grava o ficheiro FILE_ENFERMEIROS, usando a função save_binary();

    save_binary(FILE_CIDADAOS, db->cidadaos, db->num_cidadaos*sizeof(Cidadao)); //atualiza o ficheiro FILE_CIDADAOS com a informacao da base de dados

    // S11.3) Grava o ficheiro FILE_CIDADAOS, usando a função save_binary();

    semctl(sem_id, 0, IPC_RMID, NULL); //Remove o semaforo
    msgctl(msg_id, IPC_RMID, NULL); //remove a fila de mensagens
    shmctl(shm_id, IPC_RMID, NULL); //remove a zona de memoria partilhada

    sucesso("S11.4) Servidor Terminado");

    exit(0);

    // S11.4) Remove do sistema (IPC Remove) os semáforos, a Memória Partilhada e a Fila de Mensagens.
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // sucesso("S11.4) Servidor Terminado");
    // S11.5) Termina o processo servidor com exit status 0.

    debug(">");
}

void termina_servidor_dedicado(int sinal) {
    debug("<");

    // S12) Implemente a função termina_servidor_dedicado(), que irá tratar do fecho do servidor dedicado, e que:

    resposta.dados.status = CANCELADA;
    envia_resposta_cidadao();

    // S12.1) Envia a resposta para o Cidadao, chamando a função envia_resposta_cidadao() com o campo status=CANCELADA, para indicar que a consulta foi cancelada;

    liberta_vaga(vaga_ativa);
    // S12.2) Liberta a vaga vaga_ativa da BD de Vagas, invocando a função liberta_vaga(vaga_ativa);

    sucesso("S12.3) Servidor Dedicado Terminado"); //Mostra no terminal a mensagem de sucesso

    exit(0); //termina o processo do servidor dedicado com exit status 0

    // S12.3) Termina o processo do servidor dedicado com exit status 0;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    // sucesso("S12.3) Servidor Dedicado Terminado");

    debug(">");
}

/**
 * Altera o semáforo definido para Mutex na aplicação: Lock
 */
void sem_mutex_down() {
    struct sembuf lower = { .sem_num = 0, .sem_op = -1 };
    exit_on_error(semop(sem_id, &lower, 1), "Mutex-Lock: Não foi possível atualizar o semáforo");
    debug("MUTEX: Locked");
}

/**
 * Altera o semáforo definido para Mutex na aplicação: Unlock
 */
void sem_mutex_up() {
    struct sembuf raise = { .sem_num = 0, .sem_op = +1 };
    exit_on_error(semop(sem_id, &raise, 1), "Mutex-Unlock: Não foi possível atualizar o semáforo");
    debug("MUTEX: Unlocked");
}
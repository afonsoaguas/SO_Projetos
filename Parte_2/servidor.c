/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº: 99114  Nome: Afonso Valente Mareco Ferreira Águas
 ** Nome do Módulo: servidor.c
 ** Descrição/Explicação do Módulo: 
 ** A explicação está em comentários ao longo do código
 **
 ******************************************************************************/
#include "common.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

Vaga vagas[NUM_VAGAS];

int nr_enf;
Cidadao c;
int disp_enf, vaga_enf;
int pid_filho;
Enfermeiro* enfermeiros;
int process;
long enfsize;

void criar_lista(){ //Inicializa a lista de vagas
    for(int i=0; i<NUM_VAGAS; i++) //Percorre as posições todas do array vagas
        vagas[i].index_enfermeiro = -1; //Inicializa o index_enfermeiro de cada vaga a -1
    sucesso("S3) Iniciei a lista de %d vagas", NUM_VAGAS); //Mostra no terminal a mensagem de sucesso com o numero de vagas
}

void trata_CHLD (int signal){
    int enf_esc;
    process=wait(NULL); //Espera que o processo filho termine
    for(int p=0; p<NUM_VAGAS; p++){ //Percorre as posições todas do array vagas
        if(vagas[p].PID_filho == process){ //Compara o PID da vaga com o pid do filho
            enf_esc=vagas[p].index_enfermeiro;
            sucesso("S5.5.3.1) Vaga %d que era do servidor dedicado %d libertada", vaga_enf, process); //Mostra no terminal a mensagem de sucesso com o numero da vaga libertada e o pid do servidor
            enfermeiros[vagas[p].index_enfermeiro].disponibilidade = 1; //Mete a disponibilidade do enfermeiro a 1
            sucesso("S5.5.3.2) Enfermeiro %d atualizado para disponível", enf_esc); //Mostra no terminal a mensagem de sucesso com o numero do enfermeiro que foi atualizado para disponivel
            enfermeiros[vagas[p].index_enfermeiro].num_vac_dadas += 1; //Aumenta o numero de vacinas dadas do enfermeiro
            sucesso("S5.5.3.3) Enfermeiro %d atualizado para %d vacinas dadas", enf_esc, enfermeiros[enf_esc].num_vac_dadas); //Mostra no terminal a mensagem de sucesso com o numero do enfermeiro e o numero de vacinas dadas pelo enfermeiro
            vagas[p].PID_filho=-1; //Mete o PID_filho da vaga a -1
            FILE *up_enf = fopen(FILE_ENFERMEIROS, "r+"); //Abre o ficheiro FILE_ENFERMEIROS para leitura e escrita
            Enfermeiro enf_update;
            fseek(up_enf, enf_esc*sizeof(Enfermeiro), SEEK_SET); //Posiciona o ponteiro no enfermeiro que esta na posição enf_esc
            fread(&enf_update, sizeof(enf_update), 1, up_enf); //Guarda os dados do enfermeiro num novo enfermeiro provisorio
            enf_update.num_vac_dadas++; //Aumenta o numero de vacinas dadas pelo enfermeiro
            fseek(up_enf, -1*sizeof(Enfermeiro), SEEK_CUR); //Volta a posicionar o ponteiro no inicio do enfermeiro
            fwrite(&enf_update, sizeof(Enfermeiro), 1, up_enf); //Atualiza o enfermeiro no ficheiro FILE_ENFERMEIROS com o novo numero de vacinas dadas
            fclose(up_enf); //Fecha o ficheiro FILE_ENFERMEIROS
            sucesso("S5.5.3.4) Ficheiro FILE_ENFERMEIROS %d atualizado para %d vacinas dadas", enf_esc, enfermeiros[enf_esc].num_vac_dadas); //Mostra no terminal a mensagem de sucesso com o numero do enfermeiro atualizado e o numero de vacinas dadas atualizadas
            sucesso("S5.5.3.5) Retorna"); //Mostra no terminal a mensagem de sucesso
            vagas[vaga_enf].index_enfermeiro = -1; //Volta a por o index_enfermeiro da vaga a -1
            return;
        }
    }
}

void trata_TERM (int signal){ //Função para tratar o sinal SIGTERM
    sucesso("S5.6.1) SIGTERM recebido, servidor dedicado termina Cidadão"); //Mostra no terminal a mensagem de sucesso
    kill(c.PID_cidadao, SIGTERM); //Envia um sinal SIGTERM para o pid do cidadao
    exit(0); //Termina o processo cidadao
}

void trata_INT (int signal){ //Função para tratar o sinal SIGINT
    for(int v=0; v<NUM_VAGAS; v++){ //Percorre as posições todas do array vagas
        if(vagas[v].PID_filho > 0) //Verifica se o PID_filho é maior que 0
            kill(vagas[v].PID_filho, SIGTERM); //Envia um sinal SIGTERM para o PID_filho
    }
    remove(FILE_PID_SERVIDOR); //Apaga o ficheiro FILE_PID_SERVIDOR
    sucesso("S6) Servidor Terminado"); //Mostra no terminal a mensagem de sucesso
    exit(0); //Termina o processo cidadao
}

void trata_USR1 (int sinal){ //Função para tratar o sinal SIGUSR1
    
    if(access(FILE_PEDIDO_VACINA, F_OK) != 0){ //Verifica se o ficheiro FILE_PEDIDO_VACINA não existe
        erro("S5.1) Não foi possível abrir o ficheiro FILE_PEDIDO_VACINA"); //Mostra no terminal a mensagem de erro
        return;
    }
    FILE *ped_vac = fopen(FILE_PEDIDO_VACINA, "r"); //Abre o ficheiro FILE_PEDIDO_VACINA para leitura
    if(!ped_vac){ //Se não conseguir abrir o FILE_PEDIDO_VACINA
        erro("S5.1 Não foi possivel ler o ficheiro FILE_PEDIDO_VACINA"); //Mostra no terminal a mensagem de erro
        return;
    }
    fscanf(ped_vac, "%d:%99[^:]:%d:%99[^:]:%9[^:]:%d:%d", &c.num_utente, c.nome, &c.idade, c.localidade, c.nr_telemovel, &c.estado_vacinacao, &c.PID_cidadao); //Lê os dados do cidadao que estão no ficheiro FILE_PEDIDO_VACINA e cria um cidadao com os dados recebidos
    fclose(ped_vac); //Fecha o ficheiro FILE_ENFERMEIROS
    printf("Chegou o cidadão com o pedido nº %d, com nº utente %d, para ser vacinado no Centro de Saúde %s\n", c.PID_cidadao, c.num_utente, c.localidade); //Mostra no terminal a mensagem dentro de parenteses com o pid do cidadao, o numero de utente e a localidade do centro de saude
    sucesso("S5.1) Dados Cidadão: %d; %s; %d; %s; %s; 0", c.num_utente, c.nome, c.idade, c.localidade, c.nr_telemovel); //Mostra no terminal a mensagem de sucesso com os dados do cidadao

    char CScid[102] = "CS";
    strcat(CScid, c.localidade); //Junta "CS" á localidade do cidadao
    for(int k=0; k<nr_enf; k++){ //Percorre todos os enfermeiros
        if(strcmp(enfermeiros[k].CS_enfermeiro, CScid) == 0 && enfermeiros[k].disponibilidade == 1){ //Compara a localidade do enfermeiro com a do cidadao e verifica se a disponibilidade do enfermeiro esta a 1
            for(int j =0; j<NUM_VAGAS; j++){ //Percorre as posições todas do array vagas
                if(vagas[j].index_enfermeiro == -1){ //Verifica se a vaga esta ocupada
                    vagas[j].index_enfermeiro = k;
                    disp_enf = k;
                    vaga_enf = j;
                    enfermeiros[k].disponibilidade = 0; //Mete a disponibilidade do enfermeiro a 0
                    vagas[k].cidadao = c;
                    sucesso("S5.3) Vaga nº %d preenchida para o pedido %d", vaga_enf, c.PID_cidadao); //Mostra no terminal a mensagem de sucesso com o numero da vaga preenchida e o pid do cidadao
                    process = fork(); //Cria um processo filho
                    if( process == -1){ //Verifica se a função fork funcionou
                        erro("S5.4) Não foi possível criar o servidor dedicado"); //Mostra no terminal a mensagem de erro
                    }
                    if ( process == 0 ){ //Processo filho
                        pid_filho = getpid(); //Guarda o pid do processo
                        sucesso("S5.4) Servidor dedicado %d criado para o pedido %d", pid_filho, c.PID_cidadao); //Mostra no terminal a mensagem de sucesso com o pid do filho e o pid do cidadao
                        signal(SIGTERM, trata_TERM); //A função signal arma o sinal SIGTERM
                        kill(c.PID_cidadao, SIGUSR1); //Envia um sinal SIGUSR1 para o PID_cidadao
                        sucesso("S5.6.2) Servidor dedicado inicia consulta de vacinação"); //Mostra no terminal a mensagem de sucesso
                        sleep(TEMPO_CONSULTA); //O processo fica em espera até acabar o TEMPO_CONSULTA
                        sucesso("S5.6.3) Vacinação terminada para o cidadão com o pedido nº %d", c.PID_cidadao); //Mostra no terminal a mensagem de sucesso com o pid do cidadao
                        kill(c.PID_cidadao, SIGUSR2); //Envia um sinal SIGUSR2 para o PID_cidadao
                        sucesso("S5.6.4) Servidor dedicado termina consulta de vacinação"); //Mostra no terminal a mensagem de sucesso
                        exit(0); //Termina o processo cidadao
                    }else{
                        vagas[k].PID_filho = process;
                        sucesso("S5.5.1) Servidor dedicado %d na vaga %d", process, vaga_enf); //Mostra no terminal a mensagem de sucesso com o pid do processo filho e o numero da vaga
                        signal(SIGCHLD, trata_CHLD); //A função signal arma o sinal SIGCHLD
                        sucesso("S5.5.2) Servidor aguarda fim do servidor dedicado %d", process); //Mostra no terminal a mensagem de sucesso com o pid do processo filho
                        return;
                    }
                }else if (vagas[NUM_VAGAS-1].index_enfermeiro != -1 && j == NUM_VAGAS-1){ //No caso de não haver vagas
                    erro("S5.2.2) Não há vaga para vacinação para o pedido %d", c.PID_cidadao); //Mostra no terminal a mensagem de erro com o pid do cidadao
                    kill(c.PID_cidadao, SIGTERM); //Envia um sinal SIGTERM para o PID_cidadao
                    return;
                }
            }
            sucesso("S5.2.1) Enfermeiro %d disponível para o pedido %d", k, c.PID_cidadao); //Mostra no terminal a mensagem de sucesso com o pid do cidadao
        }else if (strcmp(enfermeiros[k].CS_enfermeiro, CScid) == 0 && enfermeiros[k].disponibilidade == 0){ //No caso do enfermeiro nao estar disponivel
            erro("S5.2.1)Enfermeiro %d indisponível para o pedido %d para o Centro de Saúde %s", k, c.PID_cidadao, enfermeiros[k].CS_enfermeiro); //Mostra no terminal a mensagem de erro com o numero do enfermeiro, o pid do cidadao a localidade do enfermeiro
            kill(c.PID_cidadao, SIGTERM); //Envia um sinal SIGTERM para o PID_cidadao
            exit(1); //Termina o processo cidadao
        }
    }
}

int main () {

    int pid_serv = getpid();
    printf("Servidor Covid-IUL [%d]: \n", pid_serv); //Mostra no terminal a mensagem dentro de parenteses com o pid do servidor
    
    FILE *spid = fopen(FILE_PID_SERVIDOR, "w"); //Abre o ficheiro FILE_PID_SERVIDOR para escrita
    if (access(FILE_PID_SERVIDOR, W_OK) != 0){ //Verifica se consegue abrir o ficheiro FILE_PID_SERVIDOR para escrita
        erro("S1) Não consegui registar o servidor!"); //Mostra no terminal a mensagem de erro
    }else{
        int pid_servidor = getpid();
        fprintf(spid, "%d", pid_servidor); //Escreve no ficheiro FILE_PID_SERVIDOR o pid do servidor
        fclose(spid); //Fecha o ficheiro FILE_PID_SERVIDOR
        sucesso("S1) Escrevi no ficheiro FILE_PID_SERVIDOR o PID: %d", pid_servidor); //Mostra no terminal a mensagem de sucesso com o pid do servidor
    }
    if (access(FILE_ENFERMEIROS, R_OK) == 0){ //Verifica se consegue abrir o ficheiro FILE_ENFERMEIROS para leitura
        FILE *enf = fopen(FILE_ENFERMEIROS, "r"); //Abre o ficheiro FILE_ENFERMEIROS para leitura
        fseek(enf, 0, SEEK_END ); //Posiciona o ponteiro no final do ficheiro
        enfsize = ftell(enf); //Guarda na variavel enfsize o tamanho do ficheiro em bytes
        fclose(enf); //Fecha o ficheiro FILE_ENFERMEIROS
        nr_enf = enfsize / sizeof(Enfermeiro); //Guarda na variavel nr_enf o numero de enfermeiros
        enfermeiros = malloc(sizeof(Enfermeiro)*nr_enf); //Cria uma estrutura de dados dinâmica em memoria com o tamanho necessario para o numero de enfermeiros
        FILE *enf2 = fopen(FILE_ENFERMEIROS, "r"); //Abre o ficheiro FILE_ENFERMEIROS para leitura
        fread(enfermeiros, sizeof(Enfermeiro), nr_enf, enf2); //Guarda os dados do enfermeiro na estrutura de dados
        fclose(enf2); //Fecha o ficheiro FILE_ENFERMEIROS
        sucesso("S2) Ficheiro FILE_ENFERMEIROS tem %ld bytes, ou seja, %d enfermeiros", enfsize, nr_enf); //Mostra no terminal a mensagem de sucesso com o tamanho do ficheiro FILE_ENFERMEIROS e o numero de enfermeiros
    }else{
        erro("S2) Não consegui ler o ficheiro FILE_ENFERMEIROS!"); //Mostra no terminal a mensagem de erro
    }
    
    criar_lista(); //Inicializa a lista de vagas

    signal(SIGUSR1, trata_USR1); //A função signal arma o sinal SIGUSR1
    sucesso("S4) Servidor espera pedidos"); //Mostra no terminal a mensagem de sucesso
    signal(SIGINT, trata_INT); //A função signal arma o sinal SIGINT
    while(1){ //O servidor fica á espera de receber um sinal
        pause();
    }

    debug("Esta é uma mensagem exemplo de debug.");
}
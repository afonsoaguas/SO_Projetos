/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº: 99114  Nome: Afonso Valente Mareco Ferreira Águas
 ** Nome do Módulo: cidadao.c
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

Cidadao c;
int fsize;

Cidadao criar_cidadao(Cidadao cidadao){ // Função para criar um novo cidadao
    
    printf("Número de Utente: "); //Mostra no terminal a mensagem dentro de parenteses
    scanf("%d", &cidadao.num_utente); //Espera que o utilizador introduza os dados pedidos e guarda numa variável
    scanf("%*[^\n]"); //Limpa stdin buufer
    
    printf("Nome: "); //Mostra no terminal a mensagem dentro de parenteses
    my_gets(cidadao.nome, 100); //Espera que o utilizador introduza os dados pedidos e guarda numa variável

    printf("Idade: "); //Mostra no terminal a mensagem dentro de parenteses
    scanf("%d", &cidadao.idade); //Espera que o utilizador introduza os dados pedidos e guarda numa variável
    scanf("%*[^\n]"); //Limpa stdin buufer

    printf("Localidade: "); //Mostra no terminal a mensagem dentro de parenteses
    my_gets(cidadao.localidade, 99); //Espera que o utilizador introduza os dados pedidos e guarda numa variável

    printf("Número de Telemóvel: "); //Mostra no terminal a mensagem dentro de parenteses
    my_gets(cidadao.nr_telemovel, 9); //Espera que o utilizador introduza os dados pedidos e guarda numa variável

    cidadao.estado_vacinacao=0; //Mete o estado de vacinação do cidadão a 0
    cidadao.PID_cidadao=getpid(); //Escreve o pid do processo no PID_cidadao do cidadao 

    sucesso("C1) Dados Cidadão: %d; %s; %d; %s; %s; 0", cidadao.num_utente, cidadao.nome, cidadao.idade, cidadao.localidade, cidadao.nr_telemovel); //Mostra no terminal a mensagem de sucesso com as informações do cidadao

    return cidadao;

}

void trata_INT (int signal){ //Função para tratar o sinal SIGINT
    sucesso("C5) O cidadão cancelou a vacinação, o pedido nº %d foi cancelado", c.PID_cidadao); //Mostra no terminal a mensagem de sucesso com o PID do cidadao que foi cancelado
    remove (FILE_PEDIDO_VACINA); //Apaga o ficheiro FILE_PEDIDO_VACINA
    exit(0); //Termina o processo cidadao
}

void trata_USR1 (int signal){ //Função para tratar o sinal SIGUSR1
    sucesso("C7) Vacinação do cidadão com o pedido nº %d em curso", c.PID_cidadao); //Mostra no terminal a mensagem de sucesso com o PID do cidadao que esta a ser vacinado
    remove(FILE_PEDIDO_VACINA); //Apaga o ficheiro FILE_PEDIDO_VACINA
}

void trata_USR2 (int signal){ //Função para tratar o sinal SIGUSR2
    sucesso("C8) Vacinação do cidadão com o pedido nº %d concluída", c.PID_cidadao); //Mostra no terminal a mensagem de sucesso com o PID do cidadao que foi vacinado
    exit(0); //Termina o processo cidadao
}

void trata_TERM (int signal){ //Função para tratar o sinal SIGTERM
    sucesso("C9) Não é possível vacinar o cidadão no pedido nº %d", c.PID_cidadao); //Mostra no terminal a mensagem de sucesso com o PID do cidadao que não foi possivel vacinar
    remove(FILE_PEDIDO_VACINA); //Apaga o ficheiro FILE_PEDIDO_VACINA
    exit(0); //Termina o processo cidadao
}

void trata_SIGALRM (int signal){ //Função para tratar o sinal SIGALRM
}

int main(){

    signal(SIGINT, trata_INT); //A função signal arma o sinal SIGINT

    int pid=getpid(); //Guarda na variavel pid o pid do processo
    //clrscr(); //Limpa o terminal
    printf("Cliente COVID-IUL [%d]:\n", pid); //Mostra no terminal a mensagem dentro de parenteses com o pid do processo
    
    c = criar_cidadao(c); //chama a função criar_cidadao, criando um cidadao
    sucesso("C2) PID Cidadão: %d", c.PID_cidadao); //Mostra no terminal a mensagem de sucesso com o PID do cidadao

    signal (SIGALRM, trata_SIGALRM); //A função signal arma o sinal SIGALRM

    while (access(FILE_PEDIDO_VACINA, F_OK) == 0){ //Enquanto existe o FILE_PEDIDO_VACINA
        erro("C3) Não é possivel iniciar o processo de vacinação neste momento"); //Mostra no terminal a mensagem de erro
        alarm(5); //De 5 em 5 segundos a função alarm manda um sinal SIGALRM
        pause(); //A função pause faz com que o processo fique a espera de um sinal
    }
    
    sucesso("C3) Ficheiro FILE_PEDIDO_VACINA pode ser criado"); //Mostra no terminal a mensagem de sucesso a informar que o FILE_PEDIDO_VACINA pode ser criado
     
    FILE *f = fopen(FILE_PEDIDO_VACINA, "w"); //Abre o ficheiro FILE_PEDIDO_VACINA para escrita
    if(access(FILE_PEDIDO_VACINA, F_OK) == 0){ //Verifica se o ficheiro FILE_PEDIDO_VACINA existe
        fprintf( f, "%d:%s:%d:%s:%s:%d:%d\n", c.num_utente, c.nome, c.idade, c.localidade, c.nr_telemovel, c.estado_vacinacao, c.PID_cidadao ); //Escreve no ficheiro FILE_PEDIDO_VACINA os dados do cidadao
        fclose(f); //Fecha o ficheiro FILE_PEDIDO_VACINA
        FILE *pedido_size = fopen(FILE_PEDIDO_VACINA, "r"); //Abre o ficheiro FILE_PEDIDO_VACINA para leitura
        fseek( pedido_size, 0, SEEK_END ); //Posiciona o ponteiro no final do ficheiro
        fsize = ftell( pedido_size ); //Guarda na variavel fsize o tamanho do ficheiro em bytes
        fclose(pedido_size); //Fecha o ficheiro FILE_PEDIDO_VACINA
        
        if(access(FILE_PEDIDO_VACINA, F_OK) == 0 && fsize != 0){ //Verifica se o ficheiro FILE_PEDIDO_VACINA existe e se o tamanho do ficheiro é diferente 0
            sucesso("C4) Ficheiro FILE_PEDIDO_VACINA criado e preenchido"); //Mostra no terminal a mensagem de sucesso
        }
    }else{
        erro("C4) Não é possivel criar o ficheiro FILE_PEDIDO_VACINA"); //Mostra no terminal a mensagem de erro
        exit(1); //Termina o processo cidadao
    }

    int pid_servidor;
    if(access(FILE_PID_SERVIDOR, F_OK) == 0){ //Verifica se o ficheiro FILE_PID_SERVIDOR existe
        FILE *s = fopen(FILE_PID_SERVIDOR, "r"); //Abre o ficheiro FILE_PID_SERVIDOR para leitura
        fscanf(s, "%d", &pid_servidor); //guarda na variavel "pid_servidor" o PID que esta no ficheiro FILE_PID_SERVIDOR
        fclose(s); //Fecha o ficheiro FILE_PID_SERVIDOR

        kill (pid_servidor, SIGUSR1); //Envia um sinal SIGUSR1 para o pid do servidor

        sucesso("C6) Sinal enviado ao Servidor: %d", pid_servidor); //Mostra no terminal a mensagem de sucesso com o pid do servidor
    }else{
        erro("C6) Não existe ficeiro FILE_PID_SERVIDOR!"); //Mostra no terminal a mensagem de erro
        remove(FILE_PEDIDO_VACINA); //Apaga o ficheiro FILE_PEDIDO_VACINA
        exit(1); //Termina o processo cidadao
    }
    
    signal(SIGUSR1, trata_USR1); //A função signal arma o sinal SIGUSR1

    signal(SIGUSR2, trata_USR2); //A função signal arma o sinal SIGUSR2

    signal(SIGTERM, trata_TERM); //A função signal arma o sinal SIGTERM

    while(1){ //O processo cidadao fica a espera de receber um sinal
        pause();
    }

}
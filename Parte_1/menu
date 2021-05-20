#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 99114  Nome: Afonso Valente Mareco Ferreira Águas
## Nome do Módulo: menu.sh
## Descrição/Explicação do Módulo: 
## A explicação está em comentarios ao longo do código 
##
###############################################################################

while :; do #Looping infinito

clear #Limpa o terminal

figlet -f mini Menu #O comando figlet desenha o titulo do menu em ASCII art

echo ""
#Lista das opções possiveis
echo "1. Listar cidadãos"  
echo "2. Adicionar enfermeiro"
echo "3. Stats"
echo "4. Agendar Vacinação"
echo "0. Sair"
echo ""
echo "Escolha uma das opções acima:"

read ans #Espera que o utilizador introduza a opção desejada

case $ans in
	1) echo ""  #No caso do utilizador selecionar a opção "1", o script irá executar o script lista_cidadaos.sh, mostrando todos os cidadãos registados
		./lista_cidadaos.sh
		echo ""
		echo "Prima Enter para voltar ao Menu Inicial." #Espera que o utilizador prima Enter para voltar ao menu principal
		read enter
		;;
	2) 	echo ""  #No caso do utilizador selecionar a opção "2", o script irá executar o script adiciona_enfermeiros, só o irá executar após o utilizador introduzir todos os argmuentos pedidos aseguir
		echo -n "Insira o nome: " 
		read arg1
		echo ""
		echo -n "Insira o número de cédula: "
		read arg2
		echo ""
		echo -n "Insira o Centro de Saúde(Ex: CSOeiras): "
		read arg3
		echo ""
		echo -n "Insira a Disponibilidade (0(Não) / 1(Sim)): "
		read arg4
		echo ""
		./adiciona_enfermeiros.sh "$arg1" $arg2 $arg3 $arg4
		echo ""
		echo "Prima Enter para voltar ao Menu Inicial." #Espera que o utilizador prima Enter para voltar ao menu principal
		read enter
		;;
	3) clear
		while :; do
		clear
		echo "" #No caso do utilizador selecionar a opção "3", o script irá apresentar um novo menu com as opções possiveis para executar o script stats.sh
		echo "Escolha uma das seguintes opções:"
		echo ""
		echo "1. Número de cidadãos por localidade"
		echo "2. Cidadãos registados com mais de 60 anos"
		echo "3. Enfermeiros disponíveis"
		echo "0. Voltar"
		echo ""
		read ans1
		case $ans1 in
			1) echo -n "Insira a Localidade(Ex: Oeiras): " #No caso do utilizador selecionar a opção "1" irá ser executado o script "stats.sh cidadaos" e vai ser pedida a localidade que o utilizador pretender
				read ans2
				echo ""
				./stats.sh cidadaos $ans2
				echo "" 
				echo "Prima Enter para voltar ao Menu."
				read enter
				;;
			2) echo "" #No caso do utilizador selecionar a opção "2" irá ser executado o script "stats.sh registados"
				./stats.sh registados
				echo ""
				echo "Prima Enter para voltar ao Menu."
				read enter
				;;
			3) echo ""  #No caso do utilizador selecionar a opção "3" irá ser executado o script "stats.sh enfermeiros"
				./stats.sh enfermeiros
				echo ""
				echo "Prima Enter para voltar ao Menu."
				read enter
				;;
			0) break #No caso do utilizador selecionar a opção "0" irá voltar para o menu pricipal
			;;
			*) echo "" #Caso do utilizador não escolher nenhuma das opções anteriores , irá aparecer uma mensagem de erro com as opções de escolha possiveis 
				echo "Erro: Insira uma opção valida (1/2/3/0)."
				echo ""
				echo "Prima Enter para voltar ao Menu Inicial."
				read enter
				;;
		esac
		done
		;;
	4) echo "" #No caso do utilizador selecionar a opção "4", irá ser executado o script agendamento.sh, mostrando o agendamento das vacinações
		./agendamento.sh
		echo ""
		echo "Prima Enter para voltar ao Menu Inicial."
		read enter
		;;
	0) exit ;; #No caso do utilizador selecionar a opção "0", o utilizador irá sair do script
	*) echo "" #Caso o utilizador não escolha nenhuma das opções anteriores , irá aparecer uma mensagem de erro com as opções de escolha possiveis
		echo "Erro: Escolhe uma das opções possiveis!(1/2/3/4/0)"
		echo ""
		echo "Prima Enter para voltar ao Menu Inicial."
		read enter 
		;;
esac

done

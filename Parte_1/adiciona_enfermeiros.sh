#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 99114  Nome: Afonso Valente Mareco Ferreira Águas
## Nome do Módulo: adiciona_enfermeiros.sh
## Descrição/Explicação do Módulo: 
## A explicação está em comentarios ao longo do código
##
###############################################################################

touch enfermeiros.txt #No caso de não haver, cria o ficheiro enfermeiros.txt


if [ $# -eq 4 ];then #Verifica se são dados exatamente 4 argumentos ($# - número de argumentos, -eq 4 - compara a primeira variável ($#) com o numero 4 )
	if ! [[ "$1" =~ [^0-9] ]] ; then  #Aparece uma mensagem de erro no caso do primeiro argumento ($1) dado não forem letras
		echo "Erro: Insere um nome válido." #Mensagem de erro
		exit #Sai do script
	elif [[ "$2" =~ [^0-9] ]]; then  #Aparece uma mensagem de erro no caso do segundo argumento ($2) dado não forem numeros
		echo "Erro: Insere um número de cédula válido [0-9]." #Mensagem de erro
		exit #Sai do script
	elif ! [[ "$3" =~ CS.+ ]]; then  #Aparece uma mensagem de erro no caso do terceiro argumento ($3) dado não começar com "CS"
		echo "Erro: Insere um Centro de saúde válido (Ex: CSOeiras)." #Mensagem de erro
		exit #Sai do script
	elif ! [[ "$4" == "1" || "$4" == "0" ]]; then  #Aparece uma mensagem de erro no caso do quarto argumento ($4) dado for difernete de 0 ou 1
		echo "Erro: Insere uma disponibilidade válida (0/1)" #Mensagem de erro
		exit #Sai do script
	fi

	if [[ $(grep -c "$3" enfermeiros.txt) != 0 ]]; then #O comando grep procura no ficheiro enfermeiros.txt pelo terceiro argumento ($3) dado (Centro de Saúde), no caso do output ter uma linha ou mais ( grep -c irá contar o número de linhas), aparece uma mensagem de erro
		echo "Erro: O Centro de Saúde introduzido já tem um enfermeiro registado" #Mensagem de erro
	else
		if [[ $(grep -c $2 enfermeiros.txt) == 0 ]]; then #O comando grep procura no ficheiro enfermeiros.txt pelo segundo argumento ($2) dado (Número de cédula), no caso de nao haver output (o resultado do comando grep ser 0) o enfermeiro é registado no ficheiro enfermeiros.txt
			echo "$2:$1:$3:0:$4" >> enfermeiros.txt #Insere no ficheiro enfermeiros.txt uma nova linha com os dados do enfermeiro fornecido (numero:nome:centro de saude:nº de vacinações:disponibilidade)
		else
			echo "Erro: O número de cédula profissional já esta introduzido num Centro de Saúde" #No caso do output da função grep ter uma linha ou mais, aparece esta mensagem de erro
		fi
	fi
	echo ""
	cat enfermeiros.txt #Mostra no terminal o conteudo do ficheiro enfermeiros.txt
	echo ""
else
	echo "Erro: Síntaxe: $0<nome>:<número cédula profissional>:<centro saúde associado>:<disponibilidade>" #No caso de não serem dados os quatro argumentos pedidos, aparece uma mensagem de erro com a formatação pretendida
fi

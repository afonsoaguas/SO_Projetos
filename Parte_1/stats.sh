#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 99114  Nome: Afonso Valente Mareco Ferreira Águas
## Nome do Módulo: stats.sh
## Descrição/Explicação do Módulo: 
## A explicação está em comentarios ao longo do código
##
###############################################################################

case $1 in

	cidadaos) if [[ $# -lt 2 || $# -gt 2 ]];then #No caso do primeiro argumento ($1) ser "cidadaos", verifica se os argumentos dados são exatamente 2 (<cidadaos><localidade>), caso contrário mostra a mensagem de erro
				echo "Erro: Síntaxe:$0<cidadaos><localidade>" #Mensagem de erro
			elif ! [[ "$2" =~ [^0-9] ]]; then #Aparece uma mensagem de erro no caso do segundo argumento ($2) dado não forem letras
				echo "Erro: Insere uma localidade válida!" #Mensagem de erro
			else
				echo "" #Imprime uma linha em branco
				echo "O número de cidadãos registados em $2 é $(awk -F":" -v loc="$2" '{if ( $4 == loc ) print $4 }' cidadaos.txt | wc -l )."  #O comando awk procura na coluna quatro do ficheiro cidadaos.txt (Localidades) pela localidade dada como argumento e por fim o comando, o "-v" no awk vai criar uma variavel dentro do awk com o valor da segunda variavel dada como imput ($2)
				echo ""
			fi
			;;
	registados)	if [[ $# -lt 1 || $# -gt 1 ]];then #No caso do primeiro argumento ($1) ser "registados", verifica se os argumentos dados são exatamente 1 (<registados>), caso contrário mostra a mensagem de erro
					echo "Erro: Síntaxe:$0<registados>" #Mensagem de erro
				else
					echo "" 
					echo "Cidadãos com idade superior a 60 anos:" #Mostra no terminal esta mensagem
					echo ""
					echo "$(sort -t":" -k 3 -nr cidadaos.txt | awk -F: '{if ($3 > 60) print $1":"$2":"$3 }')" #O comando sort vai ordenar a coluna 3 do ficheiro cidadaos.txt (Idades) por ordem inversa. O comando awk vai procurar na coluna três do output anterior pelas idades superiores a 60 anos, e por fim faz print do nome, do numero e da idade do cidadao correspondente
					echo ""
				fi
				;;
	enfermeiros) if [[ $# -lt 1 || $# -gt 1 ]];then #No caso do primeiro argumento ($1) ser "enfermeiros", verifica se os argumentos dados são exatamente 1 (<enfermeiros>), caso contrário mostra a mensagem de erro
					echo "Erro: Síntaxe:$0<enfermeiros>" #Mensagem de erro
				else
					echo ""
					echo "Enfermeiros disponíveis:" #Mostra no terminal esta mensagem
					echo ""
					echo "$(awk -F":" '{if (( $5 == 1 )) print $2}' enfermeiros.txt)" #O comando awk vai procurar na coluna cinco do ficheiro enfermeiros.txt pelos números iguais a "1" (Disponivel), mostrando no terminal apenas os nomes dos enfermeiros disponivies
					echo ""
				fi
				;;
	*)	echo "Erro: Síntaxe:$0 [cidadaos <localidade> | registados | enfermeiros]" #No caso de não ser dado como argumento nenhuma das opções anteriores, aparece no terminal esta mensagem de erro com as opções de imput possiveis
		;;
esac
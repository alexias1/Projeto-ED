dados <- read.csv("resultados.csv")
#lê os dados do arquivo csv chamado resultados e armazena o conteúdo no dataframe
#dados(estrutura q organiza informações em linhas e colunas)

dados_ordenados <- dados[order(dados$numero_sorteado), ]
#a função order() retorna os indices q colocariam o vetor em ordem crescente. 
#exemplo: x <- c(50, 10, 30) order(x), retorna 2 3 1. (significa = p ordenar x, o segundo elemento deve vir primeiro (10)...)
#dados[order] = acessa as linhas de dados de acordo com os indices retornados
#resumo: reorganiza as linhas do dataframe dados, colocando em ordem CRESCENTE com base na coluna numero_sorteado. o resultado é 
#armazenado em dados_ordenados.

limite_y <- max(dados_ordenados$comparacoes_abb, dados_ordenados$comparacoes_lista)
#essa linha define o valor MAXIMO do eixo Y q será usado no gráfico, garante q todas as linhas caibam no gráfico
#a função max() retorna o maior valor entre os vetores! o R concatena os dois vetores q coloquei de parametro, e retorna o maior valor 
#entre TODOS eles. exemplo: x <- c(3, 8, 9) y <- c(7, 2, 9) max(x, y) resultado = 9.
#limite_y armazena o maior valor
#resumo: pegue o maior número de comparações entre ABB e lista p definir o topo do gráfico.

plot(dados_ordenados$numero_sorteado, dados_ordenados$comparacoes_abb, type = "l", col = "blue", lwd = 2,
    xlab = "Número Sorteado", ylab = "Número de Comparações",
    main = "Comparação: ABB vs Lista", 
    ylim = c(0,limite_y))
#a função plot serve p criar gráficos.
#$numero_sorteado (todos os numeros sorteados p serem buscados) e $comparacoes_abb (todas as comparacoes feita na abb p cada numero) definem os valores do 
#eixo X e eixo Y, respectivamente.
#type = l, define que o gráfico vai ser do tipo LINHA
#col = blue define a cor da linha da abb (sempre se refere ao q esta sendo desenhado no grafico)
#lwd = 2 é a espessura da linha
#xlab é o rótulo q fica no eixo X, e ylab o rotulo do eixo y
#main é o titulo do grafico
#ylim define o INTERVALO do eixo Y, começando em 0 e indo até limite_y

lines(dados_ordenados$numero_sorteado, dados_ordenados$comparacoes_lista, col = "red", lwd = 2)
#lines adiciona uma nova linha no grafico ja criado em plot()

legend("topright", legend = c("ABB", "Lista"), col = c("blue", "red"), lwd = 2)
#legend() serve p adicionar legenda no grafico.
#topright é onde a legenda vai ser colocada
#legend define os textos q vao aparecer na legenda, abb sendo relacionado a linha azul e lista a linha vermelha
# abb -> blue, lista -> red
#resultado no gráfico -> aparece uma caixinha no canto superior direito, mostrando -----(em cor azul) ABB ------ (em cor vermelha) Lista


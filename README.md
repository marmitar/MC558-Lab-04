# MC558 - Tarefa 4

## Problema

### Engenharia de Tráfego

Um engenheiro de tráfego recebeu a tarefa de melhorar o escoamento do trânsito de uma cidade. Para tal ele recebeu um mapa da cidade definindo todos os cruzamentos da cidade e, para cada trecho de rua entre dois cruzamentos, a mão do trecho; isto é, se é de mão dupla ou de mão única, e qual o sentido dos trechos de mão única. O mapa que lhe foi dado da organização atual garante que de qualquer cruzamento da cidade se pode chegar em qualquer outro cruzamento. Este é um requisito fundamental para a organização do trânsito da cidade, caso contrário um cidadão poderia ser forçado a desrespeitar as mãos de direção para poder voltar para casa. O engenheiro tem uma proposta de como uma reorganização das mãos de direção poderia melhorar o escoamento do trânsito; porém ele não consegue garantir que a sua proposta satisfaz o requisito fundamental de permitir acesso a qualquer cruzamento a partir de qualquer outro cruzamento. Então ele contratou você, um programador experiente, para avaliar se a proposta dele satisfaz esse requisito ou não.

## Entrada e Saída

**Entrada:** A primeira linha de cada caso de teste contém dois inteiros *N* e *M* , separados por um espaço em branco, que representam, respectivamente, quantos cruzamentos (2 ≤ *N* ≤ 300) e quantos trechos de ruas entre cruzamentos há na cidade (1 ≤ *M* ≤ 600). Cada cruzamento é representado por um inteiro entre 0 e *N−1*. Cada uma das *M* linhas subsequentes de cada caso de teste contém três inteiros *A*, *B* e *D* (0 ≤ *A*, *B* < *N* , 1 ≤ *D* ≤ 2), separados por um espaço em branco, indicando que existe um trecho de rua que liga os cruzamentos *A* e *B*; se *D = 1* então o trecho é de mão única e vai do cruzamento *A* para o cruzamento *B* e se *D = 2* o trecho é de mão dupla. Você pode supor que para cada par de cruzamentos A e B existe no máximo um trecho de rua que os liga.

**Saı́da:** A saı́da deve ser escrita na saı́da padrão (terminal). A saı́da deve ser uma única linha, contendo o texto **“Adequado.”** sempre que a proposta do engenheiro satisfizer o requisito fundamental ou o texto **“Inadequado.”** caso contrário (há uma quebra de linha após o texto).

### Exemplos

```bash
$ ./t4 << EOF
  10 18
  0 1 2
  1 2 1
  1 3 1
  1 4 2
  2 3 1
  4 0 1
  5 1 1
  5 7 2
  6 1 1
  6 2 1
  6 3 1
  6 9 1
  7 4 1
  7 8 2
  8 0 1
  8 9 2
  9 3 1
  9 5 2
EOF
Inadequado.
```

```bash
$ ./t4 << EOF
  10 20
  0 1 2
  1 2 1
  1 3 1
  1 4 1
  2 0 1
  2 3 2
  4 0 1
  5 1 1
  5 2 1
  5 7 2
  6 1 1
  6 2 1
  6 3 2
  6 9 1
  7 4 2
  7 8 2
  8 0 1
  8 9 2
  9 3 1
  9 5 2
EOF
Adequado.
```

## Implementação e Submissão

-  A solução deverá ser implementada em C, C++, Pascal ou Python 2/Python 3. Só é permitido o uso de bibliotecas padrão.


- O programa deve ser submetido no SuSy, com o nome principal **t4** (por exemplo, `t4.c`).

- O número máximo de submissões é 20.

- A tarefa contém 10 testes abertos e 10 testes fechados. A nota será proporcional ao número de acertos nos testes fechados.

## Prazo

**A solução pode ser submetida até o dia 07/06/21.**

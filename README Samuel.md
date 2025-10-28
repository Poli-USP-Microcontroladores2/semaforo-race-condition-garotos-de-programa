# PSI-Microcontroladores2-Aula06
Atividade: Resolução de Race Condition com Semáforo

## 🎯 Objetivos da Atividade
Nesta atividade, os alunos deverão:
- Retomar o código gerado por IA em atividade anterior que apresenta **condições de corrida (race conditions)**.
- Trabalhar em **duplas ou trios**, com **avaliação cruzada interna** entre os integrantes do grupo.
- Aplicar **testes estruturados** com pré-condição, etapas de teste e pós-condição.
- Demonstrar como o problema de concorrência foi **identificado e resolvido** com uso de semáforo.

## 🧠 Etapas da Atividade

### **1️⃣ Revisão do Código Anterior**
- Cada integrante do grupo deverá **executar o código do colega** que contém a race condition original.
- Documentar:
  - O comportamento incorreto observado.
  - O momento em que o erro ocorre (condição específica, sequência de eventos, etc.).

### **2️⃣ Planejamento de Testes**
Para cada cenário, descreva **três casos de teste** seguindo o formato abaixo:

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | ... | ... | ... |
| 2 | ... | ... | ... |
| 3 | ... | ... | ... |

### **3️⃣ Correção e Reteste**
- Corrigir o código para **eliminar a race condition**.
- Reexecutar **os mesmos casos de teste** e registrar:
  - As mudanças feitas.
  - O resultado após a correção com evidências (capturas de tela por exemplo).

### **4️⃣ Avaliação Interna (entre colegas do mesmo grupo)**
Cada integrante deverá:
1. Executar o código original do colega conforme os testes planejados.
2. Executar o código corrigido do colega conforme os testes planejados.
3. Conferir se as condições de corrida foram eliminadas.  
4. Registrar uma **avaliação curta** (pode ser no final do README):
   - O que estava errado antes.  
   - O que mudou com a correção.
   - Se o comportamento agora é estável.  

## 📦 Entregáveis

No repositório do grupo, incluir:
1. `README.md` (este arquivo) contendo:
   - Nome dos integrantes.
   - Cenário escolhido.
   - Casos de teste.
   - Descrição da race condition e da solução.
   - Avaliação de cada colega.
2. Código-fonte organizado (considerando um código original e um corrigido por cada integrante):
   - `codigo_original/`
   - `codigo_corrigido/`
3. Evidências (prints, logs, vídeos curtos, etc.) da execução dos testes.

---

**Repositório:** entregue via GitHub Classroom (um repositório por grupo) e um PDF do markdown final no Moodle.


1) O código queria incrementar corretamente um contador compartilhado entre duas threads, cada uma fazendo NUM_INCREMENTOS vezes, para que no final o valor total fosse 2 * NUM_INCREMENTOS.

Em outras palavras, a intenção era somar os incrementos das duas threads e chegar ao resultado esperado — mas, por causa da race condition, as threads interferem entre si e o contador final sai errado.

A race condition acontece nas linhas:

```c
int temp = contador_compartilhado;
k_busy_wait(50);
contador_compartilhado = temp + 1;
```

Elas tentam incrementar o contador, mas como duas threads fazem isso ao mesmo tempo, podem sobrescrever o valor uma da outra.


2) Tabela 1: Verificação de incremento único

| Caso de Teste | Pré-condição                 | Etapas de Teste                                                              | Pós-condição Esperada                                                     |
| ------------- | ---------------------------- | ---------------------------------------------------------------------------- | ------------------------------------------------------------------------- |
| 1             | `contador_compartilhado = 0` | Iniciar Thread A e Thread B quase simultaneamente, cada uma com 1 incremento | O valor final pode ser 1 ou 2, dependendo de qual thread escreve primeiro |
| 2             | `contador_compartilhado = 1` | Executar Thread A até ler o valor, pausar, permitir que Thread B complete    | O valor final pode ser 2 em vez de 3                                      |
| 3             | `contador_compartilhado = 2` | Executar Thread A e B sem delay e contar 10 incrementos cada                 | Alguns incrementos podem ser perdidos; contador final < 22                |


Tabela 2: Teste com delays variados

Caso de Teste	Pré-condição	Etapas de Teste	Pós-condição Esperada
1	contador_compartilhado = 0	Adicionar k_busy_wait(100) em Thread A, iniciar Thread B imediatamente	Contador final menor que 2, simula atraso de thread
2	contador_compartilhado = 5	Alternar execução de threads em intervalos de 50 microssegundos	Contador final pode não ser 7, incrementos podem se perder
3	contador_compartilhado = 10	Remover todo delay e executar muitas iterações rapidamente	Resultado final sempre < valor esperado teórico

Tabela 3: Teste com múltiplas threads

Caso de Teste	Pré-condição	Etapas de Teste	Pós-condição Esperada
1	contador_compartilhado = 0	Criar 3 threads simultâneas, cada uma com 1 incremento	Contador final pode ser 1, 2 ou 3
2	contador_compartilhado = 3	Intercalar threads com delays curtos diferentes	Alguns incrementos podem ser perdidos; resultado final < 6
3	contador_compartilhado = 5	Executar 5 threads com 100 incrementos cada, sem proteção	Muitos incrementos perdidos; valor final significativamente menor que esperado

3)  Cenário 1: Interferência simultânea de threads

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | Contador_compartilhado = 0 | Forçar que Thread A e B leiam o contador quase ao mesmo tempo | Contador final = 2; incrementos corretos, sem valores repetidos|
| 2 | Contador_compartilhado = 5| Permitir que Thread A seja interrompida após ler e antes de escrever; Thread B executa completamente| Contador final = 7; cada thread incrementa exatamente uma vez |
| 3 | Contador_compartilhado = 10 | Executar múltiplos ciclos rápidos, sem delay entre threads | Contador final = 20; incrementos corretos, sem perda de contagem|

Cenário 2: Interferência por atraso artificial (k_busy_wait)

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | contador_compartilhado = 0| Aumentar o k_busy_wait() de uma thread e executar ambas|Contador final = 2; mesmo com atraso, a exclusão mútua garante incrementos corretos|
| 2 | contador_compartilhado = 5; Thread A entra em k_busy_wait enquanto Thread B lê o valor| Observar saída no terminal e LEDs| Contador final = 7; sem conflito, cada thread incrementa uma vez|
| 3 | contador_compartilhado = 10; Executar 5 ciclos de cada thread com atrasos longos| Medir inconsistência entre contador e número de incrementos | Contador final = 20; incrementos corretos mesmo com janela crítica longa|

Cenário 3: Efeito do atraso artificial (k_busy_wait)

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | contador_compartilhado = 0; k_busy_wait aumentado para 5000 μs| Executar Thread A e B simultaneamente| Contador final = 2; sem race condition, cada incremento é seguro|
| 2 | contador_compartilhado = 10; Thread B inicia antes de Thread A terminar k_busy_wait | Observar saída no terminal| Contador final = 12; incrementos corretos apesar do atraso|
| 3 | contador_compartilhado = 20; Executar 5 ciclos| Medir inconsistência entre o contador e número de ciclos | Contador final = 30; sem perda de incrementos|
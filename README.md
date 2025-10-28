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


1) Comportamento observado no código: O objetivo do código (sem a race condition) é contar quantas vezes cada LED acendeu. Contudo, observa-se perda de precisão na contagem, que conta 4 ciclos no lugar de 2 a cada registro de quatro ciclos. Além disso, nota-se que o código deveria acender um Led vermelho e, após 1000 ms, um led verde. Porém, eles acendem ao mesmo tempo, o que também explica a perda de precisão na contagem.

A ocorrência da race condition pode ser observada nas seguintes linhas de código: int temp = contador_compartilhado;  // 1️⃣ leitura
temp++;                             // 2️⃣ modificação
k_busy_wait(1000);                  // 3️⃣ atraso (janela crítica)
contador_compartilhado = temp;      // 4️⃣ escrita
O uso do k_busy_wait permite que a outra thread invada sua execução, assim imprecisando a variável "contador_compartilhado" e, assim, permitindo a ocorrência de race conditions.

2) Cenário 1: Interferência simultânea de threads

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | Contador_compartilhado = 0 | Forçar que Thread A e B leiam o contador quase ao mesmo tempo | O valor final do contador pode ser 1 (em vez de 2) devido à leitura simultânea|
| 2 | Contador_compartilhado = 5| Permitir que Thread A seja interrompida após ler e antes de escrever; Thread B executa completamente| O contador final pode permanecer em 6, não 7 |
| 3 | Contador_compartilhado = 10 | Executar múltiplos ciclos rápidos, sem delay entre threads | Alguns incrementos podem ser perdidos; contador final < valor esperado teórico|

Cenário 2: Interferência por atraso artificial (k_busy_wait)

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | contador_compartilhado = 0| Aumentar o k_busy_wait() de uma thread e executar ambas| Maior chance de threads sobrepondo leitura/escrita; contador final < 2|
| 2 | contador_compartilhado = 5; Thread A entra em k_busy_wait enquanto Thread B lê o valor| Observar saída no terminal e LEDs| Contador final pode ser menor que 7, valores duplicados impressos|
| 3 | contador_compartilhado = 10; Executar 5 ciclos de cada thread com atrasos longos| Medir inconsistência entre contador e número de incrementos | Incrementos podem ser “perdidos”, resultado final menor que 20|

Cenário 3: Efeito do atraso artificial (k_busy_wait)

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | contador_compartilhado = 0; k_busy_wait aumentado para 5000 μs| Executar Thread A e B simultaneamente| Maior chance de threads sobrepondo a leitura/escrita; contador final < 2|
| 2 | contador_compartilhado = 10; Thread B inicia antes de Thread A terminar k_busy_wait | Observar saída no terminal| Contador final incorreto, duplicação de valores impressos|
| 3 | contador_compartilhado = 20; Executar 5 ciclos| Medir inconsistência entre o contador e número de ciclos | Incrementos perdidos, resultado final menor que 30|
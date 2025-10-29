#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define THREAD_STACK_SIZE 512
#define THREAD_PRIORITY 5
#define NUM_INCREMENTOS 1000  // Reduzimos pra deixar mais rápido

volatile int contador_compartilhado = 0;

void thread_func(void *arg1, void *arg2, void *arg3)
{
    for (int i = 0; i < NUM_INCREMENTOS; i++) {
        int temp = contador_compartilhado;   // Lê o valor atual
        k_busy_wait(50);                     // Espera um pouco (simula atraso)
        contador_compartilhado = temp + 1;   // Escreve de volta
    }

    printk("Thread %s terminou.\n", (char *)arg1);
}

K_THREAD_STACK_DEFINE(threadA_stack, THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(threadB_stack, THREAD_STACK_SIZE);
struct k_thread threadA_data;
struct k_thread threadB_data;

void main(void)
{
    printk("=== Exemplo de Race Condition (forçado) ===\n");

    k_thread_create(&threadA_data, threadA_stack, THREAD_STACK_SIZE,
                    thread_func, "A", NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&threadB_data, threadB_stack, THREAD_STACK_SIZE,
                    thread_func, "B", NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_join(&threadA_data, K_FOREVER);
    k_thread_join(&threadB_data, K_FOREVER);

    printk("Valor final esperado: %d\n", 2 * NUM_INCREMENTOS);
    printk("Valor final real: %d\n", contador_compartilhado);
}
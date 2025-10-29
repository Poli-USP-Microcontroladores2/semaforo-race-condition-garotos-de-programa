#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define THREAD_STACK_SIZE 512
#define THREAD_PRIORITY 5
#define NUM_INCREMENTOS 1000

volatile int contador_compartilhado = 0;
struct k_mutex contador_mutex; // Mutex para proteger o contador

void thread_func(void *arg1, void *arg2, void *arg3)
{
    for (int i = 0; i < NUM_INCREMENTOS; i++) {
        k_mutex_lock(&contador_mutex, K_FOREVER);  // Protege a seção crítica
        contador_compartilhado++;                  // Incrementa o contador
        k_mutex_unlock(&contador_mutex);           // Libera a seção crítica
        k_busy_wait(50);                           // Simula atraso
    }
    printk("Thread %s terminou.\n", (char *)arg1);
}

K_THREAD_STACK_DEFINE(threadA_stack, THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(threadB_stack, THREAD_STACK_SIZE);
struct k_thread threadA_data;
struct k_thread threadB_data;

void main(void)
{
    printk("=== Exemplo de Race Condition (corrigido) ===\n");

    k_mutex_init(&contador_mutex);  // Inicializa o mutex

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

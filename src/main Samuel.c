#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>

// --- Configuração de LEDs via DeviceTree ---
#define LED_A_NODE DT_ALIAS(led0)  // LED verde
#define LED_B_NODE DT_ALIAS(led2)  // LED vermelho

static const struct gpio_dt_spec ledA = GPIO_DT_SPEC_GET(LED_A_NODE, gpios);
static const struct gpio_dt_spec ledB = GPIO_DT_SPEC_GET(LED_B_NODE, gpios);

// --- Prioridades e tempos ---
#define PRIO_THREAD_A 5
#define PRIO_THREAD_B 5
#define TEMPO_A_MS   1000
#define TEMPO_B_MS   1000

// --- Variável compartilhada protegida por semáforo ---
int contador_compartilhado = 10;
K_SEM_DEFINE(semaforo_contador, 1, 1);  // Inicializado com 1, máximo 1

// ----------------------------------------------------
// THREAD A — Incrementa contador e acende LED verde
// ----------------------------------------------------
void thread_A(void *p1, void *p2, void *p3)
{
    while (1) {
        gpio_pin_set_dt(&ledA, 1);  // Liga LED verde

        // Seção crítica protegida pelo semáforo
        k_sem_take(&semaforo_contador, K_FOREVER);

        int temp = contador_compartilhado;  // Leitura
        temp++;                             // Modificação
        k_busy_wait(1000);                  // Atraso artificial
        contador_compartilhado = temp;      // Escrita

        k_sem_give(&semaforo_contador);     // Libera semáforo

        gpio_pin_set_dt(&ledA, 0);          // Desliga LED verde
        printk("Thread A -> contador = %d\n", contador_compartilhado);
        k_msleep(TEMPO_A_MS);
    }
}

// ----------------------------------------------------
// THREAD B — Incrementa contador e acende LED vermelho
// ----------------------------------------------------
void thread_B(void *p1, void *p2, void *p3)
{
    while (1) {
        gpio_pin_set_dt(&ledB, 1);  // Liga LED vermelho

        // Seção crítica protegida pelo semáforo
        k_sem_take(&semaforo_contador, K_FOREVER);

        int temp = contador_compartilhado;  // Leitura
        temp++;                             // Modificação
        k_busy_wait(1000);                  // Atraso artificial
        contador_compartilhado = temp;      // Escrita

        k_sem_give(&semaforo_contador);     // Libera semáforo

        gpio_pin_set_dt(&ledB, 0);          // Desliga LED vermelho
        printk("Thread B -> contador = %d\n", contador_compartilhado);
        k_msleep(TEMPO_B_MS);
    }
}

// ----------------------------------------------------
// Definição das threads
// ----------------------------------------------------
K_THREAD_DEFINE(a_tid, 512, thread_A, NULL, NULL, NULL,
                PRIO_THREAD_A, 0, 0);
K_THREAD_DEFINE(b_tid, 512, thread_B, NULL, NULL, NULL,
                PRIO_THREAD_B, 0, 0);

// ----------------------------------------------------
// Função principal
// ----------------------------------------------------
void main(void)
{
    if (!device_is_ready(ledA.port) || !device_is_ready(ledB.port)) {
        return;
    }
    gpio_pin_configure_dt(&ledA, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&ledB, GPIO_OUTPUT_INACTIVE);

    printk("=== Demonstração sem Race Condition (com semáforo + atraso) ===\n");
    printk("Duas threads incrementando o mesmo contador de forma segura\n");
}

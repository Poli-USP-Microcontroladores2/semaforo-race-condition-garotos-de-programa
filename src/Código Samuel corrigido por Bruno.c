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
#define PRIO_THREAD_A 5  // Maior prioridade (número menor)
#define PRIO_THREAD_B 5  // Mesma prioridade (para aumentar chance de corrida)
#define TEMPO_A_MS   1000
#define TEMPO_B_MS   1000

// --- Variável compartilhada (causa de race condition) ---
volatile int contador_compartilhado = 0;

// ----------------------------------------------------
// THREAD A — Incrementa contador e acende LED verde
// ----------------------------------------------------
void thread_A(void *p1, void *p2, void *p3)
{
    while (1) {
        gpio_pin_set_dt(&ledA, 1);  // Liga LED verde

        int temp = contador_compartilhado;  // Leitura
        temp++;                             // Modificação
        k_busy_wait(1000);                  // Simula atraso (aumenta chance de interferência)
        contador_compartilhado = temp;      // Escrita

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

        int temp = contador_compartilhado;  // Leitura
        temp++;                             // Modificação
        k_busy_wait(1000);                  // Simula atraso (janela crítica)
        contador_compartilhado = temp;      // Escrita

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

    printk("=== Demonstração de Race Condition ===\n");
    printk("Duas threads incrementando o mesmo contador sem sincronização\n");
}
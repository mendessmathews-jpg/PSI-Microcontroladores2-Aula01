#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/sys/printk.h>

// --- LEDs e aliases do DeviceTree ---
#define LED_A_NODE DT_ALIAS(led0)  // LED verde
#define LED_B_NODE DT_ALIAS(led1)  // LED azul

static const struct gpio_dt_spec ledA = GPIO_DT_SPEC_GET_OR(LED_A_NODE, gpios, {0});
static const struct gpio_dt_spec ledB = GPIO_DT_SPEC_GET_OR(LED_B_NODE, gpios, {0});

// --- Configuração de Prioridades e Tempos ---
#define PRIO_THREAD_A 5     // Alta prioridade
#define PRIO_THREAD_B 7     // Baixa prioridade
#define TEMPO_A_MS    1500  // Thread A dorme por 1.5s
#define TEMPO_B_MS    1000  // Thread B dorme por 1.0s
#define STACKSIZE     1024

// ----------------------------------------------------
// THREAD A — Tarefa curta, alta prioridade
// ----------------------------------------------------
void thread_A(void *p1, void *p2, void *p3)
{
    while (1) {
        uint32_t uptime_inicio = k_uptime_get_32();
        uint32_t ciclos_inicio = k_cycle_get_32();

        gpio_pin_set_dt(&ledA, 1);

        // Processamento curto (~16.6 ms)
        for (volatile int i = 0; i < 200000; i++) {}

        gpio_pin_set_dt(&ledA, 0);

        uint32_t ciclos_fim = k_cycle_get_32();
        uint32_t duracao_us = (ciclos_fim - ciclos_inicio) / 48; // 48 MHz

        printk("[Thread A @ %u ms] Executou por %u us (Alta Prioridade %d)\n", 
               uptime_inicio, duracao_us, PRIO_THREAD_A);

        k_msleep(TEMPO_A_MS);
    }
}

// ----------------------------------------------------
// THREAD B — Tarefa longa, baixa prioridade
// ----------------------------------------------------
void thread_B(void *p1, void *p2, void *p3)
{
    while (1) {
        uint32_t uptime_inicio = k_uptime_get_32();
        uint32_t ciclos_inicio = k_cycle_get_32();

        gpio_pin_set_dt(&ledB, 1);

        // Processamento longo (sujeito à preempção)
        for (volatile int i = 0; i < 10000000; i++) {
            if (i % 2500000 == 0) {
                // Mantém CPU ocupada
            }
        }

        gpio_pin_set_dt(&ledB, 0);

        uint32_t ciclos_fim = k_cycle_get_32();
        uint32_t duracao_us = (ciclos_fim - ciclos_inicio) / 48; // 48 MHz

        printk("[Thread B @ %u ms] Executou por %u us (%u ms) (Baixa Prioridade %d)\n", 
               uptime_inicio, duracao_us, duracao_us / 1000, PRIO_THREAD_B);

        k_msleep(TEMPO_B_MS);
    }
}

// Registra as threads no kernel do Zephyr
K_THREAD_DEFINE(a_tid, STACKSIZE, thread_A, NULL, NULL, NULL, PRIO_THREAD_A, 0, 0);
K_THREAD_DEFINE(b_tid, STACKSIZE, thread_B, NULL, NULL, NULL, PRIO_THREAD_B, 0, 0);

void main(void)
{
    printk("=== Questao 1: Medicao com SysTick, Clocks e Preempcao ===\n");

    if (gpio_is_ready_dt(&ledA)) gpio_pin_configure_dt(&ledA, GPIO_OUTPUT_INACTIVE);
    if (gpio_is_ready_dt(&ledB)) gpio_pin_configure_dt(&ledB, GPIO_OUTPUT_INACTIVE);

    // Leitura e validação do Clock do Sistema (Core Clock)
    const struct device *clk = DEVICE_DT_GET(DT_NODELABEL(sim));
    if (device_is_ready(clk)) {
        uint32_t freq = 0;
        clock_control_get_rate(clk, 0, &freq);
        uint32_t div_core = (SIM->CLKDIV1 >> 28) & 0x0F;
        uint32_t core_clk = freq / (div_core + 1);
        printk("Frequencia do Core Clock: %u Hz\n", core_clk);
    }

    while (1) {
        k_sleep(K_FOREVER);
    }
}
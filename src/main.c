#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

// --- Configuração dos LEDs pelo DeviceTree ---
#define LED_A_NODE DT_ALIAS(led0)  // LED verde (GPIOB)
#define LED_B_NODE DT_ALIAS(led1)  // LED azul (GPIOD)

static const struct gpio_dt_spec ledA = GPIO_DT_SPEC_GET_OR(LED_A_NODE, gpios, {0});
static const struct gpio_dt_spec ledB = GPIO_DT_SPEC_GET_OR(LED_B_NODE, gpios, {0});

// --- Configuração do Botão Oficial (PTA16) ---
#define BUTTON_NODE DT_NODELABEL(user_button_0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

static struct gpio_callback button_cb_data;

#define PRIO_THREAD_A 5
#define PRIO_THREAD_B 7
#define STACKSIZE     1024

// ----------------------------------------------------
// ROTINA DE SERVIÇO DE INTERRUPÇÃO (ISR DO BOTÃO / JUMPER)
// ----------------------------------------------------
void button_pressed_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    uint32_t tempo_isr = k_uptime_get_32();
    printk("\n==========================================================\n");
    printk("[ISR @ %u ms] *** HARDWARE INTERRUPT DETECTADA NO PTA16! ***\n", tempo_isr);
    printk("==========================================================\n\n");
}

// ----------------------------------------------------
// THREAD A — Alta Prioridade
// ----------------------------------------------------
void thread_A(void *p1, void *p2, void *p3)
{
    while (1) {
        uint32_t uptime_inicio = k_uptime_get_32();
        uint32_t ciclos_inicio = k_cycle_get_32();

        if (gpio_is_ready_dt(&ledA)) {
            gpio_pin_set_dt(&ledA, 1);
            for (volatile int i = 0; i < 200000; i++) {}
            gpio_pin_set_dt(&ledA, 0);
        }

        uint32_t ciclos_fim = k_cycle_get_32();
        uint32_t duracao_us = (ciclos_fim - ciclos_inicio) / 48;

        printk("[Thread A @ %u ms] Executou por %u us (Alta Prioridade %d)\n", 
               uptime_inicio, duracao_us, PRIO_THREAD_A);

        k_msleep(1500);
    }
}

// ----------------------------------------------------
// THREAD B — Baixa Prioridade
// ----------------------------------------------------
void thread_B(void *p1, void *p2, void *p3)
{
    while (1) {
        uint32_t uptime_inicio = k_uptime_get_32();
        uint32_t ciclos_inicio = k_cycle_get_32();

        if (gpio_is_ready_dt(&ledB)) {
            gpio_pin_set_dt(&ledB, 1);
            for (volatile int i = 0; i < 10000000; i++) {}
            gpio_pin_set_dt(&ledB, 0);
        }

        uint32_t ciclos_fim = k_cycle_get_32();
        uint32_t duracao_us = (ciclos_fim - ciclos_inicio) / 48;

        printk("[Thread B @ %u ms] Executou por %u us (%u ms) (Baixa Prioridade %d)\n", 
               uptime_inicio, duracao_us, duracao_us / 1000, PRIO_THREAD_B);

        k_msleep(1000);
    }
}

K_THREAD_DEFINE(a_tid, STACKSIZE, thread_A, NULL, NULL, NULL, PRIO_THREAD_A, 0, 0);
K_THREAD_DEFINE(b_tid, STACKSIZE, thread_B, NULL, NULL, NULL, PRIO_THREAD_B, 0, 0);

int main(void)
{
    printk("\n=== Questao 4/5: Validacao ISR com SW0 (PTA16) ===\n");

    // Inicialização dos LEDs
    if (gpio_is_ready_dt(&ledA)) gpio_pin_configure_dt(&ledA, GPIO_OUTPUT_INACTIVE);
    if (gpio_is_ready_dt(&ledB)) gpio_pin_configure_dt(&ledB, GPIO_OUTPUT_INACTIVE);

    // Configuração do pino do botão PTA16 com Pull-Up
    if (gpio_is_ready_dt(&button)) {
        gpio_pin_configure_dt(&button, GPIO_INPUT | GPIO_PULL_UP);

        // Interrupção por borda de descida (quando encosta no GND)
        gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_FALLING);

        gpio_init_callback(&button_cb_data, button_pressed_isr, BIT(button.pin));
        gpio_add_callback(button.port, &button_cb_data);

        printk("Botao configurado: Porto %s, Pino %d (COM PULL-UP)\n", button.port->name, button.pin);
    } else {
        printk("ERRO: Botao nao esta disponivel!\n");
    }

    while (1) {
        k_sleep(K_FOREVER);
    }

    return 0;
}
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define TEMPO_VERDE_MS 1000
#define TEMPO_AZUL_MS  350

#define LED0_NODE DT_ALIAS(led0) // verde
#define LED1_NODE DT_ALIAS(led1) // azul

// Verifica se o LED verde está definido no Device Tree
#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
static const struct gpio_dt_spec led_verde = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
#else
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

// Verifica se o LED azul está definido no Device Tree
#if DT_NODE_HAS_STATUS(LED1_NODE, okay)
static const struct gpio_dt_spec led_azul = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
#else
#error "Unsupported board: led1 devicetree alias is not defined"
#endif

void main(void)
{
    int ret;

    // Verifica se ambos os pinos estão prontos para uso
    if (!gpio_is_ready_dt(&led_verde)) {
        return;
    }
    if (!gpio_is_ready_dt(&led_azul)) {
        return;
    }

    // Configura o pino do led verde como saída (inicia ativo)
    ret = gpio_pin_configure_dt(&led_verde, GPIO_OUTPUT_ACTIVE);
    
    // Configura o pino do led azul como saída (inicia inativo)
    ret = gpio_pin_configure_dt(&led_azul, GPIO_OUTPUT_INACTIVE);

    // Marca o tempo inicial do sistema em (ms)
    int64_t tempo_verde = k_uptime_get();
    int64_t tempo_azul  = k_uptime_get();

while (1) {
    int64_t agora = k_uptime_get();
    
    //Verifica e inverte o estado do led verde a cada (1000 ms)
    if (agora - tempo_verde >= TEMPO_VERDE_MS) {
        gpio_pin_toggle_dt(&led_verde);
        tempo_verde = agora; //atualiza o marcador de tempo do led verde
    }

    if (agora - tempo_azul >= TEMPO_AZUL_MS) {
        gpio_pin_toggle_dt(&led_azul);
        tempo_azul = agora; //atualiza o marcador de tempo do led azul
    }

    //pausa de 10 ms para o processador sem perder precisão
    k_msleep(10);

}
}
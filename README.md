# PSI Microcontroladores 2 - Aula 01

Implementação de firmware não-bloqueante para FRDM-KL25Z utilizando Zephyr RTOS e `k_uptime_get()`.

---

## Planeamento de Testes (Validação)

| ID | Teste | Procedimento | Resultado Esperado |
| :--- | :--- | :--- | :--- |
| **TC-01** | **LED Verde (1000 ms)** | Observar a comutação do LED Verde na placa FRDM-KL25Z. | O LED Verde alterna de estado a cada 1000 ms (1 segundo). | |
| **TC-02** | **LED Azul (350 ms)** | Observar a comutação do LED Azul na placa FRDM-KL25Z. | O LED Azul alterna de estado a cada 350 ms (desafio). |
| **TC-03** | **Execução Simultânea** | Observar o comportamento conjunto dos LEDs. | Operação não-bloqueante; na sobreposição acesa, surge a cor Ciano. |
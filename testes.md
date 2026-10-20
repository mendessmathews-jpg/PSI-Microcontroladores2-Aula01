# Planejamento de Testes para validação do código

O código opera acendendo e apagando dois LEDs por meio de threads independentes no Zephyr RTOS, portanto a forma prática e estruturada de validar seu funcionamento é:

- [ ] **Teste de Isolamento (LED Verde):** Comente temporariamente a chamada ou o loop da `thread_led_azul_entry` para verificar se a `thread_led_verde_entry` executa de forma estável. Dessa forma, é possível checar o funcionamento isolado do LED verde e validar se o intervalo entre as alternâncias permanece em 1000 ms (1 s).
- [ ] **Teste de Isolamento (LED Azul):** Restaure o LED verde, desative temporariamente a execução do LED verde e repita a verificação para o LED azul. Deve-se observar uma frequência visivelmente mais rápida no piscar (350 ms).
- [ ] **Teste Concorrente (Ambos Ativos):** Ative ambas as threads e observe a execução simultânea. Como 350 ms não é múltiplo exato do ciclo completo do LED verde, os LEDs piscarão em ritmos desalinhados sem travar a execução, demonstrando o escalonamento multithread do RTOS sem interferência mútua.
- [ ] **Estabilidade ao Longo do Tempo:** Mantenha o sistema rodando continuamente para garantir que não ocorra estouro de pilha (*stack overflow*) nem alteração no tempo de alternância do `k_msleep`.
- [ ] **OPCIONAL:** Alterar o parâmetro `SLEEP_TIME_AZUL_MS` para outro tempo de repouso e verificar se o agendador responde corretamente à mudança de temporização.
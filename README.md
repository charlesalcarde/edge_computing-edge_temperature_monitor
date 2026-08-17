Monitoramento de Temperatura — Experimento de Computação de Borda
1. Visão geral
Este projeto apresenta um experimento de aquisição, processamento, visualização e supervisão de temperatura utilizando um Arduino Uno, um Multi Function Shield, um sensor digital de temperatura DS18B20 e uma aplicação desenvolvida em Python.
O desenvolvimento foi realizado de forma incremental e dividido em duas partes principais:
Parte 1 — Processamento centralizado: o Arduino realiza a aquisição da temperatura e transmite os dados ao computador, onde são executados os cálculos, a análise e a visualização.
Parte 2 — Processamento na borda: os principais cálculos e decisões passam a ser realizados no próprio Arduino, enquanto o Python assume principalmente as funções de supervisão, visualização e persistência.
Essa evolução permite comparar, em um mesmo experimento, uma arquitetura tradicional de aquisição com uma arquitetura baseada em Edge Computing (Computação de Borda).
---
2. Objetivos
O experimento tem como objetivos:
realizar a leitura de temperatura com o sensor DS18B20;
utilizar o Arduino Uno como dispositivo de aquisição;
transmitir medições e informações processadas pela comunicação serial;
visualizar a temperatura em tempo real;
calcular indicadores estatísticos;
identificar tendências térmicas;
detectar eventos rápidos de subida e queda de temperatura;
registrar os dados coletados em arquivo CSV;
migrar progressivamente o processamento do computador para o microcontrolador;
desenvolver uma interface homem-máquina local no nó de borda;
estudar autonomia, latência, processamento local e comunicação em sistemas de Edge Computing.
---
Parte 1 — Processamento centralizado
3. Arquitetura inicial
Na primeira fase, o Arduino é responsável principalmente pela aquisição da temperatura. O processamento é realizado pelo computador.
```text
DS18B20
   |
   v
Arduino Uno
   |
   | USB / Serial
   v
Python
   |
   +-- Processamento
   +-- Visualização
   +-- Persistência
```
Nesta arquitetura, o Arduino envia os dados de temperatura e o computador executa os cálculos e a análise.
---
4. Hardware utilizado
O experimento utiliza:
Arduino Uno;
Multi Function Shield para Arduino;
sensor de temperatura DS18B20;
cabo USB com transmissão de dados;
computador para aquisição, supervisão e armazenamento.
Arduino Uno
<p align="center">
  <img src="docs/images/arduino_uno.jpg"
       alt="Arduino Uno utilizado no experimento"
       width="450">
</p>
Multi Function Shield
<p align="center">
  <img src="docs/images/arduino-multifunction-shield-pinout.jpg"
       alt="Multi Function Shield utilizado no experimento"
       width="450">
</p>
O sensor DS18B20 é conectado ao Multi Function Shield utilizando a entrada correspondente ao pino A4 do Arduino Uno.
---
5. Software utilizado
Arduino
Arduino IDE;
biblioteca `MultiFuncShield`;
biblioteca `OneWire`;
biblioteca `DallasTemperature`;
biblioteca `TimerOne`.
Python
Python;
PySerial;
Matplotlib;
NumPy.
As dependências podem ser instaladas com:
```bash
python -m pip install pyserial matplotlib numpy
```
---
6. Aquisição da temperatura
O Arduino realiza a leitura do sensor DS18B20 e transmite os valores pela porta serial.
A comunicação é realizada a:
```text
9600 baud
```
As medições são atualizadas aproximadamente a cada:
```text
1 segundo
```
---
7. Monitoramento em Python
Na Parte 1, a aplicação Python recebe os dados brutos do Arduino e realiza os cálculos necessários ao monitoramento.
O gráfico apresenta uma janela móvel correspondente aos últimos 60 minutos de aquisição. Os dados mais antigos deixam de ser exibidos para que a visualização permaneça legível durante experimentos prolongados.
São apresentados:
temperatura instantânea;
média móvel de 30 minutos;
média total;
máxima;
mínima;
amplitude térmica;
tendência térmica;
tempo de aquisição.
---
8. Média móvel de 30 minutos
Na primeira implementação, a média móvel de 30 minutos é calculada no computador.
Durante os primeiros 30 minutos, são utilizadas todas as amostras disponíveis. Após esse período, a média passa a representar efetivamente os últimos 30 minutos.
---
9. Média total
A média total considera todo o período desde o início da aquisição.
Ela pode ser calculada sem manter todas as amostras em memória, utilizando:
```text
Média total = soma acumulada das temperaturas / número total de amostras
```
---
10. Máxima, mínima e amplitude térmica
Durante a execução são registradas as temperaturas máxima e mínima do experimento.
A amplitude térmica é calculada por:
```text
Amplitude = Temperatura máxima - Temperatura mínima
```
---
11. Tendência térmica
Na arquitetura centralizada, o Python calcula a tendência de variação da temperatura utilizando uma janela de aproximadamente 5 minutos.
O resultado é expresso em:
```text
°C/min
```
A tendência é classificada como:
```text
SUBINDO
CAINDO
ESTAVEL
```
---
12. Visualização da Parte 1
<p align="center">
  <img src="docs/images/monitoramento-temperatura.png"
       alt="Monitoramento de temperatura em tempo real utilizando Arduino Uno e Python"
       width="850">
</p>
<p align="center">
  <em>Figura 1 — Primeira fase do experimento, com processamento realizado predominantemente no computador.</em>
</p>
---
Parte 2 — Processamento na borda
13. Migração do processamento para o Arduino
Na segunda fase, o Arduino deixa de atuar apenas como dispositivo de aquisição e passa a executar localmente os principais cálculos e decisões do sistema.
A arquitetura passa a ser:
```text
DS18B20
   |
   v
Arduino Uno / EDGE
   |
   +-- aquisição
   +-- média móvel
   +-- média total
   +-- máxima e mínima
   +-- amplitude
   +-- tendência
   +-- detecção de eventos
   +-- classificação térmica
   +-- interface local
   +-- alerta sonoro
   |
   | Serial
   v
Python
   |
   +-- supervisão
   +-- visualização
   +-- histórico de eventos
   +-- CSV
```
O computador passa a receber informação já processada na borda.
---
14. Limitações de memória e estratégia de agregação
O Arduino Uno possui apenas 2 kB de SRAM. Uma aquisição a cada segundo durante 30 minutos produziria:
```text
30 × 60 = 1.800 amostras
```
Se cada amostra fosse armazenada como um `float` de 4 bytes:
```text
1.800 × 4 = 7.200 bytes
```
Isso ultrapassa a memória disponível.
A solução adotada foi utilizar agregações temporais por minuto, organizadas em um buffer circular de 30 posições.
```text
leituras de 1 segundo
        |
        v
agregação por minuto
        |
        v
buffer circular de 30 minutos
        |
        v
média móvel de 30 minutos
```
Essa estratégia permite executar a média móvel diretamente no Arduino com baixo consumo de memória.
---
15. Média móvel de 30 minutos na borda
A média móvel de 30 minutos passou a ser calculada no Arduino.
O microcontrolador mantém, para cada minuto:
soma das temperaturas;
quantidade de amostras.
Quando o buffer circular avança, o minuto mais antigo é removido da janela e substituído pelas novas medições.
O Python apenas recebe o valor de `MEDIA30` já calculado.
---
16. Média total, máxima, mínima e amplitude na borda
O Arduino calcula continuamente:
```text
MEDIATOTAL
MAX
MIN
AMPL
```
A média total utiliza soma acumulada e contagem de amostras. Máxima e mínima são atualizadas a cada nova leitura e a amplitude é derivada diretamente desses valores.
---
17. Tendência térmica de 5 minutos
O Arduino calcula também uma tendência térmica de curto/médio prazo, expressa em:
```text
°C/min
```
A variável é transmitida como:
```text
TREND5
```
O comportamento é classificado como:
```text
SUBINDO
CAINDO
ESTAVEL
```
Durante os primeiros cinco minutos, enquanto ainda não há histórico suficiente para uma comparação completa, o sistema utiliza:
```text
AGUARDANDO
```
Isso evita classificar prematuramente a tendência como estável.
---
18. Detecção de eventos rápidos
O Arduino avalia a diferença entre leituras consecutivas:
```text
DELTA = temperatura atual - temperatura anterior
```
Também calcula uma taxa equivalente de variação rápida:
```text
RAPIDA = °C/min
```
O evento é classificado como:
```text
NORMAL
SUBIDA_RAPIDA
QUEDA_RAPIDA
```
A decisão é executada diretamente no microcontrolador.
---
19. Estado térmico
Além do evento instantâneo, o sistema combina a ocorrência rápida com a tendência de 5 minutos para produzir uma interpretação de mais alto nível.
Os estados possíveis são:
```text
SUBIDA_RAPIDA
QUEDA_RAPIDA
AQUECIMENTO
RESFRIAMENTO
ESTAVEL
```
Isso permite distinguir, por exemplo, uma subida rápida pontual de uma tendência sustentada de aquecimento.
---
20. Limiar ajustável de eventos rápidos
O limiar de detecção deixou de ser um valor fixo no código.
O trimpot do Multi Function Shield, conectado ao pino analógico A0, passou a controlar a sensibilidade do detector.
A faixa de ajuste utilizada é aproximadamente:
```text
0,10 °C a 1,00 °C
```
Exemplo:
```text
limiar baixo  -> maior sensibilidade
limiar alto   -> menor sensibilidade
```
O Arduino realiza uma leitura inicial do trimpot durante o `setup()`. Assim, o sistema já inicia utilizando a posição física atual do controle, sem depender de um valor fixo anterior.
O valor é transmitido ao computador como:
```text
LIMIAR
```
---
21. Interface homem-máquina local
Uma evolução importante da Parte 2 foi a criação de uma interface homem-máquina local utilizando os recursos do próprio Multi Function Shield.
O objetivo é permitir que o nó de borda seja observado e configurado mesmo sem depender do computador.
Botão S1 — seleção do display
Um toque curto em S1 alterna ciclicamente entre:
```text
TEMP -> MEDIA30 -> TREND5 -> TEMP ...
```
O display de quatro dígitos apresenta o valor correspondente ao modo selecionado.
LEDs indicadores
Os LEDs funcionam como legenda do display:
```text
LED 1 -> temperatura instantânea
LED 2 -> média móvel de 30 minutos
LED 3 -> tendência de 5 minutos
```
Isso elimina a ambiguidade de interpretar um número isolado no display.
Botão S2 — ajuste do limiar
Um pressionamento longo de S2 entra ou sai do modo de configuração do limiar.
No modo de ajuste:
o display mostra o valor do limiar;
o trimpot altera o limiar em tempo real;
os quatro LEDs piscam simultaneamente para indicar que o sistema está em modo de configuração.
Ao sair, o display e os LEDs retornam ao modo anteriormente selecionado.
---
22. Alertas sonoros locais
O buzzer do Multi Function Shield passou a sinalizar eventos rápidos diretamente no hardware.
A codificação utilizada é:
```text
SUBIDA_RAPIDA -> 1 bip
QUEDA_RAPIDA  -> 2 bips
```
O alerta é produzido no próprio Arduino, portanto independe da aplicação Python.
Esse comportamento reforça a autonomia do nó de borda: o dispositivo não apenas mede e processa, mas também reage localmente a uma condição relevante.
---
23. Telemetria produzida pela borda
A comunicação serial passou a transportar uma mensagem estruturada contendo os resultados do processamento local.
Exemplo conceitual:
```text
TEMP:30.6,
MEDIA30:30.46,
MEDIATOTAL:30.46,
MAX:30.8,
MIN:29.0,
AMPL:1.8,
TREND5:0.148,
ESTADO_TREND:SUBINDO,
DELTA:0.00,
RAPIDA:0.00,
EVENTO:NORMAL,
ESTADO_TERMICO:AQUECIMENTO,
LIMIAR:0.25
```
O computador deixa de receber somente um dado bruto e passa a receber informação interpretada na borda.
---
24. Nova função da aplicação Python
Na Parte 2, o Python assume principalmente o papel de supervisório.
Suas funções principais são:
receber a telemetria produzida pelo Arduino;
exibir temperatura e média móvel em tempo real;
apresentar os indicadores atuais;
registrar eventos históricos;
armazenar os dados em CSV.
Os principais cálculos deixaram de ser refeitos no computador.
---
25. Painel de supervisão
O painel gráfico apresenta:
temperatura atual;
média móvel de 30 minutos;
média total;
máxima;
mínima;
amplitude;
tendência de 5 minutos;
estado da tendência;
delta entre leituras;
variação rápida;
limiar de evento configurado no trimpot;
evento atual;
estado térmico;
porta serial utilizada;
horário da última atualização;
intervalo aproximado de aquisição.
A legenda e o painel lateral permitem separar claramente dados instantâneos, indicadores calculados e estados produzidos pelo sistema de borda.
---
26. Registro histórico dos eventos no gráfico
Quando o Arduino detecta:
```text
SUBIDA_RAPIDA
```
ou:
```text
QUEDA_RAPIDA
```
o Python registra o instante da ocorrência e mantém uma marca visual no gráfico.
Os eventos são apresentados com linhas verticais e os rótulos:
```text
SUBIDA
QUEDA
```
Assim, mesmo depois que o campo `EVENTO` retorna para `NORMAL`, a ocorrência continua visível no histórico da janela de 60 minutos.
---
27. Persistência em arquivo CSV
A persistência de dados foi efetivamente implementada na Parte 2.
A cada sessão, o Python cria automaticamente um arquivo com nome semelhante a:
```text
edge_temperatura_20260817_075000.csv
```
Cada linha pode registrar:
```text
Data
Hora
Temperatura
Media 30 min
Media total
Maxima
Minima
Amplitude
Trend5
Estado trend
Delta
Variacao rapida
Limiar evento
Evento
Estado termico
```
O registro do limiar é especialmente útil porque permite saber qual sensibilidade estava configurada no instante em que determinado evento ocorreu.
---
28. Redução da latência entre Arduino e Python
Durante os testes foi observado um atraso progressivo entre a ocorrência de um evento no Arduino e sua representação no gráfico.
O Arduino sinalizava imediatamente o evento pelo buzzer, enquanto o Python podia apresentar vários segundos de atraso.
A causa identificada foi o acúmulo de mensagens no buffer serial: a aplicação consumia apenas uma linha a cada atualização gráfica.
A lógica foi modificada para:
```text
ler todas as linhas disponíveis
        |
        +-- registrar todas no CSV
        +-- preservar todos os eventos
        |
        v
usar o estado mais recente para atualizar a tela
```
Após a alteração, o gráfico passou a responder praticamente em sincronismo com o evento detectado no Arduino.
---
29. Autonomia do nó de borda
A Parte 2 demonstra que o Arduino é capaz de operar como um pequeno nó de Edge Computing.
O dispositivo realiza localmente:
```text
aquisição
processamento
agregação
classificação
ajuste de sensibilidade
detecção de eventos
feedback visual
alerta sonoro
interação com o operador
```
O computador deixa de participar diretamente da tomada de decisão e passa a atuar como supervisório e repositório dos dados.
---
30. Comparação das duas arquiteturas
Parte 1 — Processamento centralizado
```text
Sensor
   |
   v
Arduino
   |
   | dados brutos
   v
Computador
   |
   +-- processamento
   +-- análise
   +-- visualização
```
Parte 2 — Processamento na borda
```text
Sensor
   |
   v
Arduino / EDGE
   |
   +-- processamento
   +-- análise
   +-- decisão
   +-- IHM
   +-- alerta
   |
   | informação processada
   v
Computador
   |
   +-- supervisão
   +-- visualização
   +-- armazenamento
```
A comparação permite estudar experimentalmente aspectos como:
latência;
volume de dados transmitidos;
utilização de memória;
tempo de resposta;
autonomia do dispositivo;
distribuição da carga computacional;
capacidade de operação independente;
eficiência da comunicação.
---
31. Evolução experimental
O desenvolvimento pode ser resumido pelas seguintes etapas:
Etapa 1 — Aquisição
```text
DS18B20 -> Arduino -> Monitor Serial
```
Etapa 2 — Visualização centralizada
```text
DS18B20 -> Arduino -> Serial -> Python -> Gráfico
```
Etapa 3 — Processamento centralizado
```text
Arduino -> Python
             |
             +-- média móvel
             +-- média total
             +-- máxima e mínima
             +-- amplitude
             +-- tendência
```
Etapa 4 — Processamento na borda
```text
DS18B20
   |
   v
Arduino / EDGE
   |
   +-- estatísticas
   +-- tendência
   +-- eventos
   +-- estado térmico
```
Etapa 5 — Interface local
```text
S1 + S2 + LEDs + display + trimpot + buzzer
```
Etapa 6 — Supervisão e persistência
```text
Arduino / EDGE -> Python -> gráfico + eventos + CSV
```
---
32. Próximas etapas
As próximas evoluções possíveis incluem:
comunicação sem fio utilizando ESP32;
uso do ESP32 como gateway entre Arduino e rede Wi-Fi;
comunicação TCP/IP;
utilização de MQTT;
dashboards acessíveis pela rede;
integração com banco de dados;
utilização de múltiplos nós de borda;
comparação quantitativa entre comunicação serial e comunicação em rede;
avaliação de latência e volume de dados transmitidos;
eventual migração integral da aplicação para o ESP32.
Uma possível arquitetura futura é:
```text
DS18B20
   |
   v
Arduino / EDGE
   |
   | UART
   v
ESP32 / Gateway
   |
   | Wi-Fi
   v
Rede
   |
   +-- Python
   +-- MQTT
   +-- Dashboard
   +-- Banco de dados
```
---
33. Objetivo acadêmico
O experimento busca demonstrar de forma prática e incremental a evolução de um sistema tradicional de aquisição de dados para uma arquitetura baseada em Computação de Borda.
A temperatura foi escolhida como variável experimental por permitir uma implementação simples, observável e controlável. Entretanto, os conceitos desenvolvidos podem ser aplicados a sistemas com múltiplos sensores, Internet das Coisas, monitoramento ambiental, automação, sistemas distribuídos e aplicações industriais.
A Parte 2 é especialmente relevante porque demonstra que o nó de borda pode não apenas adquirir dados, mas também processar, interpretar, decidir, sinalizar e interagir localmente com o operador.
---
Autor
Charles Cavalcante Alcarde
Projeto desenvolvido no contexto de estudos e experimentos em Computação de Borda (Edge Computing).
2026

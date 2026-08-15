# Monitoramento de Temperatura --- Experimento de Computação de Borda

## 1. Visão geral

Este projeto apresenta um experimento de aquisição, processamento e
visualização de temperatura utilizando um **Arduino Uno**, um **Multi
Function Shield**, um sensor digital de temperatura **DS18B20** e uma
aplicação desenvolvida em **Python**.

O projeto foi desenvolvido de forma incremental, iniciando pela
aquisição simples da temperatura no Arduino e evoluindo para um sistema
de monitoramento em tempo real com processamento estatístico, análise de
tendência e armazenamento dos dados.

Esta primeira etapa utiliza o computador para realizar o processamento
dos dados recebidos do Arduino.

Em uma etapa posterior, parte desse processamento será migrada para o
próprio microcontrolador, permitindo estudar experimentalmente os
conceitos de **Edge Computing (Computação de Borda)**.

------------------------------------------------------------------------

## 2. Objetivos

O experimento tem como objetivos:

-   realizar a leitura de temperatura com o sensor DS18B20;
-   utilizar o Arduino Uno como dispositivo de aquisição;
-   transmitir as medições através da comunicação serial;
-   receber os dados em uma aplicação Python;
-   visualizar a temperatura em tempo real;
-   calcular indicadores estatísticos;
-   identificar tendências térmicas;
-   registrar os dados coletados;
-   estudar a migração do processamento para a borda.

------------------------------------------------------------------------

## 3. Arquitetura inicial

Na primeira fase do experimento, o Arduino é responsável principalmente
pela aquisição da temperatura.

O processamento é realizado pelo computador.

``` text
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

Nesta arquitetura, o Arduino envia os dados de temperatura e o
computador executa os cálculos e a análise.

------------------------------------------------------------------------

## 4. Hardware utilizado

-   Arduino Uno
-   Multi Function Shield para Arduino
-   Sensor de temperatura DS18B20
-   Cabo USB com suporte à transmissão de dados
-   Computador para aquisição e processamento

O DS18B20 está conectado ao Multi Function Shield, utilizando a entrada
correspondente ao pino analógico **A4** do Arduino Uno.

------------------------------------------------------------------------

## 5. Software utilizado

### Arduino

-   Arduino IDE
-   Biblioteca `MultiFuncShield`
-   Biblioteca `OneWire`
-   Biblioteca `DallasTemperature`

### Python

O software de monitoramento utiliza:

-   Python
-   PySerial
-   Matplotlib
-   NumPy

As dependências podem ser instaladas com:

``` bash
python -m pip install pyserial matplotlib numpy
```

------------------------------------------------------------------------

## 6. Aquisição da temperatura

O Arduino realiza a leitura do sensor DS18B20 e transmite o valor
através da porta serial.

Exemplo:

``` text
Temperatura: 27.8 C
```

A taxa utilizada no experimento é:

``` text
9600 baud
```

As medições são atualizadas aproximadamente a cada:

``` text
1 segundo
```

------------------------------------------------------------------------

## 7. Monitoramento em Python

A aplicação Python recebe continuamente os dados enviados pelo Arduino
através da porta serial.

O gráfico apresenta uma janela móvel correspondente aos **últimos 60
minutos de aquisição**.

Os dados mais antigos deixam de ser exibidos no gráfico, permitindo que
a visualização permaneça legível mesmo durante experimentos de longa
duração.

------------------------------------------------------------------------

## 8. Temperatura instantânea

A temperatura recebida diretamente do Arduino é apresentada como uma
curva temporal.

O gráfico utiliza:

``` text
Temperatura (°C) × Horário
```

Os marcadores individuais das amostras foram removidos para melhorar a
visualização durante aquisições longas.

------------------------------------------------------------------------

## 9. Média móvel de 30 minutos

Além da temperatura instantânea, o programa calcula uma média móvel
correspondente aos últimos **30 minutos**.

A média móvel permite reduzir visualmente pequenas oscilações do sensor
e observar com maior clareza a tendência térmica do ambiente.

Durante os primeiros 30 minutos do experimento, a média é calculada
utilizando todas as amostras disponíveis até aquele instante.

Após esse período, passa a representar efetivamente uma janela móvel de
30 minutos.

------------------------------------------------------------------------

## 10. Média total

O sistema calcula também a temperatura média desde o início do
experimento.

Diferentemente da média móvel, esse valor considera todo o período de
aquisição.

Para evitar a necessidade de manter todas as amostras em memória, são
armazenados:

``` text
soma acumulada das temperaturas
número total de amostras
```

e calculado:

``` text
Média total = soma das temperaturas / número de amostras
```

------------------------------------------------------------------------

## 11. Temperaturas máxima e mínima

Durante a execução são registradas:

-   temperatura máxima;
-   horário da ocorrência da máxima;
-   temperatura mínima;
-   horário da ocorrência da mínima.

Esses valores correspondem ao experimento completo e não apenas à janela
de 60 minutos apresentada no gráfico.

------------------------------------------------------------------------

## 12. Amplitude térmica

A amplitude térmica é calculada por:

``` text
Amplitude = Temperatura máxima - Temperatura mínima
```

Esse indicador permite observar a faixa total de variação térmica
registrada durante o experimento.

------------------------------------------------------------------------

## 13. Tendência térmica

O programa calcula também a tendência de variação da temperatura.

Para reduzir a influência de pequenas oscilações do sensor, é realizada
uma regressão linear utilizando as amostras dos últimos **5 minutos**.

O resultado é expresso em:

``` text
°C/min
```

Exemplo:

``` text
Tendência: Subindo (+0.087 °C/min)
```

O sistema classifica o comportamento como:

``` text
Subindo
Caindo
Estável
```

------------------------------------------------------------------------

## 14. Tempo de aquisição

O programa registra continuamente o tempo decorrido desde o início do
experimento.

Exemplo:

``` text
Tempo de aquisição: 01:27:43
```

Isso permite acompanhar a duração total de cada sessão de monitoramento.

------------------------------------------------------------------------

## 15. Informações apresentadas

Durante a execução, o sistema pode apresentar:

``` text
Temperatura atual
Média móvel de 30 minutos
Média total
Temperatura máxima
Horário da máxima
Temperatura mínima
Horário da mínima
Amplitude térmica
Tendência em °C/min
Estado da tendência
Tempo de aquisição
```

------------------------------------------------------------------------

## 16. Registro dos dados

As medições podem ser armazenadas em arquivo CSV para posterior análise.

Cada registro pode conter:

``` text
Data
Hora
Temperatura
Média móvel de 30 minutos
Média total
Máxima
Mínima
Amplitude
Tendência
Estado da tendência
Tempo de aquisição
```

O arquivo permite posteriormente realizar análises utilizando
ferramentas como:

-   Microsoft Excel;
-   Python/Pandas;
-   ferramentas estatísticas;
-   plataformas de visualização de dados.

------------------------------------------------------------------------

## 17. Evolução do experimento

O desenvolvimento foi realizado incrementalmente.

### Etapa 1 --- Aquisição

``` text
DS18B20 → Arduino → Monitor Serial
```

### Etapa 2 --- Visualização

``` text
DS18B20 → Arduino → Serial → Python → Gráfico
```

### Etapa 3 --- Processamento

Foram adicionados:

``` text
Média móvel
Média total
Máxima
Mínima
Amplitude
Tendência
Tempo de aquisição
```

### Etapa 4 --- Persistência

``` text
Aquisição → Processamento → Registro dos dados
```

------------------------------------------------------------------------

## 18. Próxima fase --- Edge Computing

A arquitetura atual pode ser representada de forma simplificada por:

``` text
SENSOR
   |
   v
ARDUINO
   |
   | dados
   v
COMPUTADOR
   |
   +-- média móvel
   +-- máxima
   +-- mínima
   +-- amplitude
   +-- tendência
   +-- armazenamento
```

A próxima etapa do projeto será migrar progressivamente essas operações
para o Arduino.

A arquitetura passará a ser:

``` text
SENSOR
   |
   v
ARDUINO / EDGE
   |
   +-- aquisição
   +-- filtragem
   +-- média
   +-- máxima
   +-- mínima
   +-- tendência
   +-- detecção de eventos
   |
   v
COMPUTADOR
   |
   +-- visualização
   +-- armazenamento
```

------------------------------------------------------------------------

## 19. Limitações do Arduino Uno

O Arduino Uno possui recursos computacionais limitados, particularmente
em relação à memória RAM.

Uma aquisição realizada a cada segundo durante 30 minutos produziria:

``` text
30 × 60 = 1.800 amostras
```

Se cada amostra fosse armazenada como um `float` de 4 bytes:

``` text
1.800 × 4 = 7.200 bytes
```

Esse valor é superior à memória SRAM disponível no Arduino Uno.

Uma possível estratégia para processamento na borda será realizar
agregações temporais.

Por exemplo:

``` text
60 leituras de 1 segundo
          |
          v
   média de 1 minuto
          |
          v
buffer circular de 30 valores
          |
          v
média móvel de 30 minutos
```

Nesse caso, seriam necessários apenas:

``` text
30 × 4 = 120 bytes
```

para armazenar as médias utilizadas nesse cálculo.

------------------------------------------------------------------------

## 20. Detecção de eventos na borda

Uma evolução futura será permitir que o próprio dispositivo identifique
eventos relevantes.

Exemplos:

``` text
aumento rápido de temperatura
queda rápida de temperatura
temperatura acima de um limite
temperatura abaixo de um limite
mudança significativa de tendência
anomalia térmica
```

Isso permitirá experimentar uma arquitetura em que nem todas as amostras
precisam ser transmitidas.

``` text
Sensor
   |
   v
Arduino
   |
   +-- mede continuamente
   +-- processa localmente
   |
   +-- evento relevante?
          |
         SIM
          |
          v
       transmite
```

Essa estratégia permite estudar uma das características importantes da
computação de borda: **processar os dados próximo à sua origem e
transmitir apenas a informação necessária**.

------------------------------------------------------------------------

## 21. Comparação experimental futura

A evolução do projeto permitirá comparar duas arquiteturas.

### Processamento centralizado

``` text
Sensor → Arduino → dados brutos → computador → processamento
```

### Processamento na borda

``` text
Sensor → Arduino → processamento local → informação → computador
```

Entre as métricas que poderão ser comparadas estão:

-   volume de dados transmitidos;
-   número de mensagens;
-   utilização de memória;
-   tempo de resposta;
-   latência;
-   carga computacional;
-   capacidade de operação independente;
-   eficiência da comunicação.

------------------------------------------------------------------------

## 22. Objetivo acadêmico

O experimento busca demonstrar, de forma prática, a evolução de um
sistema tradicional de aquisição de dados para uma arquitetura baseada
em **computação de borda**.

A temperatura é utilizada como variável experimental por permitir uma
implementação simples e controlável, enquanto os conceitos desenvolvidos
podem posteriormente ser aplicados a sistemas envolvendo múltiplos
sensores, IoT, monitoramento ambiental, automação e sistemas
distribuídos.

------------------------------------------------------------------------

## Autor

**Charles Cavalcante Alcarde**

Projeto desenvolvido no contexto de estudos e experimentos em
**Computação de Borda (Edge Computing)**.

2026

#include <MultiFuncShield.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimerOne.h>

// ============================================================
// SENSOR DS18B20
// ============================================================

#define ONE_WIRE_BUS A4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ============================================================
// TRIMPOT - AJUSTE DO LIMIAR
// ============================================================

#define PINO_TRIMPOT A0

#define LIMIAR_MINIMO 0.10
#define LIMIAR_MAXIMO 1.00

float limiarEventoRapido = 0.30;

bool modoAjuste = false;

// ============================================================
// MODOS DO DISPLAY
// ============================================================

#define DISPLAY_TEMP     0
#define DISPLAY_MEDIA30  1
#define DISPLAY_TREND5   2

byte modoDisplay = DISPLAY_TEMP;

// ============================================================
// MEDIA MOVEL DE 30 MINUTOS
// ============================================================

#define NUM_MINUTOS 30
#define DURACAO_MINUTO 60000UL

float somaMinuto[NUM_MINUTOS];
uint16_t contagemMinuto[NUM_MINUTOS];

uint8_t indiceMinuto = 0;

unsigned long inicioMinuto = 0;

float somaJanela30 = 0.0;
unsigned long contagemJanela30 = 0;

unsigned long minutosDecorridos = 0;

// ============================================================
// MEDIA TOTAL
// ============================================================

float somaTotal = 0.0;
unsigned long contagemTotal = 0;

// ============================================================
// MAXIMO E MINIMO
// ============================================================

float temperaturaMaxima = -1000.0;
float temperaturaMinima = 1000.0;

// ============================================================
// TENDENCIA DE 5 MINUTOS
// ============================================================

#define JANELA_TENDENCIA 5
#define LIMIAR_TENDENCIA 0.02

float tendencia5 = 0.0;

// ============================================================
// EVENTO RAPIDO
// ============================================================

float temperaturaAnterior = 0.0;
unsigned long tempoAnterior = 0;

bool leituraAnteriorValida = false;

float deltaRapido = 0.0;
float variacaoRapida = 0.0;

// ============================================================
// CLASSIFICACAO DOS EVENTOS
// ============================================================

#define EVENTO_NORMAL 0
#define EVENTO_SUBIDA 1
#define EVENTO_QUEDA  2

byte eventoAtual = EVENTO_NORMAL;
byte eventoAnterior = EVENTO_NORMAL;

// ============================================================
// INTERVALO ENTRE LEITURAS
// ============================================================

#define INTERVALO_LEITURA 1000UL

unsigned long ultimaLeitura = 0;

// ============================================================
// ATUALIZA OS LEDS DO MODO NORMAL
// ============================================================

void atualizarLedModo()
{
    MFS.blinkLeds(
        LED_ALL,
        OFF
    );

    MFS.writeLeds(
        LED_ALL,
        OFF
    );

    if (modoDisplay == DISPLAY_TEMP)
    {
        MFS.writeLeds(
            LED_1,
            ON
        );
    }

    else if (modoDisplay == DISPLAY_MEDIA30)
    {
        MFS.writeLeds(
            LED_2,
            ON
        );
    }

    else if (modoDisplay == DISPLAY_TREND5)
    {
        MFS.writeLeds(
            LED_3,
            ON
        );
    }
}

// ============================================================
// LEITURA DO TRIMPOT
// ============================================================

void atualizarLimiar()
{
    int leituraPot =
        analogRead(PINO_TRIMPOT);

    limiarEventoRapido =
        LIMIAR_MINIMO +
        (
            leituraPot / 1023.0
        ) *
        (
            LIMIAR_MAXIMO -
            LIMIAR_MINIMO
        );
}

// ============================================================
// ENTRA NO MODO DE AJUSTE
// ============================================================

void entrarModoAjuste()
{
    modoAjuste = true;

    atualizarLimiar();

    MFS.writeLeds(
        LED_ALL,
        OFF
    );

    MFS.blinkLeds(
        LED_ALL,
        ON
    );

    MFS.writeLeds(
        LED_ALL,
        ON
    );

    Serial.println(
        F("MODO_AJUSTE:ATIVO")
    );
}

// ============================================================
// SAI DO MODO DE AJUSTE
// ============================================================

void sairModoAjuste()
{
    modoAjuste = false;

    MFS.blinkLeds(
        LED_ALL,
        OFF
    );

    MFS.writeLeds(
        LED_ALL,
        OFF
    );

    atualizarLedModo();

    Serial.print(
        F("MODO_AJUSTE:ENCERRADO,LIMIAR:")
    );

    Serial.println(
        limiarEventoRapido,
        2
    );
}

// ============================================================
// TRATAMENTO DO TECLADO
// ============================================================

void verificarBotoes()
{
    byte botao =
        MFS.getButton();

    switch (botao)
    {
        // ----------------------------------------------------
        // S1 - TOQUE CURTO
        // TEMP -> MEDIA30 -> TREND5
        // ----------------------------------------------------

        case BUTTON_1_SHORT_RELEASE:

            if (!modoAjuste)
            {
                modoDisplay++;

                if (modoDisplay > DISPLAY_TREND5)
                {
                    modoDisplay =
                        DISPLAY_TEMP;
                }

                atualizarLedModo();

                Serial.print(
                    F("DISPLAY:")
                );

                if (modoDisplay == DISPLAY_TEMP)
                {
                    Serial.println(
                        F("TEMP")
                    );
                }

                else if (
                    modoDisplay ==
                    DISPLAY_MEDIA30
                )
                {
                    Serial.println(
                        F("MEDIA30")
                    );
                }

                else
                {
                    Serial.println(
                        F("TREND5")
                    );
                }
            }

            break;

        // ----------------------------------------------------
        // S2 - PRESSIONAMENTO LONGO + SOLTAR
        // ENTRA / SAI DO AJUSTE
        // ----------------------------------------------------

        case BUTTON_2_LONG_RELEASE:

            if (!modoAjuste)
            {
                entrarModoAjuste();
            }

            else
            {
                sairModoAjuste();
            }

            break;

        default:

            break;
    }
}

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(9600);

    delay(1000);

    // --------------------------------------------------------
    // SENSOR
    // --------------------------------------------------------

    sensors.begin();

    // --------------------------------------------------------
    // MULTI FUNCTION SHIELD
    // --------------------------------------------------------

    Timer1.initialize(1000);

    MFS.initialize(&Timer1);

    delay(500);

    // ========================================================
    // NOVO:
    // LE O TRIMPOT NA INICIALIZACAO
    // ========================================================

    atualizarLimiar();

    // --------------------------------------------------------
    // BUFFER DA MEDIA DE 30 MINUTOS
    // --------------------------------------------------------

    for (int i = 0; i < NUM_MINUTOS; i++)
    {
        somaMinuto[i] = 0.0;
        contagemMinuto[i] = 0;
    }

    inicioMinuto = millis();

    // --------------------------------------------------------
    // ESTADO INICIAL DA INTERFACE
    // --------------------------------------------------------

    modoDisplay = DISPLAY_TEMP;
    modoAjuste = false;

    atualizarLedModo();

    // --------------------------------------------------------
    // MENSAGENS INICIAIS
    // --------------------------------------------------------

    Serial.println(
        F("Sistema EDGE iniciado")
    );

    Serial.println(
        F("S1 curto: TEMP -> MEDIA30 -> TREND5")
    );

    Serial.println(
        F("S2 longo: entrar/sair do ajuste")
    );

    Serial.print(
        F("Limiar inicial lido do trimpot: ")
    );

    Serial.print(
        limiarEventoRapido,
        2
    );

    Serial.println(
        F(" C")
    );
}

// ============================================================
// LOOP PRINCIPAL
// ============================================================

void loop()
{
    // ========================================================
    // TECLADO
    // ========================================================

    verificarBotoes();

    unsigned long agora =
        millis();

    // ========================================================
    // MODO DE AJUSTE
    // ========================================================

    if (modoAjuste)
    {
        atualizarLimiar();

        MFS.write(
            limiarEventoRapido,
            2
        );
    }

    // ========================================================
    // CONTROLE DO INTERVALO DE AQUISICAO
    // ========================================================

    if (
        agora - ultimaLeitura <
        INTERVALO_LEITURA
    )
    {
        return;
    }

    ultimaLeitura = agora;

    // ========================================================
    // LEITURA DO SENSOR
    // ========================================================

    sensors.requestTemperatures();

    float temperatura =
        sensors.getTempCByIndex(0);

    if (
        temperatura ==
        DEVICE_DISCONNECTED_C
    )
    {
        Serial.println(
            F("Erro: sensor desconectado")
        );

        return;
    }

    // ========================================================
    // TROCA DO BUCKET DE 1 MINUTO
    // ========================================================

    while (
        agora - inicioMinuto >=
        DURACAO_MINUTO
    )
    {
        inicioMinuto +=
            DURACAO_MINUTO;

        minutosDecorridos++;

        indiceMinuto =
            (
                indiceMinuto + 1
            ) % NUM_MINUTOS;

        somaJanela30 -=
            somaMinuto[indiceMinuto];

        contagemJanela30 -=
            contagemMinuto[indiceMinuto];

        somaMinuto[indiceMinuto] =
            0.0;

        contagemMinuto[indiceMinuto] =
            0;
    }

    // ========================================================
    // ADICIONA A LEITURA AO MINUTO ATUAL
    // ========================================================

    somaMinuto[indiceMinuto] +=
        temperatura;

    contagemMinuto[indiceMinuto]++;

    somaJanela30 +=
        temperatura;

    contagemJanela30++;

    // ========================================================
    // MEDIA MOVEL DE 30 MINUTOS
    // ========================================================

    float media30 = 0.0;

    if (contagemJanela30 > 0)
    {
        media30 =
            somaJanela30 /
            contagemJanela30;
    }

    // ========================================================
    // MEDIA TOTAL
    // ========================================================

    somaTotal +=
        temperatura;

    contagemTotal++;

    float mediaTotal =
        somaTotal /
        contagemTotal;

    // ========================================================
    // MAXIMO E MINIMO
    // ========================================================

    if (
        temperatura >
        temperaturaMaxima
    )
    {
        temperaturaMaxima =
            temperatura;
    }

    if (
        temperatura <
        temperaturaMinima
    )
    {
        temperaturaMinima =
            temperatura;
    }

    float amplitude =
        temperaturaMaxima -
        temperaturaMinima;

    // ========================================================
    // TENDENCIA DE 5 MINUTOS
    // ========================================================

    tendencia5 = 0.0;

    if (
        minutosDecorridos >=
        JANELA_TENDENCIA &&
        contagemMinuto[indiceMinuto] > 0
    )
    {
        float mediaMinutoAtual =
            somaMinuto[indiceMinuto] /
            contagemMinuto[indiceMinuto];

        int indiceAnterior =
            (
                indiceMinuto +
                NUM_MINUTOS -
                JANELA_TENDENCIA
            ) % NUM_MINUTOS;

        if (
            contagemMinuto[indiceAnterior] >
            0
        )
        {
            float mediaAnterior =
                somaMinuto[indiceAnterior] /
                contagemMinuto[indiceAnterior];

            tendencia5 =
                (
                    mediaMinutoAtual -
                    mediaAnterior
                ) /
                JANELA_TENDENCIA;
        }
    }

    // ========================================================
    // DELTA E VARIACAO RAPIDA
    // ========================================================

    if (!leituraAnteriorValida)
    {
        temperaturaAnterior =
            temperatura;

        tempoAnterior =
            agora;

        deltaRapido =
            0.0;

        variacaoRapida =
            0.0;

        leituraAnteriorValida =
            true;
    }

    else
    {
        unsigned long intervaloMs =
            agora -
            tempoAnterior;

        deltaRapido =
            temperatura -
            temperaturaAnterior;

        if (intervaloMs > 0)
        {
            float intervaloMinutos =
                intervaloMs /
                60000.0;

            variacaoRapida =
                deltaRapido /
                intervaloMinutos;
        }

        else
        {
            variacaoRapida =
                0.0;
        }

        temperaturaAnterior =
            temperatura;

        tempoAnterior =
            agora;
    }

    // ========================================================
    // DETECCAO DO EVENTO RAPIDO
    // ========================================================

    eventoAtual =
        EVENTO_NORMAL;

    if (
        deltaRapido >=
        limiarEventoRapido
    )
    {
        eventoAtual =
            EVENTO_SUBIDA;
    }

    else if (
        deltaRapido <=
        -limiarEventoRapido
    )
    {
        eventoAtual =
            EVENTO_QUEDA;
    }

    // ========================================================
    // ALERTA SONORO
    // ========================================================

    if (
        eventoAtual !=
        eventoAnterior
    )
    {
        if (
            eventoAtual ==
            EVENTO_SUBIDA
        )
        {
            MFS.beep(
                12,
                0,
                1
            );
        }

        else if (
            eventoAtual ==
            EVENTO_QUEDA
        )
        {
            MFS.beep(
                12,
                8,
                2
            );
        }
    }

    eventoAnterior =
        eventoAtual;

    // ========================================================
    // ESTADO DA TENDENCIA
    // ========================================================

    const char* estadoTrend;

    if (
        minutosDecorridos <
        JANELA_TENDENCIA
    )
    {
        estadoTrend =
            "AGUARDANDO";
    }

    else if (
        tendencia5 >
        LIMIAR_TENDENCIA
    )
    {
        estadoTrend =
            "SUBINDO";
    }

    else if (
        tendencia5 <
        -LIMIAR_TENDENCIA
    )
    {
        estadoTrend =
            "CAINDO";
    }

    else
    {
        estadoTrend =
            "ESTAVEL";
    }

    // ========================================================
    // NOME DO EVENTO
    // ========================================================

    const char* nomeEvento;

    if (
        eventoAtual ==
        EVENTO_SUBIDA
    )
    {
        nomeEvento =
            "SUBIDA_RAPIDA";
    }

    else if (
        eventoAtual ==
        EVENTO_QUEDA
    )
    {
        nomeEvento =
            "QUEDA_RAPIDA";
    }

    else
    {
        nomeEvento =
            "NORMAL";
    }

    // ========================================================
    // ESTADO TERMICO
    // ========================================================

    const char* estadoTermico;

    if (
        eventoAtual ==
        EVENTO_SUBIDA
    )
    {
        estadoTermico =
            "SUBIDA_RAPIDA";
    }

    else if (
        eventoAtual ==
        EVENTO_QUEDA
    )
    {
        estadoTermico =
            "QUEDA_RAPIDA";
    }

    else if (
        tendencia5 >
        LIMIAR_TENDENCIA
    )
    {
        estadoTermico =
            "AQUECIMENTO";
    }

    else if (
        tendencia5 <
        -LIMIAR_TENDENCIA
    )
    {
        estadoTermico =
            "RESFRIAMENTO";
    }

    else
    {
        estadoTermico =
            "ESTAVEL";
    }

    // ========================================================
    // DISPLAY NORMAL
    // ========================================================

    if (!modoAjuste)
    {
        if (
            modoDisplay ==
            DISPLAY_TEMP
        )
        {
            MFS.write(
                temperatura,
                1
            );
        }

        else if (
            modoDisplay ==
            DISPLAY_MEDIA30
        )
        {
            MFS.write(
                media30,
                1
            );
        }

        else if (
            modoDisplay ==
            DISPLAY_TREND5
        )
        {
            MFS.write(
                tendencia5,
                2
            );
        }
    }

    // ========================================================
    // SERIAL PARA O PYTHON
    // ========================================================

    Serial.print(F("TEMP:"));
    Serial.print(temperatura, 1);

    Serial.print(F(",MEDIA30:"));
    Serial.print(media30, 2);

    Serial.print(F(",MEDIATOTAL:"));
    Serial.print(mediaTotal, 2);

    Serial.print(F(",MAX:"));
    Serial.print(temperaturaMaxima, 1);

    Serial.print(F(",MIN:"));
    Serial.print(temperaturaMinima, 1);

    Serial.print(F(",AMPL:"));
    Serial.print(amplitude, 1);

    Serial.print(F(",TREND5:"));
    Serial.print(tendencia5, 3);

    Serial.print(F(",ESTADO_TREND:"));
    Serial.print(estadoTrend);

    Serial.print(F(",DELTA:"));
    Serial.print(deltaRapido, 2);

    Serial.print(F(",RAPIDA:"));
    Serial.print(variacaoRapida, 2);

    Serial.print(F(",EVENTO:"));
    Serial.print(nomeEvento);

    Serial.print(F(",ESTADO_TERMICO:"));
    Serial.print(estadoTermico);

    Serial.print(F(",LIMIAR:"));
    Serial.println(
        limiarEventoRapido,
        2
    );
}
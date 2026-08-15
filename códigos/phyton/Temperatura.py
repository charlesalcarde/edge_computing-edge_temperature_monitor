import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from datetime import datetime, timedelta
import numpy as np
import csv

# --------------------------------------------------
# CONFIGURACAO DA PORTA SERIAL
# --------------------------------------------------

porta = "COM7"
baud_rate = 9600

arduino = serial.Serial(porta, baud_rate, timeout=1)

# --------------------------------------------------
# VARIAVEIS PARA ARMAZENAR OS DADOS
# --------------------------------------------------

tempos = []
temperaturas = []
medias_moveis = []

temperatura_maxima = None
temperatura_minima = None

hora_maxima = None
hora_minima = None

# Variaveis para calcular a media total do experimento
soma_total = 0.0
quantidade_total = 0

# Inicio do experimento
inicio_experimento = datetime.now()

# --------------------------------------------------
# ARQUIVO DE REGISTRO
# --------------------------------------------------

nome_arquivo = (
    "temperaturas_"
    + inicio_experimento.strftime("%Y%m%d_%H%M%S")
    + ".csv"
)

# Cria o arquivo e escreve o cabecalho
with open(
    nome_arquivo,
    mode="w",
    newline="",
    encoding="utf-8"
) as arquivo:

    escritor = csv.writer(
        arquivo,
        delimiter=";"
    )

    escritor.writerow([
        "Data",
        "Hora",
        "Temperatura (C)",
        "Media movel 30 min (C)",
        "Media total (C)",
        "Maxima (C)",
        "Minima (C)",
        "Amplitude (C)",
        "Tendencia (C/min)",
        "Estado",
        "Tempo de aquisicao"
    ])

print(f"Arquivo de registro: {nome_arquivo}")

# --------------------------------------------------
# CRIACAO DO GRAFICO
# --------------------------------------------------

fig, ax = plt.subplots()


# --------------------------------------------------
# FUNCAO PARA FORMATAR O TEMPO
# --------------------------------------------------

def formatar_tempo(segundos):

    segundos = int(segundos)

    horas = segundos // 3600
    minutos = (segundos % 3600) // 60
    segundos = segundos % 60

    return f"{horas:02d}:{minutos:02d}:{segundos:02d}"


# --------------------------------------------------
# FUNCAO PARA CALCULAR A TENDENCIA
# --------------------------------------------------

def calcular_tendencia(agora):

    # Utiliza os ultimos 5 minutos
    limite_tendencia = agora - timedelta(minutes=5)

    dados = [
        (tempo, temp)
        for tempo, temp in zip(tempos, temperaturas)
        if tempo >= limite_tendencia
    ]

    if len(dados) < 2:
        return 0.0

    tempo_inicial = dados[0][0]

    # Tempo convertido para minutos
    x = np.array([
        (tempo - tempo_inicial).total_seconds() / 60
        for tempo, temp in dados
    ])

    y = np.array([
        temp
        for tempo, temp in dados
    ])

    # Regressao linear
    coeficientes = np.polyfit(x, y, 1)

    return coeficientes[0]


# --------------------------------------------------
# FUNCAO PARA GRAVAR UMA LEITURA NO ARQUIVO
# --------------------------------------------------

def gravar_dados(
    agora,
    temperatura,
    media_movel,
    media_total,
    amplitude_termica,
    tendencia,
    estado_tendencia,
    tempo_aquisicao
):

    with open(
        nome_arquivo,
        mode="a",
        newline="",
        encoding="utf-8"
    ) as arquivo:

        escritor = csv.writer(
            arquivo,
            delimiter=";"
        )

        escritor.writerow([
            agora.strftime("%d/%m/%Y"),
            agora.strftime("%H:%M:%S"),
            f"{temperatura:.1f}",
            f"{media_movel:.2f}",
            f"{media_total:.2f}",
            f"{temperatura_maxima:.1f}",
            f"{temperatura_minima:.1f}",
            f"{amplitude_termica:.1f}",
            f"{tendencia:.3f}",
            estado_tendencia,
            tempo_aquisicao
        ])


# --------------------------------------------------
# FUNCAO DE ATUALIZACAO
# --------------------------------------------------

def atualizar(frame):

    global temperatura_maxima
    global temperatura_minima
    global hora_maxima
    global hora_minima
    global soma_total
    global quantidade_total

    linha = arduino.readline().decode(
        "utf-8",
        errors="ignore"
    ).strip()

    if linha.startswith("Temperatura:"):

        try:

            valor = linha.split(":")[1]
            valor = valor.replace("C", "").strip()

            temperatura = float(valor)

            agora = datetime.now()

            # --------------------------------------------------
            # TEMPO TOTAL DE AQUISICAO
            # --------------------------------------------------

            tempo_decorrido = (
                agora - inicio_experimento
            ).total_seconds()

            tempo_aquisicao = formatar_tempo(
                tempo_decorrido
            )

            # --------------------------------------------------
            # MEDIA TOTAL
            # --------------------------------------------------

            soma_total += temperatura
            quantidade_total += 1

            media_total = soma_total / quantidade_total

            # --------------------------------------------------
            # ARMAZENA OS DADOS
            # --------------------------------------------------

            tempos.append(agora)
            temperaturas.append(temperatura)

            # --------------------------------------------------
            # MEDIA MOVEL DE 30 MINUTOS
            # --------------------------------------------------

            limite_media = agora - timedelta(minutes=30)

            valores_media = [
                temp
                for tempo, temp in zip(tempos, temperaturas)
                if tempo >= limite_media
            ]

            media_movel = (
                sum(valores_media)
                / len(valores_media)
            )

            medias_moveis.append(media_movel)

            # --------------------------------------------------
            # REMOVE DADOS COM MAIS DE 60 MINUTOS
            # DO GRAFICO
            # --------------------------------------------------

            limite_grafico = agora - timedelta(minutes=60)

            while tempos and tempos[0] < limite_grafico:

                tempos.pop(0)
                temperaturas.pop(0)
                medias_moveis.pop(0)

            # --------------------------------------------------
            # TEMPERATURA MAXIMA
            # --------------------------------------------------

            if (
                temperatura_maxima is None
                or temperatura > temperatura_maxima
            ):

                temperatura_maxima = temperatura
                hora_maxima = agora

            # --------------------------------------------------
            # TEMPERATURA MINIMA
            # --------------------------------------------------

            if (
                temperatura_minima is None
                or temperatura < temperatura_minima
            ):

                temperatura_minima = temperatura
                hora_minima = agora

            # --------------------------------------------------
            # AMPLITUDE TERMICA
            # --------------------------------------------------

            amplitude_termica = (
                temperatura_maxima
                - temperatura_minima
            )

            # --------------------------------------------------
            # TENDENCIA
            # --------------------------------------------------

            tendencia = calcular_tendencia(agora)

            if tendencia > 0.01:

                estado_tendencia = "Subindo"

            elif tendencia < -0.01:

                estado_tendencia = "Caindo"

            else:

                estado_tendencia = "Estavel"

            # --------------------------------------------------
            # GRAVA A LEITURA NO ARQUIVO
            # --------------------------------------------------

            gravar_dados(
                agora,
                temperatura,
                media_movel,
                media_total,
                amplitude_termica,
                tendencia,
                estado_tendencia,
                tempo_aquisicao
            )

            # --------------------------------------------------
            # ATUALIZA O GRAFICO
            # --------------------------------------------------

            ax.clear()

            # Temperatura instantanea
            ax.plot(
                tempos,
                temperaturas,
                linewidth=1.0,
                label="Temperatura"
            )

            # Media movel
            ax.plot(
                tempos,
                medias_moveis,
                linewidth=2.0,
                label="Media movel 30 min"
            )

            # --------------------------------------------------
            # MAXIMA E MINIMA NA LEGENDA
            # --------------------------------------------------

            legenda_maxima = (
                f"Maxima: {temperatura_maxima:.1f} C "
                f"as {hora_maxima.strftime('%H:%M:%S')}"
            )

            legenda_minima = (
                f"Minima: {temperatura_minima:.1f} C "
                f"as {hora_minima.strftime('%H:%M:%S')}"
            )

            ax.plot(
                [],
                [],
                marker="^",
                linestyle="None",
                label=legenda_maxima
            )

            ax.plot(
                [],
                [],
                marker="v",
                linestyle="None",
                label=legenda_minima
            )

            # --------------------------------------------------
            # CAIXA DE INFORMACOES
            # --------------------------------------------------

            texto_info = (
                f"Atual: {temperatura:.1f} C\n"
                f"Media 30 min: {media_movel:.2f} C\n"
                f"Media total: {media_total:.2f} C\n"
                f"Amplitude: {amplitude_termica:.1f} C\n"
                f"Tendencia: {estado_tendencia} "
                f"({tendencia:+.3f} C/min)\n"
                f"Tempo de aquisicao: {tempo_aquisicao}"
            )

            ax.text(
                0.02,
                0.95,
                texto_info,
                transform=ax.transAxes,
                verticalalignment="top",
                fontsize=10,
                bbox=dict(
                    boxstyle="round",
                    alpha=0.8
                )
            )

            # --------------------------------------------------
            # CONFIGURACAO VISUAL
            # --------------------------------------------------

            ax.set_title(
                "Temperatura em tempo real - Ultimos 60 minutos"
            )

            ax.set_xlabel("Horario")
            ax.set_ylabel("Temperatura (C)")

            ax.grid(True)

            ax.legend(loc="best")

            fig.autofmt_xdate()

            # --------------------------------------------------
            # CONSOLE
            # --------------------------------------------------

            print(
                f"Temperatura: {temperatura:.1f} C | "
                f"Media 30 min: {media_movel:.2f} C | "
                f"Media total: {media_total:.2f} C | "
                f"Amplitude: {amplitude_termica:.1f} C | "
                f"Tendencia: {estado_tendencia} "
                f"{tendencia:+.3f} C/min | "
                f"Tempo: {tempo_aquisicao}"
            )

        except ValueError:
            pass


# --------------------------------------------------
# ATUALIZACAO A CADA 1 SEGUNDO
# --------------------------------------------------

ani = FuncAnimation(
    fig,
    atualizar,
    interval=1000,
    cache_frame_data=False
)

plt.show()

arduino.close()

print()
print("Aquisicao encerrada.")
print(f"Dados gravados em: {nome_arquivo}")
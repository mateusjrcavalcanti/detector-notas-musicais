import numpy as np
import matplotlib.pyplot as plt
from scipy.fft import fft, fftfreq
import os

# Configurações
fs = 44100  # taxa de amostragem
duration = 1.0  # duração do sinal (em segundos)
output_dir = "docs"
os.makedirs(output_dir, exist_ok=True)

# Função para gerar e salvar espectro
def gerar_espectro(freq, nome_nota):
    t = np.linspace(0, duration, int(fs * duration), endpoint=False)
    # Sinal com harmônicos
    sinal = (
        np.sin(2 * np.pi * freq * t)
        + 0.5 * np.sin(2 * np.pi * freq * 2 * t)  # 2º harmônico
        + 0.25 * np.sin(2 * np.pi * freq * 3 * t)  # 3º harmônico
    )

    yf = fft(sinal)
    xf = fftfreq(len(t), 1 / fs)

    plt.figure(figsize=(8, 4))
    plt.plot(xf[:len(xf) // 2], np.abs(yf[:len(xf) // 2]), label='Espectro')
    plt.xlim(0, 2000)  # Limitar o eixo X até 2 kHz (região musical)
    
    # Destacar frequência fundamental e harmônicos
    harmonicos = [freq, freq*2, freq*3]
    cores = ['red', 'orange', 'green']
    labels = ['Fundamental', '2º Harmônico', '3º Harmônico']
    for h, cor, lab in zip(harmonicos, cores, labels):
        plt.axvline(h, color=cor, linestyle='--', alpha=0.35, label=lab)
        plt.text(h, plt.ylim()[1]*0.85, f'{h:.0f} Hz', color=cor, ha='center', fontsize=8, fontweight='bold', rotation=90, bbox=dict(facecolor='white', alpha=0.3, edgecolor='none', pad=0.5))

    # Anotar o valor do pico principal
    idx_fund = np.argmin(np.abs(xf[:len(xf)//2] - freq))
    mag_fund = np.abs(yf[:len(xf)//2])[idx_fund]
    plt.annotate(f'Pico: {freq:.1f} Hz', xy=(freq, mag_fund), xytext=(freq+100, mag_fund*1.1),
                 arrowprops=dict(facecolor='red', alpha=0.3, shrink=0.05, width=1, headwidth=6),
                 fontsize=9, color='red', fontweight='bold', bbox=dict(facecolor='white', alpha=0.3, edgecolor='none', pad=0.5))

    plt.title(f"Espectro da Nota {nome_nota} ({freq} Hz)\nFundamental e Harmônicos destacados")
    plt.xlabel("Frequência (Hz)")
    plt.ylabel("Magnitude")
    plt.grid(True, which='both', linestyle='--', alpha=0.5)
    plt.legend()
    plt.tight_layout()
    caminho = os.path.join(output_dir, f"espectro_{nome_nota}.png")
    plt.savefig(caminho)
    plt.close()
    print(f"Salvo: {caminho}")

# Notas a serem geradas (cromáticas, igual ao projeto)
notas = {
    "C": 261.63,   # Dó
    "C#": 277.18,  # Dó#
    "D": 293.66,   # Ré
    "D#": 311.13,  # Ré#
    "E": 329.63,   # Mi
    "F": 349.23,   # Fá
    "F#": 369.99,  # Fá#
    "G": 392.00,   # Sol
    "G#": 415.30,  # Sol#
    "A": 440.00,   # Lá
    "A#": 466.16,  # Lá#
    "B": 493.88    # Si
}

for nome, freq in notas.items():
    gerar_espectro(freq, nome)

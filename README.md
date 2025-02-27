# 🎵 Detector e Reprodutor de Notas Musicais - BitDogLab 🎶

Este projeto consiste em um sistema embarcado capaz de **detectar e reproduzir notas musicais** utilizando **exclusivamente os periféricos da placa BitDogLab**. Ele processa o sinal captado pelo microfone, identifica a frequência dominante via **Transformada Rápida de Fourier (FFT)** e exibe o resultado no **display OLED e na matriz de LEDs**. Além disso, permite a reprodução das notas musicais via **buzzer com PWM**.

---

## 📌 **Características Principais**

✅ Detecção de notas musicais com **análise espectral (FFT)**\
✅ Exibição da frequência e da nota detectada no **display OLED (SSD1306)**\
✅ Representação gráfica da nota na **matriz de LEDs**\
✅ Reprodução sonora da nota via **buzzer utilizando PWM**\
✅ Interface interativa com **botões físicos e joystick**\
✅ Uso exclusivo dos periféricos da **placa BitDogLab**

---

## 🛠 **Hardware Utilizado**

- **Placa BitDogLab**
- **Microfone embutido** para captura do som
- **Conversor Analógico-Digital (ADC)** para processar o áudio
- **Display OLED (SSD1306)** para exibição de informações
- **Buzzer** para reprodução das notas musicais
- **Matriz de LEDs** para representar visualmente as notas
- **Botões e joystick** para interação com o usuário

---

## 🏗 **Arquitetura do Projeto**

O projeto foi dividido em diferentes módulos para facilitar a organização e a implementação do sistema:

1. **Captura de Áudio** → Leitura do microfone via ADC
2. **Processamento FFT** → Transformada Rápida de Fourier para análise do sinal
3. **Identificação da Nota** → Mapeamento da frequência detectada para a nota musical correspondente
4. **Feedback Visual** → Exibição no display OLED e matriz de LEDs
5. **Reprodução Sonora** → Geração da nota musical correspondente no buzzer via PWM
6. **Interação com Usuário** → Uso de botões e joystick para controle

---

## 📂 **Estrutura do Código**

```
📂 detector-notas-musicais/
├── 📄 detector-notas-musicais.c   # Código principal do projeto
├──── 📂libs
├──── 📄 buzzer.c                     # Módulo responsável pelo controle do buzzer (PWM)
├──── 📄 animation.c                  # Controle da matriz de LEDs
├──── 📄 leds.c                       # Funções para exibição de padrões na matriz de LEDs
├──── 📄 ssd1306.c                    # Biblioteca para controle do display OLED
├──── 📄 notas.h                      # Tabela de notas musicais e suas frequências
├──── 📄 font.h                       # Definições de fontes para o display OLED
├── 📄 pio_matrix.pio               # Código da PIO para controle dos LEDs
└── 📄 README.md                    # Documentação do projeto
```

---

## 🚀 **Instalação e Configuração**

### **1️⃣ Configurar a Placa BitDogLab**

Antes de rodar o projeto, é necessário configurar os periféricos corretamente:

- Conectar o **display OLED** via I²C nos pinos **SDA (GPIO14) e SCL (GPIO15)**
- Garantir que o **buzzer** está ligado ao **GPIO designado para PWM**
- Verificar a **alimentação do microfone e configuração do ADC**

### **2️⃣ Compilar e Carregar o Código**

Para compilar o projeto e carregar na placa BitDogLab, utilize o **SDK do Raspberry Pi Pico** com a ferramenta `CMake`:

```sh
mkdir build
cd build
cmake ..
make
```

Após a compilação, carregue o binário gerado na BitDogLab.

---

## 🎮 **Como Usar**

1. **Ligue a placa BitDogLab**
2. **Modo Detecção** → O sistema inicia capturando áudio do microfone e identificando a nota correspondente
3. **Modo Reprodução** → Pressione um botão para ativar a reprodução da nota correspondente no buzzer
4. **Visualização LED** → A matriz de LEDs exibirá a nota detectada de forma visual

---

## 🛠 **Possíveis Melhorias**

🔹 Implementação de **filtros digitais** para reduzir interferências no sinal de áudio\
🔹 Adaptação para **detecção de múltiplas notas simultâneas (acordes)**\
🔹 Otimização do código para **reduzir o consumo de energia**\
🔹 Expansão do projeto para reconhecimento de escalas musicais

---

## 📽️ **Vídeo Demontrativo**

## 📄 **Documentação**
https://github.com/limarich/detector-notas-musicais-atividade-final-embarcatech/blob/main/Sistema%20de%20An%C3%A1lise%20e%20Reprodu%C3%A7%C3%A3o%20de%20Notas%20Musicais%20Utilizando%20a%20BitdogLab.pdf
🚀 **Desenvolvido com tecnologia embarcada e paixão por música!** 🎶

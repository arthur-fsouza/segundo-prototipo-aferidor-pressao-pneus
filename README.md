# Segundo Protótipo do Projeto Aferidor de Pressão de Pneus

Este projeto foi feito por **Arthur Ferreira de Souza** e **João Pedro Alexandre Silva** como atividade para o projeto integrador III da FATEC Jundiaí.

## Descrição

Esse foi um projeto feito ao longo do ano onde nós montamos e programamos um protótipo de um dispositivo que mede a pressão e temperatura interna de pneus de carro.

O intuito do projeto é um dispositivo que monitore de forma precisa e confiável pneus de carros para a calibração do usuário.

### Hardware

Utilizamos os seguintes dispositivos:

**ESP32**
Como o intuito era utilizar o ESP-IDF, utilizamos como microcontrolador central o ESP32. É um dispositivo barato mas muito versátil e de boa qualidade.

**SMP3011**
Para a detecção da pressão e temperatura, utilizamos o sensor SMP3011 que utiliza comunicação I2C. Dispositivo fornecido pela FATEC Jundiaí.
No caso deste projeto, utilizamos a versão *manométrica* ou *gauge*. Isso significa que a pressão lida não é a pressão absoluta, mas sim a diferença com a pressão atmosférica. Caso esteja desconectado, a leitura é próxima a `0`, já que não há diferença de pressão.

[*Datasheet do SMP3011*](https://pt.scribd.com/document/627622073/SMP3011%E5%9E%8BI2C-%E9%80%9A%E8%AE%AF%E5%8D%8F%E8%AE%AE%E6%8C%87%E4%BB%A4%E8%AF%B4%E6%98%8E-V2-English)

**LCD ST7789**
Utilizamos um LCD tipo ST7789 de 320x170 que utiliza comunicação SPI. Nele, mostramos as informações de pressão e temperatura, junto de uma métrica se a pressão está boa ou ruim.
Caso se queira utilizar outro tipo de display, será necessário a utilização de outra biblioteca, já que a que utilizamos (`lcd_st7789.hpp`) é apenas para esse LCD específico.

**Botões**
Existem dois botões utilizados.

O de local é utilizado para alternar entre as 3 possíveis configurações de local, definindo quais unidades serão mostradas. Esses modos são:
- **LATAM:** Pressão PSI e temperatura Celsius
- **EUA:** Pressão Bar e temperatura Fahrenheit
- **EURO:** Pressão Bar e temperatura Celsius

O de ON-OFF é utilizado para ligar e desligar o dispositivo.

**Carcaça**
Temos apenas o protótipo para o encapsulamento dos dispositivos. Porém, ele conta com uma abertura para ver o LCD, aberturas para os botões e um cano que o usuário conecta no bico do pneu.

### Software

O software é feito em C++ utilizando do ESP-IDF v6.1.1.
Fomos recomendados a usar uma versão anterior do ESP-IDF, porém decidimos utilizar a mais recente por conta da praticidade para se instalar na máquina dos integrantes.

Utilizamos o **LVGL** como componente externo. Essa biblioteca é responsável pela parte visual, gerindo a interface do usuário no LCD.

Sobre o desligamento e ligamento, o sistema faz uso da funcionalidade de *deep sleep* do ESP. Não só isso, mas o backlight do LCD também é desligado. Dessa forma consume menos energia.

## Compilação e Execução

Para compilar o projeto, é necessário utilizar o **ESP-IDF** e suas devidas dependências. Recomendamos a versão `6.1.1` em que foi feita o projeto.
*Testamos na versão 6.0.1, porém não funcionou. Por conta disso, não recomendados tentar compilar com versões anteriores.*
É apenas necessário conectar seu ESP32 ao computador/máquina com ESP-IDF e fazer a build e flash do programa.

Para montagem, utilizamos um **ESP32**. Utilizamos um ESP32 comum (no ESP-IDF, nomeado como apenas `esp32`).
*Não podemos afirmar que outras arquiteturas funcionam, já que testamos apenas com esta.*

Os pinos utilizados são:
- **21**: SDA
- **22**: SCL
- **23**: Backlight LCD
- **19**: MOSI
- **18**: SCLK
- **4**: RES
- **2**: DC
- **5**: CS
- **12**: Botão de local
- **13**: Botões de ON/OFF

Caso o usuário deseje mudar quais pinos serão utilizados, é necessário modificar o arquivo `main/pinos.hpp`.

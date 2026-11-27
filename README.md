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
Para a detecção da pressão e temperatura, utilizamos o sensor SMP3011 fornecido pela FATEC Jundiaí.
No caso deste projeto, utilizamos a versão *monométrica* ou *gauge*. Isso significa que a pressão lida não é a pressão absoluta, mas sim a diferença com a pressão atmosférica. Caso esteja desconectado, a leitura é próxima a `0`, já que não há diferença de pressão.

[*Datasheet do SMP3011*](https://pt.scribd.com/document/627622073/SMP3011%E5%9E%8BI2C-%E9%80%9A%E8%AE%AF%E5%8D%8F%E8%AE%AE%E6%8C%87%E4%BB%A4%E8%AF%B4%E6%98%8E-V2-English)

**LCD**
Utilizamos um LCD de 320x170. Nele, mostramos as informações de pressão e temperatura, junto de uma métrica se a pressão está boa ou ruim.

**Botões**
Os botões servem para mudar o local. Esse local refere-se a quais medidas serão mostradas:

- Pressão: PSI ou Bar
- Temperatura: Celsius ou Fahrenheit

**Carcaça**
Temos apenas o protótipo para o encapsulamento dos dispositivos. Porém, ele conta com uma abertura para ver o LCD, aberturas para os botões e um cano que o usuário conecta no bico do pneu.

### Software

O software é feito em C++ utilizando do ESP-IDF v6.0.1.
Fomos recomendados a usar uma versão anterior do ESP-IDF, porém decidimos utilizar a mais recente por conta da praticidade para se instalar na máquina dos integrantes.

Utilizamos o **LVGL** como componente externo. Essa biblioteca é responsável pela parte visual, gerindo a interface do usuário no LCD.

## Compilação e Execução

Para compilar o projeto, é necessário utilizar o **ESP-IDF** e suas devidas dependências. Recomendamos a versão `6.0.1` em que foi feita o projeto.
*Outras versões podem ou não funcionar, porém não temos como afirmar com certeza, já que apenas testamos esta.*
É apenas necessário conectar seu ESP32 ao computador/máquina com ESP-IDF e fazer a build e flash do programa.

Para montagem, utilizamos um **ESP32**. Utilizamos um ESP32 comum (no ESP-IDF, nomeado como apenas `esp32`).
*Não podemos afirmar que outras arquiteturas funcionam, já que testamos apenas com esta.*

Os pinos utilizados são:
- **23**: SDA
- **18**: SCL
- **4**: RES
- **2**: DC
- **5**: CS
- **12, 13, 14**: Botões de local, LATAM, EUA e EURO, respectivamente

Caso o usuário deseje mudar quais pinos serão utilizados, é necessário modificar o arquivo `main/pinos.hpp`.

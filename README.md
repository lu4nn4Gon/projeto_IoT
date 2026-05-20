# Cofre Eletrônico com Arduino UNO

Sistema de cofre eletrônico com autenticação por senha, trava automática, alarme sonoro e detecção de tentativa de arrombamento.

---

## Descrição do projeto

Este projeto simula um cofre eletrônico utilizando Arduino UNO. O sistema permite que o usuário digite uma senha por meio de um teclado matricial 4x4. Caso a senha esteja correta, o servo motor é acionado, simulando a abertura da trava do cofre.

O projeto também possui um sistema de segurança: após três tentativas incorretas, o cofre é bloqueado por 30 segundos. Além disso, um sensor LDR é utilizado para detectar incidência de luz dentro do cofre, simulando uma possível tentativa de arrombamento.

---

## Funcionalidades

- Digitação de senha pelo teclado matricial
- Confirmação da senha com a tecla `#`
- Abertura da trava com servo motor
- Fechamento manual do cofre pela tecla `B`
- Bloqueio automático após 3 tentativas incorretas
- Alarme sonoro com buzzer
- Detecção de arrombamento com sensor LDR
- Mensagens de orientação exibidas no LCD 16x2

---

## Componentes utilizados

| Componente | Função |
|---|---|
| Arduino UNO | Controla todo o sistema |
| Teclado matricial 4x4 | Permite a digitação da senha |
| LCD 16x2 | Exibe mensagens para o usuário |
| Servo motor | Simula a abertura e o fechamento da trava |
| Buzzer | Emite sons de alerta e confirmação |
| Sensor LDR | Detecta luminosidade em caso de abertura indevida |
| Resistor 10kΩ | Utilizado no circuito do LDR |
| Potenciômetro | Ajusta o contraste do LCD |
| Protoboard | Auxilia na montagem do circuito |
| Jumpers | Realizam as conexões entre os componentes |

---

## Teclas de controle

| Tecla | Ação |
|---|---|
| `A` | Inicia a digitação da senha |
| `0` a `9` | Digitação dos números da senha |
| `#` | Confirma a senha digitada |
| `*` | Apaga a senha digitada e reinicia a entrada |
| `B` | Fecha o cofre |

---

## Como usar

1. Abra o link que leva ao nosso projeto no tinkercad.
2. Clique em Iniciar simulação.
5. Ajuste o potenciômetro até o texto aparecer corretamente no LCD.
6. Pressione `A` para iniciar a digitação da senha.
7. Digite a senha e pressione `#` para confirmar.

### Senha padrão

```cpp
123
```

Para trocar a senha, altere esta linha no código:

```cpp
const String SENHA_MESTRA = "123";
```

---

## Diagrama de conexão

Figura 1 — Visão geral do circuito no Tinkercad.
<img width="612" height="475" alt="image" src="https://github.com/user-attachments/assets/6a2fc18d-ef1c-442d-b704-691f9f33fc5c" />

Figura 2 — Detalhamento das conexões entre os componentes e o Arduino UNO.
<img width="808" height="437" alt="image" src="https://github.com/user-attachments/assets/0457c644-b150-4d4f-a39b-0c53809a6f04" />


A tabela abaixo apresenta o mapeamento dos componentes utilizados e seus respectivos pinos conectados ao Arduino UNO.

| Componente | Pinos Arduino |
|---|---|
| LCD RS, EN | D12, D11 |
| LCD D4-D7 | D5, D4, D3, D2 |
| Teclado linhas | A0, A1, A2, A3 |
| Teclado colunas | D10, D9, D8, D7 |
| Servo motor | D13 |
| Buzzer | D6 |
| LDR | A5 |

---

## Funcionamento do sistema

O sistema inicia em estado de bloqueio aguardando interação do usuário. Ao pressionar a tecla `A`, o usuário pode iniciar a digitação da senha no teclado matricial.

As teclas digitadas são exibidas no display LCD. Após pressionar `#`, o Arduino compara a senha digitada com a senha cadastrada no sistema.

Caso a senha esteja correta, o servo motor é acionado, simulando a abertura da trava do cofre. O LCD exibe uma mensagem de acesso permitido e o buzzer emite um som de confirmação.

Caso a senha esteja incorreta, o LCD informa erro de autenticação e o buzzer emite um alerta sonoro.

Após três tentativas incorretas consecutivas, o sistema entra em modo de bloqueio por 30 segundos, impedindo novas tentativas de acesso.

O sensor LDR monitora a presença de luz dentro do cofre. Caso seja detectada luminosidade em uma situação indevida, o sistema dispara um alerta sonoro indicando possível tentativa de arrombamento.

---

## Estrutura dos arquivos

| Arquivo | Descrição |
|---|---|
| `cofre.ino` | Código-fonte do Arduino comentado|
| `README.md` | Documentação do projeto |

---

## Plataforma utilizada

- Arduino IDE
- Tinkercad

---

## Link da simulação
Projeto disponível no Tinkercad: https://www.tinkercad.com/things/ddfB7MbH5Hb-projeto-iot?sharecode=S4zYsG35jiMVw5HP-UX5JBoNqYOetIutJxvNPjVVZYE

## Autores

- Gabriel Andrade Rosa Oliveira
- Bruno Budano Mello 
- Luanna Pinto Gonçalves

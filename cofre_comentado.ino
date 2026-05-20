// ─────────────────────────────────────────────────────────────────────────────
// COFRE ELETRÔNICO COM ARDUINO UNO
// Projeto: sistema de cofre com senha, trava por servo motor, LCD,
// buzzer e sensor LDR para detecção de tentativa de arrombamento.
// ─────────────────────────────────────────────────────────────────────────────

// Bibliotecas utilizadas no projeto
#include <Keypad.h>         // Biblioteca para leitura do teclado matricial 4x4
#include <LiquidCrystal.h>  // Biblioteca para controle do display LCD 16x2
#include <Servo.h>          // Biblioteca para controle do servo motor

// ─────────────────────────────────────────────────────────────────────────────
// 1. DEFINIÇÃO DOS PINOS
// Nesta parte são definidos os pinos do Arduino conectados a cada componente.
// ─────────────────────────────────────────────────────────────────────────────

// Pinos do display LCD 16x2
// O LCD está sendo usado no modo de 4 bits, utilizando os pinos RS, EN, D4, D5, D6 e D7.
const int PIN_LCD_RS = 12;
const int PIN_LCD_EN = 11;
const int PIN_LCD_D4 = 5;
const int PIN_LCD_D5 = 4;
const int PIN_LCD_D6 = 3;
const int PIN_LCD_D7 = 2;

// Pino do servo motor
// O servo simula a trava do cofre.
// Em 0 graus, o cofre fica fechado.
// Em 90 graus, o cofre fica aberto.
const int PIN_SERVO = 13;

// Pino do buzzer
// O buzzer é usado para emitir sons de tecla, erro e alarme.
const int PIN_BUZZER = 6;

// Pino do sensor LDR
// O LDR detecta entrada de luz dentro do cofre, simulando uma tentativa de arrombamento.
const int PIN_LDR = A5;

// ─────────────────────────────────────────────────────────────────────────────
// 2. CONFIGURAÇÃO DO TECLADO MATRICIAL 4x4
// O teclado possui 4 linhas e 4 colunas. Cada tecla é identificada pela posição
// em que a linha e a coluna se cruzam.
// ─────────────────────────────────────────────────────────────────────────────

const byte LINHAS = 4;
const byte COLUNAS = 4;

// Pinos das linhas do teclado
byte pinosLinhas[LINHAS] = {A0, A1, A2, A3};

// Pinos das colunas do teclado
byte pinosColunas[COLUNAS] = {10, 9, 8, 7};

// Mapa das teclas do teclado matricial
char teclas[LINHAS][COLUNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// ─────────────────────────────────────────────────────────────────────────────
// 3. CONFIGURAÇÕES GERAIS DO SISTEMA
// Aqui ficam os valores principais do projeto, como senha, limite de tentativas,
// tempo de bloqueio, posições do servo e sons do buzzer.
// ─────────────────────────────────────────────────────────────────────────────

// Senha padrão do cofre
const String SENHA_MESTRA = "123";

// Quantidade máxima de tentativas incorretas antes do bloqueio
const int MAX_TENTATIVAS = 3;

// Valor limite do LDR para identificar entrada de luz
// Se a leitura ficar abaixo desse valor, o sistema entende que houve violação.
const int LIMITE_LUZ = 900;

// Tempo de bloqueio após muitas tentativas incorretas, em segundos
const int TEMPO_BLOQUEIO = 30;

// Posições do servo motor
const int SERVO_ABERTO = 90;
const int SERVO_FECHADO = 0;

// Sons utilizados no buzzer
const int FREQ_TECLA = 2500;       // Som curto ao pressionar tecla
const int DUR_TECLA = 50;

const int FREQ_ERRO = 400;         // Som grave ao errar a senha
const int DUR_ERRO = 500;

const int FREQ_ALARME_MIN = 600;   // Frequência mínima do alarme
const int FREQ_ALARME_MAX = 1200;  // Frequência máxima do alarme

// ─────────────────────────────────────────────────────────────────────────────
// 4. CRIAÇÃO DOS OBJETOS DOS COMPONENTES
// Aqui são criados os objetos usados para controlar teclado, LCD e servo.
// ─────────────────────────────────────────────────────────────────────────────

// Objeto responsável pela leitura do teclado matricial
Keypad teclado = Keypad(
  makeKeymap(teclas),
  pinosLinhas,
  pinosColunas,
  LINHAS,
  COLUNAS
);

// Objeto responsável pelo controle do display LCD
LiquidCrystal lcd = LiquidCrystal(
  PIN_LCD_RS,
  PIN_LCD_EN,
  PIN_LCD_D4,
  PIN_LCD_D5,
  PIN_LCD_D6,
  PIN_LCD_D7
);

// Objeto responsável pelo controle do servo motor
Servo motorAcesso;

// ─────────────────────────────────────────────────────────────────────────────
// 5. MÁQUINA DE ESTADOS DO SISTEMA
// O funcionamento do cofre foi dividido em estados para organizar melhor a lógica.
//
// Estados:
// TRANCADO  -> cofre fechado, aguardando o usuário iniciar a senha.
// DIGITANDO -> usuário está digitando a senha.
// ABERTO    -> senha correta, cofre aberto.
// BLOQUEADO -> muitas tentativas erradas, sistema bloqueado temporariamente.
// VIOLADO   -> sensor LDR detectou possível arrombamento.
// ─────────────────────────────────────────────────────────────────────────────

enum Estado {
  TRANCADO,
  DIGITANDO,
  ABERTO,
  BLOQUEADO,
  VIOLADO
};

// Estado inicial do sistema
Estado estadoAtual = TRANCADO;

// Armazena temporariamente a senha digitada pelo usuário
String inputSenha = "";

// Conta quantas tentativas incorretas foram feitas em sequência
int tentativasFalhas = 0;

// ─────────────────────────────────────────────────────────────────────────────
// 6. PROTÓTIPOS DAS FUNÇÕES
// Essas declarações informam ao Arduino quais funções existem no programa.
// ─────────────────────────────────────────────────────────────────────────────

void inicializarHardware();
void gerenciarInterface();
void verificarSenha();
void monitorarSensorLuz();
void ajustarAcesso(bool abrir);
void tocarSomTecla();
void tocarSomErro();
void dispararAlarme();

// ─────────────────────────────────────────────────────────────────────────────
// 7. SETUP
// Executado apenas uma vez quando o Arduino é ligado ou reiniciado.
// ─────────────────────────────────────────────────────────────────────────────

void setup() {
  // Inicializa a comunicação serial para acompanhar leituras no Monitor Serial
  Serial.begin(9600);

  // Configura LCD, servo, buzzer, LDR e estado inicial do cofre
  inicializarHardware();
}

// ─────────────────────────────────────────────────────────────────────────────
// 8. LOOP
// Executado continuamente enquanto o Arduino estiver ligado.
// ─────────────────────────────────────────────────────────────────────────────

void loop() {
  // Primeiro verifica se houve alguma tentativa de violação pelo sensor LDR
  monitorarSensorLuz();

  // Depois processa as ações do usuário no teclado e atualiza o LCD
  gerenciarInterface();
}

// ─────────────────────────────────────────────────────────────────────────────
// 9. INICIALIZAÇÃO DOS COMPONENTES
// Configura os componentes principais e garante que o cofre comece fechado.
// ─────────────────────────────────────────────────────────────────────────────

void inicializarHardware() {
  // Inicializa o LCD com 16 colunas e 2 linhas
  lcd.begin(16, 2);

  // Conecta o servo ao pino definido
  motorAcesso.attach(PIN_SERVO);

  // Configura o buzzer como saída
  pinMode(PIN_BUZZER, OUTPUT);

  // Configura o LDR como entrada analógica
  pinMode(PIN_LDR, INPUT);

  // Por segurança, o sistema sempre inicia com o cofre fechado
  ajustarAcesso(false);

  // Mensagem inicial exibida ao ligar o sistema
  lcd.print("Sistema Pronto");
  delay(1000);
  lcd.clear();
}

// ─────────────────────────────────────────────────────────────────────────────
// 10. MONITORAMENTO DO SENSOR LDR
// O LDR verifica se entrou luz dentro do cofre quando ele deveria estar fechado.
// Caso isso aconteça, o sistema entra no estado VIOLADO e dispara o alarme.
// ─────────────────────────────────────────────────────────────────────────────

void monitorarSensorLuz() {
  // Se o cofre estiver aberto corretamente, a entrada de luz é normal.
  // Se o sistema já estiver violado, não precisa verificar novamente.
  if (estadoAtual == ABERTO || estadoAtual == VIOLADO) {
    return;
  }

  // Faz a leitura analógica do sensor LDR
  int leitura = analogRead(PIN_LDR);

  // Mostra a leitura no Monitor Serial para facilitar testes e calibração
  Serial.print("Luz: ");
  Serial.println(leitura);

  // Se a leitura ficar abaixo do limite definido, considera violação
  if (leitura < LIMITE_LUZ) {
    estadoAtual = VIOLADO;
    lcd.clear();
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// 11. GERENCIAMENTO DA INTERFACE
// Esta função controla o comportamento do sistema de acordo com o estado atual.
// Ela lê o teclado, atualiza o LCD, chama sons e faz as transições de estado.
// ─────────────────────────────────────────────────────────────────────────────

void gerenciarInterface() {
  // Lê a tecla pressionada. Se nenhuma tecla for pressionada, retorna vazio.
  char tecla = teclado.getKey();

  switch (estadoAtual) {

    // ─────────────────────────────────────────────────────────────────────────
    // Estado TRANCADO
    // O cofre está fechado e aguardando o usuário pressionar A para digitar senha.
    // ─────────────────────────────────────────────────────────────────────────
    case TRANCADO:
      lcd.setCursor(0, 0);
      lcd.print("Cofre: TRANCADO ");

      lcd.setCursor(0, 1);
      lcd.print("Aperte A p/ Senha");

      if (tecla == 'A') {
        inputSenha = "";
        estadoAtual = DIGITANDO;
        lcd.clear();
        tocarSomTecla();
      }
      break;

    // ─────────────────────────────────────────────────────────────────────────
    // Estado DIGITANDO
    // O usuário digita a senha.
    // A tecla # confirma.
    // A tecla * limpa a senha digitada.
    // ─────────────────────────────────────────────────────────────────────────
    case DIGITANDO:
      lcd.setCursor(0, 0);
      lcd.print("Digite a Senha:");

      if (tecla) {
        if (tecla == '#') {
          // Confirma a senha digitada
          verificarSenha();

        } else if (tecla == '*') {
          // Apaga toda a senha digitada
          inputSenha = "";
          tocarSomTecla();

        } else {
          // Adiciona a tecla pressionada à senha
          inputSenha += tecla;
          tocarSomTecla();
        }

        // Mostra a senha de forma mascarada no LCD usando asteriscos
        String mascara = "";

        for (int i = 0; i < (int)inputSenha.length(); i++) {
          mascara += '*';
        }

        lcd.setCursor(0, 1);
        lcd.print(mascara + "                ");
      }
      break;

    // ─────────────────────────────────────────────────────────────────────────
    // Estado ABERTO
    // A senha foi aceita e o servo abriu a trava.
    // O usuário pode pressionar B para fechar o cofre novamente.
    // ─────────────────────────────────────────────────────────────────────────
    case ABERTO:
      lcd.setCursor(0, 0);
      lcd.print("ACESSO LIBERADO ");

      lcd.setCursor(0, 1);
      lcd.print("Aperte B p/ Sair");

      if (tecla == 'B') {
        ajustarAcesso(false);
        tentativasFalhas = 0;
        estadoAtual = TRANCADO;
        lcd.clear();
        tocarSomTecla();
      }
      break;

    // ─────────────────────────────────────────────────────────────────────────
    // Estado BLOQUEADO
    // O sistema bloqueia por 30 segundos após muitas tentativas incorretas.
    // Durante esse tempo, o usuário não consegue tentar nova senha.
    // ─────────────────────────────────────────────────────────────────────────
    case BLOQUEADO:
      dispararAlarme();

      for (int t = TEMPO_BLOQUEIO; t > 0; t--) {
        lcd.setCursor(0, 0);
        lcd.print("SISTEMA BLOQUEADO");

        lcd.setCursor(0, 1);
        lcd.print("Aguarde: ");
        lcd.print(t);
        lcd.print("s   ");

        delay(1000);
      }

      // Após o tempo de bloqueio, o sistema volta ao estado inicial
      tentativasFalhas = 0;
      estadoAtual = TRANCADO;
      lcd.clear();
      break;

    // ─────────────────────────────────────────────────────────────────────────
    // Estado VIOLADO
    // O sensor LDR detectou luz dentro do cofre em momento indevido.
    // O sistema permanece em alarme até ser reiniciado.
    // ─────────────────────────────────────────────────────────────────────────
    case VIOLADO:
      lcd.setCursor(0, 0);
      lcd.print(" ALERTA MAXIMO! ");

      lcd.setCursor(0, 1);
      lcd.print(" COFRE VIOLADO! ");

      dispararAlarme();
      break;
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// 12. VERIFICAÇÃO DA SENHA
// Compara a senha digitada com a senha definida no código.
// Se estiver correta, abre o cofre.
// Se estiver errada, aumenta o contador de falhas.
// ─────────────────────────────────────────────────────────────────────────────

void verificarSenha() {
  if (inputSenha == SENHA_MESTRA) {
    // Senha correta: abre o cofre
    ajustarAcesso(true);
    estadoAtual = ABERTO;
    lcd.clear();

    // Dois bips rápidos indicam acesso liberado
    tocarSomTecla();
    delay(50);
    tocarSomTecla();

  } else {
    // Senha incorreta: aumenta o número de tentativas falhas
    tentativasFalhas++;

    if (tentativasFalhas >= MAX_TENTATIVAS) {
      // Se atingir o limite de erros, bloqueia o sistema
      estadoAtual = BLOQUEADO;

    } else {
      // Caso ainda tenha tentativas disponíveis, mostra erro no LCD
      lcd.setCursor(0, 1);
      lcd.print("Senha Incorreta!");

      tocarSomErro();

      delay(1500);

      // Limpa a senha digitada para uma nova tentativa
      inputSenha = "";
      lcd.clear();
    }
  }
}

// ─────────────────────────────────────────────────────────────────────────────
// 13. SOM DE TECLA
// Emite um bip curto sempre que uma tecla válida é pressionada.
// ─────────────────────────────────────────────────────────────────────────────

void tocarSomTecla() {
  tone(PIN_BUZZER, FREQ_TECLA, DUR_TECLA);
}

// ─────────────────────────────────────────────────────────────────────────────
// 14. SOM DE ERRO
// Emite um som grave para indicar senha incorreta.
// ─────────────────────────────────────────────────────────────────────────────

void tocarSomErro() {
  tone(PIN_BUZZER, FREQ_ERRO, DUR_ERRO);
}

// ─────────────────────────────────────────────────────────────────────────────
// 15. ALARME SONORO
// Gera um som de alarme variando a frequência do buzzer.
// Esse efeito deixa o alerta mais perceptível do que um som fixo.
// ─────────────────────────────────────────────────────────────────────────────

void dispararAlarme() {
  for (int j = 0; j < 3; j++) {
    // Aumenta gradualmente a frequência do som
    for (int f = FREQ_ALARME_MIN; f < FREQ_ALARME_MAX; f += 10) {
      tone(PIN_BUZZER, f);
      delay(5);
    }

    // Diminui gradualmente a frequência do som
    for (int f = FREQ_ALARME_MAX; f > FREQ_ALARME_MIN; f -= 10) {
      tone(PIN_BUZZER, f);
      delay(5);
    }
  }

  // Para o buzzer ao final da sequência
  noTone(PIN_BUZZER);
}

// ─────────────────────────────────────────────────────────────────────────────
// 16. CONTROLE DO SERVO MOTOR
// Move o servo para abrir ou fechar a trava do cofre.
// abrir = true  -> servo vai para 90 graus.
// abrir = false -> servo volta para 0 grau.
// ─────────────────────────────────────────────────────────────────────────────

void ajustarAcesso(bool abrir) {
  if (abrir) {
    motorAcesso.write(SERVO_ABERTO);
  } else {
    motorAcesso.write(SERVO_FECHADO);
  }
}

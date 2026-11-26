const int pinoSensor = 3;       // Coletor do BC548 do sensor (com INPUT_PULLUP) — pino 3
const int pinoLedInterno = 13;  // LED embutido no Arduino Uno
const int pinoBuzzer = 8;       // Saída para o buzzer (ou base do 2.º BC548)

const unsigned long intervaloLeitura = 50;    // Leitura a cada 50 ms
const unsigned long tempoConfirmacao = 150;   // Tempo mínimo em LOW para confirmar água
const unsigned long tempoBipLigado = 150;     // Duração do bip (ms)
const unsigned long tempoBipDesligado = 250;  // Pausa entre bips (ms)

unsigned long ultimaLeitura = 0;
unsigned long inicioEstadoBaixo = 0;
unsigned long ultimoCicloBip = 0;

bool confirmouAgua = false;
bool estadoBipAlto = false;

void setup() {
  pinMode(pinoSensor, INPUT_PULLUP);
  pinMode(pinoLedInterno, OUTPUT);
  pinMode(pinoBuzzer, OUTPUT);

  digitalWrite(pinoLedInterno, LOW);
  digitalWrite(pinoBuzzer, LOW);

  Serial.begin(9600);
  Serial.println("Sistema iniciado no pino 3. Aguardando leitura...");
}

void loop() {
  unsigned long agora = millis();

  if (agora - ultimaLeitura >= intervaloLeitura) {
    ultimaLeitura = agora;
    int leitura = digitalRead(pinoSensor);

    // Com INPUT_PULLUP, leitura == LOW indica que o transistor conduziu (água detetada)
    if (leitura == LOW) {
      if (inicioEstadoBaixo == 0) {
        inicioEstadoBaixo = agora;
      }
      if ((agora - inicioEstadoBaixo) >= tempoConfirmacao && !confirmouAgua) {
        confirmouAgua = true;
        estadoBipAlto = true;
        ultimoCicloBip = agora;
        Serial.println("ALERTA: Nivel de agua atingido!");
      }
    } else {
      inicioEstadoBaixo = 0;
      if (confirmouAgua) {
        confirmouAgua = false;
        estadoBipAlto = false;
        digitalWrite(pinoLedInterno, LOW);
        digitalWrite(pinoBuzzer, LOW);
        Serial.println("Status normal: sem agua nas sondas.");
      }
    }
  }

  if (confirmouAgua) {
    unsigned long duracaoAtual = estadoBipAlto ? tempoBipLigado : tempoBipDesligado;
    if (agora - ultimoCicloBip >= duracaoAtual) {
      ultimoCicloBip = agora;
      estadoBipAlto = !estadoBipAlto;
    }

    digitalWrite(pinoLedInterno, estadoBipAlto ? HIGH : LOW);
    digitalWrite(pinoBuzzer, estadoBipAlto ? HIGH : LOW);
  }
}
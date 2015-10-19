void setup() {
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  int button, clicks, i;

  // Apaga todos os LEDs
  for (i = 0; i < 4; i++)
    digitalWrite(10 + i, LOW);

  // Aguarda o recebimento de dados
  if (Serial.available()) {
    // Lê mensagem enviada
    button = Serial.read();
    clicks = Serial.read();

    // Informa qual botao foi pressionado através do LED 10
    blink(10, button+1);
    // Quantidade de vezes que o botão foi pressionado
    blink(10+button+1, clicks);

    // Informa que mensagem foi processada corretamente
    Serial.write(0);
  }
}

void blink(ledNumber, times) {
  int i;

  for (i = 0; i < times; i++) {
    digitalWrite(ledNumber, HIGH);
    delay(800);
    digitalWrite(ledNumber, LOW);
    delay(200);
  }
}

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "ESP32-CAR";
const char* password = "12345678";

#define ENA 5
#define ENB 23
#define IN1 22
#define IN2 21
#define IN3 19
#define IN4 18

#define PAN_PIN 14
#define TILT_PIN 27
Servo servoPan;
Servo servoTilt;

#define TRIG_PIN 13
#define ECHO_PIN 12

int speedVal = 150;
WebServer server(80);

#define PWM_A 0
#define PWM_B 1

long distanceCM = 0;

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.println("🔌 ESP32 Access Point oluşturuldu.");
  Serial.print("📶 Ağ Adı: "); Serial.println(ssid);
  Serial.print("🔑 Şifre: "); Serial.println(password);
  Serial.print("🌐 IP Adresi: "); Serial.println(WiFi.softAPIP());

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  ledcSetup(PWM_A, 1000, 8);
  ledcAttachPin(ENA, PWM_A);
  ledcSetup(PWM_B, 1000, 8);
  ledcAttachPin(ENB, PWM_B);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  servoPan.setPeriodHertz(50);
  servoTilt.setPeriodHertz(50);
  servoPan.attach(PAN_PIN, 500, 2400);
  servoTilt.attach(TILT_PIN, 500, 2400);
  servoPan.write(90);
  servoTilt.write(90);

  stopMotors();

  server.on("/", handleRoot);
  server.on("/move", HTTP_POST, handleMove);
  server.on("/speed", HTTP_POST, handleSpeed);
  server.on("/distance", handleDistance);
  server.begin();
  Serial.println("🌐 Web sunucusu başlatıldı.");
}

void loop() {
  server.handleClient();
  distanceCM = readDistanceCM();
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html><html><head>
  <meta charset="UTF-8"><title>ESP32 Araç Kontrol</title>
  <style>
    body { font-family: Arial; text-align: center; background: #eaf6ff; padding-top: 30px; }
    h1 { color: #0077cc; }
    .btn {
      padding: 15px 30px;
      margin: 10px;
      font-size: 18px;
      background: #0077cc;
      color: white;
      border: none;
      border-radius: 8px;
      cursor: pointer;
    }
    .btn:hover { background: #005fa3; }
    input[type=range] { width: 300px; }
  </style>
</head>
<body>
  <h1>ESP32 Motor Kontrol</h1>
  <div>
    <button class="btn" onclick="sendMove('forward')">sag</button><br>
    <button class="btn" onclick="sendMove('left')">ileri</button>
    <button class="btn" onclick="sendMove('stop')">Dur</button>
    <button class="btn" onclick="sendMove('right')">geri</button><br>
    <button class="btn" onclick="sendMove('backward')">sol</button><br><br>
    <button class="btn" onclick="sendMove('pantilt')">Pan-Tilt Hareket</button>
  </div>
  <br>
  <label for="speed">Hız: </label>
  <input type="range" id="speed" min="0" max="255" value="150" oninput="updateSpeed(this.value)">
  <span id="speedVal">150</span>
  <h2 id="distance">Mesafe: -- cm</h2>
  <script>
    function sendMove(dir) {
      fetch('/move?dir=' + dir, { method: 'POST' });
    }
    function updateSpeed(val) {
      document.getElementById("speedVal").innerText = val;
      fetch('/speed?val=' + val, { method: 'POST' });
    }
    function updateDistance() {
      fetch('/distance')
        .then(res => res.text())
        .then(data => {
          document.getElementById("distance").innerText = "Mesafe: " + data + " cm";
        });
    }
    setInterval(updateDistance, 1000);
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleMove() {
  if (!server.hasArg("dir")) {
    server.send(400, "text/plain", "Komut eksik");
    return;
  }
  String dir = server.arg("dir");

  if (dir == "forward") forward();
  else if (dir == "backward") backward();
  else if (dir == "left") turnLeft();
  else if (dir == "right") turnRight();
  else if (dir == "pantilt") {
    servoPan.write(120);
    servoTilt.write(60);
    delay(1000);
    servoPan.write(60);
    servoTilt.write(120);
    delay(1000);
    servoPan.write(90);
    servoTilt.write(90);
  }
  else stopMotors();

  server.send(200, "text/plain", "OK");
}

void handleSpeed() {
  if (server.hasArg("val")) {
    speedVal = constrain(server.arg("val").toInt(), 0, 255);
    ledcWrite(PWM_A, speedVal);
    ledcWrite(PWM_B, speedVal);
    Serial.println("Yeni hız: " + String(speedVal));
  }
  server.send(200, "text/plain", "OK");
}

void handleDistance() {
  server.send(200, "text/plain", String(distanceCM));
}

long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

void forward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  ledcWrite(PWM_A, speedVal);
  ledcWrite(PWM_B, speedVal);
}
void backward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(PWM_A, speedVal);
  ledcWrite(PWM_B, speedVal);
}
void turnLeft() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  ledcWrite(PWM_A, speedVal);
  ledcWrite(PWM_B, speedVal);
}
void turnRight() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(PWM_A, speedVal);
  ledcWrite(PWM_B, speedVal);
}
void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  ledcWrite(PWM_A, 0);
  ledcWrite(PWM_B, 0);
}

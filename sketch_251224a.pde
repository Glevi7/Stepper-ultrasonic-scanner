import processing.serial.*;
import java.util.ArrayList;

Serial myPort;

//data from radar
int angle = 0;
int distance = -1;
int mode = 0;

//store ping
class Ping {
  int a;
  int d;
  int birthTime;

  Ping(int angle, int dist) {
    a = angle;
    d = dist;
    birthTime = millis();
  }
}

ArrayList<Ping> pings = new ArrayList<Ping>();

//GUI
PFont font;

void setup() {
  size(800, 500);
  smooth();

  myPort = new Serial(this, Serial.list()[0], 9600);
  myPort.bufferUntil('\n');

  font = createFont("Arial", 16);
  textFont(font);
}

void draw() {
  background(0);

  drawRadar();
  drawSweepLine();
  drawPings();
  drawHUD();
}

// serial input
void serialEvent(Serial myPort) {
  String data = myPort.readStringUntil('\n');
  if (data == null) return;

  data = trim(data);
  String[] values = split(data, ',');

  if (values.length == 3) {
    angle = int(values[0]);
    distance = int(values[1]);
    mode = int(values[2]);

    if (distance > 0 && distance <= 200) {
      pings.add(new Ping(angle, distance));
    }
  }
}

// radar interface
void drawRadar() {
  pushMatrix();
  translate(width/2, height);
  stroke(0, 255, 0);
  noFill();

  for (int r = 100; r <= 400; r += 100) {
    arc(0, 0, r*2, r*2, PI, TWO_PI);
  }

  for (int a = 0; a <= 180; a += 30) {
    float rad = radians(a);
    line(0, 0, 400*cos(rad), -400*sin(rad));
  }

  popMatrix();
}

void drawSweepLine() {
  pushMatrix();
  translate(width/2, height);
  stroke(0, 255, 0);
  float rad = radians(angle);
  line(0, 0, 400*cos(rad), -400*sin(rad));
  popMatrix();
}

// fading effect
void drawPings() {
  pushMatrix();
  translate(width/2, height);

  for (int i = pings.size() - 1; i >= 0; i--) {
    Ping p = pings.get(i);

    float age = millis() - p.birthTime;
    float life = 1500.0; 

    if (age > life) {
      pings.remove(i);
      continue;
    }

    float alpha = map(age, 0, life, 255, 0);
    float r = map(p.d, 0, 200, 0, 400);
    float rad = radians(p.a);

    stroke(255, 0, 0, alpha);
    strokeWeight(3);
    point(r*cos(rad), -r*sin(rad));
  }

  strokeWeight(1);
  popMatrix();
}

// hud
void drawHUD() {
  fill(0, 200);
  noStroke();
  rect(0, 0, 250, 70);

  fill(0, 255, 0);
  textAlign(LEFT, TOP);
  text("Angle: " + angle + "°", 10, 10);

  if (distance > 0) {
    text("Distance: " + distance + " cm", 10, 30);
  } else {
    text("Distance: ---", 10, 30);
  }

  textAlign(RIGHT, TOP);
  text(mode == 1 ? "MANUAL" : "AUTO", width - 10, 10);
}

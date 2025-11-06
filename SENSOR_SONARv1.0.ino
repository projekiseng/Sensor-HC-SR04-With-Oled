#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NewPing.h>
#include <math.h>

// Definisi pin untuk HC-SR04
#define TRIGGER_PIN  5
#define ECHO_PIN     18
#define MAX_DISTANCE 200 // Maksimum jarak dalam cm

// Konfigurasi OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Objek sonar
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int angle = 0; // Sudut awal radar
int direction = 3; // Kecepatan dan arah sweep (positif = kanan, negatif = kiri)

const int MAX_POINTS = 10; // Maksimum jumlah titik
struct Point {
    int x, y, lifetime;
};
Point points[MAX_POINTS];

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 gagal dimulai"));
        for (;;);
    }
    
    display.clearDisplay();
    display.display();
    Serial.println(F("Radar dimulai..."));
}

void loop() {
    display.clearDisplay();
    
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT - 1;

    // Gambar lingkaran radar (deteksi dari luar ke dalam)
    display.drawCircle(centerX, centerY, 50, WHITE);  // Lingkaran luar (jarak jauh)
    display.drawCircle(centerX, centerY, 35, WHITE);  // Lingkaran tengah (jarak sedang)
    display.drawCircle(centerX, centerY, 20, WHITE);  // Lingkaran dalam (jarak dekat)
    
    int distance = sonar.ping_cm();
    Serial.print(F("Jarak: "));
    Serial.print(distance);
    Serial.println(F(" cm"));
    
    if (distance == 0) distance = MAX_DISTANCE;
    
    float rad = angle * PI / 180;
    int sweepX = centerX + cos(rad) * 50;
    int sweepY = centerY - sin(rad) * 50;
    
    // Efek sweep (garis bolak-balik)
    display.drawLine(centerX, centerY, sweepX, sweepY, WHITE);
    
    // Menentukan di lingkaran mana objek berada
    if (distance < MAX_DISTANCE) {
        int objRadius;
        if (distance >= 130) {
            objRadius = 50;  // Objek jauh (lingkaran luar)
        } else if (distance >= 65) {
            objRadius = 35;  // Objek sedang (lingkaran tengah)
        } else {
            objRadius = 20;  // Objek dekat (lingkaran dalam)
        }

        int objX = centerX + cos(rad) * objRadius;
        int objY = centerY - sin(rad) * objRadius;

        Serial.print(F("Objek di: "));
        Serial.print(objX);
        Serial.print(F(", "));
        Serial.println(objY);
        
        for (int i = 0; i < MAX_POINTS; i++) {
            if (points[i].lifetime <= 0) { // Cari slot kosong
                points[i] = {objX, objY, 20}; // Titik bertahan 20 siklus
                break;
            }
        }
    }
    
    // Gambar titik objek dan gerakkan ke arah pusat
    for (int i = 0; i < MAX_POINTS; i++) {
        if (points[i].lifetime > 0) {
            display.fillCircle(points[i].x, points[i].y, 2, WHITE);
            points[i].y += 1; // Gerakkan titik ke arah pusat
            points[i].lifetime--;
        }
    }
    
    display.display();
    
    // Logika pergerakan garis bolak-balik
    angle += direction;
    if (angle >= 180 || angle <= 0) {
        direction = -direction; // Ubah arah sweep
    }
    
    delay(50);
}
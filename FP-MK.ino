#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6ASMAAEhR"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "vqN13dx5N-y7br84qsfoOWFqOJ2s2Nhd"

#include <Wire.h>
#include <WiFi.h>
#include <OneWire.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2 // Pin Data ke D2
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // I2C Address
const int DS18B20_INDEX = 0;

OneWire oneWire(ONE_WIRE_BUS); // Set instance comm untuk DS18B20
DallasTemperature sensors(&oneWire);
Adafruit_INA219 ina219;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char ssid[] = "DDoS";     // Ganti dengan SSID WiFi Anda
char password[] = "davidoff"; // Ganti dengan password WiFi Anda

bool inaDetected = false;

void setup(void)
{
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  while (!Serial)
  {
    // Akan menunda Zero, Leonardo, dsb. hingga konsol serial terbuka
    delay(1);
  }

  // Koneksi ke Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  Serial.println("INA219 & DS18B20 starting up...");
  sensors.begin(); // DS18B20 begin

  inaDetected = ina219.begin();
  if (!inaDetected)
  {
    Serial.println("Failed to find INA219 chip");
    // You can handle this case as needed
  }

  if (inaDetected)
  {
    ina219.setCalibration_16V_400mA();
  }

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Clear the display
  display.clearDisplay();
  display.display();
  delay(2000);
}

void loop(void)
{
  Blynk.run();

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("Done");

  float busvoltage = 0;
  float current_mA = 0;
  float power_mW = 0;
  float tempC = sensors.getTempCByIndex(DS18B20_INDEX);

  // Mengukur nilai-nilai INA219
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();

  // Menampilkan nilai-nilai di Serial Monitor
  Serial.print("Bus Voltage:   ");
  Serial.print(busvoltage);
  Serial.println(" V");
  Serial.print("Current:       ");
  Serial.print(current_mA);
  Serial.println(" mA");
  Serial.print("Power:         ");
  Serial.print(power_mW);
  Serial.println(" mW");

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temp:          ");
    Serial.print(tempC);
    Serial.println(" C");
  }
  else
  {
    Serial.println("Error: Could not read temperature data, check resistor Data to VCC");
  }

  // Menampilkan nilai-nilai di layar OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 10);
  display.print("Bus: ");
  display.print(busvoltage);
  display.println(" V");

  display.setCursor(0, 20);
  display.print("Current: ");
  display.print(current_mA);
  display.println(" mA");

  display.setCursor(0, 30);
  display.print("Power: ");
  display.print(power_mW);
  display.println(" mW");

  display.setCursor(0, 40);
  display.print("Temp: ");
  display.print(tempC);
  display.println(" C");

    display.display();

  Blynk.virtualWrite(0, busvoltage);
  Blynk.virtualWrite(1, current_mA);
  Blynk.virtualWrite(2, power_mW);
  Blynk.virtualWrite(3, tempC);

    Blynk.run();


  delay(2000);
}

#include <MultiFuncShield.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS A4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup()
{
  Serial.begin(9600);
  delay(1000);

  sensors.begin();

  MFS.initialize(&Timer1);

  delay(500);

  Serial.println("Sistema iniciado");
}

void loop()
{
  sensors.requestTemperatures();

  float temperatura = sensors.getTempCByIndex(0);

  // Display do shield
  MFS.write(temperatura, 1);

  // Monitor Serial
  Serial.print("Temperatura: ");
  Serial.print(temperatura, 1);
  Serial.println(" C");

  delay(1000);
}
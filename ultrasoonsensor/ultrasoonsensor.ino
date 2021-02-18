#define triggerPin 12     // Pin voor de trigger
#define echoPin 12         // Pin voor de echo
#define soundSpeed 343.0  // Snelheid van het geluid (m/s)

// Init variables
long echoTime = 0;
float distance = 0;

// Eenmalige doorloop (setup)
void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // Wacht op seriele monitor
  }
  Serial.println("--- Seriele monitor gestart ---");
  pinMode(triggerPin, OUTPUT);  // Zet trigger pin als uitgang
  pinMode(echoPin, INPUT);      // Zet echo pin als ingang
}

// Continue doorloop (loop)
void loop()
{
  // Zend startpuls (trigger)
  digitalWrite(triggerPin, LOW);    // Make sure trigger pin is low
  delayMicroseconds(2);             // for at least 2µs
  digitalWrite(triggerPin, HIGH);   // Set trigger pin HIGH
  delayMicroseconds(10);            // for at least 10µs
  digitalWrite(triggerPin, LOW);    // Set LOW again

  // Meet lengte van de echo puls
  echoTime = pulseInLong(echoPin, HIGH);
  // en bereken hiermee de afstand (in cm)
  distance = float(echoTime) / 2 * (soundSpeed / 10000.0);
  // Geef weer op de seriele monitor
  Serial.print("Afstand = ");
  Serial.print((String)distance);
  Serial.println(" cm");
  // And wacht 500ms
  delay(500);
}

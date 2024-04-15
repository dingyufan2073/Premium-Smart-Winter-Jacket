// Assign hardware pins
const int bat_adc = 1; 
const int temp_1_pin = 2;
const int temp_2_pin = 3;
const int temp_3_pin = 4;
const int temp_4_pin = 5;
const int relay_1 = 9;
const int relay_2 = 10;
const int button_int = 20; 
const int buzzer_pin = 21; 

// Assign data variable
double bat_voltage = 0;
double bat_voltage_threshold = 6;
double temp_1 = 0;
double temp_2 = 0;
double temp_3 = 0;
double temp_4 = 0;
int current_temp_threshold = 0;


// Define notes
int melody[] = {
  262, 196, 196, 220, 196, 0, 247, 262
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
// Define different temp threshold, 37°C human temperature, 22-30°C low blood pressure and possibly coma, 24-27°C unconfortable, 30-33˚C shallow breathing, 32˚ and 40˚C heat cramps and exhaustion, 40-50˚C heat exhaustion, 54 lead to heatstroke
int temp_threshold[] = {
  22, 25, 28, 31, 34, 37, 40
};


// Interrupt function
void interrupt(){
  // When interrupt triggered, switch the temperature threshold
  if (current_temp_threshold < (sizeof(temp_threshold)/sizeof(temp_threshold[0]))){
    current_temp_threshold++;
  }else if (current_temp_threshold == (sizeof(temp_threshold)/sizeof(temp_threshold[0]))){
    current_temp_threshold = 0;
  }
}


void setup() {
  // Start the serial monitor
  Serial.begin(115200);
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  // Interrupt when push button pressed, RISING edge
  attachInterrupt(digitalPinToInterrupt(button_int), interrupt, RISING);
}

void loop() {
  // Read the batteries voltage divider output voltage
  bat_voltage = analogRead(bat_adc) * 3.3 / 4095;
  // Find the acutal battery voltage, R1 = 5k, R2 = 3k, Vin(max)=8.4V
  bat_voltage = bat_voltage * 8000 / 3000;
  // If low battery capacity, alert user to charge battery, non-stop
  while (bat_voltage <= bat_voltage_threshold){
    digitalWrite(relay_1, HIGH);
    digitalWrite(relay_2, HIGH);
    for (int thisNote = 0; thisNote < 8; thisNote++) {
      // to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(buzzer_pin, melody[thisNote], noteDuration);
      // to distinguish the notes, set a minimum time between them.
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(buzzer_pin);
    }
  }
  //Read the temperature signal voltage
  temp_1 = analogRead(temp_1_pin) * 3.3 / 4095;
  temp_2 = analogRead(temp_1_pin) * 3.3 / 4095;
  temp_3 = analogRead(temp_1_pin) * 3.3 / 4095;
  temp_4 = analogRead(temp_1_pin) * 3.3 / 4095;
  // Convert the voltage to actual temperature, 10mV/deg
  temp_1 = temp_1 * 1000 / 10;
  temp_2 = temp_2 * 1000 / 10;
  temp_3 = temp_3 * 1000 / 10;
  temp_4 = temp_4 * 1000 / 10;
  // if either one of the temperature sensor exceeds the threshold temperature (front body)
  if ( (temp_1 > temp_threshold[current_temp_threshold]) || (temp_2 > temp_threshold[current_temp_threshold]) ){
    // Turn on the relay will turn off the heater
    digitalWrite(relay_1, HIGH);
  }else{
     // Turn off the relay will turn on the heater
    digitalWrite(relay_1, LOW);
  }
  // if either one of the temperature sensor exceeds the threshold temperature (back body)
  if ( (temp_3 > temp_threshold[current_temp_threshold]) || (temp_4 > temp_threshold[current_temp_threshold]) ){
    // Turn on the relay will turn off the heater
    digitalWrite(relay_2, HIGH);
  }else{
     // Turn off the relay will turn on the heater
    digitalWrite(relay_2, LOW);
  }

  //delay 30s
  delay(30000);
}

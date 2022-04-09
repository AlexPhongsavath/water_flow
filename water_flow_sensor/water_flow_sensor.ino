
// WATER SENSOR "YF- S201" and "Water Relay" in MCU "ESP"
// IMPLEMENTING the code base from https://www.seeedstudio.com/blog/2020/05/11/how-to-use-water-flow-sensor-with-arduino/

//---------------------------------------------------------------------------------
//Libraries:
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//---------------------------------------------------------------------------------
// Configuration:

// BLYNK_______
  #define BLYNK_PRINT Serial // allow other devices to communicate with the board


// connection and authorization to the internet
char auth[] = "AuthToken"; // token authorization form BLYNK
char ssid[] = "Network name"; // wifi name
char password[] = "Network password"; // wifi password


// *water relay
const int relay_pin = 5;

// *water flow sensor____
volatile int pulse_count; // Measure "pulses" from water sensor in RAM for more accuracy
int flow_sensor_pin = 2;
int liter_hour;


// *time control____
const int happen_every = 500;
unsigned long current_time;
unsigned long previous_time; 

//---------------------------------------------------------------------------------------------------------------

// interupt service routine function 
// To learn more https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
void pulse_detecting(){
  pulse_count++;
}


void setup() {

  //start using Blynk server
  Blynk.begin(auth, ssid, password);

  // water relay____
  pinMode(relay_pin, OUTPUT);
  
  // water flow sensor_____
  pinMode(flow_sensor_pin, INPUT);
  digitalWrite(flow_sensor_pin, HIGH); // optional for dectecting when the sensor stop working 
  attachInterrupt(digitalPinToInterrupt(flow_sensor_pin), pulse_detecting, RISING); // the pulse_count is come from this interupt fucntion
  sei(); // enable interupt code
  
  // time____
  current_time = millis();
  
}

void loop() {

  relay_function();

  //show result in every 1000 miliseconds or 1 second
 if (current_time - previous_time >= happen_every){
      int liter_hour = water_rate_calculation(pulse_count);
      pulse_count = 0; // reset the value of pulse_count

      // displaying
      Serial.print(liter_hour, DEC); // show value in decimal
      Serial.println("Liter/hour");
      Blynk.virtualWrite(V2, liter_hour);
      Blynk.run();

    // update time
     previous_time = current_time; 
  }
  
}


/* Sending LOW signal to make the current flow 
 * Sending HIGH signal to make the current flow stop
 * 
 */
void relay_function(){
  const int interval = 5000; 
  if(current_time - previous_time >= interval){
      digitalWrite(relay_pin, LOW);
      Serial.println("Current Flowing");
      
      digitalWrite(relay_pin, HIGH);
      Serial.println("Current not flowing");
      previous_time = current_time;
  }
 
  
}


int water_rate_calculation(int pulse_count){
      
      liter_hour = (pulse_count * 60 / 7.5); //liter per hour // the 450 will be different from other type of "water flow sensor"
      /* optional:
        //liter_second = pulse_count/ 450; // liter per second
        //liter_minute = (pulse_count * 60)/ 450 or (pulse_count / 7.5 ) // liter per minute
      */
      return liter_hour;
}

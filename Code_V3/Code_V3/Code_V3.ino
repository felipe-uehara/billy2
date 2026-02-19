#include <Bluepad32.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 5 //pin of RGB control
#define NUM_LEDS 1 //two leds blinking together

/* teste feito em cima do código-exemplo "Controller" da lib ESP32_Bluepad

Fonte: https://racheldebarros.com/esp32-projects/connect-your-game-controller-to-an-esp32/
Fonte 2: https://racheldebarros.com/esp32-projects/control-leds-motors-servos-with-a-game-controller-and-esp32/

Board: DOIT ESP32 DEVKIT V1
*/

const int LED_state = 2; //LED that indicates the current state of the conexion between the ESP32 and the Dualshock controller

//Lists of Drawers
const int D1_Forward = 13, D1_Backward = 14, 
          D2_Forward = 26, D2_Backward = 32; 

//Push-Buttons of the drawers (linked by a multiplexer)
const int PB11 = 25, PB12 = 27, PB21 = 19, PB22 = 23;
/*
PB11 - back push-button of the drawer 1 
PB12 - front push-button of the drawer 1 
PB21 - back push-button of the drawer 2 
PB22 - front push-button of the drawer 2 
*/


//Locomotion Pins
const int Left_Forward = 4, Left_Backward = 18, 
          Right_Forward = 33, Right_Backward = 22;

//Initial PWM Duty-Cycle for the drawers
const int speed = 255; // 100%

//Maximum PWM's Duty-Cycle of Locomotion
const int max_speed_loc = 200;

//PWM Duty Cycle of the motors
int motor1, motor2;

int drawer;
//Direction of the motor: 0 (forward), 1 (backward)
int direction1, direction2;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

ControllerPtr myControllers[BP32_MAX_GAMEPADS];


// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      digitalWrite(LED_state, HIGH);
      break;
      }
    }

    if (!foundEmptySlot) {
      Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      digitalWrite(LED_state, HIGH);
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      digitalWrite(LED_state, LOW);
      break;
    }
  }

    if (!foundController) {
      Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

// ========= SEE CONTROLLER VALUES IN SERIAL MONITOR ========= //

void dumpGamepad(ControllerPtr ctl) {
  Serial.printf(
  "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
  "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
  ctl->index(),        // Controller Index
  ctl->dpad(),         // D-pad
  ctl->buttons(),      // bitmask of pressed buttons
  ctl->axisX(),        // (-511 - 512) left X Axis
  ctl->axisY(),        // (-511 - 512) left Y axis
  ctl->axisRX(),       // (-511 - 512) right X axis
  ctl->axisRY(),       // (-511 - 512) right Y axis
  ctl->brake(),        // (0 - 1023): brake button
  ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
  ctl->miscButtons(),  // bitmask of pressed "misc" buttons
  ctl->gyroX(),        // Gyro X
  ctl->gyroY(),        // Gyro Y
  ctl->gyroZ(),        // Gyro Z
  ctl->accelX(),       // Accelerometer X
  ctl->accelY(),       // Accelerometer Y
  ctl->accelZ()        // Accelerometer Z
  );
}

// ========= GAME CONTROLLER ACTIONS SECTION ========= //

void processGamepad(ControllerPtr ctl) {
  // There are different ways to query whether a button is pressed.
  // By query each button individually:
  //  a(), b(), x(), y(), l1(), etc...

  

  if (ctl->axisY() > -25 && ctl->axisY() < 25){
    motor1 = 0;
  }
  

  if (ctl->axisRY() > -25 && ctl->axisRY() < 25){
    motor2 = 0;
  }
      

    if(ctl->axisY() <= -25){
      motor1 = map(ctl->axisY(), 0, -508, 0, max_speed_loc); 
      direction1 = 1;
      Serial.println("Left_Forward");
    }

    if(ctl->axisY() >= 25){
      motor1 = map(ctl->axisY(), 0, 512, 0, max_speed_loc);
      direction1 = 0; // 0 = para trás
      Serial.println("LeftBackward");
    }

    if(ctl->axisRY() <= -25){
      motor2 = map(ctl->axisRY(), 0, -508, 0, max_speed_loc);
      direction2 = 1; // 1 = para frente
      Serial.println("RightForward");
    }
        

    if(ctl->axisRY() >= 25){
      motor2 = map(ctl->axisRY(), 0, 512, 0, max_speed_loc);
      direction2 = 0;
      Serial.println("RightBackward");
    }
    
    locomotions(motor1, direction1, motor2, direction2); 

 

  //Drawer 1 - Closing//
  //== PS4 X button = 0x0001 ==//
  if (ctl->buttons() == 0x0001) {
     // code for when X button is pushed
    if (digitalRead(PB11)){
      analogWrite(D1_Backward, 0);
      //drawers(0);
      Serial.println("Gaveta 1 já fechada!");
    }
      
    if(digitalRead(PB11) == 0){
      analogWrite(D1_Backward, 180);
      //drawers(4);
      Serial.println("Gaveta 1 fechando...");
    }    
  }

  if (ctl->buttons() != 0x0001 || digitalRead(PB11) == 1) 
    analogWrite(D1_Backward, 0);
  
  
  //drawers(drawer);


  //LED Strip - WHITE //
  //== PS4 Dpad UP button = 0x01 ==//
  if (ctl->dpad() == 0x01) 
    setPixelColor(255,255,255); //white 
   

  //LED Strip - RED //
  //==PS4 Dpad DOWN button = 0x02==//
  if (ctl->dpad() == 0x02)
    setPixelColor(255,0,0); 

  //LED Strip - BLUE //
  //== PS4 Dpad LEFT button = 0x08 ==//
  if (ctl->dpad() == 0x08) {
    // code for when dpad left button is pushed
    setPixelColor(0,0,255); 
  }


  //LED Strip - YELLOW//
  //== PS4 Dpad RIGHT button = 0x04 ==//
  if (ctl->dpad() == 0x04) {
    setPixelColor(255,170,0);  
  }

  //LED Strip - OFF//
  //== PS4 L1 trigger button = 0x0010 ==//
  if (ctl->buttons() == 0x0010) {
    setPixelColor(0,0,0);
  }


  //== PS4 R2 trigger button = 0x0080 ==//
  if (ctl->buttons() == 0x0080) {
    // code for when R2 button is pushed
    digitalWrite(LED_state, LOW);
    //digitalWrite(LED_strip, LOW);
    analogWrite(D1_Forward, 0);
    analogWrite(D1_Backward, 0);
    analogWrite(D2_Forward, 0);
    analogWrite(D2_Backward, 0);
    analogWrite(Left_Forward, 0);
    analogWrite(Left_Backward, 0);
    analogWrite(Right_Forward, 0);
    analogWrite(Right_Backward, 0);
    Serial.println("");
    Serial.println("Periféricos Zerados!");
    Serial.println("");
  }
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
         processGamepad(myController);
      }
      else {
        Serial.println("Unsupported controller");
      }
    }
  }
}

void locomotions(int motor1, int direction1, int motor2, int direction2){
  if (direction1==1){
    analogWrite(Left_Forward, motor1);
    analogWrite(Left_Backward, 0);
    Serial.println("motor1, direction1");
  }

  if (direction1==0){
    analogWrite(Left_Forward, 0);
    analogWrite(Left_Backward, motor1);
  }

  if (direction2==1){
    analogWrite(Right_Forward, motor2);
    analogWrite(Right_Backward, 0);
  }

  if (direction2==0){
    analogWrite(Right_Forward, 0);
    analogWrite(Right_Backward, motor2);
  }

  /*else{
    analogWrite(Right_Forward, 0);
    analogWrite(Right_Backward, 0);
    analogWrite(Left_Forward, 0);
    analogWrite(Left_Backward, 0);
  }*/
  
  Serial.println(motor1);
  Serial.println(direction1);
  Serial.println(motor2);
  Serial.println(direction2);
}

void drawers(int dd1){
  /*  dd = indica qual motor vai rodar e em qual direção
      0 = gaveta 1 Fechando
      1 = gaveta 1 Abrindo
      2 = gaveta 2 Fechando
      3 = gaveta 2 Abrindo 
      4 = tudo parado
  */

  if (dd1 == 0){
    analogWrite(D1_Forward, 0);
    analogWrite(D1_Backward, speed);
  }

  if (dd1 == 1){
    analogWrite(D1_Forward, speed);
    analogWrite(D1_Backward, 0);
    Serial.println("frente m1");
  }

  if (dd1 == 2){
    analogWrite(D2_Forward, 0);
    analogWrite(D2_Backward, speed);
  }

  if (dd1 == 3){
    analogWrite(D2_Forward, speed);
    analogWrite(D2_Backward, 0);
  }

  if (dd1 == 4){
    analogWrite(D1_Forward, 0);
    analogWrite(D1_Backward, 0);
    analogWrite(D2_Forward, 0);
    analogWrite(D2_Forward, 0);
  }
  Serial.println("fim drawers");
}

void setPixelColor(int red, int green, int blue) {

  strip.setPixelColor(0, strip.Color(red, green, blue));
  strip.show();

}


// Arduino setup function. Runs in CPU 1
void setup() {
  Serial.begin(115200);

  //Definition of the pinage
  pinMode(LED_state, OUTPUT);
  //pinMode(LED_strip, OUTPUT);

  pinMode(PB11, INPUT_PULLDOWN);
  pinMode(PB12, INPUT_PULLDOWN);
  pinMode(PB21, INPUT_PULLDOWN);
  pinMode(PB22, INPUT_PULLDOWN);


  pinMode(D1_Forward, OUTPUT);
  pinMode(D1_Backward, OUTPUT);
  pinMode(D2_Forward, OUTPUT);
  pinMode(D2_Backward, OUTPUT);

  pinMode(Left_Forward, OUTPUT);
  pinMode(Left_Backward, OUTPUT);
  pinMode(Right_Forward, OUTPUT);
  pinMode(Right_Backward, OUTPUT);

  //Initialize with all components in zero
  digitalWrite(LED_state, LOW);
  //digitalWrite(LED_strip, LOW);
  analogWrite(D1_Forward, 0);
  analogWrite(D1_Backward, 0);
  analogWrite(D2_Forward, 0);
  analogWrite(D2_Backward, 0);
  analogWrite(Left_Forward, 0);
  analogWrite(Left_Backward, 0);
  analogWrite(Right_Forward, 0);
  analogWrite(Right_Backward, 0);

  motor1 = constrain(motor1, 0, 255);
  motor2 = constrain(motor2, 0, 255);

  drawer= 4;


  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But it might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();

  // Enables mouse / touchpad support for gamepads that support them.
  // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
  // - First one: the gamepad
  // - Second one, which is a "virtual device", is a mouse.
  // By default, it is disabled.
  BP32.enableVirtualDevice(false);

  strip.begin();
  strip.show();
}

// Arduino loop function. Runs in CPU 1.
void loop() {
  // This call fetches all the controllers' data.
  // Call this function in your main loop.

  //bool LED_strip_last_state = LOW;
  bool dataUpdated = BP32.update();
  if (dataUpdated)
    processControllers();

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    // vTaskDelay(1);
  delay(100);
  //Serial.println("oii");
}
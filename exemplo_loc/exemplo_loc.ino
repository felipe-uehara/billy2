#include <Bluepad32.h>

/* teste feito em cima do código-exemplo "Controller" da lib ESP32_Bluepad

Fonte: https://racheldebarros.com/esp32-projects/connect-your-game-controller-to-an-esp32/
Fonte 2: https://racheldebarros.com/esp32-projects/control-leds-motors-servos-with-a-game-controller-and-esp32/

Board: DOIT ESP32 DEVKIT V1
*/

const int LED_state = 2;


//Locomotion Pins
const int Left_Forward = 13, Left_Backward = 14,
          Right_Forward = 18, Right_Backward = 19;


//Initial PWM Duty-Cycle for the drawers
const int speed = 30;  //30/255 = 11.7%

int motor1, motor2;

int direction1, direction2;


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
  
  /*if (ctl->axisY() <= -25 || ctl->axisY() >= 25 || ctl->axisRY() <= -25 || ctl->axisRY() >= 25) {
    if (ctl->axisY() > -25 && ctl->axisY() < 25 && ctl->axisRY() > -25 && ctl->axisRY() < 25) {
      //IDLE State
      analogWrite(Left_Forward, 0);
      analogWrite(Left_Backward, 0);
      analogWrite(Right_Forward, 0);
      analogWrite(Right_Backward, 0);
    }
    int current_axisY = ctl->axisY();
    int current_axisRY = ctl->axisRY();

    if (ctl->axisY() > -25 && ctl->axisY() < 25){
      analogWrite(Left_Backward, 0);
      analogWrite(Left_Forward, 0);
      direction1 = 1; //1 = para frente
      motor1 = 0;
    }

    if (ctl->axisRY() > -25 && ctl->axisRY() < 25){
      analogWrite(Right_Backward, 0);
      analogWrite(Right_Forward, 0);
      direction2 = 1;
      motor2 = 0;
    }*/

    if (ctl->axisY() > -25 && ctl->axisY() < 25)
      motor1 = 0;

    if (ctl->axisRY() > -25 && ctl->axisRY() < 25)
      motor2 = 0;

    if(ctl->axisY() <= -25){
      motor1 = map(ctl->axisY(), 0, -508, 0, 255); 
      //analogWrite(Left_Forward, motor1);
      direction1 = 1;
      Serial.println("oiii");
    }

    if(ctl->axisY() >= 25){
      motor1 = map(ctl->axisY(), 0, 512, 0, 255);
      //analogWrite(Left_Backward, motor1);
      direction1 = 0; // 0 = para trás
      Serial.println("tchauu");
    }

    if(ctl->axisRY() <= -25){
      motor2 = map(ctl->axisRY(), 0, -508, 0, 255);
      //analogWrite(Right_Forward, motor2);
      direction2 = 1; // 1 = para frente
      Serial.println("Thunder");
    }
        

    if(ctl->axisRY() >= 25){
      motor2 = map(ctl->axisRY(), 0, 512, 0, 255);
      //analogWrite(Right_Backward, motor2);
      direction2 = 0;
      Serial.println("KTorze");
    }
    
    
      
    Serial.println(motor1);
    Serial.println(motor2);
    Serial.println(direction1);
    Serial.println(direction2);
    /*Serial.println(ctl->axisRY());
    Serial.println(ctl->axisY());*/
    //teste();
    locomotions(motor1, direction1, motor2, direction2);
    
    
    
/*
    if (ctl->axisY() > -25 && ctl->axisY() < 25 && ctl->axisRY() <= -25) {
      while(ctl->axisY() == current_axisY){
      //Left Motor Stopped and Rigth Motor Moving Forward
      //motor1 = -255 * ctl->axisRY()/(483);  //Mod(-508-(-25)), in which -508 is the max and -25 the min
      motor1 = map(ctl->axisRY(), 0, -508, 0, 255);
      /*analogWrite(Right_Forward, motor1);
      analogWrite(Right_Backward, 0);
      analogWrite(Left_Forward, 0);
      analogWrite(Left_Backward, 0);*/

    /*  Serial.println("parado frente");
      //Serial.print(motor1);

      //teste(motor1, 0);
      if (ctl->axisY() != current_axisY)
        teste();
      }
      
    }*/
/*
    if (ctl->axisY() > -25 && ctl->axisY() < 25 && ctl->axisRY() >= 25) {
      //Left Motor Stopped and Rigth Motor Moving Backward
      analogWrite(Left_Forward, 0);
      analogWrite(Left_Backward, 0);

      //motor1 = 255 * ctl->axisRY() / (483);  //Mod(512-25), in which 512 is the max and 25 the min
      motor1 = map(ctl->axisRY(), 0, 508, 0, 255);
      analogWrite(Right_Forward, 0);

      analogWrite(Right_Backward, motor1);
      Serial.print("parado trás ");
      Serial.println(motor1);
    }

    if (ctl->axisY() <= -25 && ctl->axisRY() > -25 && ctl->axisRY() < 25) {
      //Left Motor Moving Forward and Rigth Motor Stopped
      //motor2 = -255 * ctl->axisY() / 483;
      motor2 = map(ctl->axisY(), 0, -508, 0, 255);
      //Serial.println("oiiiii");
      analogWrite(Left_Forward, motor2);
      analogWrite(Left_Backward, 0);

      //motor1 = ctl->axisRY()/(483); //Mod(512-25), in which 512 is the max and 25 the min
      analogWrite(Right_Forward, 0);
      analogWrite(Right_Backward, 0);
      Serial.println("frente parado");
      Serial.println(motor2);
    }

    if (ctl->axisY() >= 25 && ctl->axisRY() > -25 && ctl->axisRY() < 25) {
      //Left Motor Moving Backward and Rigth Motor Stopped
      //motor2 = 255 * ctl->axisY() / 483;
      motor2 = map(ctl->axisY(), 0, 508, 0, 255);

      analogWrite(Left_Forward, motor2);
      analogWrite(Left_Backward, 0);

      //motor1 = ctl->axisRY()/(487); //Mod(512-25), in which 512 is the max and 25 the min
      analogWrite(Right_Forward, 0);
      analogWrite(Right_Backward, 0);

      Serial.println("trás parado");
      Serial.println(motor2);
    }

    if (ctl->axisY() <= -25 && ctl->axisRY() <= -25) {
      //Both Motors moving Forward
      //motor2 = -255 * ctl->axisY() / 483;
      motor2 = map(ctl->axisY(), 0, -508, 0, 255);
      analogWrite(Left_Forward, motor2);
      analogWrite(Left_Backward, 0);

      //motor1 = -255 * ctl->axisRY() / (487);  //Mod(512-25), in which 512 is the max and 25 the min
      motor1 = map(ctl->axisRY(), 0, -508, 0, 255);
      analogWrite(Right_Forward, motor1);
      analogWrite(Right_Backward, 0);
      
      Serial.print("frente frente");
      Serial.println(motor1);
      Serial.print ("motor2: ");
      Serial.println(motor2);
    }
/*
    if (ctl->axisY() >= 25 && ctl->axisRY() >= 25) {
      //Both Motors moving Backward
      //motor2 = 255 * ctl->axisY() / 483;
      motor2 = map(ctl->axisY(), 0, 508, 0, 255);
      analogWrite(Left_Forward, 0);
      analogWrite(Left_Backward, motor2);

      //motor1 = 255 * ctl->axisRY() / (487);  //Mod(512-25), in which 512 is the max and 25 the min
      motor1 = map(ctl->axisRY(), 0, 508, 0, 255);
      analogWrite(Right_Forward, 0);
      analogWrite(Right_Backward, motor1);


      Serial.println("trás trás");
      Serial.print(motor1);
      Serial.print("motor2: ");
      Serial.println(motor2);
    }

    if (ctl->axisY() <= -25 && ctl->axisRY() >= 25) {
      //Left Motor moving Backward and Right Motor moving Forward
      //motor2 = 255 * ctl->axisY() / 483;
      motor2 = map(ctl->axisY(), 0, 508, 0, 255);
      analogWrite(Left_Forward, 0);
      analogWrite(Left_Backward, motor2);

      //motor1 = -255 * ctl->axisRY() / (487);  //Mod(512-25), in which 512 is the max and 25 the min
      motor1 = map(ctl->axisRY(), 0, -508, 0, 255);
      analogWrite(Right_Forward, motor1);
      analogWrite(Right_Backward, 0);

      Serial.println("trás frente");
      Serial.print(motor1);
      Serial.print("motor2: ");
      Serial.println(motor2);
    }
  

  //== LEFT JOYSTICK DEADZONE ==//
  if (ctl->axisY() > -25 && ctl->axisY() < 25 && ctl->axisX() > -25 && ctl->axisX() < 25) {
    // code for when left joystick is at idle
    analogWrite(Left_Forward, 0);
    analogWrite(Left_Backward, 0);
    analogWrite(Right_Forward, 0);
    analogWrite(Right_Backward, 0);
  }

  //== RIGHT JOYSTICK DEADZONE ==//
  if (ctl->axisRY() > -25 && ctl->axisRY() < 25 && ctl->axisRX() > -25 && ctl->axisRX() < 25) {
    // code for when left joystick is at idle
    analogWrite(Left_Forward, 0);
    analogWrite(Left_Backward, 0);
    analogWrite(Right_Forward, 0);
    analogWrite(Right_Backward, 0);
  }

  //== PS4 R2 trigger button = 0x0080 ==//
  if (ctl->buttons() == 0x0080) {
    // code for when R2 button is pushed
    digitalWrite(LED_state, LOW);
    analogWrite(Left_Forward, 0);
    analogWrite(Left_Backward, 0);
    analogWrite(Right_Forward, 0);
    analogWrite(Right_Backward, 0);
    Serial.println("");
    Serial.println("Periféricos Zerados!");
    Serial.println("");
  }


  else {
    analogWrite(Left_Forward, 0);
    analogWrite(Left_Backward, 0);
    analogWrite(Right_Forward, 0);
    analogWrite(Right_Backward, 0);
  }
  */
  delay(10);
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      } else {
        Serial.println("Unsupported controller");
      }
    }
  }
}

void teste(){
  analogWrite(Left_Forward, 110);
  analogWrite(Left_Backward, 110);
  analogWrite(Right_Forward, 110);
  analogWrite(Right_Backward, 110);
  Serial.println("testeeeeeeeeeeee");

  //Serial.println(motor1);
}

void locomotions(int motor1, int direction1, int motor2, int direction2){
  if (direction1==1){
    analogWrite(Left_Forward, motor1);
    analogWrite(Left_Backward, 0);
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
    analogWrite(Left_Forward, 0);
    analogWrite(Left_Backward, 0);
    analogWrite(Right_Forward, 0);
    analogWrite(Right_Backward, 0);
  }*/
}


// Arduino setup function. Runs in CPU 1
void setup() {
  Serial.begin(115200);

  pinMode(Left_Forward, OUTPUT);
  pinMode(Left_Backward, OUTPUT);
  pinMode(Right_Forward, OUTPUT);
  pinMode(Right_Backward, OUTPUT);

  //Initialize with all components in zero
  analogWrite(Left_Forward, 0);
  analogWrite(Left_Backward, 0);
  analogWrite(Right_Forward, 0);
  analogWrite(Right_Backward, 0);

  pinMode(LED_state, OUTPUT);

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
}

// Arduino loop function. Runs in CPU 1.
void loop() {
  // This call fetches all the controllers' data.
  // Call this function in your main loop.

  //bool LED_strip_last_state = LOW;
  bool dataUpdated = BP32.update();
  if (dataUpdated)
    processControllers();
    //teste();

  // The main loop must have some kind of "yield to lower priority task" event.
  // Otherwise, the watchdog will get triggered.
  // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
  // Detailed info here:
  // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

  //vTaskDelay(1);
  //teste();
  delay(10);

  //analogWrite
}
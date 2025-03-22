#include <ArduinoBLE.h>

// BLE UUIDs (matching your MicroPython remote)
#define REMOTE_SERVICE_UUID "1848"
#define BUTTON_CHAR_UUID "2A6E"
#define DEVICE_NAME "KevsRobots"

// Set the time between motor on and motor off
int wait_in_milliseconds = 250;

// Variables
BLEDevice peripheral;
bool connected = false;

int motor_A = 12;
int motor_B = 13;

int motor_A_speed = 10; // may also be `10` or `3`
int motor_B_speed = 11; //


void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial Monitor to open

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // setup motors
  pinMode(motor_A, OUTPUT);
  pinMode(motor_B, OUTPUT);

  Serial.println("BLE Central started. Scanning for KevsRobots...");
}

void loop() {
  // Scan for the remote if not connected
  if (!connected) {
    BLE.scanForName(DEVICE_NAME);
    BLEDevice foundDevice = BLE.available();

    if (foundDevice) {
      if (foundDevice.localName() == DEVICE_NAME) {
        Serial.println("Found KevsRobots!");
        BLE.stopScan();
        connectToPeripheral(foundDevice);
      }
    }
  }

  // Poll for notifications when connected
  if (connected && peripheral.connected()) {
    BLE.poll();
  } else if (connected && !peripheral.connected()) {
    Serial.println("Disconnected from KevsRobots!");
    connected = false;
    Serial.println("Scanning for KevsRobots again...");
  }
}

void connectToPeripheral(BLEDevice device) {
  peripheral = device;

  Serial.print("Connecting to ");
  Serial.println(peripheral.localName());
  
  if (peripheral.connect()) {
    Serial.println("Connected successfully!");
  } else {
    Serial.println("Connection failed!");
    return;
  }

  // Discover attributes
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered.");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  // Get the service
  BLEService service = peripheral.service(REMOTE_SERVICE_UUID);
  if (!service) {
    Serial.println("Service not found!");
    peripheral.disconnect();
    return;
  }

  // Get the button characteristic
  BLECharacteristic buttonChar = service.characteristic(BUTTON_CHAR_UUID);
  if (!buttonChar) {
    Serial.println("Characteristic not found!");
    peripheral.disconnect();
    return;
  }

  // Subscribe to notifications
  if (buttonChar.canSubscribe() && buttonChar.subscribe()) {
    Serial.println("Subscribed to button characteristic notifications.");
    buttonChar.setEventHandler(BLEUpdated, buttonCharHandler);  // Fixed: Use BLEUpdated
    connected = true;
  } else {
    Serial.println("Failed to subscribe to notifications!");
    peripheral.disconnect();
    return;
  }
}

void forward(){

  Serial.println("Moving motors forward");
  // set the direction to forward
  digitalWrite(motor_A, LOW);
  digitalWrite(motor_B, HIGH);

  // set to full speed
  analogWrite(motor_A_speed, 255);
  analogWrite(motor_B_speed, 255);

  // wait
  delay(wait_in_milliseconds);
}

void backward(){

  Serial.println("Moving motors backward");
  // set the direction to forward
  digitalWrite(motor_A, HIGH);
  digitalWrite(motor_B, LOW);

  // set to full speed
  analogWrite(motor_A_speed, 255);
  analogWrite(motor_B_speed, 255);

  // wait
  delay(wait_in_milliseconds);
}

void left(){

  Serial.println("Moving motors left");
  // set the direction to forward
  digitalWrite(motor_A, LOW);
  digitalWrite(motor_B, LOW);

  // set to full speed
  analogWrite(motor_A_speed, 255);
  analogWrite(motor_B_speed, 255);

  // wait
  delay(wait_in_milliseconds);
}

void right(){

  Serial.println("Moving motors right");
  // set the direction to forward
  digitalWrite(motor_A, HIGH);
  digitalWrite(motor_B, HIGH);

  // set to full speed
  analogWrite(motor_A_speed, 255);
  analogWrite(motor_B_speed, 255);

  // wait
  delay(wait_in_milliseconds);
}

void stop(){

  // stop the motors
  analogWrite(motor_A_speed, 0);
  analogWrite(motor_B_speed, 0);
}

// Callback function for characteristic events
void buttonCharHandler(BLEDevice device, BLECharacteristic characteristic) {
  uint8_t buffer[20];  // Buffer for the characteristic value (max 20 bytes)
  int len = characteristic.valueLength();

  if (len > 0) {
    characteristic.readValue(buffer, len);
    String command = String((char*)buffer).substring(0, len);
    Serial.print("Received command: ");
    Serial.println(command);

    // Example: Check some commands
    if (command == "Up_down") {
      Serial.println("Up button pressed!");
      forward();
    } else if (command == "Up_up") {
        stop();
      }
     else if (command == "Down_up") {
      Serial.println("Down button released!");
        stop();
    }
    else if (command == "Down_down") {
      Serial.println("Down button released!");
      backward();
    }
     else if (command == "Left_up") {
      Serial.println("Left button released!");
        stop();
    }
    else if (command == "Left_down") {
      Serial.println("Left button released!");
      left();
    }
     else if (command == "Right_up") {
      Serial.println("Right button released!");
        stop();
    }
    else if (command == "Right_down") {
      Serial.println("Right button released!");
      right();
    }
    // Add more command checks as needed
  }
}

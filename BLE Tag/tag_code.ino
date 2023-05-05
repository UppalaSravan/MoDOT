//Server Code
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEUUID.h>

#define SERVICE_UUID "7ecbb852-c08c-11ed-afa1-0242ac120002"
#define CHARACTERISTIC_UUID "a9ba8b24-c08c-11ed-afa1-0242ac120002"
BLEUUID my_uuid = BLEUUID("6362b48b-130c-4e9e-9e48-c068f959084b");
// BLEUUID my_uuid = BLEUUID("00001010-0000-1000-8000-00805f9b34fb");
const int Buzzer = D7; 
int scanTime = 5; //In seconds
BLEScan* pBLEScan;
bool buzz;
int c=0;
// class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
//     void onResult(BLEAdvertisedDevice advertisedDevice) {
//       Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
//     }
// };
void setup() {
  // tone(Buzzer, 200, 250);
  Serial.begin(9600);
  Serial.println("Scanning...");
  pinMode(Buzzer, OUTPUT);
  BLEDevice::init("test-DEVICE1");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  BLEDevice::startAdvertising();
  BLEAddress address = BLEDevice::getAddress();
  Serial.print("\nAddress");
  Serial.print(address.toString().c_str());
  BLEScan *scan = BLEDevice::getScan();     //initializing an instance of the scan
  scan->setActiveScan(true);                // call setActiveScan function
  BLEScanResults results = scan->start(1); 
  for (int i = 0; i < results.getCount(); i++) {
    buzz=false;
    c=0;
    if (results.getDevice(i).getServiceUUID().equals(my_uuid)) { 
    BLEAdvertisedDevice device = results.getDevice(i);          //instantiate the device
      int rssi = device.getRSSI();                                // get the RSSI value
      std::__cxx11::string name = device.getName();
      Serial.print("Name--");
      Serial.print(name.c_str());
      Serial.print(" ");
      Serial.print("rssi--");
      Serial.print(rssi);
      Serial.print(" ");
      Serial.print("SERVICE UUID--");
      Serial.print(device.getServiceUUID().toString().c_str());
      Serial.print(" ");
      Serial.print("Distance--");
      Serial.print(get_distance(rssi));
      Serial.println("");
      if (get_distance(rssi) > 3 ) {
        buzz=false;  
      }
      if (get_distance(rssi) < 5 && get_distance(rssi)!=NULL) {
        buzz=true;        
        while(buzz && c<10)
        {
          c=c+1;
        tone(Buzzer, 1000, 250);  //turn buzzer on
        delay(200); //turn buzzer off
        delay(200);   
        }           // turn the LED off 
      } 
      else if (get_distance(rssi) < 3 && get_distance(rssi)!=NULL) {
        buzz=true;       
        while(buzz && c<10)
        { 
          c=c+1;
        tone(Buzzer,1500, 250);//turn buzzer on   
        }          // turn the LED off 
      } 
          
    } // delete results fromBLEScan buffer to release memory
  }
}
float get_distance(int rssi_val) {
  float measured_power = -69.0;
  float distance = pow(10.0, ((measured_power - rssi_val) / (10.0 * 2.14)));
  return distance;
}

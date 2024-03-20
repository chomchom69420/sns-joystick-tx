#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0xD8, 0xBC, 0x38, 0xE5, 0x01, 0x88};

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    uint8_t j1PotX;
    uint8_t j2PotY;
} struct_message;

//message struct for outgoing messages
struct_message outgoing_message;

//message struct for incoming messages
struct_message incoming_message;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  Serial.println("Data received, idk why cuz I am Tx");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  //Read adc data and store in message
  outgoing_message.j1PotX = analogRead(GPIO_NUM_36);
  outgoing_message.j2PotY = analogRead(GPIO_NUM_39);

  // Serial.print("j1PotX = ");
  // Serial.print(outgoing_message.j1PotX);
  // Serial.print(" j2PotY = ");
  // Serial.print(outgoing_message.j2PotY);
  // Serial.println();

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoing_message, sizeof(outgoing_message));
   
  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // }
  // else {
  //   Serial.println("Error sending the data");
  // }
  delay(200);
}
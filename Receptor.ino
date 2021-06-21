#include <SPI.h>
#include <LoRa.h>
char st;

void recebe_dados(){
 // verifica se há pacote de dados para receber
  int packetSize = LoRa.parsePacket(); //analiza tamanho dos dados à receber
  if (packetSize) { //caso haja dados para receber, executa o seguinte
    while (LoRa.available()) { //enquanto LoRa estiver recebendo dados
     st = (char)LoRa.read (); //armazena dados no tipo TEXTO na variavel st
    Serial.print(st); //imprime no console todo o texto recebido
    }
    Serial.print(","); //após os dados recebidos pela telemetria, imprime dado local separado por vírgula
    Serial.println(LoRa.packetRssi()*(-1)); //imprime potência exercida em milliwatts
  }

}



void setup() {
  delay(5000);
  Serial.begin(9600);
  while (!Serial);

 // Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
 recebe_dados();
}

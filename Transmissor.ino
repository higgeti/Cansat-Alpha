#include <Servo.h>
#include <SPI.h>
#include <LoRa.h>
#include <SFE_BMP180.h>     //inclui biblioteca para o BMP180


int n = 0;

float ath = 0;              //all time high, maior altitude registrada
SFE_BMP180 bmp180;          //cria um objeto chamado BMP180
float Po = 1013.25;         //constante da pressão à nível do mar em hPa
double T, P, alt;           //variáveis usadas pela biblioteca do BMP180
int servo = 0;              //posição do servo 
Servo Servo_paraquedas;     //objeto servo chamado Servo_paraquedas
int pin = A7;                       //pino analógico para ler o LDR

int raios = 0;        //armazena quantidade de raios contados
bool raio = 0;        //verifica estado se há raio ou não no momento
int LDR = 0;          //armazena valor lido do pino A7
int voltage = 0;      //lê o valor do pino A7
int voltage_ath=0;    //armazena maior valor lido de A7
int pin5v;            //usado apenas para manter um pino sempre em 5v próximo ao ldr para facilitar ligação na placa
float apogeu=0;       //armazena maior valor alcançado em altitude
int raio_ant=0;       //variável de intertravamento para só somar um raio quando há mudança de estado de 0 para 1;





void conta_raios(){
  
  LDR = (analogRead(A7));  //lê o valor na porta A0
   voltage = map(LDR, 0, 1023, 0, 1023);  //mapeia o valor da porta A0 entre 1 e 5
 if (voltage > voltage_ath) voltage_ath = voltage;  //se valor lido for maior que o maior valor lido, armazena o novo valor como o maior
// else if (voltage_ath < 300) voltage_ath=300;     // se o maior valor lido for menor que 50, armazena valor 50 para nunca ser menor que 50   precisa voltar esssa linha depois
   if(voltage >= (voltage_ath*0.5)){   raio = 1;}  // compara valor lido com 80% do maior valor, se for verdadeiro, é considerado um raio
   
 if(voltage < (voltage_ath*0.49)){ raio = 0;}   //quando valor lido for menor que 70% do maior valor, considerar que não há mais raio

if(raio != raio_ant){    //verifica se já não estamos em estado de raio para não sobrecontar
  if(raio==1){  // caso estivermos em condição de raio:
    raios++;    // soma 1 ao valor
    
    }
  
  
  }
 unsigned long currentMillis = millis();                            //
                                                                    //
  if ((raio != 1) && currentMillis - previousMillis >= interval) {  //    caso não tenha raio e passar 20 segundos:
    // save the last time you blinked the LED                       //
    previousMillis = currentMillis;                                 //
   // voltage_ath = (voltage_ath*0.9);                                //    reduz o valor máximo para 90% dele
  }
  
raio_ant = raio;

   Serial.print(voltage);
   Serial.print("\t");
  Serial.print(apogeu);
 Serial.print("\t");
  Serial.print(raio);
   Serial.print("\t");
   Serial.println(raios-1);
  
}

void verifica_paraquedas(){

  if(apogeu > alt+3)
      {
       abre_paraquedas();
      }
}

  
void fecha_paraquedas(){
  servo=90;
 // analogWrite(3, servo);
 Servo_paraquedas.write(servo);
}


void abre_paraquedas(){
  servo=200;
 //analogWrite(3, servo);
Servo_paraquedas.write(servo);
}

void atmosfera(){
 char status;                //define algumas variáveis necessárias para os cálculos
  //double T, P, alt;
  bool success = false;
 

  status = bmp180.startTemperature();     //inicia os processos para os cálculos

  if (status != 0) {
    delay(5);
    status = bmp180.getTemperature(T);    //recebe o valor da temperatura em Celsius

    if (status != 0) {
      status = bmp180.startPressure(3);   //inicia o processo de cálculo de pressão com 3 leituras por amostra
 
      if (status != 0) {
        delay(status);
        status = bmp180.getPressure(P, T);  //calcula pressão atmosférica local já com o valor da temperatura

        if (status != 0) {
          alt = bmp180.altitude(P, Po);     //informa ao computador os valores da pressão local e pressão à nível do mar para calculo de altitude

//          Serial.print("Pressão: ");  //imprime no monitor serial os dados computados
//          Serial.print(P);
//          Serial.print(" hPa");
//          Serial.print(" ");
//         Serial.print("\t Temperatura: ");
//          Serial.print(T);
//         Serial.print(" C");
//          Serial.print(" ");
//         Serial.print("\t Altitude: ");
//          Serial.print(alt);
//          Serial.print(" Metros");

        if(alt >= apogeu) apogeu=alt;
          
        }
      }
    }
  }
}
  
void envia_dados(){
//  Serial.print("  ");
 // Serial.print("Enviando Pacote: ");
//  Serial.print(counter);
//  Serial.println("  ");
//analogico = analogRead(A3); // Lê valor do sensor LDR
//if(analogico >1023)analogico = 1024; //Delimita valor máximo para que não haja overflow
//raio = map(analogico, 1024, 0, 0, 1024);
  // Essa parte envia os pacotes de dados de forma serial
  LoRa.beginPacket(); //Envia bits informando que começará a transmitir os dados
 // LoRa.print("counter"); //imprime contador, original do codigo
//  LoRa.print(","); //imprime uma virgula para separar os dados para o arquivo CSV
  LoRa.print(voltage); //imprime valor lido pelo sensor
  LoRa.print(","); //imprime uma virgula para separar os dados para o arquivo CSV
  LoRa.print(P*0.000986923,9); //imprime valor lido pelo sensor com 9 casas decimais
  LoRa.print(","); //imprime uma virgula para separar os dados para o arquivo CSV
  LoRa.print(T); //imprime valor lido pelo sensor
  LoRa.print(","); //imprime uma virgula para separar os dados para o arquivo CSV
  LoRa.print(alt); //imprime valor lido pelo sensor
  LoRa.print(",");
  LoRa.print(raios);
  LoRa.print(",");
  LoRa.print(apogeu);
  LoRa.print(",");
  LoRa.print(servo);
  LoRa.endPacket(); //Informa que aqui encerram os dados a serem enviados

//  counter++; //soma no contador para facilitar o gráfico em relação ao tempo
 // delay(10); //aguarda 10 milésimos para não sobrecarregar os dados
  }

void setup() {
  //delay(10000);
  Serial.begin(9600);
  pinMode(pin, INPUT_PULLUP);

   Servo_paraquedas.attach(3);
    bool success = bmp180.begin();    //inicia o objeto bmp180

  if (success) {
    Serial.println("BMP180 Iniciado");    //se tiver sucesso em iniciar, avisa o usuário
  }
  while (!Serial);

  Serial.println("LoRa Transmissor");

  if (!LoRa.begin(433E6)) {
    Serial.println("Falha ao iniciar o LoRa!");
    while (1);
  }
//  pinMode(A3, INPUT);
  atmosfera();
  long offAt = 0;
  fecha_paraquedas();
}



void loop() {
    conta_raios();
    atmosfera();
    envia_dados();
    verifica_paraquedas();
}

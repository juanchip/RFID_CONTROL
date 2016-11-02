#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
 
MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key; 
// Array vacio para guardar la key
byte nuidPICC[3];
// Tarjetas habilitadas
byte tarjebus[4] = {84,143,13,153};
byte ROOT[4] = {181,201,171,117};
byte sectorA[4] = {132,246,164,48};
byte sectorF[4] = {148,134,221,245};
void setup() 
{ 
  pinMode(6, OUTPUT);
  Serial.begin(9600);
  SPI.begin();          //Inicia bus SPI
  rfid.PCD_Init();      // Inicia RFID

  for (byte i = 0; i < 6; i++) 
    { 
     key.keyByte[i] = 0xFF;
    }

  Serial.println(F("Escaner Tarjetas 1.1 By Juan Blanc"));
  Serial.print(F("TIpo de Codificacion de la tarjeta"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  Serial.println(F(""));
}
// ------------------------------------------------------------------
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
  }
}
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
  Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   Serial.print(buffer[i], DEC);
  }
}
//------------------------------------------------------------------

void loop() {

  // Buscamos tarjetas
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // SI la encuentra la leemos
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("Tipo de tarjeta: "));
  //Consulta al micro incorporado en la tarjeta, el tipo de tarjeta...
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  //Lo Muestra en consola
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Dectecta si tiene PICC de tipo MIFARE
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
      {
        Serial.println(F("Error."));
        return;
      }
//Almacena los datos en el array...
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) 
    {
//si se guardo, detecto un tipo de tarjeta valido y prosige
       
         beep(200); // Beep para saber la lectura correcta
        if( acceso(rfid.uid.uidByte,rfid.uid.size,tarjebus) == 1 )
        {
          beepOK(200);
          beepOK(200);
         
          Serial.print("Tarjebus detectada\n");
         }
        else if ( acceso(rfid.uid.uidByte,rfid.uid.size,sectorA) == 1)
        {
          beepOK(200);
          beepOK(200);
          
          Serial.print("FDD Sector A Detectada\n");
        }
        else if ( acceso(rfid.uid.uidByte,rfid.uid.size,sectorF) == 1)
        {
          beepOK(200);
          beepOK(200);
          
          Serial.print("FDD Sector F Detectada\n");
        }
        else if (acceso(rfid.uid.uidByte,rfid.uid.size,ROOT) == 1)
        {
          beepOK(200);
          beepOK(200);
          
          Serial.print("SOS ROOOOOOOOOOOOOOT!!\n");
        }
        
     
       rfid.PICC_HaltA();
       rfid.PCD_StopCrypto1();
    }
    delay(600);
}
bool acceso(byte *buffer, byte tamanio_buf, byte *almacenada)
{
   unsigned int validacion = 0;
   for (byte i = 0; i < tamanio_buf; i++) {
    if(buffer[i] == almacenada[i])
    {
      validacion++;
    }
      } //Fin for
    if(validacion == 4)
    {
      return 1;
    }
    else 
    {
      return 0;
    }
  
}
void beep(unsigned char delayms){
  analogWrite(6, 20);      
  delay(delayms);          
  analogWrite(6, 0);      
  delay(delayms);          
}  
void beepOK(unsigned char delayms){
  analogWrite(6, 20);      
  delay(25);          
  analogWrite(6, 0);      
  delay(25);
  analogWrite(6, 20);      
  delay(25);          
  analogWrite(6, 0);      
  delay(delayms);           
}  


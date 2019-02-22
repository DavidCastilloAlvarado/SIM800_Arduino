HardwareSerial &mySerial      = Serial2;
void setup(){
Serial.begin(9600);       // Iniciamos la comunicacion serie
mySerial.begin(9600);     // Iniciamos una segunda comunicacion serie
delay(1000);              // Pausa de 1 segundo
EnviaSMS();               // Llamada a la funcion que envia el SMS
}
 
void loop(){
if (mySerial.available()){          // Si la comunicacion SoftwareSerial tiene datos
  Serial.write(mySerial.read());    // Los sacamos por la comunicacion serie normal
} 
  
if (Serial.available()){            // Si la comunicacion serie normal tiene datos
  while(Serial.available()) {       // y mientras tenga datos que mostrar 
    mySerial.write(Serial.read());  // Los sacamos por la comunicacion SoftwareSerial
  } 
  mySerial.println();               // Enviamos un fin de linea
}
}
 
// Funcion para el envio de un SMS
void EnviaSMS(){              
 mySerial.println("AT+CMGF=1");                 // Activamos la funcion de envio de SMS
 delay(100);                                    // Pequeña pausa
 mySerial.println("AT+CMGS=\"+51999999999\"");  // Definimos el numero del destinatario en formato internacional
 delay(100);                                    // Pequeña pausa
 mySerial.print("Hola Mundo!");                 // Definimos el cuerpo del mensaje
 delay(500);                                    // Pequeña pausa
 mySerial.print(char(26));                      // Enviamos el equivalente a Control+Z 
 delay(100);                                    // Pequeña pausa
 mySerial.println("");                          // Enviamos un fin de linea
 delay(100);                                    // Pequeña pausa
}

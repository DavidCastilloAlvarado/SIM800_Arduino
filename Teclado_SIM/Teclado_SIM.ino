/*
NOMBRE: Castillo Alvarado, David
PROYECTO: Test de SIM800L con teclado alfanumerico
A= llamadas
B=Colgar
C=reset - en llamadas
D=Contestar Llamada
#=Marcar para mandar mensajes de texto usando la interfaz del monitor serial

*/

#include <SIM800.h>
#include <Keypad.h> // Controla el teclado
int LED=13; // Led de parpadeo, cuando hay llamada
unsigned long bauds = 19200; //Define los baudios para el serial y el SIM
char push;
char msg[200]=" ";
int i=0;
int j=0;
char phonenumber[20]="+51          ";
char phonesms[]="\"+51         \"";
// Parametros para el teclado alfanumerico
const byte filas = 4;
const byte columnas = 4;
byte pinsFilas[filas] = {22, 24, 26, 28};
byte pinsColumnas[columnas] = {30, 32, 34, 36}; 
char teclas[filas][columnas] =  {
  {'1', '2', '3', 'A'},                       // Declaración del teclado
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};

Keypad teclado = Keypad(makeKeymap(teclas), pinsFilas, pinsColumnas, filas, columnas);
HardwareSerial &simComx = Serial2;
String Comando;

void setup() {
  Serial.begin(bauds);  
  Serial.print("  :::Espere el reconocimiento de la SIM:::\n ");
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); 
  while (!Serial) {;}
  SIM.begin(bauds);
  delay(100);
  SIM.setTimeout(3000); // Tiempo de respuesta del servidor
  SIM.cmdBenchmark(true);
  SIM.test                ();
  SIM.ringerVolume      (100);
  SIM.loudSpeakerVol    (100);
  Serial.println("...");
  Serial.println("... Listo");
  Serial.println("  :::Ingrese Número de celular:::\n ");
  Serial.println("Use el Teclado Externo");
  simComx.println("AT+CMGF=1");              //Configuramos el módulo para trabajar con los SMS en modo texto
  delay(1000);                               //Pausa de 1 segundo
  simComx.println("AT+CNMI=1,2,0,0,0");      //Configuramos el módulo para que nos muestre los SMS recibidos por comunicacion serie
  
  
}
void loop(){
    push = teclado.getKey();
    if (push != NO_KEY){
    switch (push) {  
      case 'A':
        Serial.println("\nllamando a:: ");
        phonenumber[3+i] = 59;
        i=0;
        Serial.print(phonenumber);
        SIM.originCall          (phonenumber);
        delay(3000);   
        reset();
        break;      
      case 'B':
        reset();
        break;  
      case 'C':
        SIM.endCall             ();
        reset();
        break;
      case 'D':
        Serial.println("Contestaste la llamada");
        SIM.answerCall          ();
        delay(300);
        reset();
        break;
     /* case '*':
        Serial.println("Presionaste *");
        delay(300);
        reset();
        break;*/
      case '#':
        Serial.println("\n Zona de Mensajes");
        Serial.println("\n Introdusca Numero Telefonico");
        Serial.println("\n Presione 'A' cuando acabe te colocar el numero ");
        i=0;
        while(push != 'A'){
          push = teclado.getKey();
          if((push!= NO_KEY) && ( push!='A')){
          phonesms[4+i] = push;
          i=i+1;
          Serial.print(push);}
          }
        i=0; j=0;
        Serial.println("\n");
        Serial.println(phonesms);
        Serial.println("\n Introdusca el mensaje");
        Serial.println("\n Introdusca por el Monitor serial");
        Serial.println("\n Presione Enter para mandar el mensaje");
        while(i==0){
            if (Serial.available()>0){            // Si la comunicacion serie normal tiene datos
             delay(100);                          // Tiempo de espera para que lleguen los datos
              while(Serial.available()>0) {       // y mientras tenga datos que mostrar 
                msg[j]=Serial.read();           // Colocamos los datos del monitor al mensaje
              j++;
              } 
              i=1;
            }
          }
          Serial.println(msg);
          SIM.smsFormat(SET, "1");             // Se configura para mandar mensajes
          SIM.smsSend(phonesms, msg);       // Manda mensaje
          Serial.println("\n ");
        Serial.println("\n Mensaje Enviado");
        delay(300);
        reset();
        break;
      default: //Se presenta el numero en la pantalla y se guarda en un char
          if(1){
          phonenumber[3+i] = push;
          i=i+1;
          Serial.print(push);
         /* if (i > 8){
            Serial.println("\n");
             i = 0;}*/
          }
      }
    }
if (simComx.available()){                                //Si hay datos disponibles
  Comando = simComx.readString();                        //Los almacenamos en la variable Comando
  Serial.println("NUEVO SMS ENTRANTE: " + Comando);      //Los sacamos por comunicacion serie
} 

}

void reset() {
  Serial.println("");
  for(int i =0;i<=9;i++){
  phonenumber[3+i]=NO_KEY;
  }
  for(int i =0;i<=9;i++){
  phonesms[5+i]=NO_KEY;
  }
  i=0;
  msg[200]="";
}




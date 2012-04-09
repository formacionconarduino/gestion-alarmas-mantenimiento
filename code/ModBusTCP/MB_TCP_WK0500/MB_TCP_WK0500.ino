#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

#include "HRWK0500.h"

#include "Mudbus.h" 

//#define DirDefectoMB 18 //RV
//unsigned char DirMB =1; //RV



//unsigned long wdog = 0;         //RV
//unsigned long tprev ;           //RV MIRAR EL DATO MIEMBRO  PreviousActivityTime DE LA CLASE MUDBUS 



Mudbus Mb;//
//Function codes 1(read coils), 3(read registers), 5(write coil), 6(write register)
//signed int Mb.R[0 to 125] and bool Mb.C[0 to 128] MB_N_R MB_N_C
//Port 502 (defined in Mudbus.h) MB_PORT

void setup()
{
  uint8_t mac[]     = { 0x90, 0xA2, 0xDA, 0x00, 0x65, 0x55 };//ésta es la dirección MAC de mi arduino no del wk0500
  uint8_t ip[]      = { 192, 168, 1, 50 };//y éstas la de mi casa
  uint8_t gateway[] = { 192, 168, 1, 1 };//
  uint8_t subnet[]  = { 255, 255, 255, 0 };//
  int pin;
  
  Ethernet.begin(mac, ip, gateway, subnet);//
  //Avoid pins 4,10,11,12,13 when using ethernet shield
  //en el WK0400 tendremos problemas con DIN3,SWIN0,SWIN1, SWIN2 y SWIN3
  delay(5000);
  Serial.begin(9600);
  
  for( pin=DIN1; pin <= DIN2; pin++) pinMode(pin, pinIO[pin]);
  //este jaleo es por las patillas 4, 10,11,12 y 13
  for(pin=DIN4; pin < SWIN0;pin++) pinMode(pin, pinIO[pin]);
  for(pin=POTIN; pin <=AINAUX;pin++) pinMode(pin, pinIO[pin]);
 //INICIALIZAR EL ESCLAVO ????
 /*if (DirMB >246 || DirMB <1 ){//LO DEL 246 HAY QUE VERIFICARLO
            DirMB=DirDefectoMB; //asigno valor si no era correcto
            //EEPROM.write (EP_DirMB,DirMB); //DE MOMENTO NO ESCRIBIR EN LA EEPROM
            }*/
   //R[] ES UN DATO MIEMBRO DE LA CLASE MUDBUS
   Mb.R[HR_CMDWK]=0xFFFF;
   Mb.R[HR_VERSION]=0412;//abril 2012
 //  Mb.R[HR_N_DIR_MB]=DirMB; //guardo en holding register la direccion de MB
  // Mb.R[HR_tcicloMax]=0; //para que realice correctamente la detrminación del ciclo máximo
  // Mb.R[HR_TMP_WD]=666; //lo he puesto a voleo, nº de la bestia del perro guardian
   InicializaElementos(); 
}

void InicializaElementos()//----------------------------------------------------
{
  //no sé si encender algún LED o mandar algún mensaje
}

void loop()
{
  Mb.R[HR_CMDWK]=0xFFFF;
  Mb.Run();
  Actualizar_Entradas();
  //lo de lógica negativa poner aquí, para el tema de las ENTRADAS
  if(Mb.R[HR_CMDWK] == 0xFFFF) ProcesarNormal(); //NO ha llegado ningún comando del maestro
     else ProcesarComando();//Ha llegado un comando desde el WK0500 
//lo de lógica negativa poner aquí, para el tema de las SALIDAS
Actualizar_Salidas();
}

/*Actualizar_Entradas*****************************************************************************
*Esta función, al inicio de loop() actualiza todas las entradas con los valores que corresponda
*para poder procesarlas en el bucle loop(), guarda los datos leídos en el bit menos significativo
*del correspondiente registro de valor de escena del elemento leído
*/
void Actualizar_Entradas()
{
  //Esto habrá que modificarlo para cada módulo
  //He considerado que la escena 0 (LSB de valor de escena) es la que leo o escribo
   int pin;
   int HR_valorEscena;
   for (pin=DIN1; pin <= AINAUX; pin++){
   HR_valorEscena=HR_INFO+(pin-2)*5+3;
    //en la fórmula pone (pin-2)porque DIN1=2, el 5 (cada elemento digital usa (5*2)=10 bytes)
    //y es la posición tercera la del valor, considero la escena 0 la que hay que sacar
    if (pin==DIN1 || pin==DIN2 || pin==DIN4 || pin==DIN5 || pin==DIN6 ) //entradas digitales
       Mb.R[HR_valorEscena]=(Mb.R[HR_valorEscena]&0xFFFE)|digitalRead(pin);
    if(pin==POTIN)  Mb.R[HR_POTIN_ID+3]=  analogRead(pin);//entradas analógicas
    if(pin==AINAUX) Mb.R[HR_AINAUX_ID+3]= analogRead(pin);//entradas analógicas
  }
}

/*ProcesarNormal*****************************************************************************
*Esta función, es la que se repite continuamente en el bucle loop() y realiza los procesos que
*se deseen con las entradas y las salidas
*/
void ProcesarNormal()
{
  //este es un sencillo ejemplo para probar el funcionamiento de lo escrito hasta ahora
  //sólo utilizo RELÉ1, RELÉ2 Y DIN6
  Mb.R[HR_RELE1_ID+3]=(Mb.R[HR_RELE1_ID+3]&0xFFFE) | (Mb.R[HR_DIN6_ID+3]&0x1); //depende de la entrada DIN6
  //regs[HR_RELE2_ID+3]=memoria; // la enciendo siempre esto es para las pruebas
  
  
 
  digitalWrite(RELE2,Mb.R[HR_RELE2_ID+3]);
  
   
}

/*Actualizar_Salidas*****************************************************************************
*Esta función, antes de terminar loop() actualiza todas las salidas con los valores que corresponda
*según las actualizaciones realizadas en loop().
*/
void Actualizar_Salidas()
{
  //Esto habrá que modificarlo para cada módulo
  //He considerado que la escena 0 (LSB de valor de escena) es la que leo o escribo
  int HR_valorEscena;
  int pin;
  for (pin=RELE1; pin <= RELE4; pin++){
    HR_valorEscena=HR_INFO+(pin-2)*5+3;
    //en la fórmula pone (pin-2)porque DIN1=2, el 5 (cada elemento digital usa (5*2)=10 bytes)
    //y es la posición tercera la del valor, considero la escena 0 la que hay que sacar
    //para RELE1 sería 10+(8-2)*5+3 =43 que es la dirección del valor de escena del RELE1
    if (pin==RELE1 || pin==RELE2 || pin==RELE3 || pin==RELE4)
       digitalWrite(pin,Mb.R[HR_valorEscena]&0x0001); //No consigo que me funcione como quiero con un switch
    }
}
#include "ComandosWK0500.h"

/*ProcesarComando*****************************************************************************OK
*Si desde el inicio de la ejecución del loop() se ha recibido alguna orden desde el maestro WK0500
*esta función analiza el código recibido y los datos y verifica si existe el objeto y realiza la
*función indicada sobre él, devolviendo al master el mensaje adecuado.
*/
void ProcesarComando()
{
   if(Mb.R[HR_CMDWK]==CMD_ACT_ELE) ActivarElemento(); 
   if(Mb.R[HR_CMDWK]==CMD_DES_ELE) DesactivarElemento();
  //no me gusta lo que hace con el switch, además en lenguaje de alto nivel quedará más bonito
  //pero el código máquina generado es casi el mismo
  
 /* switch(regs[HR_CMDWK])
  {
   
         case CMD_ACT_ELE:ActivarElemento(); break;
         case CMD_DES_ELE:DesactivarElemento(); break;
         ------------------------------------*/
         
      /*   case CMD_ACT_GRP: ActivarGrupo(); break;
         case CMD_DES_GRP: DesactivarGrupo(); break;
         case CMD_EXE_ESC: EjecutarEscena(); break;
         case CMD_PUL_ELE: PulsoElemento();break;

         case CMD_CPH_ELE: ConsultarProgramaHorariosElemento();break;
         case CMD_CPH_GRP: ConsultarProgramaHorariosGrupo();break;
         case CMD_CPH_ESC: ConsultarProgramaHorariosEscena();break;
         case CMD_BPH_ELE: BorrarProgramaHorario();break;
         case CMD_APH_ELE: AnadirProgramaHorario();break;
         case CMD_MPH_ELE: ModificarProgramaHorario();break;*/
    
        // case CMD_CPUERTA: ConsultarPuertas();break;
        // case CMD_BPUERTA: BloquearPuerta();break;
        // case CMD_DPUERTA: DesbloquearPuerta();break;
        // case CMD_ALT_TRJ: AltaTarjetaPuerta();break;
        // case CMD_BAJ_TRJ: BajaTarjetaPuerta();break;
        // case CMD_BAJ_SIS: BajaTarjetaSistema();break;
        // case CMD_ALT_MTR: AltaTarjetaMaestra();break;
        // case CMD_ALT_SMT: AltaTarjetaSuperMaestra();break;
        // case CMD_BRT_CRR: BorradoTotalCerradura();break;

        // case CMD_LEE_TMP: LeerTemperatura();break;
        // case CMD_MOD_CSG: ModificarConsignaActual();break;
        // case CMD_MOD_MDD: ModificarConsignaModoDia();break; 
        // case CMD_MOD_MDN: ModificarConsignaModoNoche();break; 
        // case CMD_MOD_CAG: ModificarConsignaModoActualGlobalmente();break;
        // case CMD_MOD_MDG: ModificarConsignaModoDiaGlobalmente();break;
        // case CMD_MOD_MNG: ModificarConsignaModoNocheGlobalmente();break;
        // case CMD_ACT_HVA: ActivarHVAC();break;  
        // case CMD_DES_HVA: DesactivarHVAC();break;  
        // case CMD_ACT_HVG: ActivarHVACGlobalmente();break;
        // case CMD_DES_HVG: DesactivarHVACGlobalmente();break;
        // case CMD_MOD_LTS: ModificarLimiteTemperaturaSuperior();break;  
        // case CMD_MOD_LTI: ModificarLimiteTemperaturaInferior();break;  
        // case CMD_MOD_TSG: ModificarLimiteTemperaturaSuperiorGlobalmente();break;  
        // case CMD_MOD_TIG: ModificarLimiteTemperaturaInferiorGlobalmente();break;  

     //  default: ComandoError(); break;
    
  
}


enum { //definici�n de pines en el WK0500
      LED_AMARILLO=60,LED_ROJO
     };
int pinIO[]={0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0};  //0=INPUT, 1=OUTPUT, las dos primeras son para pin 0 y 1 que no existen 

enum {//direcciones asignadas de la EEPROM
      EP_DirMB, //direcci�n 0
      EP_AjustesMB,
      EP_LogicaNeg, //ESTAS Y LA SIGUIENTE NO LAS MANDO ESCRIBIR EN NINGUN SITIO TADAVIA
      EP_tiempo_WD
     };  
     
enum {// valores usados para la configuraci�n del ModBus
        COMM_BPS = 115200, //* baud rate */
        MB_SLAVE = 1,	   //* modbus slave id *///QUITAR uso variable DirMB
        PARITY = 'e'       //* even parity */
     };

/*Nombres para los Holding Registers para el WK0500*/
enum {        
        HR_INICIO,
        HR_CMDWK,       /* comando Winkhel */
        HR_ARG1,             //aqu� tengo que dejar hueco para los par�metros de los comados
        HR_ESTADO=5, //Esto esta copiado de los de winkhel
        HR_TMP_WD,   //Tiempo de WatchDog
        HR_VERSION,  //versi�n del programa
        HR_N_DIR_MB, //nueva direcci�n del remoto en ModBus
        HR_N_AJU_MB, //puede que haya que quitarlo
        HR_INFO=10,     //direcci�n de inicio de los HR con datos de elementos
      
        
        HR_tciclo=113,        //tiempo de ejecuci�n de loop() HR106 no
        HR_tcicloMax,     //tiempo m�ximo de loop()
        HR_tcicloMB,      //n�mero de loop() desde la última orden del master, este registro va a toda pastilla
        HR_Log_Neg,
        MB_REGS	 	//* n�mero total de registros en el esclavo */
};

 
 enum{
        OFFSET_IDELE=0,//offset del identificador de elemento
        OFFSET_IDTPS,//offset identificador de tipo y n�salida en este caso la 2, lo que no s� que hacer con el otro byte
        OFFSET_ESCN, //offset escenas a las que pertenece
        OFFSET_VESC//offset valor de escena
 };        

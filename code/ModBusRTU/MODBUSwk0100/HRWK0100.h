enum { //definici�n de pines en el WK0100, asigna pines del 2 al 17 seg�n el orden
      DIN1=2,DIN2,RELE1,DOUT1,DOUT2,RELE2,RELE3,RELE4,RELE5,RELE6,RELE7,RELE8,DIN3=17//ME FALTAN AIN0 HASTA AIN2
     };
int pinIO[]={0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0};  //0=INPUT, 1=OUTPUT, las dos primeras son para pin 0 y 1 que no existen 
//LA LÍNEA ANTERIOR HAY QUE CORREGIR LO DE DIN3 PATA 17 Y LAS 3 ENTRADAS ANALOGICAS
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

/*Nombres para los Holding Registers para el WK0100*/
enum {        
        HR_CMDWK,       /* comando Winkhel */
        HR_ARG1,             //aqu� tengo que dejar hueco para los par�metros de los comados
        HR_ESTADO=5, //Esto esta copiado de los de winkhel
        HR_TMP_WD,   //Tiempo de WatchDog
        HR_VERSION,  //versi�n del programa
        HR_N_DIR_MB, //nueva direcci�n del remoto en ModBus
        HR_N_AJU_MB, //puede que haya que quitarlo
        HR_INFO=10,     //direcci�n de inicio de los HR con datos de elementos
        HR_DIN1_ID=10,//esto sera la direcci�n del HR donde empiezan los datos del elemento digital
        HR_DIN2_ID=15,//utilizo 10 bytes para cada elemento aunque creo que me vale con 8
        HR_RELE1_ID=20,
        HR_DOUT1_ID=25,
        HR_DOUT2_ID=30,
        HR_RELE2_ID=35,
        HR_RELE3_ID=40,
        HR_RELE4_ID=45,
        HR_RELE5_ID=50,
        HR_RELE6_ID=55,
        HR_RELE7_ID=60,
        HR_RELE8_ID=65,
        
        HR_DIN3_ID=85,//El salto es porque es la patilla17 y por no modificar el algoritmo pongo este número

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
 
 //relación de tipos de elementos
  enum{ALUMBRADO,ENCHUFE, 
       S_GAS=8, S_PRESENCIA,INTERRUPTOR,//la tontería del 8 es por poner unos de entrada y otros de salida y que estos sean del 8 al 15
       TIPO=16
      };  
  //relación de escenas
  enum{ ESCENA0=1,ESCENA1=2,  
        ESCENA2=4, INCENDIO=8, NOCHE=16 //esto es por ser creativo en los nombres de las escenas    
        //LOS VALORES ASIGNADOS SON PARA FACILITAR EL CODIGO POSTERIOR
      };

// Alvaro Martinez 
// Erick Cardenas
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct {
 short int ETQ;
 short int Datos[8];
} T_LINEA_CACHE;

//Esta funcion inicializa los valores de las variables de T_LINEA_CACHE, y lee el fichero RAM.bin para almacenar los caracteres del fichero en la variable RAM
void startUp(T_LINEA_CACHE* cache, unsigned char* ram, FILE* fRAM, FILE* fAccesos);

//Funcion que ejecuta la lectura del fichero RAM.bin, y almacena los caracteres en la varaible RAM
void startRAM(FILE* f, char* ram);

//Esta funcion retorna el valor de la direccion de memoria en hexagecimal
short int accesoMemoria(FILE* f);

//Funciones para obtener datos de las direcciones de memoria
short int numLinea(short int direccion);
short int etiqueta(short int direccion);
short int palabra(short int direccion);
short int numBloque(short int direccion);

//Funcion que comprueba que la etiqueta de la direccion se corresponde con la etiqueta cargada en la linea correspondiente de la cache 
int cargadoEnCache(short int direccion, T_LINEA_CACHE cache[4]);

//Funciones para gestionar los aciertos y los fallos de cache
void falloCache(short int direccion, T_LINEA_CACHE* cache, char* ram);
char aciertoCache(short int direccion, T_LINEA_CACHE* cache);

//Esta funcion imprime el array de structs de cache
void imprimirCache(T_LINEA_CACHE *cache);

unsigned int tiempoGlobal = 0;	//Lleva la cuenta del "tiempo" que tarda en ejecutarse
unsigned int numFallos = 0;		//Cuenta en numero de fallos que se producen

int main(int argc, char** argv){
	
	T_LINEA_CACHE cache[4];
	unsigned char ram[1024];
	short int siguienteAcceso = 0;
	FILE* fRAM;
	FILE* fAccesos;
	char texto[100];
	int cantidad = 0;
	int i;
	
	//Abrimos los ficheros en formato lectura
	fRAM = fopen("RAM.bin", "r");
	fAccesos = fopen("accesos_memoria.txt", "r");
	
	//Comprobamos que son accesibles
	if(fRAM == NULL){
		printf("\nNo se ha podido abrir el fichero RAM.bin");
		exit(-1);
	}
	if(fAccesos == NULL){
		printf("\nNo se ha podido abrir el fichero accesos_memoria.txt");
		exit(-1);
	}
	
	startUp(cache, ram, fRAM, fAccesos);//Inicializamos las variables de cache, y almacenamos los valores de RAM
	
	while(siguienteAcceso!=-1){
		siguienteAcceso = accesoMemoria(fAccesos); //cargamos el valor de la direccion, en caso de encontrase al final del fichero retornara -1
		
		//Comprobamos si se encuentra al final del fichero
		if(siguienteAcceso == -1){	//Si se encuentra al final del fichero mostramos los datos pedidos por pantalla
			printf("\n*****Se ha acabado de leer las direcciones de memoria*****");
			printf("\nNumero de accesos: %d Numero de fallos: %d Tiempo medio de acceso: %f", cantidad, numFallos, ((float)tiempoGlobal/(float)cantidad));
			printf("\nTexto leido: ");
			for(i = 0; i < cantidad; i++){
				printf("%c", texto[i]);
			}
			return 0;
		}else{	//Sino esta al final del fichero
			
			//Comprobamos que la etiqueta de la direccion se corresponde con la etiqueta cargada en la linea correspondiente de la cache
			if (cargadoEnCache(siguienteAcceso, cache)==0){
				falloCache(siguienteAcceso,cache,ram);
			}
			texto[cantidad] = aciertoCache(siguienteAcceso, cache);
			cantidad++;
			imprimirCache(cache);	//Imprime la cache
		}
		sleep(2);
	}
	return 0;
}

void startUp(T_LINEA_CACHE* cache, unsigned char* ram, FILE* fRAM, FILE* fAccesos){
	int i;
	int j;
	
	//Inicializamos la RAM
	startRAM(fRAM, ram);
	
	//Inicializamos la cache
	for(i=0; i < 4; i++){
		cache[i].ETQ = 0xFF;
		for(j = 0; j < 8; j++){
			cache[i].Datos[j] = 0;
		}
	}
	printf("\nEstado inicial de la cache");
	imprimirCache(cache);
	
}
//Esta funcion almacena los 4 primeros bytes que se encuentren en el fichero abierto en formato hexagecimal en la variable direccion, y finalmente retorna ese valor
short int accesoMemoria(FILE* fAccesos){
	short int direccion = 0;
	char aux[4] = "\n";
	
	fread(aux,4,1,fAccesos); //Lee los 4 primeros bytes que encuentra en el fichero y los almacena en formato string	
	direccion = (short int)strtol(aux,NULL,16); //Convierte el string a formato numérico (teniendo en cuenta que estaban en base 16)
	aux[0] = getc(fAccesos); //Desecha el siguiente caracter "\n" contiguo a cada direccion 
	
	if(feof(fAccesos)){
		return(-1);
	}
	return direccion;
}

short int numLinea(short int direccion){ //Mantiene exclusivamente los bits de la linea, después los desplaza para que ocupen los bits de menor peso
	return ((direccion & 0b0000011000) >> 3);
}
short int etiqueta(short int direccion){ //Mantiene exclusivamente los bits de la etiqueta, después los desplaza para que ocupen los bits de menor peso
	return ((direccion & 0b1111100000) >> 5);
}
short int palabra(short int direccion){ //Mantiene exclusivamente los bits de la palabra
	return (direccion & 0b0000000111);
}
short int numBloque(short int direccion){//Mantiene exclusivamente los bits del bloque, después los desplaza para que ocupen los bits de menor peso
	return ((direccion & 0b1111111000) >> 3);
}
int cargadoEnCache(short int direccion, T_LINEA_CACHE* cache){
	tiempoGlobal++;
	if(cache[numLinea(direccion)].ETQ == etiqueta(direccion)){ //Comprueba si la etiqueta de la direccion se corresponde con la etiqueta cargada en 
		return 1;											  //la linea correspondiente de la cache
	}else{
		return 0;
	}
}
void startRAM(FILE *fichero, char* ram){
	fichero = fopen("RAM.bin", "r");	// abrimos el fichero
	int cont = 0;	// contador para avanzar la posicion del array
	char c = 0;	//char auxiliar
	c = getc(fichero);// cargamos el primer caracter en c

	// este bucle lee caracter por caracter hasta que se llegue al final del fichero
	while(!feof(fichero))
	{
		
		if(c != '\r' && c != '\0')
		{
			ram[cont] = c;
			cont++;
		}
		c =getc(fichero);
	}
	
	fclose(fichero);
}

void falloCache(short int direccion, T_LINEA_CACHE* cache, char* ram){
	numFallos++; //Aumenta el numero de fallos
	short int linea = numLinea(direccion); //Obtiene una sola vez el numero de linea
	short int bloque = numBloque(direccion); //Obtiene una sola vez el numero de bloque
	int i;
	printf("\nT: %d, Fallo de CACHE %d, ADDR %04X ETQ %X", tiempoGlobal, numFallos, direccion,etiqueta(direccion));
	printf(" linea %02X palabra %02X bloque %02X", linea, palabra(direccion), bloque); //Imprime los datos
	tiempoGlobal+=10; //Simula el tiempo de acceso a la ram
	printf("\nCargando el bloque %02X en la linea %02X", bloque, linea);
	cache[linea].ETQ = etiqueta(direccion); //Carga la etiqueta en la cache
	for (i = 0; i < 8; i++){ //Actualiza los datos de la cache
		cache[linea].Datos[i] = ram[bloque*8+i]; //Los datos comienzan en la direccion del bloque (multiplicado por su tamaño unitario)
	}
}
char aciertoCache(short int direccion, T_LINEA_CACHE* cache){
	short int linea = numLinea(direccion);
	short int numPalabra = palabra(direccion);
	printf("\nT: %d, Acierto de CACHE, ADDR %04X ETQ %X", tiempoGlobal, direccion, etiqueta(direccion));
	printf(" linea %02X palabra %02X DATO %02X", linea, numPalabra, cache[linea].Datos[numPalabra]);
	return cache[linea].Datos[numPalabra]; //Devuelve el caracter leido desde la cache
}
void imprimirCache(T_LINEA_CACHE *cache){
	int i;
	//Bucle que imprime la cache los datos de cada etiqueta de la cache
	for(i = 0; i<4; i++){
		printf("\nETQ: %02X \t Datos:",cache[i].ETQ);
		for(int j = 7; j>=0;j--){
			printf(" %02X", cache[i].Datos[j]);
		}		
	}
	printf("\n");
}
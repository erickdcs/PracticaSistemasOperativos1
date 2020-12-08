// Alvaro Martinez 
// Erick Cardenas
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
typedef struct {
 short int ETQ;
 short int Datos[8];
} T_LINEA_CACHE;

void startUp(T_LINEA_CACHE* cache, unsigned char* ram, FILE* fRAM, FILE* fAccesos);
unsigned char* startRAM(FILE* f);
short int accesoMemoria(FILE* f);
short int numLinea(short int direccion);
short int etiqueta(short int direccion);
short int palabra(short int direccion);
short int numBloque(short int direccion);
int cargadoEnCache(short int direccion, T_LINEA_CACHE cache[4]);
void falloCache(short int direccion, T_LINEA_CACHE* cache, char* ram);

unsigned int tiempoGlobal = 0;
unsigned int numFallos = 0;











int main(int argc, char** argv){
	
	T_LINEA_CACHE cache[4];
	unsigned char ram[1024];
	short int siguienteAcceso;
	FILE* fRAM;
	FILE* fAccesos;
	char texto[100];
	
	startUp(cache, ram, fRAM, fAccesos);
	printf("\nMemoria iniciada");
	fAccesos = fopen("accesos_memoria.txt", "r");
	siguienteAcceso = accesoMemoria(fAccesos);
	if (cargadoEnCache(siguienteAcceso, cache)==0){
		falloCache(siguienteAcceso,cache,ram);
	}else{
		printf("Esta cargado en cache");
	}
	
	
	
	
	

	
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}

void startUp(T_LINEA_CACHE* cache, unsigned char* ram, FILE* fRAM, FILE* fAccesos){
	int i;
	int j;
	
	//Abrimos los ficheros en formato lectura
	fRAM = fopen("RAM.bin", "r");
	printf("\nAbriendo RAM");
	fAccesos = fopen("accesos_memoria.txt", "r");
	printf("\nAbriendo accesos");
	
	//Comprobamos que son accesibles
	if(fRAM == NULL){
		printf("\nNo se ha podido abrir el fichero RAM.bin");
		exit(-1);
	}
	if(fAccesos == NULL){
		printf("\nNo se ha podido abrir el fichero accesos_memoria.txt");
		exit(-1);
	}
	
	//Inicializamos la RAM
	ram = startRAM(fRAM);
	
	//Inicializamos la cache
	for(i=0; i < 4; i++){
		printf("\nEtiqueta cambiada");
		cache[i].ETQ = 0xFF;
		for(j = 0; j < 8; j++){
			printf("\nDato cambiado");
			cache[i].Datos[j] = 0;
		}
	}
	printf("\nAccesos iniciados");
	for(i=0; i < 4 ; i++){
		printf("\n ETIQ: %x", cache[i].ETQ);
		for(j=0; j < 8; j++){
			printf(" Dato: %d",cache[i].Datos[j]);  
		}
	}
	
}

short int accesoMemoria(FILE* fAccesos){
	short int direccion = 0;
	char aux[4] = "\n";
	
	if(fAccesos == NULL) exit(-1);
	fread(aux,4,1,fAccesos); //Lee los 4 primeros bytes que encuentra en el fichero y los almacena en formato string 
	printf("\n Direccion leida: %s", aux);
	direccion = (short int)strtol(aux,NULL,16); //Convierte el string a formato numérico (teniendo en cuenta que estaban en base 16)
	aux[0] = getc(fAccesos); //Desecha el siguiente caracter "\n" contiguo a cada direccion 
	
	printf("\nSe quiere acceder a la direccion: %x\n", direccion);
	return direccion;
}

short int numLinea(short int direccion){ //Mantiene exclusivamente los bits de la linea, después los desplaza para que ocupen los bits de menor peso
	return ((direccion & 0b0001100000) >> 5);
}
short int etiqueta(short int direccion){ //Mantiene exclusivamente los bits de la etiqueta, después los desplaza para que ocupen los bits de menor peso
	return ((direccion & 0b1110000000) >> 7);
}
short int palabra(short int direccion){ //Mantiene exclusivamente los bits de la palabra, después los desplaza para que ocupen los bits de menor peso
	return (direccion & 0b0000011111);
}
short int numBloque(short int direccion){
	return ((direccion & 0b1111100000) >> 5);
}
int cargadoEnCache(short int direccion, T_LINEA_CACHE* cache){
		tiempoGlobal++;
	if(cache[numLinea(direccion)].ETQ == etiqueta(direccion)){ //Comprueba si la etiqueta de la direccion se corresponde con la etiqueta cargada en 
		return 1;											  //la linea correspondiente de la cache
	}else{
		return 0;
	}
}
unsigned char* startRAM(FILE *fichero){
	
	unsigned char linea[1024];	//	array de char donde se almacenara la informacion
	fichero = fopen("RAM.bin", "r");	// abrimos el fichero
	int cont = 0;	// contador para avanzar la posicion del array
	char c = 0;	//char auxiliar
	c = getc(fichero);// cargamos el primer caracter en c

	/// este bucle lee caracter por caracter hasta que se llegue al final del fichero
	while(!feof(fichero))
	{
		
		if(c != '\r' && c != '\0')
		{
			linea[cont] = c;
			cont++;
		}
		c =getc(fichero);
	}
	
	fclose(fichero);
	return linea;
}
void falloCache(short int direccion, T_LINEA_CACHE* cache, char* ram){
	numFallos++;
	short int linea = numLinea(direccion);
	short int bloque = numBloque(direccion);
	int i;
	printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X", tiempoGlobal, numFallos, direccion,etiqueta(direccion));
	printf(" linea %02X palabra %02X bloque %02X", linea, palabra(direccion), bloque);
	tiempoGlobal+=10;
	cache[linea].ETQ = etiqueta(direccion);
	for (i = 0; i < 8; i++){
		cache[linea].Datos[i] = ram[bloque+i];
	}
}

void imprimirCache(T_LINEA_CACHE cache){
	int i  =0;
	while(i<4){
		printf("ETQ: %x",cache.ETQ);
		for(int j = 8; j>0;j--){
			printf("Datos: %x", cache.Datos[j]);
		}
	}
}
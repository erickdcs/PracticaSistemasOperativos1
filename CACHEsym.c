// Alvaro Martinez 
// Erick Cardenas

typedef struct {
 short int ETQ;
 short int Datos[8];
} T_LINEA_CACHE;

void startUp(T_LINEA_CACHE* cache, unsigned char* ram, FILE* f);
unsigned char[1024] startRAM(FILE* f);
void accessMemory(FILE* f);













int main(int argc, char** argv){
	int tiempoGlobal = 0;
	int numFallos = 0;
	T_LINEA_CACHE chache[4];
	unsigned char[1024] ram;
	short int* siguienteAcceso;
	FILE* fRAM;
	FILE* fAccesos;
	char texto[100];
	
	startUp(cache, ram, fRAM, fAccesos);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}

void startUp(T_LINEA_CACHE* cache, unsigned char* ram, FILE* fRAM, FILE* fAccesos){
	int i;
	int j;
	
	fRAM = fopen("RAM.bin", "r");
	fAccesos = fopen("accesos_memoria.txt", "r");
	
	if(RAM == null){
		printf("No se ha podido abrir el fichero RAM.bin");
		exit(-1);
	}
	if(accesos == null){
		printf("No se ha podido abrir el fichero accesos_memoria.txt");
		exit(-1);
	}
	
	ram = startRam();
	
	for(i=0; i < 4; i++){
		cache[i].ETQ = 0xFF;
		for(j = 0; j < 8; j++){
			cache[i].Datos[j] = 0;
		}
	}
}
/*
Projeto:
    Leitor/Exibidor de arquivos .class.
*/

#include "Funcoes.h"
using namespace std;

int main(){
    
    /* cout << "Arquivos .class disponiveis: " << endl;
    string path = "./exemplos";
    int i = 0;

    for (const auto & entry : std::__fs::filesystem::directory_iterator(path)) {
        cout << i << " - " << entry.path() << endl;
        i++;
    }

    string arq;
    int arq_num;
    cout << endl << "Digite o NUMERO do arquivo:" << endl;
    cin >> setw(1) >> arq_num;

    i = 0;
    for (const auto & entry : std::__fs::filesystem::directory_iterator(path)) {
        //cout << arq_num << " " << i << endl;
        if(i == arq_num) {
            arq = entry.path();
            break;
        }
        i++;
    } */

    string arq;
    cout << "Arquivos:\n exemplos/Point.class\n exemplos/FooCorporation.class\n exemplos/double_aritmetica.class\n exemplos/long_cast.class\n exemplos/lookupswitch.class\n exemplos/method_test.class\n exemplos/multi_sem_string.class\n exemplos/soma_certo.class" << endl;
    cout <<"Nome do arquivo .class que deseja exibir: " << endl;
    cin >> arq;

    ifstream arqv(arq, ios::binary); // Leitura do arquivo .class
    ClassFile *cf = (ClassFile *) malloc(sizeof(ClassFile));

    cf->magic = lerArqM(arqv);
    if(cf->magic != 0xcafebabe){ // Verifica se eh um arquivo .class
        setlocale(LC_ALL, "");
        cout << ("Arquivo .class não encontrado") << endl;
        setlocale(LC_ALL, NULL);
        return 0;
    }
    
    cf->versionm = lerArq(arqv);
    cf->versionM = lerArq(arqv);
    cf->constant_pool_count = lerArq(arqv);
    getCpInfo(cf, arqv);
    cf->acess_flags = lerArq(arqv);
    cf->this_class = lerArq(arqv);
    cf->super_class = lerArq(arqv);
    cf->interfaces_count = lerArq(arqv);
    getInterface(cf, arqv);
    cf->fields_count = lerArq(arqv);
    getFields(cf, arqv);
    getMethods(cf, arqv);
    cf->count_atributos = lerArq(arqv);
    cf->attributes = readAttributeArray(cf->count_atributos, cf, arqv);

    // imprimir em um arquivo txt
    FILE *gerado;
    int result;

    gerado = fopen("resultados/result.txt", "w+");  // Cria um arquivo texto para gravação
        if (gerado == NULL){
            printf("Erro ao criar arquivo\n");
        }

    result = fputs("---------------- General Information ----------------\n", gerado);
    fprintf(gerado,"Magic = %#010x\n", cf->magic);
    fprintf(gerado,"Minor Version = %u\n", cf->versionm);
    fprintf(gerado,"Major Version = %u\n", cf->versionM);
    float javav= (1 + ((float)(cf->versionM - 44))/10);
    fprintf(gerado,"Java Version = %.3f\n", javav);
    fprintf(gerado,"Constant Pool Count = %u\n", cf->constant_pool_count);
    fclose(gerado);
    
    generalInformation(cf->acess_flags); 
    mostraClassName(cf);

    gerado = fopen("resultados/result.txt", "a");
    fprintf(gerado,"Interfaces Count = %u\n", cf->interfaces_count);
    fprintf(gerado,"Fields Count = %u\n", cf->fields_count);
    fprintf(gerado,"Methods Count = %u\n", cf->methods_count);
    fprintf(gerado,"Attributes Count = %u\n", cf->count_atributos);

    fprintf(gerado,"\n---> COSTANT POOL\n");
    fclose(gerado);
    ConstantePool(cf);

    gerado =  fopen("resultados/result.txt", "a");
    fprintf(gerado,"\n---> INTERFACES\n");
    fclose(gerado);
    interface(cf);
    

    gerado =  fopen("resultados/result.txt", "a");
    fprintf(gerado,"\n---> FIELDS\n");
    fclose(gerado);
    fields(cf, arqv);
    

    gerado =  fopen("resultados/result.txt", "a");
    fprintf(gerado,"\n---> METHODS\n"); 
    fclose(gerado);
    methods(cf, arqv);
   
    gerado =  fopen("resultados/result.txt", "a");
    fprintf(gerado,"\n---> ATTRIBUTES\n");
    fclose(gerado);
    attributes(cf, cf->count_atributos, cf->attributes, arqv);
    
    free(cf->interfaces);
    free(cf->methods);
    free(cf->fields);
    free(cf->attributes);
    free(cf->constant_pool);
    
    free(cf);
    arqv.close();

    return 0;
}
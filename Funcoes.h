
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <bits/stdc++.h>
#include <fstream>
#include <math.h>
#include <iostream>
//#include <filesystem>

// Solução para erro no MacOS
#if !defined(__MACH__)
    #include <malloc.h>
#endif

// Solução para erro no MacOS
#if !defined(__MACH__)
    #include <malloc.h>
#endif

#define uchar unsigned char
#define ushort unsigned short
#define uint unsigned int
#define ulong unsigned long
using namespace std;



// Struct representando a tabela_excecoes
struct tabela_excecoes{
    ushort pcinicio;
    ushort pcfim;
    ushort manipulapc;
    ushort pegartipo;
}; 
typedef struct tabela_excecoes tabela_excecoes;


struct info_de_cp{
    uchar tag;
    union{                  // uniao entre os tipos diferentes 
        struct{
            ushort nome_do_indice;
        }Class;
    
        struct{
            ushort indice_class;
            ushort inform_tiponome;
        }def_field;

        struct{
            ushort indice_class;
            ushort inform_tiponome;
        }def_method;

        struct{
            ushort indice_class;
            ushort inform_tiponome;
        }Interfacedef_method;

        struct{
            ushort indice_string;
        }String;

        struct{
            uint bytes;
        }Integer;

        struct{
            uint bytes;
        }Float;

        struct{
            uint high_bytes;
            uint low_bytes;
        }Long;

        struct{
            uint high_bytes;
            uint low_bytes;
        }Double;

        struct{
            ushort nome_do_indice;
            ushort descricao_de_tipo;
        }NameAndType;

        struct{
            ushort tamanho; 
            uchar* bytes;
        }Utf8;
        
        struct{
            uchar referencia_tipo;
            ushort indice_referencia;
        }MethodHandle;  

        struct{
            ushort descricao_de_tipo;
        }MethodType; 

        struct{
            ushort bootstrap_method_attr_indice;
            ushort indice_tiponome;
        }InvokeDynamic;
    }CONST; 
};
typedef struct info_de_cp info_de_cp;

// Struct representando Local Variable Table
struct tabela_variavel_local{
    ushort pcinicio;
    ushort tamanho;
    ushort nome_do_indice;
    ushort descricao_de_tipo;
    ushort indice;
};
typedef struct tabela_variavel_local tabela_variavel_local;

// Struct representando Line Number Table
struct tabela_tipo_linha{
    ushort pcinicio;
    ushort line_number;
};
typedef struct tabela_tipo_linha tabela_tipo_linha;


// Struct representando informacao dos atributos
struct info_atributos{
    ushort attribute_nome_do_indice;
    ushort attribute_tamanho;
    union{      //para cada tipo de atributo existe uma struct com seus respectivos componentes
        struct{
            ushort constantvalue_indice;
        }valor_constante;

        struct{
            ushort max_stack;
            ushort max_locals; 
            uint code_tamanho;
            uchar* code;
            ushort tamanho_tabela_excecoes;
            tabela_excecoes* exc_table;
            ushort count_atributos;
            info_atributos* attributes;
        }Code_attribute;

        struct{
            ushort num_excecoes;
            ushort* tabela_indice_excecoes;
        }Exceptions_attribute;

        struct{
            ushort sourcefile;
        }SourceFile;

        struct{
            ushort tabela_tipo_linha_tamanho;
            tabela_tipo_linha* line_table;
        }LineNumberTable;

        struct{
            ushort tabela_variavel_local_tamanho;
            tabela_variavel_local* local_variable;
        }LocalVariableTable_attribute;

    }ATTR;
};
typedef struct info_atributos info_atributos;

// Struct representando os Fields
struct info_field{
    ushort access_flags;
    ushort nome_do_indice;
    ushort descricao_de_tipo;
    ushort count_atributos;
    info_atributos* attributes;             //vetor do tipo info_atributos
};
typedef struct info_field info_field;

// Struct representando os Metodos
struct method_info{
    ushort access_flags;
    ushort nome_do_indice;
    ushort descricao_de_tipo;
    ushort count_atributos;
    info_atributos* attributes;             //vetor do tipo info_atributos
};
typedef struct method_info method_info;

// Struct representando o Arquivo Class
struct ClassFile{
    uint magic;
    ushort versionm;
    ushort versionM;
    ushort constant_pool_count;
    info_de_cp* constant_pool;
    ushort acess_flags;
    ushort this_class;
    ushort super_class;
    ushort interfaces_count;
    ushort* interfaces;
    ushort fields_count;
    info_field* fields;
    ushort methods_count;
    method_info* methods;
    ushort count_atributos;
    info_atributos* attributes;
};
typedef struct ClassFile ClassFile;

/* FUNCOES */
double* toDouble(long long num);
long toLong(ulong num);
void getInterface(ClassFile* cf, ifstream& arqv);
void generalInformation(ushort acess_flags);
void attributes(ClassFile* cf, ushort count_atributos, info_atributos* attributes, ifstream& arqv);
void mostraClassName(ClassFile* cf);
void mostraExceptions(ClassFile* cf, info_atributos* attributes);
void interface(ClassFile* cf);
void getMethods(ClassFile* cf, ifstream& arqv);
void getFields(ClassFile* cf, ifstream& arqv);
short getAttributeType (info_atributos* atrib, ClassFile* cf, ifstream& arqv);
ushort lerArq(ifstream& arqv);
uint lerArqM(ifstream& arqv);
void getCpInfo(ClassFile* cf, ifstream& arqv);
void ConstantePool(ClassFile* cf);
void setUTF8(char* str);
void coutindice(ClassFile* cf, ushort indice);
void printCPInfo(ClassFile* cf, ushort indice);
float toFloat(uint num);
info_atributos* readAttributeArray(ushort count_atributos, ClassFile* cf, ifstream& arqv);
void fields(ClassFile* cf, ifstream& arqv);
void mostraLineNumberTable(ClassFile* cf, info_atributos* attributes);
void methods(ClassFile* cf, ifstream& arqv);
void mostraCode(ClassFile* cf, info_atributos* attr, ifstream& arqv);
void mostraMethodFlags(ushort access_flags);
void printInstructions(ClassFile* cf, ushort indice);
void mostraByteCode(info_atributos* attr, info_de_cp* constantPool, ClassFile* cf);
//void gerador(ClassFile* cf, ifstream& arqv);
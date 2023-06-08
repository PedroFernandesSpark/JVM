#include "Funcoes.h"

const char* OPCODE[] = {
    "nop","aconst_null","iconst_m1","iconst_0","iconst_1","iconst_2","iconst_3","iconst_4","iconst_5",
    "lconst_0", "lconst_1","fconst_0","fconst_1","fconst_2","dconst_0","dconst_1","bipush","sipush",
    "ldc","ldc_w","ldc2_w","iload","lload","fload","dload","aload","iload_0","iload_1","iload_2","iload_3",
    "lload_0","lload_1","lload_2","lload_3","fload_0","fload_1","fload_2","fload_3","dload_0","dload_1",
    "dload_2","dload_3","aload_0","aload_1","aload_2","aload_3","iaload","laload","faload","daload",
    "aaload","baload","caload","saload","istore","lstore","fstore","dstore","astore","istore_0",
    "istore_1","istore_2","istore_3","lstore_0","lstore_1","lstore_2","lstore_3","fstore_0","fstore_1",
    "fstore_2","fstore_3","dstore_0","dstore_1","dstore_2","dstore_3","astore_0","astore_1","astore_2",
    "astore_3","iastore","lastore","fastore","dastore","aastore","bastore","castore","sastore","pop",
    "pop2","dup","dup_x1","dup_x2","dup2","dup2_x1","dup2_x2","swap","iadd","ladd","fadd","dadd","isub",
    "lsub","fsub","dsub","imul","lmul","fmul","dmul","idiv","ldiv","fdiv","ddiv","irem","lrem","frem",
    "drem","ineg","lneg","fneg","dneg","ishl","lshl","ishr","lshr","iushr","lushr","iand","land","ior",
    "lor","ixor","lxor","iinc","i2l","i2f","i2d","l2i","l2f","l2d","f2i","f2l","f2d","d2i","d2l","d2f",
    "i2b","i2c","i2s","lcmp","fcmpl","fcmpg","dcmpl","dcmpg","ifeq","ifne","iflt","ifge","ifgt","ifle",
    "if_icmpeq","if_icmpne","if_icmplt","if_icmpge","if_icmpgt","if_icmple","if_acmpeq","if_acmpne","goto",
    "jsr","ret","tableswitch","lookupswitch","ireturn","lreturn","freturn","dreturn","areturn","return",
    "getstatic","putstatic","getfield","putfield","invokevirtual","invokespecial","invokestatic",
    "invokeinterface","invokedynamic","new","newarray","anewarray","arraytamanho","athrow","checkcast",
    "instanceof","monitorenter","monitorexit","wide","multianewarray","ifnull","ifnonnull","goto_w",
    "jsr_w","breakpoint","NULL","NULL","NULL","NULL","NULL","NULL","NULL",
    "NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL",
    "NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL",
    "NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL",
    "NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL",
    "NULL","NULL","NULL","NULL","impdep1","impdep2"
};

int pc=0;

float toFloat(uint num){
    float result;
    memcpy(&result, &num, sizeof(float));
    return result;
}

// high e low bytes -> double
double toDouble(info_de_cp* cp){
    long long valor = ((long long)(cp->CONST.Double.high_bytes)<<32) | (long long)cp->CONST.Double.low_bytes;
	int sinal = ((valor>>63) == 0) ? 1 : -1;
	int expon = ((valor>>52) & 0x7ffL);
	long long mant = (expon == 0) ? ((valor & 0xfffffffffffffL) << 1) : ((valor & 0xfffffffffffffL) | 0x10000000000000L);
	double retorno = sinal*mant*(pow(2,expon-1075));
	return retorno;
}

// unsigned de 64 bits -> long
long toLong(ulong num) {
    long result;
    memcpy(&result, &num, sizeof(long));
    return result;
}

//Get funtions

// read 1 byte(unsigned char)
uchar Ler(ifstream &arqv){ 
    uchar byte = arqv.get();

    return byte;
}

// read 2 bytes(unsigned short)
ushort lerArq(ifstream& arqv){
    ushort byte2;
    byte2 = Ler(arqv);
    byte2 = (byte2 << 8) | Ler(arqv);
    return byte2;
}

// read 4 bytes(unsigned int)
uint lerArqM(ifstream& arqv){
    uint byte4;
    byte4 = lerArq(arqv);
    byte4 = (byte4 << 16) | lerArq(arqv);
    return byte4;
}

// read interface
void getInterface(ClassFile* cf, ifstream& arqv) {
    if(cf->interfaces_count > 0){
        cf->interfaces = (ushort*)malloc(sizeof(ushort) * cf->interfaces_count);
        for(ushort* i = cf->interfaces; i < cf->interfaces + cf->interfaces_count; i++){
            *i = lerArq(arqv);
        }
    }
    else{
        cf->interfaces = NULL;
    }
}

// read methods
void getMethods(ClassFile* cf, ifstream& arqv){
    cf->methods_count = lerArq(arqv);
    if (cf->methods_count > 0){
        cf->methods = (method_info*)malloc(sizeof(method_info) * cf->methods_count);
        for(method_info* i = cf->methods; i < cf->methods + cf->methods_count; i++) {
            i->access_flags = lerArq(arqv);
            i->nome_do_indice = lerArq(arqv);
            i->descricao_de_tipo = lerArq(arqv);;
            i->count_atributos = lerArq(arqv);
            i->attributes = readAttributeArray(i->count_atributos, cf, arqv);
        }
    }
    else
        cf->methods = NULL;
}

// identifies AttributeType number
short getAttributeType (info_atributos *atrib, ClassFile* cf, ifstream& arqv){
    FILE *gerado;
    ushort a_nome_do_indice = atrib->attribute_nome_do_indice;
    info_de_cp* cp = cf->constant_pool + a_nome_do_indice - 1;

    ushort tamanho = cp->CONST.Utf8.tamanho;
    string bytes;

    for(int i = 0; i < cp->CONST.Utf8.tamanho; i++) {
        bytes.push_back(cp->CONST.Utf8.bytes[i]);
    }
    bytes.push_back('\0');

    if(tamanho != 0){
        switch(bytes[0]){
            case 'C':
                if(!strcmp((char *) bytes.c_str(), "Code")){
                    return  1;  // code
                }
                else if(!strcmp((char *) bytes.c_str(), "ConstantValue")){
                    return  0;  // constant value
                }
                break;

            case 'E':
                if(!strcmp((char *) bytes.c_str(), "Exceptions")){
                    return  2; // exceptions attribute
                }
                break;

            case 'L':
                if(!strcmp((char *) bytes.c_str(), "LineNumberTable")){
                    return  6;  //line number table
                }
                else if(!strcmp((char *) bytes.c_str(), "LocalVariableTable")){
                    return  7;  //local variable table
                }
                break;
            case 'S':
                if(!strcmp((char *) bytes.c_str(), "SourceFile")){
                    return  5;  //source file
                }else{
                    for(int i = 0; i < atrib->attribute_tamanho; i++) {
                        Ler(arqv);
                    }
                }
                break;
            default:
                gerado =  fopen("resultados/result.txt", "a");
                printf("Atributo nao encontrado\n");
                fprintf(gerado,"Atributo nao encontrado\n");
                fclose(gerado);
                return  9;  //unknown
        }
    }
    return  9;//unknown
}

// read attribute
info_atributos* readAttributeArray(ushort count_atributos, ClassFile* cf, ifstream& arqv){
    info_atributos* vet = (info_atributos*)malloc(count_atributos * sizeof(info_atributos));
    info_atributos* attr = vet;

    while(attr < (vet + count_atributos)){
        attr->attribute_nome_do_indice = lerArq(arqv); //
        attr->attribute_tamanho = lerArqM(arqv);

        //attribute type
        short attributeType = getAttributeType(attr, cf, arqv);

        if (attributeType == 0){        // constantvalue
            attr->ATTR.valor_constante.constantvalue_indice = lerArq(arqv);
        }
        else if (attributeType == 1){   // code
            attr->ATTR.Code_attribute.max_stack = lerArq(arqv);
            attr->ATTR.Code_attribute.max_locals = lerArq(arqv);
            attr->ATTR.Code_attribute.code_tamanho = lerArqM(arqv);

            if(attr->ATTR.Code_attribute.code_tamanho > 0){
                attr->ATTR.Code_attribute.code = (uchar *) malloc(attr->ATTR.Code_attribute.code_tamanho * sizeof(uchar));

                uchar *code_aux = attr->ATTR.Code_attribute.code;
                while(code_aux < (attr->ATTR.Code_attribute.code + attr->ATTR.Code_attribute.code_tamanho)){
                    *code_aux = Ler(arqv);
                    code_aux++;
                }
            }

            attr->ATTR.Code_attribute.tamanho_tabela_excecoes = lerArq(arqv);
            if(attr->ATTR.Code_attribute.tamanho_tabela_excecoes){
                attr->ATTR.Code_attribute.exc_table =
                (tabela_excecoes *) malloc(attr->ATTR.Code_attribute.tamanho_tabela_excecoes * sizeof(tabela_excecoes));

                tabela_excecoes *e_table_aux = attr->ATTR.Code_attribute.exc_table;
                while (e_table_aux < (attr->ATTR.Code_attribute.exc_table + attr->ATTR.Code_attribute.tamanho_tabela_excecoes)){
                    e_table_aux->pcinicio = lerArq(arqv);
                    e_table_aux->pcfim = lerArq(arqv);
                    e_table_aux->manipulapc = lerArq(arqv);
                    e_table_aux->pegartipo = lerArq(arqv);
                    e_table_aux++;
                }
            }
            attr->ATTR.Code_attribute.count_atributos = lerArq(arqv);
            attr->ATTR.Code_attribute.attributes = readAttributeArray(attr->ATTR.Code_attribute.count_atributos, cf, arqv);
        }

        else if (attributeType == 2){ // exceptions
            attr->ATTR.Exceptions_attribute.num_excecoes = lerArq(arqv);
            attr->ATTR.Exceptions_attribute.tabela_indice_excecoes = (ushort *) malloc(attr->ATTR.Exceptions_attribute.num_excecoes * sizeof(ushort));
            ushort * e_indice_table_aux = attr->ATTR.Exceptions_attribute.tabela_indice_excecoes;
            while(e_indice_table_aux < (attr->ATTR.Exceptions_attribute.tabela_indice_excecoes + attr->ATTR.Exceptions_attribute.num_excecoes)){
                *e_indice_table_aux = lerArq(arqv);
            }
        }
        else if (attributeType == 5){ //sourcefile
            attr->ATTR.SourceFile.sourcefile = lerArq(arqv);
        }
        else if (attributeType == 6){ //linenumbertable
            ushort byte2 = lerArq(arqv);
            attr->ATTR.LineNumberTable.tabela_tipo_linha_tamanho = byte2;
            if(attr->ATTR.LineNumberTable.tabela_tipo_linha_tamanho > 0){
                attr->ATTR.LineNumberTable.line_table = (tabela_tipo_linha*)
                malloc(attr->ATTR.LineNumberTable.tabela_tipo_linha_tamanho * sizeof(tabela_tipo_linha));

                tabela_tipo_linha* l_number_aux = attr->ATTR.LineNumberTable.line_table;
                while(l_number_aux < (attr->ATTR.LineNumberTable.line_table + attr->ATTR.LineNumberTable.tabela_tipo_linha_tamanho)){
                    l_number_aux->pcinicio = lerArq(arqv);
                    l_number_aux->line_number = lerArq(arqv);
                    l_number_aux++;
                }
            }
        }
        else if (attributeType == 7){ // variabletable
            attr->ATTR.LocalVariableTable_attribute.tabela_variavel_local_tamanho = lerArq(arqv);

            if(attr->ATTR.LocalVariableTable_attribute.tabela_variavel_local_tamanho)
            {
                attr->ATTR.LocalVariableTable_attribute.local_variable = (tabela_variavel_local *)
                malloc(attr->ATTR.LocalVariableTable_attribute.tabela_variavel_local_tamanho * sizeof(tabela_variavel_local));

                tabela_variavel_local* l_variable_aux = attr->ATTR.LocalVariableTable_attribute.local_variable;
                while (l_variable_aux < (attr->ATTR.LocalVariableTable_attribute.local_variable + attr->ATTR.LocalVariableTable_attribute.tabela_variavel_local_tamanho))
                {
                    l_variable_aux->pcinicio = lerArq(arqv);
                    l_variable_aux->tamanho = lerArq(arqv);
                    l_variable_aux->nome_do_indice = lerArq(arqv);
                    l_variable_aux->descricao_de_tipo = lerArq(arqv);
                    l_variable_aux->indice = lerArq(arqv);
                    l_variable_aux++;
                }
            }
        }
        attr++;
    }
    return vet;
}

// read Fields
void getFields(ClassFile* cf, ifstream& arqv){
    if(cf->fields_count > 0){
        cf->fields = (info_field* ) malloc((cf->fields_count)*sizeof(info_field));
        info_field* aux_field = cf->fields;
        while(aux_field < (cf->fields + cf->fields_count)){
            aux_field->access_flags = lerArq(arqv);
            aux_field->nome_do_indice = lerArq(arqv);
            aux_field->descricao_de_tipo = lerArq(arqv);
            aux_field->count_atributos = lerArq(arqv);
            aux_field->attributes = readAttributeArray(aux_field->count_atributos, cf, arqv);
            aux_field++;
        }
    }else{
        cf->fields = NULL;
    }
}

// read constant pool
void getCpInfo(ClassFile* cf, ifstream& arqv){
    cf->constant_pool = (info_de_cp *) malloc((cf->constant_pool_count-1)*sizeof(info_de_cp));
    info_de_cp* cp;
    for(cp = cf->constant_pool; cp < (cf->constant_pool + cf->constant_pool_count - 1); cp++){
        uchar a = Ler(arqv);
        cp->tag = a;
        switch(cp->tag){
            // utf_8
            case 1:
                cp->CONST.Utf8.tamanho = lerArq(arqv);
                if(cp->CONST.Utf8.tamanho>0){
                    cp->CONST.Utf8.bytes = (uchar*) malloc((cp->CONST.Utf8.tamanho + 1)*sizeof(uchar));
                    for(ushort j=0;j<cp->CONST.Utf8.tamanho;j++){
                        cp->CONST.Utf8.bytes[j] = Ler(arqv);
                    }
                    cp->CONST.Utf8.bytes[cp->CONST.Utf8.tamanho] = '\0';
                    
                }else{
                    cp->CONST.Utf8.bytes = NULL;
                }
                break;
            // int
            case 3:  
                cp->CONST.Integer.bytes = lerArqM(arqv);
                break;
            // float
            case 4: 
                cp->CONST.Float.bytes = lerArqM(arqv);
                break;
            // long
            case 5:
                cp->CONST.Long.high_bytes = lerArqM(arqv);
                cp->CONST.Long.low_bytes = lerArqM(arqv);
                cp++;
                break;
            // double
            case 6: 
                cp->CONST.Double.high_bytes = lerArqM(arqv);
                cp->CONST.Double.low_bytes = lerArqM(arqv);
                cp++;
                break;
            // class
            case 7:
                cp->CONST.Class.nome_do_indice = lerArq(arqv);
                break;
            // string
            case 8:  
                cp->CONST.String.indice_string = lerArq(arqv);
                break;
            // field reference
            case 9:  
                cp->CONST.def_field.indice_class = lerArq(arqv);
                cp->CONST.def_field.inform_tiponome = lerArq(arqv);
                break;
            // method reference
            case 10:
                cp->CONST.def_method.indice_class = lerArq(arqv);
                cp->CONST.def_method.inform_tiponome = lerArq(arqv);
                break;
            // interface method reference
            case 11:
                cp->CONST.Interfacedef_method.indice_class = lerArq(arqv);
                cp->CONST.Interfacedef_method.inform_tiponome = lerArq(arqv);
                break;
            // name and type
            case 12: 
                cp->CONST.NameAndType.nome_do_indice = lerArq(arqv);
                cp->CONST.NameAndType.descricao_de_tipo = lerArq(arqv);
                break;
            // method handle
            case 15:
                cp->CONST.MethodHandle.referencia_tipo = Ler(arqv);
                cp->CONST.MethodHandle.indice_referencia = lerArq(arqv);
                break;
            // method type
            case 16:
                cp->CONST.MethodType.descricao_de_tipo = lerArq(arqv);
                break;
            // invoke dynamic
            case 18:
                cp->CONST.InvokeDynamic.bootstrap_method_attr_indice = lerArq(arqv);
                cp->CONST.InvokeDynamic.indice_tiponome = lerArq(arqv);
                break;

            default:
                printf("Erro!");
                exit(EXIT_FAILURE);
        }
    }
}

//Print funtions


// Print in UTF8
void setUTF8(char* ss){
    FILE *gerado;
    gerado =  fopen("resultados/result.txt", "a");

    setlocale(LC_ALL, "");
    string str(ss);
    fprintf(gerado,"%s",ss);
    setlocale(LC_ALL, NULL);
    fclose(gerado);
}


// print do pool of constant pool
void ConstantePool(ClassFile* cf){
    ushort i;
    FILE *gerado;

    for(i = 0; i < (cf->constant_pool_count-1); i++){
        gerado =  fopen("resultados/result.txt", "a");
        fprintf(gerado,"\t[%d] ",i+1);
        fclose(gerado);
        printCPInfo(cf, i);
        if(((cf->constant_pool + i)->tag == 5) or ((cf->constant_pool + i)->tag == 6)){
            i++;
        }
    }
    gerado =  fopen("resultados/result.txt", "a");
    fprintf(gerado,"\n");
    fclose(gerado);
}


// print informations constant pool
void printCPInfo(ClassFile* cf, ushort indice){
    FILE *gerado;
    stringstream ss;
    info_de_cp  * cp = cf->constant_pool + indice;
    ulong show_long;

    
    switch (cp->tag){
        case 1:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "UTF-8:\n\t\tString: ");
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char*) cp->CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado,"\n");
            fclose(gerado);
            break;
        case 3:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Integer: \n");
            fprintf(gerado, "\t\tBytes: 0x");
            fprintf(gerado,"%08x\n", cp->CONST.Float.bytes);
            fprintf(gerado, "\t\tInteger: %d", cp->CONST.Integer.bytes);
            fclose(gerado);
            break;
        case 4:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Float: \n");
            fprintf(gerado,"\t\tBytes: 0x");
            fprintf(gerado, "%08x\n", cp->CONST.Float.bytes);
            fprintf(gerado,"\t\tFloat: %f\n", toFloat(cp->CONST.Float.bytes));
            fclose(gerado);
            break;
        case 5:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Long: \n");
            fprintf(gerado,"\t\tHigh Bytes: 0x%u\n", cp->CONST.Long.high_bytes);
            fprintf(gerado,"\t\tLow Bytes: 0x%u\n", cp->CONST.Long.low_bytes);
            show_long = ((unsigned long long)cp->CONST.Long.high_bytes << 32) | (unsigned long long)cp->CONST.Long.low_bytes;
            fprintf(gerado, "\t\tLong: %ld\n", toLong(show_long));
            fclose(gerado);
            break;
        case 6:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Double: \n");
            fprintf(gerado,"\t\tHigh Bytes: 0x%u\n", cp->CONST.Double.high_bytes);
            fprintf(gerado,"\t\tLow Bytes: 0x%u\n", cp->CONST.Double.low_bytes);
            fprintf(gerado, "\t\tDouble: %f\n", toDouble(cp));
            fclose(gerado);
            break;
        case 7:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Class: \n");
            fprintf(gerado, "\t\tClass Name: %u ", cp->CONST.Class.nome_do_indice);
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cp->CONST.Class.nome_do_indice -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado, "\n");
            fclose(gerado);
            break;
        case 8:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "String: \n");
            fprintf(gerado, "\t\tClass Name: %u", cp->CONST.String.indice_string);
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cp->CONST.String.indice_string -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado, "\n");
            fclose(gerado);
            break;
        case 9:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Field Reference: \n");
            fprintf(gerado, "\t\tClass Name: %u ", cp->CONST.def_field.indice_class);
            fprintf(gerado,(char *)cf->constant_pool[cf->constant_pool[cp->CONST.def_field.indice_class -1].CONST.Class.nome_do_indice -1].CONST.Utf8.bytes);
            fprintf(gerado, "\n");
            fprintf(gerado, "\t\tName and Type: %u ", cp->CONST.def_field.inform_tiponome);
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cf->constant_pool[cp->CONST.def_field.inform_tiponome -1].CONST.NameAndType.nome_do_indice -1].CONST.Utf8.bytes);
            fprintf(gerado, ":");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cf->constant_pool[cp->CONST.def_field.inform_tiponome -1].CONST.NameAndType.descricao_de_tipo -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado,"\n");
            fclose(gerado);
            break;
        case 10:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Method Reference: \n");
            fprintf(gerado, "\t\tClass Name: %u ", cp->CONST.def_method.indice_class);
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cf->constant_pool[cp->CONST.def_method.indice_class -1].CONST.Class.nome_do_indice -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado,"\n");
            fprintf(gerado, "\t\tName and Type: %u ", cp->CONST.def_method.inform_tiponome);
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cf->constant_pool[cp->CONST.def_method.inform_tiponome -1].CONST.NameAndType.nome_do_indice -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado,":");
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cf->constant_pool[cp->CONST.def_method.inform_tiponome -1].CONST.NameAndType.descricao_de_tipo -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado,"\n");
            fclose(gerado);
            break;
        case 11:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Interface Method reference: \n");
            fprintf(gerado, "Class Name: %u ", cp->CONST.Interfacedef_method.indice_class);
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cf->constant_pool[cp->CONST.Interfacedef_method.indice_class -1].CONST.Class.nome_do_indice -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado, "\n");
            fprintf(gerado,"\t\tName and Type: %u ", cp->CONST.Interfacedef_method.inform_tiponome);
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cf->constant_pool[cp->CONST.Interfacedef_method.inform_tiponome-1].CONST.NameAndType.nome_do_indice -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado, ":");
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cf->constant_pool[cp->CONST.Interfacedef_method.inform_tiponome -1].CONST.NameAndType.descricao_de_tipo -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado,"\n");
            fclose(gerado);
            break;
        case 12:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Name and Type: \n");
            fprintf(gerado, "\t\tName: %x ", cp->CONST.NameAndType.nome_do_indice);
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cp->CONST.NameAndType.nome_do_indice-1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado,"\n");
            fprintf(gerado, "\t\tDescriptor: %u ", cp->CONST.NameAndType.descricao_de_tipo);
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cp->CONST.NameAndType.descricao_de_tipo -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado,"\n");
            fclose(gerado);
            break;
        case 15:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Method Handle: ");
            fprintf(gerado, "\t\tReference Kind: %u\n", cp->CONST.MethodHandle.referencia_tipo);
            fprintf(gerado, "\t\tReference Index: %u\n", cp->CONST.MethodHandle.indice_referencia );
            fclose(gerado);
            break;
        case 16:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Method Type: ");
            fprintf(gerado,"\t\tClass Name: %u ", cp->CONST.MethodType.descricao_de_tipo);
            setlocale(LC_ALL, "");
            fprintf(gerado,"%s", (char *) cf->constant_pool[cp->CONST.MethodType.descricao_de_tipo -1].CONST.Utf8.bytes);
            setlocale(LC_ALL, NULL);
            fprintf(gerado, "\n");
            fclose(gerado);
            break;
        case 18:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "Invoke Dynamic: \n");
            fprintf(gerado, "\t\tBootstrap Method: %u ", cp->CONST.InvokeDynamic.bootstrap_method_attr_indice);
            fprintf(gerado, "\t\tName and Type: %u ",cf->constant_pool[indice].CONST.InvokeDynamic.indice_tiponome);
            fprintf(gerado, "\n");
            fclose(gerado);
            break;
        default:
            gerado =  fopen("resultados/result.txt", "a");
            fprintf(gerado, "ERRO!!!\n");
            fclose(gerado);
            break;
    }
    
}

// print Interfaces
void interface(ClassFile* cf){
    uint j = 0;
    FILE *gerado;
    gerado =  fopen("resultados/result.txt", "a");

    if(cf->interfaces_count == 0){
        fprintf(gerado,"\tNo Intefaces\n");
    }
    fclose(gerado);
    for(ushort * i = cf->interfaces; i < cf->interfaces+cf->interfaces_count; i++){
        gerado =  fopen("resultados/result.txt", "a");
        fprintf(gerado,"\tInterface[%d]: ",j);
        fprintf(gerado,"cp_info#%d\n",*i);
        fclose(gerado);

        printCPInfo(cf, *i-1);

        gerado =  fopen("resultados/result.txt", "a");
        fprintf(gerado,"\n");
        fclose(gerado);
        j++;
    }
    
}

// print Metodos
void methods(ClassFile* cf, ifstream& arqv) {
    FILE *gerado;
    gerado =  fopen("resultados/result.txt", "a");

    fprintf(gerado,"\tMethod count: %u\n",cf->methods_count);

    if(cf->methods_count == 0){
        fprintf(gerado,"Nao possui methods\n");
    }
    fclose(gerado);

    ushort i = 0;
    for(method_info* it = cf->methods; it < cf->methods + cf->methods_count; ++it) {
        

        gerado =  fopen("resultados/result.txt", "a");
        fprintf(gerado,"\tMethod[%d]: ",i);
        fprintf(gerado,(char *)cf->constant_pool[it->nome_do_indice-1].CONST.Utf8.bytes);       
        fprintf(gerado,"\n\tDescriptor: ");
        fprintf(gerado,(char *)cf->constant_pool[it->descricao_de_tipo-1].CONST.Utf8.bytes);       
        
        fprintf(gerado,"\n\t");
        fclose(gerado);

        mostraMethodFlags(it->access_flags);

        gerado =  fopen("resultados/result.txt", "a");
        fprintf(gerado,"\tMember Count: %u\n",it->count_atributos);
        fclose(gerado);

        
        attributes(cf, it->count_atributos, it->attributes, arqv);
        
        gerado =  fopen("resultados/result.txt", "a");
        fprintf(gerado,"\n");
        fclose(gerado);

        i++;
    }
    cout << endl;
    fprintf(gerado,"\n");
    fclose(gerado);
}

// print Exceptions
void mostraExceptions(ClassFile* cf, info_atributos* attributes){
    ushort number = attributes->ATTR.Exceptions_attribute.num_excecoes;
    cout << "Number of exceptions: " << number << endl;
    ushort contador = 0;
    for(ushort * i = attributes->ATTR.Exceptions_attribute.tabela_indice_excecoes; i < i+number; i++){
        cout << "cp_info#" << contador;
        printCPInfo(cf, *i);
        contador++;
    }
}

// print Line Number Table
void mostraLineNumberTable(ClassFile* cf, info_atributos* attributes){
    ushort tam = attributes->ATTR.LineNumberTable.tabela_tipo_linha_tamanho;
    cout << "\tLine Number Table tamanho: " << tam << endl;
    cout << "\tAtributte info: " << endl;
    ushort contador = 0;
    cout << "\tNr.\t\tStartPC\t\tLine Number" << endl;
    tabela_tipo_linha * i = attributes->ATTR.LineNumberTable.line_table;
    while(contador<tam){
        cout << "\t" << contador << "\t\t" << i->pcinicio;
        cout << "\t\t\t" << i->line_number << endl;
        contador++;
        i++;
    }
}

// print Local Variable Table
void mostraLocalVariableTable(ClassFile* cf, info_atributos* attributes){
    ushort tam = attributes->ATTR.LocalVariableTable_attribute.tabela_variavel_local_tamanho;
    cout << "\tLocal Variable tamanho: " << tam << endl;
    cout << "\tAtributte info: " << endl;
    ushort contador = 0;
    cout << "Nr.\t\tStartPC\t\tTamanho\t\tName\t\tDescriptor\t\tindice" << endl;
    tabela_variavel_local * i = attributes->ATTR.LocalVariableTable_attribute.local_variable;
    while(contador<tam){
        cout << contador << "\t\t" << i->pcinicio;
        cout << "\t\t" << i->tamanho << "\t\t";
        cout << (char *)cf->constant_pool[i->nome_do_indice].CONST.Utf8.bytes;
        cout << "\t\t";
        cout << (char *)cf->constant_pool[i->descricao_de_tipo].CONST.Utf8.bytes;
        cout << "\t\t";
        cout << (char *)cf->constant_pool[i->indice].CONST.Utf8.bytes;
        contador++;
        i++;
    }
}

// auxiliar print of atributos (decide qual atributo deve ser impreco)
// Funcao auxiliar de imprimir os atributos (decide qual atributo deve ser impreco)
void attributes(ClassFile* cf, ushort count_atributos, info_atributos* attributes, ifstream& arqv){

    freopen("resultados/result.txt", "a", stdout);

    for (info_atributos* it = attributes; it < attributes + count_atributos; it++) {
        short attribute_type = getAttributeType(it, cf, arqv);
        cout << endl;
        cout << "\tAttribute name indice: " << "cp_info #" << it->attribute_nome_do_indice << "  ";
        cout << cf->constant_pool[it->attribute_nome_do_indice-1].CONST.Utf8.bytes;
        
        cout << endl;
        cout << "\tAttribute tamanho: " << it->attribute_tamanho << endl;
        ushort indice;
        switch(attribute_type){
            case 0: // constant value
                indice = attributes->ATTR.valor_constante.constantvalue_indice;
                cout << "\tConstant Value indice: cp_info#" << indice << endl;
                cout << "\t";
                printCPInfo(cf, indice-1);
                break;
            case 1: // code
                mostraCode(cf, it, arqv);     
                break;
            case 2: // exceptions 
                mostraExceptions(cf, it);
                break;
            case 5: // source file
                cout << "\tSource file name indice: " << "cp_info #" << it->ATTR.SourceFile.sourcefile << "  ";
                cout << cf->constant_pool[it->ATTR.SourceFile.sourcefile-1].CONST.Utf8.bytes;
                cout << endl;
                break;
            case 6: // line number table
                mostraLineNumberTable(cf, it);
                break;
            case 7: // local variable table
                mostraLocalVariableTable(cf, it);
                break;
            default:
                break;
        }
        
    }
}

// print funcoes do atribute code
void printInstructions(ClassFile* cf, ushort indice){
    info_de_cp * cp = cf->constant_pool + indice;
    ulong show_long;
    switch (cp->tag){
        case 1:
            setUTF8((char*) cp->CONST.Utf8.bytes);
            break;
        case 3:
            cout << toFloat(cp->CONST.Float.bytes);
            break;
        case 4:
            cout << toFloat(cp->CONST.Float.bytes);
            break;
        case 5:
            show_long = ((unsigned long long)cp->CONST.Long.high_bytes << 32) | (unsigned long long)cp->CONST.Long.low_bytes;
            cout << toLong(show_long);
            break;
        case 6:
            cout << toDouble(cp);
            break;
        case 7:
            cout << (char *)cf->constant_pool[cp->CONST.Class.nome_do_indice -1].CONST.Utf8.bytes;
            break;
        case 8:
            cout << (char *)cf->constant_pool[cp->CONST.String.indice_string -1].CONST.Utf8.bytes;
            break;
        case 9:
            cout << (char *)cf->constant_pool[cf->constant_pool[cp->CONST.def_field.indice_class -1].CONST.Class.nome_do_indice -1].CONST.Utf8.bytes;
            cout << ".";
            cout << (char *)cf->constant_pool[cf->constant_pool[cp->CONST.def_field.inform_tiponome -1].CONST.NameAndType.nome_do_indice -1].CONST.Utf8.bytes;
            break;
        case 10:
            cout << (char *)cf->constant_pool[cf->constant_pool[cp->CONST.def_method.indice_class -1].CONST.Class.nome_do_indice -1].CONST.Utf8.bytes;
            cout << ".";
            cout << (char *)cf->constant_pool[cf->constant_pool[cp->CONST.def_method.inform_tiponome -1].CONST.NameAndType.nome_do_indice -1].CONST.Utf8.bytes;
            break;
        case 11:
            cout << (char *)cf->constant_pool[cf->constant_pool[cp->CONST.Interfacedef_method.indice_class -1].CONST.Class.nome_do_indice -1].CONST.Utf8.bytes;
            cout << ".";
            cout << (char *)cf->constant_pool[cf->constant_pool[cp->CONST.Interfacedef_method.inform_tiponome -1].CONST.NameAndType.nome_do_indice -1].CONST.Utf8.bytes;
            break;
        case 12:
            cout << (char *)cf->constant_pool[cp->CONST.NameAndType.nome_do_indice-1].CONST.Utf8.bytes;
            cout << ".";
            cout << (char *)cf->constant_pool[cp->CONST.NameAndType.descricao_de_tipo-1].CONST.Utf8.bytes;
            break;
        case 16:
            cout << (char *)cf->constant_pool[cp->CONST.MethodType.descricao_de_tipo-1].CONST.Utf8.bytes;
            break;
        case 18:
            cout << cp->CONST.InvokeDynamic.bootstrap_method_attr_indice-1 << ".";
            cout << cf->constant_pool[indice].CONST.InvokeDynamic.indice_tiponome;
            break;
        default:
            cout << "ERRO!!!" << endl;
            break;
    }   
}

// mostra bytecode
void mostraByteCode(info_atributos* attr, info_de_cp* constantPool, ClassFile* cf){
	uint code_tamanho = attr->ATTR.Code_attribute.code_tamanho;
	uchar *code = attr->ATTR.Code_attribute.code;
    
	uint i = 0;
	while(i < code_tamanho){
	    cout << "\t[" << i << "]  \t";
        cout << OPCODE[*(code+i)];

		if (code[i] <= 0x0f || (code[i] >= 0x1a && code[i] <= 0x35) || (code[i] >= 0x3b && code[i] <= 0x83)
				|| (code[i] >= 0x85 && code[i] <= 0x98) || (code[i] >= 0xac && code[i] <= 0xb1) || code[i] == 0xbe || code[i] == 0xbf
				|| code[i] == 0xc2 || code[i] == 0xc3) {
			cout << endl;
			i += 1;
		} else if (code[i] == 0x12) { // ldc
			printf("\t\t\t\t#%d ", (short)code[i + 1]);
            
            ushort number = ((short)0x00|(short)code[i+1]);
            cout.setf(ios::fixed);
            printInstructions(cf, number-1);
            cout.unsetf(ios::fixed);
            cout << endl;
            
            i += 2;
		} else if (code[i] == 0xbc) { // newarray
			uchar atype = code[i + 1];
			const char* types[] = { "boolean", "char", "float", "double", "byte", "short", "int", "long" };
			printf("\t\t%d (%s)\n", atype, types[atype - 4]);
			i += 2;
		} else if (code[i] == 0x10 || (code[i] >= 0x15 && code[i] <= 0x19) || (code[i] >= 0x36 && code[i] <= 0x3a) || code[i] == 0xa9) {
			printf(" \t\t%d\n", code[i + 1]);
			i += 2;
		} else if (code[i] == 0x11) { // sipush
			int16_t number = (code[i + 1] << 8) | code[i + 2];
			printf(" \t\t%d\n", number);
			i += 3;
		} else if (code[i] == 0x84) { // iinc
			printf("\t\t%d by %d\n", code[i + 1], code[i + 2]);
			i += 3;
		} else if (code[i] == 0x13 || code[i] == 0x14 || (code[i] >= 0xb2 && code[i] <= 0xb8) || code[i] == 0xbb || code[i] == 0xba || code[i] == 0xbd
				|| code[i] == 0xc0 || code[i] == 0xc1) { // usa CP
			ushort number = (code[i + 1] << 8) | code[i + 2];
			printf("\t\t#%d ", number);
            
            printInstructions(cf, int(number-1));
            cout << endl;
            
			i += 3;
		} else if ((code[i] >= 0x99 && code[i] <= 0xa8) || code[i] == 0xc6 || code[i] == 0xc7) {
			int number = (short)(code[i + 1] << 8) | (short)code[i + 2];
			printf("\t\t%d (%+d)\n", i + number, number);
			i += 3;
		} else if (code[i] == 0xc5) { // multianewarray
			ushort number = (code[i + 1] << 8) | code[i + 2];
			printf("\t\t\t#%d", number);

            printInstructions(cf, int(number-1));
    
            printf(" dim %d", code[i + 3]);
            cout << endl;
			i += 4;
		} else if (code[i] == 0xb9) { // invokeinterface
			ushort number = (code[i + 1] << 8) | code[i + 2];
			printf("\t\t\t#%d", number);
            
            printInstructions(cf, int(number-1));
            cout << endl;
            
			assert(code[i + 4] == 0);
			i += 5;
		} else if (code[i] == 0xc8 || code[i] == 0xc9) { // goto_w e jsr_w
			int number = (code[i + 1] << 24) | (code[i + 2] << 16) | (code[i + 3] << 8) | code[i + 4];
			printf("\t\t%f (%+f)\n", toFloat(i + number), toFloat(number));
            i += 5;
		}else if(code[i] == 0xc4){ // wide
			ushort indicebyte = (code[i + 2] << 8) | code[i + 3];

			printf("\n");
			printf("\t%d\t%s %d", i + 1, OPCODE[(*code+i + 1)], indicebyte);

			if (code[i + 1] == 0x84) {
				int constbyte = (code[i + 4] << 8) | code[i + 5];
				printf(" by %d", constbyte);
				i += 6;
			}else{
				i += 4;
			}

			printf("\n");
		} else if (code[i] == 0xaa){ // tableswitch
			uchar padding = (i + 1) % 4;
			int defaultbytes = (code[padding + i + 1] << 24) | (code[padding + i + 2] << 16) | (code[padding + i + 3] << 8)
					| code[padding + i + 4];
			int lowbytes = (code[padding + i + 5] << 24) | (code[padding + i + 6] << 16) | (code[padding + i + 7] << 8)
					| code[padding + i + 8];
			int highbytes = (code[padding + i + 9] << 24) | (code[padding + i + 10] << 16) | (code[padding + i + 11] << 8)
					| code[padding + i + 12];

			printf(" %d to %d\n", lowbytes, highbytes);

			uint howManyBytes = 1 + padding + 12;
			int offsets = highbytes - lowbytes + 1;

			for (uint n = 0; (signed) n < (signed) offsets; n++) {
				int offset = (code[i + howManyBytes] << 24) | (code[i + howManyBytes + 1] << 16) | (code[i + howManyBytes + 2] << 8)
						| code[i + howManyBytes + 3];
				printf("\t\t\t%d: %d (%+d)\n", lowbytes, i + offset, offset);

				lowbytes++; 
				howManyBytes += 4;
			}
			printf("\t\t\tdefault: %d (%+d)\n", i + defaultbytes, defaultbytes);

			i += howManyBytes;
        }else if (code[i] == 0xab){ // lookupswitch
			uchar padding = (i + 1) % 4;
			int defaultbytes = (code[padding + i + 1] << 24) | (code[padding + i + 2] << 16) | (code[padding + i + 3] << 8)
					| code[padding + i + 4];
			int npairs = (code[padding + i + 5] << 24) | (code[padding + i + 6] << 16) | (code[padding + i + 7] << 8)
					| code[padding + i + 8];

			uint howManyBytes = 1 + padding + 8; 

			printf(" %d\n", npairs);

			for (uint n = 0; (signed) n < (signed) npairs; n++) {
				int match = (code[i + howManyBytes] << 24) | (code[i + howManyBytes + 1] << 16) | (code[i + howManyBytes + 2] << 8)
						| code[i + howManyBytes + 3];
				int offset = (code[i + howManyBytes + 4] << 24) | (code[i + howManyBytes + 5] << 16) | (code[i + howManyBytes + 6] << 8)
						| code[i + howManyBytes + 7];
				printf("\t\t\t%d: %d (%+d)\n", match, i + offset, offset);
				howManyBytes += 8;
			}
			printf("\t\t\tdefault: %d (%+d)\n", i + defaultbytes, defaultbytes);

			i += howManyBytes;
        }else{
            cout << "Instruction not found!" << endl;
        }
    }
}
// print code
void mostraCode(ClassFile* cf, info_atributos* attr, ifstream& arqv){
    FILE *gerado;
    gerado =  fopen("resultados/result.txt", "a");
    fprintf(gerado,"\tAtributes info:\n\tMaximum stack size: %u\n", attr->ATTR.Code_attribute.max_stack);
    fprintf(gerado,"\tMaximum local variables: %u\n",attr->ATTR.Code_attribute.max_locals);
    fprintf(gerado,"\tCode tamanho: %u\n",attr->ATTR.Code_attribute.code_tamanho);
    
    fclose(gerado);

    mostraByteCode(attr, cf->constant_pool, cf);
    gerado =  fopen("resultados/result.txt", "a");
    fprintf(gerado,"\tException table tamanho: %u\n", attr->ATTR.Code_attribute.tamanho_tabela_excecoes);
    fclose(gerado);
    attributes(cf, attr->ATTR.Code_attribute.count_atributos, attr->ATTR.Code_attribute.attributes, arqv);
}

// print informations class e super class
void mostraClassName(ClassFile* cf){
    FILE *gerado;

    ushort indice = cf->this_class;
    ushort indice2 = cf->super_class;
    info_de_cp* class_cp = cf->constant_pool + indice - 1;
    info_de_cp* class_cp2 = cf->constant_pool + indice2 - 1;
    
    char* aux = (char*)cf->constant_pool[class_cp->CONST.Class.nome_do_indice - 1].CONST.Utf8.bytes;
    
    gerado =  fopen("resultados/result.txt", "a");
    fprintf(gerado,"This Class: cp_info#%d <", indice);
    fprintf(gerado,aux);
    fclose(gerado);
    

    char* aux2 = (char*)cf->constant_pool[class_cp2->CONST.Class.nome_do_indice - 1].CONST.Utf8.bytes;
    

    gerado =  fopen("resultados/result.txt", "a");
    fprintf(gerado,">\nSuper Class: cp_info#%d <",indice2);
    fprintf(gerado,aux2);
    fprintf(gerado,">\n");
    fclose(gerado);
}

void generalInformation(ushort access_flags){
    FILE *gerado;
    stringstream ss;

    gerado =  fopen("resultados/result.txt", "a");
    vector<ushort> pos = {0x0001, 0x0010, 0x0020, 0x0200, 0x0400, 0x1000, 0x2000, 0x4000,
                            0x0002, 0x0004, 0x0008, 0x0040, 0x0080};
   
    ss << hex << access_flags;
    string res = ss.str();
    int i = stoi(res);
    fprintf(gerado,"Access Flags: 0x%d", i);
    fprintf(gerado," [ ");

    for(ushort i=0; i<13; i++){
        if(access_flags & pos[i]){
            if(i==0){
                fprintf(gerado,"public");
            }
            else if(i==1){
                fprintf(gerado,"final");
            }
            else if(i==2){
                fprintf(gerado,"super");
            }
            else if(i==3){
                fprintf(gerado,"interface");
            }
            else if(i==4){
                fprintf(gerado,"abstract");
            }
            else if(i==5){
                fprintf(gerado,"synthetic");
            }
            else if(i==6){
                fprintf(gerado,"annotation");
            }
            else if(i==7){
                fprintf(gerado,"enum");
            }
            else if(i==8){
                fprintf(gerado,"private");
            }
            else if(i==9){
                fprintf(gerado,"protected");
            }
            else if(i==10){
                fprintf(gerado,"static");
            }
            else if(i==11){
                fprintf(gerado,"volatile");
            }
            else if(i==12){
                fprintf(gerado,"transient");
            }
        }
    }
    
    fprintf(gerado," ]\n");
    fclose(gerado);

}

void mostraMethodFlags(ushort access_flags){
    FILE *gerado;
    stringstream ss;
    gerado =  fopen("resultados/result.txt", "a");

    vector<ushort> pos ={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x0100,0x0400,0x0800,0x1000};

    
    ss << hex << access_flags;
    string res = ss.str();
    int i = stoi(res);
    fprintf(gerado,"Access Flags: 0x%d [ ", i);

    for(ushort i=0; i<12; i++){
        if(access_flags & pos[i]){
            if(i==0){
                fprintf(gerado,"public");
            }
            else if(i==1){
                fprintf(gerado,"private");
            }
            else if(i==2){
                fprintf(gerado,"protected");
            }
            else if(i==3){
                fprintf(gerado,"static");
            }
            else if(i==4){
                fprintf(gerado,"final");
            }
            else if(i==5){
                fprintf(gerado,"synchronized");
            }
            else if(i==6){
                fprintf(gerado,"bridge");
            }
            else if(i==7){
                fprintf(gerado,"varargs");
            }
            else if(i==8){
                fprintf(gerado,"native");
            }
            else if(i==9){
                fprintf(gerado,"abstract");
            }
            else if(i==10){
                fprintf(gerado,"strict");
            }
            else if(i==11){
                fprintf(gerado,"synthetic");
            }
           
        }
    }

    fprintf(gerado," ]\n");
    fclose(gerado);
}

void fields(ClassFile* cf, ifstream& arqv){
    ushort i = 0;
    info_field *aux_field;
    FILE *gerado;
    gerado =  fopen("resultados/result.txt", "a");

    if(cf->fields_count == 0){
        fprintf(gerado,"\tNo Fields\n");
    }else{
        fprintf(gerado,"\tMember Count: %u\n\n",cf->fields_count);
    }

    fclose(gerado);

    for(aux_field = cf->fields; aux_field < cf->fields + cf->fields_count; aux_field++){
        gerado =  fopen("resultados/result.txt", "a");
        fprintf(gerado,"\tField[%d]: \n",i);
        fprintf(gerado,"\tName: cp_info#%d ",aux_field->nome_do_indice);
        fprintf(gerado,(char *)cf->constant_pool[aux_field->nome_do_indice-1].CONST.Utf8.bytes);
        fprintf(gerado,"\n\tDescriptor: cp_info#%u ",aux_field->descricao_de_tipo);
        fprintf(gerado,(char *)cf->constant_pool[aux_field->descricao_de_tipo-1].CONST.Utf8.bytes);
        fprintf(gerado,"\n\t",i);
        fclose(gerado);

        generalInformation(aux_field->access_flags);
        
        if(aux_field->count_atributos > 0){
            attributes(cf, aux_field->count_atributos, aux_field->attributes, arqv);
        }

        i++;
        gerado =  fopen("resultados/result.txt", "a");
        fprintf(gerado,"\n");
        fclose(gerado);
    }
}
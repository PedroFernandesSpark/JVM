#include "Funcoes.h"
using namespace std;

int main() {
    string filename;
    cout << "Files:\n examples/Point.class\n examples/FooCorporation.class\n examples/double_arithmetic.class\n examples/long_cast.class\n examples/lookupswitch.class\n examples/method_test.class\n examples/multi_without_string.class\n examples/correct_sum.class" << endl;
    cout << "Enter the .class file name you want to display: " << endl;
    cin >> filename;
    ifstream file(filename, ios::binary); // Open the .class file
    if (!file) {
        cout << "File not found: " << filename << endl;
        return 0;
    }

    ClassFile *cf = (ClassFile *) malloc(sizeof(ClassFile));

    cf->magic = readFileMagic(file);
    if (cf->magic != 0xcafebabe) { // Check if it is a .class file
        cout << "Invalid .class file: " << filename << endl;
        delete cf;
        file.close();
        return 0;
    }

    cf->minor_version = readFile(file);
    cf->major_version = readFile(file);
    cf->constant_pool_count = readFile(file);
    readConstantPool(cf, file);
    cf->access_flags = readFile(file);
    cf->this_class = readFile(file);
    cf->super_class = readFile(file);
    cf->interfaces_count = readFile(file);
    readInterfaces(cf, file);
    cf->fields_count = readFile(file);
    readFields(cf, file);
    readMethods(cf, file);
    cf->attributes_count = readFile(file);
    cf->attributes = readAttributeArray(cf->attributes_count, cf, file);

    FILE *resultFile;
    resultFile = fopen("results/result.txt", "w");
    if (!resultFile) {
        cout << "Error creating result file." << endl;
        delete cf;
        file.close();
        return 0;
    }

    fprintf(resultFile, "---------------- General Information ----------------\n");
    fprintf(resultFile, "Magic = %#010x\n", cf->magic);
    fprintf(resultFile, "Minor Version = %u\n", cf->minor_version);
    fprintf(resultFile, "Major Version = %u\n", cf->major_version);
    float javaVersion = (1 + ((float)(cf->major_version - 44))/10);
    fprintf(resultFile, "Java Version = %.3f\n", javaVersion);
    fprintf(resultFile, "Constant Pool Count = %u\n", cf->constant_pool_count);
    fclose(resultFile);

    printGeneralInformation(cf->access_flags);
    printClassName(cf);

    resultFile = fopen("results/result.txt", "a");
    if (!resultFile) {
        cout << "Error opening result file." << endl;
        delete cf;
        file.close();
        return 0;
    }

    fprintf(resultFile, "Interfaces Count = %u\n", cf->interfaces_count);
    fprintf(resultFile, "Fields Count = %u\n", cf->fields_count);
    fprintf(resultFile, "Methods Count = %u\n", cf->methods_count);
    fprintf(resultFile, "Attributes Count = %u\n", cf->attributes_count);
    fprintf(resultFile, "\n---> CONSTANT POOL\n");
    fclose(resultFile);

    printConstantPool(cf);

    resultFile = fopen("results/result.txt", "a");
    if (!resultFile) {
        cout << "Error opening result file." << endl;
        delete cf;
        file.close();
        return 0;
    }

    fprintf(resultFile, "\n---> INTERFACES\n");
    fclose(resultFile);

    printInterfaces(cf);

    resultFile = fopen("results/result.txt", "a");
    if (!resultFile) {
        cout << "Error opening result file." << endl;
        delete cf;
        file.close();
        return 0;
    }

    fprintf(resultFile, "\n---> FIELDS\n");
    fclose(resultFile);

    printFields(cf, file);

    resultFile = fopen("results/result.txt", "a");
    if (!resultFile) {
        cout << "Error opening result file." << endl;
        delete cf;
        file.close();
        return 0;
    }

    fprintf(resultFile, "\n---> METHODS\n");
    fclose(resultFile);

    printMethods(cf, file);

    resultFile = fopen("results/result.txt", "a");
    if (!resultFile) {
        cout << "Error opening result file." << endl;
        delete cf;
        file.close();
        return 0;
    }

    fprintf(resultFile, "\n---> ATTRIBUTES\n");
    fclose(resultFile);

    printAttributes(cf, file);

    delete cf;
    file.close();

    return 0;
}
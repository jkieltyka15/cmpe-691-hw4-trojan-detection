#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {

    char* input_path = NULL;
    char* output_path = NULL;

    char buffer[BUFFER_SIZE];

    // check parameters
    if (3 != argc) {
        perror("expecting: ./parse-data <input file> <output file>");
    }

    input_path = argv[1];
    output_path = argv[2];

    // open input file
    FILE* input_file = fopen(input_path, "r");
    if (NULL == input_file) {
        perror("failed to open input file");
        return 1;
    }

    // open output file
    FILE* output_file = fopen(output_path, "w");
    if (NULL == input_file) {
        perror("failed to open output file");
        fclose(input_file);
        return 1;
    }

    // read input file until EOF
    while (!feof(input_file)) {

        // read line of input file
        if (NULL == fgets(buffer, sizeof(buffer), input_file)) {
            perror("failed to read input file line");
            break;
        }

        // parse line of input file

        // check if signal is in the top 30
    }

    fclose(input_file);
    fclose(output_file);
}
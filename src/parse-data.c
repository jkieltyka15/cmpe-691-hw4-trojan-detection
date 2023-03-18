#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include<stdlib.h>
#include <string.h>
#include <math.h>

#define NUM_OF_GATES 30

#define BUFFER_SIZE 1024
#define FIELD_SIZE 128

#define NUM_OF_FIELDS 5

#define INPUT_PIN 'I'
#define IGNORE_PIN "---"
#define ASTERISK_VAL 256

typedef struct {
    char name[FIELD_SIZE];
    uint32_t cc;
    uint32_t cc0;
    uint32_t cc1;
} gate_t;

int main(int argc, char* argv[]) {

    char buffer[BUFFER_SIZE];

    gate_t gate[NUM_OF_GATES];
    uint32_t gate_len = 0;

    char* input_path = NULL;
    char* output_path = NULL;
    FILE* input_file = NULL;
    FILE* output_file = NULL;

    // check parameters
    if (3 != argc) {
        perror("expecting: ./parse-data <input file> <output file>");
    }

    input_path = argv[1];
    output_path = argv[2];

    // open input file
    input_file = fopen(input_path, "r");
    if (NULL == input_file) {
        perror("failed to open input file");
        return 1;
    }

    // open output file
    output_file = fopen(output_path, "w");
    if (NULL == input_file) {
        perror("failed to open output file");
        fclose(input_file);
        return 1;
    }

    // read input file until EOF
    while (!feof(input_file)) {

        // used for parsing line from input file
        int32_t fields_set = 0;
        char pin_name[FIELD_SIZE];
        char pin_type;
        char testability[FIELD_SIZE];
        char s_testability[FIELD_SIZE];
        char gate_name[FIELD_SIZE];
        char cc0_str[4];
        char cc1_str[4];

        // used for determining ranking
        uint32_t cc = 0;
        uint32_t cc0 = 0;
        uint32_t cc1 = 0;

        // read line of input file
        if (NULL == fgets(buffer, sizeof(buffer), input_file) && !feof(input_file)) {
            perror("failed to read input file line");
            break;
        }

        // parse line of input file
        fields_set = sscanf(buffer, "%s %c %s %s %s", pin_name, &pin_type, testability, s_testability, gate_name);
        if (NUM_OF_FIELDS != fields_set) {
            continue;
        }

        // ignore pin
        if (0 == strcmp(pin_name, IGNORE_PIN)) {
            continue;
        }

        // not an input pin
        if (INPUT_PIN != pin_type) {
            continue;
        }

        // check for duplicate gate
        uint32_t duplicate = 0;
        for (int i = 0; gate_len > i; i++) {
            if (0 == strncmp(gate_name, gate[i].name, strlen(gate_name))) {
                duplicate = 1;
                break;
            }
        }
        if (duplicate) {
            continue;
        }

        // check that beginning bracket is present for testability
        if ('(' != testability[0]) {
            continue;
        }

        // clear CC0 and CC1
        for (int i = 0; i < sizeof(cc0_str); i++) {
            cc0_str[i] = 0;
            cc1_str[i] = 0;
        }

        // parse CC0
        for(int i = 1; strlen(testability) > i; i++) {
            
            if ('*' == testability[i]) {
                cc0 = ASTERISK_VAL;
                i++;
            } 
            
            if ('-' == testability[i]) {

                // parse CC1
                for (int j = i + 1; strlen(testability) > j; j++) {
                    if ('*' == testability[j]) {
                        cc1 = ASTERISK_VAL;
                        break;
                    } else if ('-' == testability[j]) {
                        break;
                    }
                    cc1_str[j - i - 1] = testability[j];
                }
                break;
            }
            cc0_str[i - 1] = testability[i];
        }
        cc0 = ASTERISK_VAL == cc0 ? cc0 : (uint32_t)atoi(cc0_str) + 1;
        cc1 = ASTERISK_VAL == cc1 ? cc1 : (uint32_t)atoi(cc1_str) + 1;
        if (cc0 > ASTERISK_VAL || cc1 > ASTERISK_VAL) {
            continue;
        }

        // calculate CC
        cc = (uint32_t)sqrt(pow((double)cc0, 2) + pow((double)cc1, 2));

        // check if signal is in the top 30
        int rank = -1;
        for (int i = 0; gate_len > i; i++) {
            if (cc > gate[i].cc) {
                rank = i;
                break;
            }
        }

        // not in the top 30 or last
        if (-1 == rank) {
            if (NUM_OF_GATES > gate_len) {
                strcpy(gate[gate_len].name, gate_name);
                gate[gate_len].cc0 = cc0;
                gate[gate_len].cc1 = cc1;
                gate[gate_len].cc = cc;
                rank = gate_len;
                gate_len++;
            }
            continue;
        }

        // shift gates for new ranking
        for (int i = gate_len - 1; i > rank; i--) {
            strcpy(gate[i].name, gate[i - 1].name);
            gate[i].cc0 = gate[i - 1].cc0;
            gate[i].cc1 = gate[i - 1].cc1;
            gate[i].cc = gate[i - 1].cc;
        }
        strcpy(gate[rank].name, gate_name);
        gate[rank].cc0 = cc0;
        gate[rank].cc1 = cc1;
        gate[rank].cc = cc;
    }

    // print the top 30 gates to the output file
    for (int i = 0; i < gate_len; i++) {
        fprintf(output_file, "rank %d: gate=\'%s\' CC0=%d CC1=%d CC=%d\n", i + 1, gate[i].name, gate[i].cc0, gate[i].cc1, gate[i].cc);
    }

    // cleanup
    fclose(input_file);
    fclose(output_file);
}
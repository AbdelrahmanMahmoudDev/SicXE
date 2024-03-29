#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <Windows.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define f32 float
#define f64 double

#define b32 uint32_t

struct Symbol_Table_Entry
{
    char location[30];
    char label[30];
};

static void CreateStringOfSize(u32 count, char **result)
{
    *result = (char *)malloc((count) * sizeof(char));
}

static void CreateStringArrayOfSize(u32 rows, u32 columns, char **result)
{
    *result = (char *)malloc((rows * columns) * sizeof(char));
}
    // Currently, heap allocations occur without deletion
    // TODO: Create memory arenas for different allocations
    // TODO: Dynamically allocate based on file size
int main(int argc, char **argv)
{
    char exe_path[260];
    GetModuleFileNameA(NULL, exe_path, sizeof(exe_path));

    // TODO: Get data path dynamically 
    char data_path[260];
    strcpy_s(data_path, sizeof(data_path), exe_path);
    strcat_s(data_path, "\\..\\data\\");
    strcat_s(data_path, "Example.txt");
    data_path[259] = '\0';

    FILE *text_file = fopen("Example.txt", "rb");
    
    if(text_file == nullptr)
    {
        // log file not found
        return 0;
    }

    // character count
    fseek (text_file , 0 , SEEK_END);
    u64 file_size = ftell(text_file);
    fseek(text_file, 0, SEEK_SET);

    /* // line count
    u32 line_count = 0;
    for(u32 c = getc(text_file); c != EOF; c = getc(text_file))
    {
        if (c == '\n')
        {
            ++line_count;
        }
    } */

    // char *source_statement = (char *)malloc(file_size * sizeof(char));
    char source_statement[500];

    size_t check = fread(source_statement, 1, file_size, text_file);
    if(check == (u32)file_size)
    {
        printf("File successfully read\n");
    }
    else
    {
        if(feof(text_file))
        {
            printf("Unexpected end of file\n");
        }
        else if(ferror(text_file))
        {
            printf("Error reading file\n");
        }
    }

    source_statement[file_size] = '\0';


    char modifiable_statement[500];

    char *format_one_instructions[6];
    format_one_instructions[0] = "FIX";
    format_one_instructions[1] = "FLOAT";
    format_one_instructions[2] = "HIO";
    format_one_instructions[3] = "NORM";
    format_one_instructions[4] = "SIO";
    format_one_instructions[5] = "TIO";

    char *format_two_instructions[11];
    format_two_instructions[0] = "ADDR";
    format_two_instructions[1] = "CLEAR";
    format_two_instructions[2] = "COMPR";
    format_two_instructions[3] = "DIVR";
    format_two_instructions[4] = "MULR";
    format_two_instructions[5] = "RMO";
    format_two_instructions[6] = "SHIFTL";
    format_two_instructions[7] = "SHIFTR";
    format_two_instructions[8] = "SUBR";
    format_two_instructions[9] = "SVC";
    format_two_instructions[10] = "TIXR";

    char label[50];

    char instruction[50];

    char operand[50];

    strcpy(modifiable_statement, source_statement);
    char *next_line;
    next_line = strtok(modifiable_statement, "\n");
    u32 program_start = 0;

    Symbol_Table_Entry symbol_entries[500];
    u32 entry_collection_index = 0;

    char instruction_cache[500][50];
    u32 index_into_instruction_cache = 0;

    char operand_cache[500][50];
    u32 index_into_operand_cache = 0;

   while(next_line != 0)
   {
    u32 count = 1;
    for (u32 i = 0; next_line[i] != '\0'; i++)
    {
        if (next_line[i] == ' ' && next_line[i+1] != ' ')
            count++;    
    }

    if(count == 3)
    {
        if(strcmp(instruction, "START") == 0)
        {
            program_start = atoi(operand);
        }
            sscanf(next_line, "%s %s %s", label, instruction, operand);
            strcpy(symbol_entries[entry_collection_index].label, label);
            entry_collection_index++;

    }
    else
    {
        sscanf(next_line, "%s %s", instruction, operand);
        strcpy(symbol_entries[entry_collection_index].label, "null");
        entry_collection_index++;
    } 

    strcpy(instruction_cache[index_into_instruction_cache], instruction);
    index_into_instruction_cache++;

    strcpy(operand_cache[index_into_operand_cache], operand);
    index_into_operand_cache++;

    next_line = strtok(0, "\n");  
   }

    u32 location_counter[500] = {};
    location_counter[0] = program_start;
    u32 location_counter_index = 1;

    char format_four_token = '+';
    char format_five_token = '&';
    char format_six_token = '$';
    b32 is_format_one = false;
    b32 is_format_two = false;
    for(u32 instruction_index = 0; instruction_index < 500; ++instruction_index)
    {
        // directive checking
        // RESW. RESB. WORD. BYTE
        if(strcmp(instruction_cache[instruction_index], "RESB") == 0)
        {
            location_counter[location_counter_index] = location_counter[location_counter_index - 1] + atoi(operand_cache[instruction_index]);
            location_counter_index++;
        }
        else if(strcmp(instruction_cache[instruction_index], "RESW") == 0)
        {
            location_counter[location_counter_index] = location_counter[location_counter_index - 1] + (atoi(operand_cache[instruction_index]) * 3);
            location_counter_index++;
        }
        else if(strcmp(instruction_cache[instruction_index], "WORD") == 0)
        {
            location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 3;
            location_counter_index++;
        }
        else if(strcmp(instruction_cache[instruction_index], "START") == 0)
        {
            location_counter[location_counter_index + 1] = location_counter[location_counter_index];
            location_counter_index++;
        }
        else if(strcmp(instruction_cache[instruction_index], "BYTE") == 0)
        {
            char comparable[30];
            strcpy_s(comparable, sizeof(comparable), operand_cache[instruction_index]);
            if(comparable[0] == 'X')
            {
                // hexadecimal
                // X'05'
                u32 count = (u32)strlen(operand_cache[instruction_index]);
                count -= 3;
                f64 size_of_increment = ceil((f64)(count / 2));

                location_counter[location_counter_index] = location_counter[location_counter_index - 1] + (u32)(size_of_increment);
                location_counter_index++;
            }
            else
            {
                // ASCII
                // C'EOF'
                u32 count = (u32)strlen(operand_cache[instruction_index]);
                count -= 3;

                location_counter[location_counter_index] = location_counter[location_counter_index - 1] + count;
                location_counter_index++;
            }
        }
        else
        {
            // format checking
            for(u32 format_one_index = 0; format_one_index < 6; ++format_one_index)
            {
                if(strcmp(instruction_cache[instruction_index], format_one_instructions[format_one_index]) == 0)
                {
                    is_format_one = true;
                    //instruction is format one (1 byte)
                    location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 1;
                    location_counter_index++;
                }
            }

            for(u32 format_two_index = 0; format_two_index < 11; ++format_two_index)
            {
                if(strcmp(instruction_cache[instruction_index], format_two_instructions[format_two_index]) == 0)
                {
                    is_format_two = true;
                    //instruction is format 2 (2 bytes)
                    location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 2;
                    location_counter_index++;
                }
            }

            if(!is_format_one && !is_format_two)
            {
                char comparable[30];
                strcpy_s(comparable,
                         sizeof(comparable), 
                         instruction_cache[instruction_index]);

                if(strcmp(instruction_cache[instruction_index], "BASE") == 0)
                {
                        location_counter[location_counter_index] = location_counter[location_counter_index - 1];
                        location_counter_index++;
                }
                else
                {
                    if(comparable[0] == format_four_token)
                    {
                        // format 4 (4 bytes)
                        location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 4;
                        location_counter_index++;
                    }
                    else if(comparable[0] == format_five_token)
                    {
                        // format 5 (3 bytes)
                        location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 3;
                        location_counter_index++;
                    }
                    else if(comparable[0] == format_six_token)
                    {
                        // format 6 (4 bytes)
                        location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 4;
                        location_counter_index++;
                    }
                    else
                    {
                        // format 3 (3 bytes)
                        location_counter[location_counter_index] = location_counter[location_counter_index - 1] + 3;
                        location_counter_index++;
                    }
                }
            }
        }
    }

    char hexadecimal_location_counter[500][50] = {};
    // Time to get location counter values in hexadecimal
    char buffer[512];
    for(u32 location_index = 0; location_index < 500; ++location_index)
    {
		sprintf_s(buffer, sizeof(buffer), "%x\n", location_counter[location_index]);
        strcpy(hexadecimal_location_counter[location_index], buffer);
        strcpy(buffer, " ");
    }

    for(u32 index = 0; index < 500; ++index)
    {
        strcpy(symbol_entries[index].location, hexadecimal_location_counter[index]);
    }

    FILE *output_file = fopen("out.txt", "wb");

    if(output_file == NULL)
    {
        // Something is wrong
    }

    for (u32 location_index = 0; location_index < 500; ++location_index)
    {
         fprintf(output_file, "%s\n", hexadecimal_location_counter[location_index]);
    } 

    output_file = fopen("symbTable.txt", "wb");
    if (output_file == NULL)
    {
        // something is wrong
    }

    for (u32 symbol_index = 0; symbol_index < 500; ++symbol_index)
    {
        if(strcmp(symbol_entries[symbol_index].label, "null") != 0)
        {
            fprintf(output_file, "%s\t%s\n", symbol_entries[symbol_index].label, symbol_entries[symbol_index].location);
        }
    }

    printf("Symbol\tLocation\n");
    for(u32 term_index = 1; term_index < 500; ++term_index)
    {
        printf("%s\t%s", symbol_entries[term_index].label, symbol_entries[term_index].location);
    }

    fclose(output_file);

    fclose(text_file);
    system("PAUSE");
    return(0);
}
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// //import functions from pa1.c
// int32_t is_ascii(char str[]);
// int32_t capitalize_ascii(char str[]);
// int32_t width_from_start_byte(char start_byte);
// int32_t utf8_strlen(char str[]);
// int32_t codepoint_index_to_byte_index(char str[], int32_t cpi);
// void utf8_substring(char str[], int32_t cpi_start, int32_t cpi_end, char result[]);
// int32_t codepoint_at(char str[], int32_t cpi);
// char is_animal_emoji_at(char str[], int32_t cpi);

//returns 0 if not ascii and 1 if is ascii
int32_t is_ascii(char str[]) {
    int32_t ascii = 1;

    //ascii values between 0 and 127, so out of range will return false
    for(int i = 0; str[i] != 0; i+= 1) {
        if(str[i] > 127 || str[i] < 0)
            ascii = 0;
    }
    
    return ascii;
}

//capitalizes any lowercase ascii in string
int32_t capitalize_ascii(char str[]) {
    int32_t updated = 0; //counts how many chars updated to uppercase

    int index = 0;
    while(str[index] != 0 && str[index] != '\n') {
        
        //if in lowercase ascii range, add 32 to make upper
        //increment updated to keep track of how many chars changed
        if(str[index] >= 97 && str[index] <= 122) {
            str[index] = str[index] - 32;
            updated += 1;
        }
        index += 1;
    }
    return updated;
}

//returns how many bytes used in char based on start byte
int32_t width_from_start_byte(char start_byte) {
    int32_t width = 0;
    //firstFour masks all bytes except firstFour
    int32_t firstFour = (start_byte & 0b11110000);

    //ascii values start with 01xxxxxx
    if(firstFour <= 0b01110000 && firstFour >= 0b00000000) 
        width = 1;
    else if (firstFour == 0b11000000) //2 byte start with 110xxxxx
        width = 2;
    else if (firstFour == 0b11100000) //3 byte start with 1110xxxx
        width = 3;
    else if (firstFour == 0b11110000) //4 byte start with 1111xxxx
        width = 4;
    else    // continuation bytes start with 10xxxxxx
        width = -1;

    return width;
}

//increments over string and adds up length of codepoints
int32_t utf8_strlen(char str[]) {
    int32_t length = 0;
    int32_t index = 0;
    int32_t byte = 0;

    while(str[index] != 0) {
        //calc how many continuation bytes follow starting byte
        byte = width_from_start_byte(str[index]); 
        if (byte <= 4 && byte >= 1) {   
            length += 1; //increment length by one when reach new char in string
            index += byte; //add # of bytes to index to get to next start byte
        }
        else {
            length = -1; //in case of error, return -1 
            return length;
        }
    }
    return length;
}

//return starting byte if mult bytes
//returns byte index based on given code point index
int32_t codepoint_index_to_byte_index(char str[], int32_t cpi) {
    int32_t index = 0;
    int32_t bi = 0;
    
    //if cpi > length, return error
    if(cpi > utf8_strlen(str))
        return -1;

    //moves to next start byte 
    while(index < cpi) {
        index += 1;
        bi += width_from_start_byte(str[bi]);
    }
    return bi;
}

//converts cpi start/end to bi and traverses over translated bi to copy over bytes
void utf8_substring(char str[], int32_t cpi_start, int32_t cpi_end, char result[]) {
    int32_t length = utf8_strlen(str);

    //in the case that string not long enough for ending cpi
    //set ending cpi to last code point in string
    if (length < cpi_end)
        cpi_end = length;

    if(cpi_start > cpi_end || cpi_start < 0 || cpi_end < 0)
        return;

    int32_t bi = 0;
    int32_t bi_start = bi_start = codepoint_index_to_byte_index(str, cpi_start);
    int32_t bi_end = bi_end = codepoint_index_to_byte_index(str, cpi_end);;

    int result_index = 0;
    
    for(bi = bi_start; bi < bi_end; bi++) {
        result[result_index++] = str[bi];
    }
    
    //prevents running on into memory 
    result[result_index] = 0;
}

//returns decimal value of code point index
int32_t codepoint_at(char str[], int32_t cpi) {
    int32_t decimal = 0;
    int32_t bi = 0;
    int32_t bi2 = 0;
    int32_t bi3 = 0;
    int32_t bi4 = 0;
    bi = codepoint_index_to_byte_index(str, cpi);

    //1 byte
    if(width_from_start_byte(str[bi]) == 1)
        decimal = str[bi];
    else if(width_from_start_byte(str[bi]) == 2) { //2 bytes
        bi2 = bi + 1;
        decimal = ((str[bi] & 0b00011111) * 64 + (str[bi2] & 0b00111111));
    }
    else if(width_from_start_byte(str[bi]) == 3) { //3 bytes
        bi2 = bi + 1;
        bi3 = bi + 2;
        decimal = ((str[bi] & 0b00001111) * 4096 + (str[bi2] & 0b00111111) * 64 + (str[bi3] & 0b00111111));
    }
    else if(width_from_start_byte(str[bi]) == 4) { //4 bytes
        bi2 = bi + 1;
        bi3 = bi + 2;
        bi4 = bi + 3;
        decimal = ((str[bi] & 0b00001111) * 262144 + (str[bi2] & 0b00111111) * 4096 + (str[bi3] & 0b00111111) * 64 + (str[bi4] & 0b00111111));
    }

    return decimal;
}

//returns 1 if emoji at codepoint index is animal emoji
//returns 0 if not
char is_animal_emoji_at(char str[], int32_t cpi) {
    char boolean = 0;
    if((codepoint_at(str, cpi) >= 128000 && codepoint_at(str, cpi) <= 128063) || (codepoint_at(str, cpi) >= 129408 && codepoint_at(str, cpi) <= 129454))
        boolean = 1;
    return boolean;
}

//takes char and outputs next char in utf8 encoding
void next_utf8_char(char str[], int32_t cpi, char result[]) {
    int codepoint = codepoint_at(str, cpi);
    codepoint++;
     
    if (codepoint <= 0x7F) { // 1 byte
        result[0] = (char)codepoint;
        result[1] = '\0';
    } else if (codepoint <= 0x7FF) { // 2 bytes
        result[0] = 0xC0 | (codepoint >> 6);
        result[1] = 0x80 | (codepoint & 0x3F);
        result[2] = '\0';
    } else if (codepoint <= 0xFFFF) { // 3 bytes
        result[0] = 0xE0 | (codepoint >> 12);
        result[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        result[2] = 0x80 | (codepoint & 0x3F);
    } else if (codepoint <= 0x10FFFF) { // 4 bytes
        result[0] = 0xF0 | (codepoint >> 18);
        result[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        result[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        result[3] = 0x80 | (codepoint & 0x3F);
        result[4] = '\0';
    }
    
}

int main() {
    //declare arrays and ints
    char buffer[100];  
    char input[100];
    char result[100];
    char animal_emojis[100];

    int32_t index = 0;
    int32_t byte_length = 0;
    
    //prompt for input
    printf("%s", "Enter a UTF-8 encoded string: ");
    fgets(buffer, 50, stdin);
    //printf("%s \n", buffer);
    
    //debugging
    // for (int i = 0; input[i] != '\0'; i++) {
    //     printf("Byte %d: %02x\n", i, (unsigned char)buffer[i]);
    // }

    //copy user input to another array since capitalize_ascii
    //will change the array passed in
    
    strcpy(input, buffer);

    for(int i = 0; i < 1000; i++) {
        if(buffer[i] >= 250) {
            buffer[i] = 0;
        }
    }
    
    printf("\nValid ASCII: %s\n", is_ascii(buffer) ? "true" : "false");
    capitalize_ascii(buffer);
    printf("Uppercased ASCII: \"%s\"\n", buffer);

    while(input[index] != 0) {
        byte_length += width_from_start_byte(input[index]);
        index += width_from_start_byte(input[index]);
    }

    printf("Length in bytes: %d\n", (byte_length));
    printf("Number of code points: %d\n", (utf8_strlen(input)));
    printf("Bytes per code point: ");

    //get bi from cpi
    //go to bi in array
    //print byte at that codepoint
    for(int i = 0; i < utf8_strlen(input); i ++){
        printf("%d ", width_from_start_byte(input[codepoint_index_to_byte_index(input, i)]));
    }

    utf8_substring(input, 0, 6, result);
    printf("\nSubstring of the first 6 code points: \"%s\"\n", result);
    printf("Code points as decimal numbers: ");
    for(int i = 0; i < utf8_strlen(input); i++){
        printf("%d ", codepoint_at(input, i));
    }

    printf("\nAnimal emojis: ");
    //if animal emoji, print
    //if not, keep traversing through array
    for(int i = 0; i < utf8_strlen(input); i++){
        if(is_animal_emoji_at(input, i) == 1) {
            utf8_substring(input, i, i + 1, animal_emojis);
            printf("%s ", animal_emojis);
        } 
    }
    printf("\n"); 
    char next_char[50];
    int32_t idx = 3;
    next_utf8_char(input, idx, next_char);
    printf("Next Character of Codepoint at Index 3: %s", next_char);
    printf("\n"); 
    return 0;
}

/* 
input: 🐩🐶🐕🙊, 😸,🦀 to 🦮🐀 to 🐿️

output: Valid ASCII: false
Uppercased ASCII: "🐩🐶🐕🙊, 😸,🦀 TO 🦮🐀 TO 🐿�"
Length in bytes: 50
Number of code points: 21
Bytes per code point: 4 4 4 4 1 1 4 1 4 1 1 1 1 4 4 1 1 1 1 4 3 
Substring of the first 6 code points: "🐩🐶🐕🙊, "
Code points as decimal numbers: 128041 128054 128021 128586 44 32 128568 44 129408 32 116 111 32 129454 128000 32 116 111 32 128063 65024 
Animal emojis: 🐩 🐶 🐕 🦀 🦮 🐀 🐿 
Next Character of Codepoint at Index 3: 🙋
*/
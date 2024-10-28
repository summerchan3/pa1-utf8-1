#include <stdio.h>
#include <stdint.h>
#include <string.h>

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
    while(str[index] != 0) {
        
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
    int32_t bi_start = 0;
    int32_t bi_end = 0;

    //subtract one from cpi_end since it is exclusive not inclusive
    bi_start = codepoint_index_to_byte_index(str, cpi_start);
    bi_end = codepoint_index_to_byte_index(str, cpi_end - 1) + width_from_start_byte(cpi_end - 1);
    while (bi < bi_end) { 
        result[bi] = str[bi_start];
        bi += 1;
        bi_start += 1;
    }

    //prevents running on into memory 
    result[bi] = 0;
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

/*
 int main() {
    printf("Is 🔥 ASCII? %d\n", is_ascii("🔥"));
    printf("Is abcd ASCII? %d\n", is_ascii("abcd"));

    int32_t ret = 0;
    char str[] = "abcd";
    ret = capitalize_ascii(str);
    printf("Capitalized String: %s\nCharacters updated: %d\n", str, ret);

    char s[] = "Héy"; // same as { 'H', 0xC3, 0xA9, 'y', 0 },   é is start byte + 1 cont. byte
    printf("Width: %d bytes\n", width_from_start_byte(s[1])); // start byte 0xC3 indicates 2-byte sequence
    printf("Width: %d bytes\n", width_from_start_byte(s[2]));

    char str2[] = "Joséph";

    printf("Length of string %s is %d\n", str2, utf8_strlen(str2));
    int32_t idx = 4;
    printf("Codepoint index %d is byte index %d\n", idx, codepoint_index_to_byte_index("Joséph", idx));

    char result[17];
    utf8_substring("🦀🦮🦮🦀🦀🦮🦮", 3, 7, result);
    printf("String: %s\nSubstring: %s\n", "🦀🦮🦮🦀🦀🦮🦮", result);    

    char str4[] = "Joséph";
    int32_t idx2 = 3;
    printf("Codepoint at %d in %s is animal? %d\n", idx2, str4, codepoint_at(str4, idx2));

    char strE[] = "🦀🐀🦮🐿️";
    int32_t idx3 = 2;
    printf("Codepoint at %d in %s is %d\n", idx3, strE, codepoint_at(strE, idx3));
    printf("Codepoint at %d in %s is %d\n", idx3, strE, is_animal_emoji_at(strE, idx3));
    printf("Codepoint at %d in %s is %d\n", idx2, str4, is_animal_emoji_at(str4, idx2));
   
    return 0;
} */
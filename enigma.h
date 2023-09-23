#ifndef ENIGMA_H_
#define ENIGMA_H_

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// --------------------------------------------------------------
// DATA STRUCTURES

#define ALPHABET_SIZE 26
#define PLUGBOARD_SIZE 10

typedef uint8_t Wiring[ALPHABET_SIZE];

typedef enum {
  RO_FORWARD,
  RO_BACKWARD,  
} RotorOrder;

// NOTE: for now we only handle single ring_setting values
typedef struct {
  Wiring rotor_wiring;
  uint8_t offset;
  uint8_t ring_position;
} Rotor;

typedef struct {
  uint8_t (*board)[2];
  size_t board_size;
} Plugboard;

// To properly configure an Enigma machine you need four different settings:
//
// - Rotor order
// - Ring setting for each rotor
// - Starting position for each rotor
// - Plugboard connections
// - Type of reflector
// 
typedef struct {
  Plugboard plugboard;
  Rotor rotors[3];
  Wiring reflector;
} Enigma;

// Macros
#define CHAR2CODE(ch) ((uint8_t) ((ch) - 'A'))
#define CODE2CHAR(code) ((char) ('A' + (code)))

// Function signatures
Enigma *init_enigma(const char *rotor_names[3], const char *Reflector, Plugboard plugboard);
void init_wiring(Wiring wiring, const char *alphabet, size_t alphabet_len);
void init_rotor(Rotor *r, const char *rotor_name);
void init_reflector(Wiring wiring, const char *reflector_name);
void destroy_enigma(Enigma *e);

uint8_t apply_rotor(Rotor *r, const uint8_t plaintext_code);
uint8_t apply_rotors(Enigma *e, const uint8_t plaintext_code, RotorOrder order);
uint8_t apply_plugboard(Enigma *e, const uint8_t plaintext_code);
uint8_t apply_reflector(Enigma *e, const uint8_t plaintext_code);
void apply_enigma(Enigma *e, const uint8_t *input, size_t input_len, uint8_t *output);

void enigma_encrypt(Enigma *e, const uint8_t *plaintext, size_t plaintext_len, uint8_t *ciphertext);
void enigma_decrypt(Enigma *e, const uint8_t *ciphertext, size_t ciphertext_len, uint8_t *plaintext);

#endif // ENIGMA_H_

#ifdef ENIGMA_IMPLEMENTATION

// --------------------------------------------------------------
// ENIGMA MODELS

void init_wiring(Wiring wiring, const char *alphabet, size_t alphabet_len) {
  for (size_t i = 0; i < alphabet_len; i++) {
    wiring[i] = (uint8_t) alphabet[i] - 'A';
  }
}

// https://www.cryptomuseum.com/crypto/enigma/wiring.htm
void init_rotor(Rotor *r, const char *rotor_name) {
  r->offset = 0;
  
  if (strcmp(rotor_name, "I") == 0 ) {
    r->ring_position = (uint8_t) 'R' - 'A';
    init_wiring(r->rotor_wiring, "EKMFLGDQVZNTOWYHXUSPAIBRCJ", ALPHABET_SIZE);    
  } else if (strcmp(rotor_name, "II") == 0 ) {
    r->ring_position = (uint8_t) 'F' - 'A';
    init_wiring(r->rotor_wiring, "AJDKSIRUXBLHWTMCQGZNPYFVOE", ALPHABET_SIZE);        
  } else if (strcmp(rotor_name, "III") == 0 ) {
    r->ring_position = (uint8_t) 'W' - 'A';
    init_wiring(r->rotor_wiring, "BDFHJLCPRTXVZNYEIWGAKMUSQO", ALPHABET_SIZE);
  } else if (strcmp(rotor_name, "IV") == 0 ) {
    r->ring_position = (uint8_t) 'K' - 'A';
    init_wiring(r->rotor_wiring, "ESOVPZJAYQUIRHXLNFTGKDCMWB", ALPHABET_SIZE);        
  } else if (strcmp(rotor_name, "V") == 0 ) {
    r->ring_position = (uint8_t) 'A' - 'A';
    init_wiring(r->rotor_wiring, "VZBRGITYUPSDNHLXAWMJQOFECK", ALPHABET_SIZE);        
  }
}

void init_reflector(Wiring wiring, const char *reflector_name) {
  if (strncmp(reflector_name, "A", 1) == 0 ) {
    init_wiring(wiring, "EJMZALYXVBWFCRQUONTSPIKHGD", ALPHABET_SIZE);
  } else if (strncmp(reflector_name, "B", 1) == 0 ) {
    init_wiring(wiring, "YRUHQSLDPXNGOKMIEBFZCWVJAT", ALPHABET_SIZE);
  } else if (strncmp(reflector_name, "C", 1) == 0 ) {
    init_wiring(wiring, "FVPJIAOYEDRZXWGCTKUQSBNMHL", ALPHABET_SIZE);
  }
}

Enigma *init_enigma(const char *rotor_names[3], const char *reflector_name, Plugboard plugboard) {
  Enigma *e = malloc(sizeof(Enigma));
  
  init_rotor(&e->rotors[0], rotor_names[0]);
  init_rotor(&e->rotors[1], rotor_names[1]);
  init_rotor(&e->rotors[2], rotor_names[2]);

  init_reflector(e->reflector, reflector_name);

  if (plugboard.board_size > PLUGBOARD_SIZE) {
    printf("[ERROR]: init_enigma() - supplied plugboard size (%ld) greater than maxium (%d)\n", plugboard.board_size, PLUGBOARD_SIZE);
    exit(0);
  }
  e->plugboard = plugboard;

  return e;
}

void destroy_enigma(Enigma *e) {
  if (e) {
    free(e);
  }
}

// --------------------------------------------------------------
// CORE LOGIC

uint8_t apply_rotor(Rotor *r, const uint8_t plaintext_code) {
  uint8_t char_code = plaintext_code;

  // TODO: make sure this computation makes sense with respect to the
  // actual functioning of the rotor. I need to review the exact
  // mechanical movement of the rotor.
  char_code = r->rotor_wiring[char_code + r->offset];

  return char_code;
}

uint8_t apply_rotors(Enigma *e, const uint8_t plaintext_code, RotorOrder order) {
  // TODO: implement correctly the double-stepping mechanism
  // 
  // https://en.wikipedia.org/wiki/Enigma_machine
  
  uint8_t char_code = plaintext_code;
  
  switch(order) {

  case RO_FORWARD: {
    // First, use the rotors to obtain the final char_code
    char_code = apply_rotor(&e->rotors[0], char_code);
    char_code = apply_rotor(&e->rotors[1], char_code);
    char_code = apply_rotor(&e->rotors[2], char_code);

    // Then, perform the movement on each rotor
    //
    // Double stepping caused by the claw mechanism used for rotating
    // the rotors makes the second rotor move twice in a row, if the
    // first movement brings it in the turnover position during the
    // first rotation.
    //
    //  https://www.youtube.com/watch?v=5StZlF-clPc
    // 
    if (e->rotors[1].offset == e->rotors[1].ring_position) {
      e->rotors[2].offset = (e->rotors[2].offset + 1) % ALPHABET_SIZE;
      e->rotors[1].offset = (e->rotors[1].offset + 1) % ALPHABET_SIZE;      
    } else if (e->rotors[0].offset == e->rotors[0].ring_position) {
      e->rotors[1].offset = (e->rotors[1].offset + 1) % ALPHABET_SIZE;
    }
    
    e->rotors[0].offset = (e->rotors[0].offset + 1) % ALPHABET_SIZE;
    
  } break;

  case RO_BACKWARD: {
    char_code = apply_rotor(&e->rotors[2], char_code);
    char_code = apply_rotor(&e->rotors[1], char_code);
    char_code = apply_rotor(&e->rotors[0], char_code);
    
    // no need to move rotors here
    
  } break;

  default: assert(0 && "apply_rotors(): Unreachable");
  }
  
  return char_code;
}

uint8_t apply_plugboard(Enigma *e, const uint8_t plaintext_code) {
  for (size_t i = 0; i < e->plugboard.board_size; i++) {
    if (plaintext_code == e->plugboard.board[i][0]) {
      return e->plugboard.board[i][1];
    } else if (plaintext_code == e->plugboard.board[i][1]) {
      return e->plugboard.board[i][0];
    }
  }
  return plaintext_code;
}

uint8_t apply_reflector(Enigma *e, const uint8_t plaintext_code) {
  return e->reflector[plaintext_code];
}

void apply_enigma(Enigma *e, const uint8_t *input, size_t input_len, uint8_t *output) {
  // Assumes output has been already allocated with a null-terminating
  // string and that len(output) == input_len.

  for (size_t i = 0; i < input_len; i++) {
    uint8_t input_char = input[i];

    // Transform character into char_code
    uint8_t char_code = CHAR2CODE(input_char);
    
    // FORWARD PASS
    char_code = apply_plugboard(e, char_code);    
    char_code = apply_rotors(e, char_code, RO_FORWARD);

    // REFLECTOR
    char_code = apply_reflector(e, char_code);

    // BACKWARD PASS
    char_code = apply_rotors(e, char_code, RO_BACKWARD);    
    char_code = apply_plugboard(e, char_code);

    // Transform char_code into character
    uint8_t output_char = CODE2CHAR(char_code);

    output[i] = output_char;
  }
}

void enigma_encrypt(Enigma *e, const uint8_t *plaintext, size_t plaintext_len, uint8_t *ciphertext) {
  assert(plaintext_len == strlen((const char*)ciphertext) && "enigma_encrypt(): strlen(ciphertext) != plaintext_len");
  apply_enigma(e, plaintext, plaintext_len, ciphertext);
  printf("[INFO] enigma_encrypt(): '%s' -> '%s'\n", plaintext, ciphertext); 
}

void enigma_decrypt(Enigma *e, const uint8_t *ciphertext, size_t ciphertext_len, uint8_t *plaintext) {
  assert(ciphertext_len == strlen((const char*)plaintext) && "enigma_encrypt(): strlen(ciphertext) != plaintext_len");
  apply_enigma(e, ciphertext, ciphertext_len, plaintext);
  printf("[INFO] enigma_decrypt(): '%s' -> '%s'\n", ciphertext, plaintext);
}

#endif // ENIGMA_IMPLEMENTATION

#ifndef ENIGMA_H_
#define ENIGMA_H_

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// TODO: to test out
// http://wiki.franklinheath.co.uk/index.php/Enigma/Sample_Messages
// https://cryptii.com/pipes/enigma-machine

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
  Wiring forward_wiring;
  Wiring backward_wiring;
  uint8_t position;
  uint8_t notch;
  uint8_t ring;
  char *name;
} Rotor;

typedef struct {
  uint8_t (*board)[2];
  size_t board_size;
} Plugboard;

typedef struct {
  Wiring wiring;
  char *name;
} Reflector;

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
  Reflector reflector;
} Enigma;

// Macros
#define CHAR2CODE(ch) ((uint8_t) ((ch) - 'A'))
#define CODE2CHAR(code) ((char) ('A' + (code)))

// Function signatures
Enigma *init_enigma(const char *rotor_names[3], const char *Reflector, Plugboard plugboard);
void init_wiring(Wiring wiring, const char *alphabet, size_t alphabet_len);
void reverse_wiring(Wiring new_wiring, Wiring old_wiring, size_t wiring_len);
void init_rotor(Rotor *r, const char *rotor_name);
void init_reflector(Reflector *ref, const char *reflector_name);
void destroy_enigma(Enigma *e);

uint8_t apply_rotor(Rotor *r, const uint8_t plaintext_code, RotorOrder order);
void move_rotors(Enigma *e);
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
    wiring[i] = (uint8_t) (alphabet[i] - 'A');
  }
}

// Used to change the direction of old_wiring into new_wiring.
//
// old_wiring[X] = Y if and only if old_wiring[Y] = X
// 
void reverse_wiring(Wiring new_wiring, Wiring old_wiring, size_t wiring_len) {
  for(size_t i = 0; i < wiring_len; i++) {
    new_wiring[old_wiring[i]] = (uint8_t)i;    
  }  
}

// https://www.cryptomuseum.com/crypto/enigma/wiring.htm
void init_rotor(Rotor *r, const char *rotor_name) {
  if (strcmp(rotor_name, "I") == 0 ) {
    r->notch = (uint8_t) 'Q' - 'A';
    init_wiring(r->forward_wiring, "EKMFLGDQVZNTOWYHXUSPAIBRCJ", ALPHABET_SIZE);
  } else if (strcmp(rotor_name, "II") == 0 ) {
    r->notch = (uint8_t) 'E' - 'A';
    init_wiring(r->forward_wiring, "AJDKSIRUXBLHWTMCQGZNPYFVOE", ALPHABET_SIZE);
  } else if (strcmp(rotor_name, "III") == 0 ) {
    r->notch = (uint8_t) 'V' - 'A';
    init_wiring(r->forward_wiring, "BDFHJLCPRTXVZNYEIWGAKMUSQO", ALPHABET_SIZE);
  } else if (strcmp(rotor_name, "IV") == 0 ) {
    r->notch = (uint8_t) 'J' - 'A';
    init_wiring(r->forward_wiring, "ESOVPZJAYQUIRHXLNFTGKDCMWB", ALPHABET_SIZE);        
  } else if (strcmp(rotor_name, "V") == 0 ) {
    r->notch = (uint8_t) 'Z' - 'A';
    init_wiring(r->forward_wiring, "VZBRGITYUPSDNHLXAWMJQOFECK", ALPHABET_SIZE);        
  }
  r->position = 0;
  r->ring = 0;  
  reverse_wiring(r->backward_wiring, r->forward_wiring, ALPHABET_SIZE);

  size_t len = strlen(rotor_name);
  r->name = malloc(sizeof(char) * len);
  strncpy(r->name, rotor_name, len);
}

void init_reflector(Reflector *ref, const char *reflector_name) {
  if (strcmp(reflector_name, "A") == 0 ) {
    init_wiring(ref->wiring, "EJMZALYXVBWFCRQUONTSPIKHGD", ALPHABET_SIZE);
  } else if (strcmp(reflector_name, "B") == 0 ) {
    init_wiring(ref->wiring, "YRUHQSLDPXNGOKMIEBFZCWVJAT", ALPHABET_SIZE);
  } else if (strcmp(reflector_name, "C") == 0 ) {
    init_wiring(ref->wiring, "FVPJIAOYEDRZXWGCTKUQSBNMHL", ALPHABET_SIZE);
  }

  size_t len = strlen(reflector_name);
  ref->name = malloc(sizeof(char) * len);
  strncpy(ref->name, reflector_name, len);
}

Enigma *init_enigma(const char *rotor_names[3], const char *reflector_name, Plugboard plugboard) {
  Enigma *e = malloc(sizeof(Enigma));

  // We specify rotors in the init array from left to right. Given
  // however that the most frequent rotor is the right-most rotor, we
  // save that on index-0.
  init_rotor(&e->rotors[0], rotor_names[2]);
  init_rotor(&e->rotors[1], rotor_names[1]);
  init_rotor(&e->rotors[2], rotor_names[0]);

  init_reflector(&e->reflector, reflector_name);

  if (plugboard.board_size > PLUGBOARD_SIZE) {
    printf("[ERROR]: init_enigma() - supplied plugboard size (%ld) greater than maxium (%d)\n", plugboard.board_size, PLUGBOARD_SIZE);
    exit(0);
  }
  e->plugboard = plugboard;
  
  return e;
}

void destroy_enigma(Enigma *e) {
  if (e) {
    free(e->reflector.name);
    free(e->rotors[0].name);
    free(e->rotors[1].name);
    free(e->rotors[2].name);
    free(e);
  }
}

// --------------------------------------------------------------
// CORE LOGIC

uint8_t apply_rotor(Rotor *r, uint8_t char_code, RotorOrder order) {
  char_code = (char_code - r->ring + r->position + 26) % ALPHABET_SIZE;
  if (order == RO_FORWARD)  {
    char_code = r->forward_wiring[char_code];
  } else if (order == RO_BACKWARD) {
    char_code = r->backward_wiring[char_code];
  } else {
    assert(0 && "Unreachable");
  }
  char_code = (char_code + r->ring - r->position + 26) % ALPHABET_SIZE;

  return char_code;  
}

void move_rotors(Enigma *e) {
  // https://en.wikipedia.org/wiki/Enigma_machine
  // https://www.youtube.com/watch?v=ds8HoowfewA
  // 
  // Double stepping caused by the claw mechanism used for rotating
  // the rotors makes the second rotor move twice in a row, if the
  // first movement brings it in the turnover position during the
  // first rotation.
  //
  //  https://www.youtube.com/watch?v=5StZlF-clPc
  // 
  if (e->rotors[1].position == e->rotors[1].notch) {
    e->rotors[2].position = (e->rotors[2].position + 1) % ALPHABET_SIZE;
    e->rotors[1].position = (e->rotors[1].position + 1) % ALPHABET_SIZE;      
  } else if (e->rotors[0].position == e->rotors[0].notch) {
    e->rotors[1].position = (e->rotors[1].position + 1) % ALPHABET_SIZE;
  }
  e->rotors[0].position = (e->rotors[0].position + 1) % ALPHABET_SIZE;
}


uint8_t apply_rotors(Enigma *e, uint8_t char_code, RotorOrder order) {  
  switch(order) {

  case RO_FORWARD: {
    char_code = apply_rotor(&e->rotors[0], char_code, RO_FORWARD);
    char_code = apply_rotor(&e->rotors[1], char_code, RO_FORWARD);
    char_code = apply_rotor(&e->rotors[2], char_code, RO_FORWARD);
  } break;

  case RO_BACKWARD: {
    char_code = apply_rotor(&e->rotors[2], char_code, RO_BACKWARD);
    char_code = apply_rotor(&e->rotors[1], char_code, RO_BACKWARD);
    char_code = apply_rotor(&e->rotors[0], char_code, RO_BACKWARD);
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
  return e->reflector.wiring[plaintext_code];
}

void apply_enigma(Enigma *e, const uint8_t *input, size_t input_len, uint8_t *output) {
  // Assumes output has been already allocated with a null-terminating
  // string and that len(output) == input_len.

  for (size_t i = 0; i < input_len; i++) {
    uint8_t input_char = input[i];

    // Transform character into char_code
    uint8_t char_code = CHAR2CODE(input_char);

    // Movement is executed before encryption
    move_rotors(e);
    
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
#ifdef ENIGMA_DEBUG
  printf("[INFO] enigma_encrypt(): '%s' -> '%s'\n", plaintext, ciphertext);
#endif  
}

void enigma_decrypt(Enigma *e, const uint8_t *ciphertext, size_t ciphertext_len, uint8_t *plaintext) {
  assert(ciphertext_len == strlen((const char*)plaintext) && "enigma_encrypt(): strlen(ciphertext) != plaintext_len");
  apply_enigma(e, ciphertext, ciphertext_len, plaintext);
#ifdef ENIGMA_DEBUG
  printf("[INFO] enigma_decrypt(): '%s' -> '%s'\n", ciphertext, plaintext);
#endif
}

#endif // ENIGMA_IMPLEMENTATION

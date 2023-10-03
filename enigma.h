#ifndef ENIGMA_H_
#define ENIGMA_H_

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// to test out
// - http://wiki.franklinheath.co.uk/index.php/Enigma/Sample_Messages
// - https://cryptii.com/pipes/enigma-machine

// --------------------------------------------------------------
// DATA STRUCTURES

#define ALPHABET_SIZE 26
#define PLUGBOARD_SIZE 10
#define ROTORS_N 3

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
  char *name;
  char *wiring;
  uint8_t notch;
} RotorModel;

typedef struct {
  uint8_t board[PLUGBOARD_SIZE][2];
  size_t board_size;
} Plugboard;

typedef struct {
  Wiring wiring;
  char *name;
} Reflector;

typedef struct {
  char *name;
  char *wiring;
} ReflectorModel;

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
  Rotor rotors[ROTORS_N];
  Reflector reflector;
} Enigma;

// Macros
#define CHAR2CODE(ch) ((uint8_t) ((ch) - 'A'))
#define CODE2CHAR(code) ((char) ('A' + (code)))

// Function signatures
Enigma *init_enigma(const char *rotor_names[ROTORS_N], const uint8_t rotor_positions[ROTORS_N], const uint8_t rotor_ring_settings[ROTORS_N],
                    const char *reflector_name, uint8_t (*plugboard)[2],
                    size_t plugboard_size);

void init_wiring(Wiring wiring, const char *alphabet, size_t alphabet_len);
void reverse_wiring(Wiring new_wiring, Wiring old_wiring, size_t wiring_len);
char *copy_str(const char *src, const size_t length);

void init_rotor(Rotor *r, const char *rotor_name, const uint8_t position, const uint8_t ring_settings);
void init_rotors(Enigma *e, const char *rotor_names[ROTORS_N], const uint8_t rotor_positions[ROTORS_N], const uint8_t rotor_ring_settings[ROTORS_N]);
void init_reflector(Enigma *e, const char *reflector_name);
void destroy_enigma(Enigma *e);

uint8_t apply_rotor(Rotor *r, const uint8_t plaintext_code, RotorOrder order);
void move_rotors(Enigma *e);
uint8_t apply_rotors(Enigma *e, const uint8_t plaintext_code, RotorOrder order);
uint8_t apply_plugboard(Enigma *e, const uint8_t plaintext_code);
uint8_t apply_reflector(Enigma *e, const uint8_t plaintext_code);
void apply_enigma(Enigma *e, const uint8_t *input, size_t input_len, uint8_t *output);

void enigma_encrypt(Enigma *e, const char *plaintext, size_t plaintext_len, char *ciphertext);
void enigma_decrypt(Enigma *e, const char *ciphertext, size_t ciphertext_len, char *plaintext);

#endif // ENIGMA_H_

#ifdef ENIGMA_IMPLEMENTATION

// --------------------------------------------------------------
// ENIGMA MODELS

RotorModel KNOWN_ROTORS[] = {
  {"M3-I", "EKMFLGDQVZNTOWYHXUSPAIBRCJ", CHAR2CODE('Q')},
  {"M3-II", "AJDKSIRUXBLHWTMCQGZNPYFVOE", CHAR2CODE('E')},
  {"M3-III", "BDFHJLCPRTXVZNYEIWGAKMUSQO", CHAR2CODE('V')},
  {"M3-IV", "ESOVPZJAYQUIRHXLNFTGKDCMWB", CHAR2CODE('J')},
  {"M3-V", "VZBRGITYUPSDNHLXAWMJQOFECK", CHAR2CODE('Z')},
  {NULL, NULL, 0},
};

ReflectorModel KNOWN_REFLECTORS[] = {
  {"M3-A", "EJMZALYXVBWFCRQUONTSPIKHGD"},
  {"M3-B", "YRUHQSLDPXNGOKMIEBFZCWVJAT"},
  {"M3-C", "FVPJIAOYEDRZXWGCTKUQSBNMHL"},
  {NULL, NULL}
};

// --------------------------------------------------------------
// INITIALIZATION LOGIC

void init_wiring(Wiring wiring, const char *alphabet, size_t alphabet_len) {
  for (size_t i = 0; i < alphabet_len; i++) {
    wiring[i] = CHAR2CODE(alphabet[i]);
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

char *copy_str(const char *src, const size_t length) {
  char *dst = malloc(sizeof(char) * (length+1));
  strncpy(dst, src, length);
  dst[length] = '\0';
  return dst;
}

// https://www.cryptomuseum.com/crypto/enigma/wiring.htm
void init_rotor(Rotor *r, const char *rotor_name, const uint8_t position, const uint8_t ring) {
  size_t rotor_len;
  uint8_t found = 0;
  
  for(size_t i = 0; KNOWN_ROTORS[i].name != NULL; i++) {
    if (strcmp(KNOWN_ROTORS[i].name, rotor_name) == 0) {
      found = 1;
      
      char *wiring = KNOWN_ROTORS[i].wiring;
      char *name = KNOWN_ROTORS[i].name;
      uint8_t notch = KNOWN_ROTORS[i].notch;

      init_wiring(r->forward_wiring, wiring, ALPHABET_SIZE);
      reverse_wiring(r->backward_wiring, r->forward_wiring, ALPHABET_SIZE);

      r->notch = notch;      
      r->position = position;
      r->ring = ring;

      rotor_len = strlen(name);      
      r->name = copy_str(rotor_name, rotor_len);      
      
      break;
    }
  }

  if (!found) {
    assert(0 && "init_rotor(): unsupported rotor\n");
  }
}

void init_rotors(Enigma *e, const char *rotor_names[ROTORS_N], const uint8_t rotor_positions[ROTORS_N], const uint8_t rotor_ring_settings[ROTORS_N]) {
  // We specify rotors in the init array from left to right. Given
  // however that the most frequent rotor is the right-most rotor, we
  // save that on index-0.
  for (size_t i = 0; i < ROTORS_N; i++) {
    e->rotors[i].name = NULL;
    init_rotor(&e->rotors[i], rotor_names[ROTORS_N - 1 - i], rotor_positions[ROTORS_N - 1 - i], rotor_ring_settings[ROTORS_N - 1 - i]);
  }
}

void init_reflector(Enigma *e, const char *reflector_name) {
  size_t ref_len;
  uint8_t found = 0;

  for (size_t i = 0; KNOWN_REFLECTORS[i].name != NULL; i++) {
    if (strcmp(KNOWN_REFLECTORS[i].name, reflector_name) == 0) {
      found = 1;

      char *name = KNOWN_REFLECTORS[i].name;
      char *wiring = KNOWN_REFLECTORS[i].wiring;

      init_wiring(e->reflector.wiring, wiring, ALPHABET_SIZE);

      ref_len = strlen(name);
      e->reflector.name = copy_str(reflector_name, ref_len);
    }
  }

  if (!found) {
    assert(0 && "init_reflector(): unsupported reflector\n");
  }  
  
}

void init_plugboard(Enigma *e, uint8_t(*board)[2], size_t plugboard_size) {
  e->plugboard.board_size = plugboard_size;
  for(size_t i = 0; i < plugboard_size; i++) {
    e->plugboard.board[i][0] = CHAR2CODE(board[i][0]);
    e->plugboard.board[i][1] = CHAR2CODE(board[i][1]);
  }
}

Enigma *init_enigma(const char *rotor_names[ROTORS_N], const uint8_t rotor_positions[ROTORS_N], const uint8_t rotor_ring_settings[ROTORS_N],
                    const char *reflector_name, uint8_t (*plugboard)[2],
                    size_t plugboard_size) {

  if (plugboard_size > PLUGBOARD_SIZE) {
    printf("[ERROR]: init_enigma() - supplied plugboard size (%ld) greater than maxium (%d)\n", plugboard_size, PLUGBOARD_SIZE);
    exit(0);
  }
  
  Enigma *e = calloc(1, sizeof(Enigma));
  
  init_rotors(e, rotor_names, rotor_positions, rotor_ring_settings);
  init_reflector(e, reflector_name);
  init_plugboard(e, plugboard, plugboard_size);
  
  return e;
}

// --------------------------------------------------------------
// DESTRUCTION LOGIC

void destroy_rotor(Rotor *r) {
  if(r->name)
    free(r->name);
}

void destroy_rotors(Rotor *rotors) {
  for (size_t i = 0; i < ROTORS_N; i++) {
    destroy_rotor(&rotors[i]);
  }
}

void destroy_plugboard(Enigma *e) {
  for(size_t i = 0; i < e->plugboard.board_size; i++) {
    e->plugboard.board[i][0] = 255;
    e->plugboard.board[i][1] = 255;
  }
  e->plugboard.board_size = 0;
}

void destroy_reflector(Enigma *e) {
  if(e->reflector.name)
    free(e->reflector.name);
}

void destroy_enigma(Enigma *e) {
  if (e) {
    destroy_rotors(e->rotors);
    destroy_reflector(e);
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

void enigma_encrypt(Enigma* e, const char* plaintext, size_t plaintext_len, char* ciphertext) {
  assert(plaintext_len == strlen(ciphertext) && "enigma_encrypt(): strlen(ciphertext) != plaintext_len");
  apply_enigma(e, (const uint8_t*)plaintext, plaintext_len, (uint8_t*) ciphertext);
#ifdef ENIGMA_DEBUG
  printf("[INFO] enigma_encrypt(): '%s' -> '%s'\n", plaintext, ciphertext);
#endif  
}

void enigma_decrypt(Enigma *e, const char *ciphertext, size_t ciphertext_len, char *plaintext) {
  assert(ciphertext_len == strlen(plaintext) && "enigma_encrypt(): strlen(ciphertext) != plaintext_len");
  apply_enigma(e, (const uint8_t*)ciphertext, ciphertext_len, (uint8_t*)plaintext);
#ifdef ENIGMA_DEBUG
  printf("[INFO] enigma_decrypt(): '%s' -> '%s'\n", ciphertext, plaintext);
#endif
}

#endif // ENIGMA_IMPLEMENTATION

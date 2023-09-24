// Simple CLI interface for showcasing the Enigma header-only library

#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#define ENIGMA_IMPLEMENTATION
#include "../enigma.h"

#define MAX_ARGS 128

// ----------------------------------------

typedef enum {
  C_QUIT = 0,  
  C_HELP,
  C_INFO,
  C_SET,
  C_ENCRYPT,
  C_DECRYPT,
  C_UNKNOWN
} CommandType;

typedef struct {
  CommandType type;
  char **args;
  size_t n_args;
} Command;

CommandType string2command_type(const char *cmd);
Command parse_args(char *buff);

void execute_quit(char **args, size_t n_args);
void execute_help(char **args, size_t n_args);
void execute_info(char **args, size_t n_args);
void execute_set(char **args, size_t n_args);
void execute_encrypt(char **args, size_t n_args);
void execute_decrypt(char **args, size_t n_args);

// ----------------------------------------

Enigma *ENIGMA;

// NOTE: Be careful, because the specific index of these functions
// DOES MATTER. There is a matching 1-to-1 between the CommandType
// enum and the index of these function.
void (*CLI_ACTION_TABLE[]) (char **args, size_t n_args) = {
  execute_quit,
  execute_help,  
  execute_info,
  execute_set,
  execute_encrypt,
  execute_decrypt,
};

// ----------------------------------------

CommandType string2command_type(const char *cmd) {
  if (strncmp(cmd, "help", 4) == 0) {
    return C_HELP;
  } else if (strncmp(cmd, "quit", 4) == 0) {
    return C_QUIT;
  } else if (strncmp(cmd, "info", 4) == 0) {
    return C_INFO;
  } else if (strncmp(cmd, "set", 4) == 0) {
    return C_SET;
  } else if (strncmp(cmd, "encrypt", 4) == 0) {
    return C_ENCRYPT;
  } else if (strncmp(cmd, "decrypt", 4) == 0) {
    return C_DECRYPT;
  } else {
    return C_UNKNOWN;
  }
}

Command parse_args(char *buff) {
  Command c;
  
  char *cmd_str = strtok(buff, " ");
  c.type = string2command_type(cmd_str);

  c.args = malloc(sizeof(char*) * MAX_ARGS);
  size_t i = 0;
  do {
    c.args[i++] = strtok(NULL, " ");
  } while (c.args[i-1] != NULL);
  c.n_args = i - 1;
  
  return c;
}

// ----------------------------------------

void execute_cmd(Command cmd) {
  CLI_ACTION_TABLE[cmd.type](cmd.args, cmd.n_args);
}

void execute_quit(char **args, size_t n_args) {
  printf("Enigma> About to exit...\n");
  exit(0);
}

void execute_help(char **args, size_t n_args) {
  printf("Enigma> List of commands...\n\n");
  printf("        quit – exit from enigma cli\n");
  printf("        help – list available commands\n");
  printf("        info – print enigma configuration\n");
  printf("        set <rotors> | set <reflector> | set <plugboard> – change enigma configuration\n");
  printf("        encrypt <plaintext> – encrypt plaintext\n");
  printf("        decrypt <ciphertext> – decrypt ciphertext\n");
  printf("\n");  
}

void execute_info(char **args, size_t n_args) {
  printf("Enigma> Current configuration...\n");
  // rotors
  printf("        Rotors (from left to right): %s, %s, %s\n",
	 ENIGMA->rotors[2].name,
	 ENIGMA->rotors[1].name,
	 ENIGMA->rotors[0].name
	 );
  printf("               Position: %d, %d, %d\n",
	 ENIGMA->rotors[2].position,
	 ENIGMA->rotors[1].position,
	 ENIGMA->rotors[0].position
	 );
  printf("                   Ring: %d, %d, %d\n",
	 ENIGMA->rotors[2].ring,
	 ENIGMA->rotors[1].ring,
	 ENIGMA->rotors[0].ring
	 );    
  
  // reflector
  printf("        Reflector: %s\n", ENIGMA->reflector.name);

  // plugboard
  printf("        Plugboard: %ld plugs\n", ENIGMA->plugboard.board_size);

  for (size_t i = 0; i < ENIGMA->plugboard.board_size; i++) {
    printf("                   (%c, %c)\n",
	   CODE2CHAR(ENIGMA->plugboard.board[i][0]),
	   CODE2CHAR(ENIGMA->plugboard.board[i][1])
	   );
  }
  
  return;
}

void execute_set(char **args, size_t n_args) {
  if (n_args < 1) {
    printf("Enigma> set requires at least 1 arg!\n");
    return;
  }

  char *set_type = *args++;
  if (strncmp(set_type, "rotor", 5) == 0) {
    // set rotor <rotor_name> <position> <ring>
    if (n_args < 5) {
      printf("Enigma> set rotor requires at least 5 args: set rotor <rotor_index_to_swap> <rotor_name> <position> <ring>!\n");
      return;
    }
    char *rotor_index_to_swap = *args++;
    char *rotor_name = *args++;
    char *pos = *args++;
    char *ring = *args++;

    // TODO finish this
    
  } else if (strncmp(set_type, "reflector", 9) == 0) {
    // set reflector <reflector_name>
    if (n_args < 2) {
      printf("Enigma> set reflector requires at least 2 args: set reflector <reflector_name>!\n");
      return;
    }
    char *reflector_name = *args++;
    init_reflector(&ENIGMA->reflector, reflector_name);
  } else if (strncmp(set_type, "plugboard", 9) == 0) {
    // set plugboard <k1-v1> <k2-v2> <k3-v3>    
    if ((n_args < 2) || (n_args > 11)) {
      printf("Enigma> set reflector requires at least 2 args and no more than 11 args: set reflector <k1-v1> <k2-v2> ...!\n");
      return;
    }

    // TODO finish this
  }
}

void execute_encrypt(char **args, size_t n_args) {
  if (n_args != 1) {
    printf("Enigma> encrypt requires only 1 arg, the plaintext to encrypt!\n");
    return;
  }
  
  char *plaintext = args[0];
  size_t length = strlen(plaintext);
  char *ciphertext = malloc((length+1) * sizeof(char));
  strncpy(ciphertext, plaintext, length);
  ciphertext[length] = '\0';
  
  enigma_encrypt(ENIGMA, (uint8_t*)plaintext, length, (uint8_t*)ciphertext);
  printf("%s\n", ciphertext);
  free(ciphertext);
}

void execute_decrypt(char **args, size_t n_args) {
  if (n_args != 1) {
    printf("Enigma> decrypt requires only 1 arg, the ciphertext to encrypt!\n");
    return;
  }
  
  char *ciphertext = args[0];
  size_t length = strlen(ciphertext);
  char *plaintext = malloc((length+1) * sizeof(char));
  strncpy(plaintext, ciphertext, length);
  plaintext[length] = '\0';
  
  enigma_decrypt(ENIGMA, (uint8_t*)ciphertext, length, (uint8_t*)plaintext);
  printf("%s\n", plaintext);
  free(plaintext);  
}

// ----------------------------------------

int main(void) {
  // default enigma
  ENIGMA = init_enigma((const char *[]){"II", "I", "III"},  // rotors
		       "A",                                 // reflector
		       (Plugboard) {                        // plugboard
			 (uint8_t [][2]){
			   {'A' - 'A', 'M' - 'A'}, {'F' - 'A', 'I' - 'A'},
			   {'N' - 'A', 'V' - 'A'}, {'P' - 'A', 'S' - 'A'},
			   {'T' - 'A', 'U' - 'A'}, {'W' - 'A', 'Z' - 'A'},			   
			 },
			 6}
		       );
  
  // REPL
  int done = 0;  
  while (!done) {
    char *buff;
    buff = readline("Enigma> ");

    if (buff == NULL) {
      printf("Enigma> Terminating process...\n");
      free(buff);
      exit(0);
    }

    Command cmd = parse_args(buff);
    if (cmd.type == C_UNKNOWN) {
      printf("Enigma> Unknown command...type 'help' for more documentation\n");
    } else {
      execute_cmd(cmd);
    }

    // NOTE: Is it okay if in parse_args() we call the funciton
    // strtok() on the buffer allocated by readline() or will this
    // cause problems with memory management? can we just free it as
    // usual?
    free(buff);
  }
  
  return 0;
}

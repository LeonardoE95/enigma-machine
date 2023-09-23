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
  C_INIT,
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
void execute_init(char **args, size_t n_args);
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
  execute_init,
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
  } else if (strncmp(cmd, "init", 4) == 0) {
    return C_INIT;
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
  exit(0);
}

void execute_help(char **args, size_t n_args) {
  // TODO...
  printf("help\n");
  return;
}

void execute_info(char **args, size_t n_args) {
  // TODO...
  printf("info\n");
  return;
}

void execute_init(char **args, size_t n_args) {
  // TODO...
  printf("init\n");
  return;
}

void execute_encrypt(char **args, size_t n_args) {
  // TODO...
  printf("encrypt\n");  
  return;
}

void execute_decrypt(char **args, size_t n_args) {
  // TODO...
  printf("decrypt\n");  
  return;
}

// ----------------------------------------

int main(void) {

  // default enigma
  const char *rotors[3] = { "I", "II", "III" };
  Plugboard plug = { NULL, 0 };
  ENIGMA = init_enigma(rotors, "A", plug);
  
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

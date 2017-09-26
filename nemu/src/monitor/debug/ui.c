#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

extern CPU_state cpu;

uint32_t expr(char *e, bool *success);

void create_wp(char* expr, int val);
void remove_wp(int NO);
void print_wp();

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    cpu_exec(1);
  }
  else {
    i = atoi(args);
    if(i < 0) {
        printf("Exception: Unexpected instruction count \'%s\'.\n", arg);
    }
    else
        cpu_exec(i);
  }
  return 0;
}

static int cmd_info(char *args) {
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    printf("Exception: Subcommand SUBCMD is required.\n");
  }
  else {
    if (strcmp(arg, "r") == 0) {
      for (i = 0; i < 8; i++) {
        printf("%s: 0x%08X\n", reg_name(i, 4), reg_l(i));
      }
      printf("eip: 0x%08X\n", cpu.eip);
    }
    else if (strcmp(arg, "w") == 0) {
      print_wp();
    }
    else {
      printf("Exception: Unknown subcommand \'%s\'.\n", arg);
    }
  }
  return 0;
}

static int cmd_p(char *args) {
  uint32_t i;
  bool success;


  if (args == NULL) {
    printf("Exception: Expression field EXPR is required.\n");
  }
  else {
    i = expr(args, &success);
    if (success)
      printf("Result = %d\n       = 0x%08X\n", i, i);
  }
  return 0;
}

static int cmd_x(char *args) {
  char *arg = strtok(NULL, " ");
  uint32_t N;
  uint32_t Add;
  char *exp = strtok(NULL, " ");  
  bool success;
  uint32_t MemContent;
  bool IsFirst = true;
  unsigned char ByteNow;

  if (arg == NULL || exp == NULL) {
    printf("Exception: Count field N and Expression field EXPR is required.\n");
  }
  else {
    N = atoi(arg);
    if (N <= 0)
      printf("Exception: N must be greater than 0.\n");
    else {
      Add = expr(exp, &success);
      if (!success)
        printf("Exception: Unexpected address \'%s\'.\n", args + strlen(arg) + 1);
      else {
        printf("%d byte(s) of memory mapped from 0x%08X:\n\n", 4 * N, Add);
        printf("            00 01 02 03 04 05 06 07\n\n");
        while (N--) {
          MemContent = vaddr_read(Add, 4);
          if (IsFirst) {
            printf("0x%08X  ", Add);
          }
          ByteNow = MemContent & 0xFF;
          printf("%02X ", ByteNow);
          MemContent = MemContent >> 8;
          ByteNow = MemContent & 0xFF;
          printf("%02X ", ByteNow);
          MemContent = MemContent >> 8;
          ByteNow = MemContent & 0xFF;
          printf("%02X ", ByteNow);
          MemContent = MemContent >> 8;
          ByteNow = MemContent & 0xFF;
          printf("%02X ", ByteNow);
          if (!IsFirst)
              printf("\n");
          IsFirst = !IsFirst;
          Add += 4;
        }
      }
      if (!IsFirst)
        printf("\n");
    }
  }
  return 0;
}

static int cmd_w(char *args) {
  bool success;
  int val;

  if (args == NULL) {
    printf("Exception: Expression field EXPR is required.\n");
  }
  else {
    // Execute the expression to check if there are any mistakes in the expression
    if (strlen(args) > 32 * 16) {
      printf("Exception: Expression too long.\n");
      return 0;
    }
    val = expr(args, &success);
    
    if (!success) {
      printf("Exception: Watchpoint not set due to error(s) in your expression.\n");
      return 0;
    }
    create_wp(args, val);
  }
  return 0;
}

static int cmd_d(char *args) {
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    printf("Exception: Field N is required.\n");
  }
  else {
    i = atoi(args);
    remove_wp(i);
  }
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "N - Step in for N steps", cmd_si },
  { "info", "SUBCMD - Provide program status. SUBCMD = r or w", cmd_info },
  { "p", "EXPR - Evaluate expression", cmd_p },
  { "x", "N EXPR - Output 4N bytes from the address evaluated form EXPR", cmd_x },
  { "w", "EXPR - Set a watchpoint at the address evaluated from EXPR", cmd_w },
  { "d", "N - Delete the watchpoint numbered N", cmd_d }
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

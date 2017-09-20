#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, 
  TK_DECNUM = 255,
  TK_HEXNUM = 254,
  TK_REGNAME = 253,
  TK_LP = '(',
  TK_RP = ')',
  TK_NEG = 248,
  TK_DEREF = 247,
  TK_LGCNOT = '!',
  TK_NOT = '~', 
  TK_MUL = '*',
  TK_DIV = '/',
  TK_MOD = '%',
  TK_ADD = '+',
  TK_SUB = '-',
  TK_LEFTSHFT = 246,
  TK_RIGHTSHFT = 245,
  TK_LEQ = 244,
  TK_GEQ = 243,
  TK_LE = 242,
  TK_GE = 241,
  TK_EQ = 252,
  TK_NEQ = 251,
  TK_AND = 240,
  TK_XOR = 239,
  TK_OR = 238,
  TK_LGCAND = 250,
  TK_LGCOR = 249

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {"0x[0-9a-fA-F]+", TK_HEXNUM},
  {"[0-9]+", TK_DECNUM}, 
  {"\\$[a-z]+", TK_REGNAME},
  {" +", TK_NOTYPE},    // spaces
  {"\\(", '('},         // left parenthese
  {"\\)", ')'},         // right parenthese
  {"\\*", '*'},         // multiply or derefrence
  {"\\/", '/'},         // divide
  {"\\%", '%'},         // mod
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // substitude
  {"<<", TK_LEFTSHFT},  // left shift
  {">>", TK_RIGHTSHFT}, // right shift
  {"<=", TK_LEQ},       // less or equal
  {">=", TK_GEQ},       // greater or equal
  {"<", TK_LE},         // less
  {">", TK_GE},         // greater
  {"==", TK_EQ},        // equal
  {"\\!=", TK_NEQ},     // not equal
  {"\\&\\&", TK_LGCAND},// logical and
  {"\\|\\|", TK_LGCOR}, // logical or
  {"\\!", '!'},         // logical not
  {"\\&", TK_AND} ,     // and
  {"\\^", TK_XOR},      // xor
  {"\\|", TK_OR} ,      // or
  {"\\~", '~'}          // not
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        int IndexInTokenStr = 0;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        
        // Don't have to handle spaces.
        if (rules[i].token_type == TK_NOTYPE)
          break;
         
        if (substr_len >= 32) {
          printf("Exception: Token too long at position %d with length of %d.\n", position, substr_len);
          return false;
        }
        
        if (nr_token >= 32) {
            printf("Exception: Expression is too long.\n");
            return false;
        }
        
        tokens[nr_token].type = rules[i].token_type;
        
        switch (rules[i].token_type) {
          case TK_DECNUM:
          case TK_HEXNUM:
          case TK_REGNAME:
            while (substr_len--) {
              tokens[nr_token].str[IndexInTokenStr] = substr_start[IndexInTokenStr];
              IndexInTokenStr++;
            }
            tokens[nr_token].str[IndexInTokenStr] = 0;
            break;
          case TK_LP:
          case TK_RP:
          case TK_MUL:
          case TK_DIV:
          case TK_ADD:
          case TK_SUB:
          case TK_LEFTSHFT:
          case TK_RIGHTSHFT:
          case TK_LEQ:
          case TK_GEQ:
          case TK_LE:
          case TK_GE:
          case TK_EQ:
          case TK_NEQ:
          case TK_AND:
          case TK_XOR:
          case TK_OR:
          case TK_LGCAND:
          case TK_LGCOR:
          case TK_LGCNOT:
          case TK_NOT:
          case TK_MOD:
            break;
          default: 
            printf("Unexpected: You are not expected to be here. Do please report this to the developer.\n");
        }
        nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t finddom(int p, int q) {
  int last = -1;
  int lastlvl = 0;
  int parentheselvl = 0;
  
  while (q >= p) {
    if (parentheselvl != 0) {
      if (tokens[q].type == TK_LP)
        parentheselvl--;
      if (tokens[q].type == TK_RP)
        parentheselvl++;
      q--;
      continue;
    }
    switch (tokens[q].type) {
      case TK_RP:
        parentheselvl++;
        break;
      case TK_NEG:
      case TK_DEREF:
      case TK_NOT:
      case TK_LGCNOT:
        if (lastlvl <= 3) { // Right combined
          last = q;
          lastlvl = 3;
        }
        break;
      case TK_MUL:
      case TK_DIV:
      case TK_MOD:
        if (lastlvl < 5) {
          last = q;
          lastlvl = 5;
        }
        break;
      case TK_ADD:
      case TK_SUB:
        if (lastlvl < 6) {
          last = q;
          lastlvl = 6;
        }
        break;
      case TK_LEFTSHFT:
      case TK_RIGHTSHFT:
        if (lastlvl < 7) {
          last = q;
          lastlvl = 7;
        }
        break;
      case TK_LEQ:
      case TK_GEQ:
      case TK_LE:
      case TK_GE:
        if (lastlvl < 8) {
          last = q;
          lastlvl = 8;
        }
        break;
      case TK_EQ:
      case TK_NEQ:
        if (lastlvl < 9) {
          last = q;
          lastlvl = 9;
        }
        break;
      case TK_AND:
        if (lastlvl < 10) {
          last = q;
          lastlvl = 10;
        }
        break;
      case TK_XOR:
        if (lastlvl < 11) {
          last = q;
          lastlvl = 11;
        }
        break;
      case TK_OR:
        if (lastlvl < 12) {
          last = q;
          lastlvl = 12;
        }
        break;
      case TK_LGCAND:
        if (lastlvl < 13) {
          last = q;
          lastlvl = 13;
        }
        break;
      case TK_LGCOR:
        if (lastlvl < 14) {
          last = q;
          lastlvl = 14;
        }
        break;
      default:
        break;
    }
    q--;
  }
  if (parentheselvl != 0) {
    printf("Exception: Unmatched parenthese \'%s\'.\n", ((parentheselvl < 0) ? "(" : ")"));
    last = -1;
  }
  return last;
}

bool check_parentheses(int p, int q, bool *success) {
  int parentheselvl = 0;
  bool retval = true;
  
  if (tokens[p].type != TK_LP || tokens[q].type != TK_RP)
    return false;
  
  p += 1;
  while (p < q) {
    if (tokens[p].type == TK_LP)
      parentheselvl += 1;
    else if (tokens[p].type == TK_RP) {
      if (parentheselvl == 0 && retval == true)
        retval = false;
      parentheselvl -= 1;
    }
    p += 1;
  }
  if (parentheselvl == 0)
    return retval;
  *success = false;
  printf("Exception: Unmatched parenthese \'%s\'.\n", ((parentheselvl > 0) ? "(" : ")"));
  return false;
}

uint32_t hexstr2int(char* hexnum) {
  uint32_t result = 0;
  int p = 2;
  while (hexnum[p]) {
    if (hexnum[p] >= '0' && hexnum[p] <= '9')
      result = result * 16 + (hexnum[p] - '0');
    else if (hexnum[p] >= 'A' && hexnum[p] <= 'F')
      result = result * 16 + (hexnum[p] - 'A' + 10);  
    else if (hexnum[p] >= 'a' && hexnum[p] <= 'f')
      result = result * 16 + (hexnum[p] - 'a' + 10); 
    p += 1; 
  }
  return result;
}


uint32_t eval(int p, int q, bool *success) {
  int domop;
  uint32_t val1;
  uint32_t val2;
  int i;
  
  if (p > q) {
    printf("Exception: Bad expression. p = %d, q = %d\n", p, q);
    *success = false;
    return 0;
  }
  else if (p == q) {
    if (tokens[p].type == TK_DECNUM)
      return atoi(tokens[p].str);
    if (tokens[p].type == TK_HEXNUM)
      return hexstr2int(tokens[p].str);
    if (tokens[p].type == TK_REGNAME) {
      if (!strcmp(tokens[p].str, "$eip"))
        return cpu.eip;
      /*
         str + 1 is used to ignore the $ in regname
      */
      for (i = 0; i < 8; i++) {
        if (!strcmp(tokens[p].str + 1, reg_name(i, 4))) {
          return reg_l(i);
        }
        if (!strcmp(tokens[p].str + 1, reg_name(i, 2))) {
          return reg_w(i);
        }
        if (!strcmp(tokens[p].str + 1, reg_name(i, 1))) {
          return reg_b(i);
        }
      }
    }
    *success = false;
    printf("Exception: Orphan operator.\n");
    return 0;
  }
  else if (check_parentheses(p, q, success) == true) {
    return eval(p + 1, q - 1, success);
  }
  else if (*success) {
    domop = finddom(p, q);
    
    if (domop == -1) {
      *success = false;
      return 0;
    }
    
    
    if (tokens[domop].type == TK_DEREF || tokens[domop].type == TK_NEG || tokens[domop].type == TK_LGCNOT || tokens[domop].type == TK_NOT) {
      /* 
         We define a * or a - as deref or neg if and only if the token before
         it is an operator, so as long as there are still tokens before deref/neg
         in the subexpr, deref/neg will not be the domop.
      */ 
      val1 = eval(p + 1, q, success);
      if (*success == false)
        return 0;
      
      if (tokens[domop].type == TK_DEREF)
        return vaddr_read(val1, 4);
      if (tokens[domop].type == TK_NEG)  
        return -val1;
      if (tokens[domop].type == TK_LGCNOT)  
        return !val1;
      if (tokens[domop].type == TK_NOT)  
        return ~val1;
    }
    
    val1 = eval(p, domop - 1, success);
    val2 = eval(domop + 1, q, success);
    if (*success == false)
      return 0;
    
    switch (tokens[domop].type) {
      case TK_ADD: return val1 + val2;
      case TK_SUB: return val1 - val2;
      case TK_MUL: return val1 * val2;
      case TK_DIV:
        if (val2 == 0) {
          *success = false;
          printf("Exception: Cannot be divided by 0.\n");
          return 0;
        }
        else
          return val1 / val2;
      case TK_MOD:
        if (val2 == 0) {
          *success = false;
          printf("Exception: Cannot be divided by 0.\n");
          return 0;
        }
        else
          return val1 % val2;
      case TK_LEFTSHFT: return val1 << val2;
      case TK_RIGHTSHFT: return val1 >> val2;
      case TK_LEQ: return val1 <= val2;
      case TK_GEQ: return val1 >= val2;
      case TK_LE: return val1 < val2; 
      case TK_GE: return val1 > val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 & val2;
      case TK_XOR: return val1 ^ val2;
      case TK_OR: return val1 | val2;
      case TK_LGCAND: return val1 && val2;
      case TK_LGCOR: return val1 || val2;
      
      default: 
        *success = false;
        printf("Unexpected: You are not expected to be here. Do please report this to the developer.\n");
        return 0;
    }
  }
  return 0;
}

uint32_t expr(char *e, bool *success) {
  *success = true;
  int i;
  
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  
  /* TODO: Insert codes to evaluate the expression. */
  for(i = 0; i < nr_token; i++) {
    if (tokens[i].type == TK_MUL && (i == 0 || (tokens[i - 1].type != TK_RP && tokens[i - 1].type != TK_DECNUM && tokens[i - 1].type != TK_HEXNUM && tokens[i - 1].type != TK_REGNAME)))
      tokens[i].type = TK_DEREF;
    if (tokens[i].type == TK_SUB && (i == 0 || (tokens[i - 1].type != TK_RP && tokens[i - 1].type != TK_DECNUM && tokens[i - 1].type != TK_HEXNUM && tokens[i - 1].type != TK_REGNAME)))
      tokens[i].type = TK_NEG;
  }
  
  return eval(0, nr_token - 1, success);
}

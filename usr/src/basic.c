#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Line {
    int num;
    char text[256];
    struct _Line *next;
} Line;

Line *programHead = NULL;
int variables[26] = {0};

char *Trim(char *str) {
    char *end;

    while (*str == ' ') str++;

    if (*str == 0) return str;

    end = str + strlen(str) - 1;
    while (end > str && *end == ' ') end--;
    end[1] = '\0';

    return str;
}

void ClearProgram() {
    Line *curr = programHead;
    while (curr != NULL) {
        Line *next = curr->next;
        free(curr);
        curr = next;
    }
    programHead = NULL;
}

void ProcessLine(int num, char *text) {
    Line **curr = &programHead;

    while (*curr != NULL && (*curr)->num < num) curr = &((*curr)->next);

    if (*curr != NULL && (*curr)->num == num) {
        if (text[0] == '\0' || strcmp(text, "\n") == 0) {
            Line *temp = *curr;
            *curr = (*curr)->next;
            free(temp);
        } else
            strcpy((*curr)->text, text);

        return;
    }

    if (text[0] != '\0' && strcmp(text, "\n") != 0) {
        Line *newNode = malloc(sizeof(Line));
        newNode->num = num;
        strcpy(newNode->text, text);
        newNode->next = *curr;
        *curr = newNode;
    }
}

typedef enum {
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_END,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char text[256];
    int value;
} Token;

Line *FindLine(int target) {
    Line *curr = programHead;
    while (curr != NULL) {
        if (curr->num == target) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

Token GetNextToken(char **input) {
    Token t;
    t.text[0] = '\0';

    while (**input == ' ' || **input == '\t') (*input)++;

    if (**input == '\0' || **input == '\n') {
        t.type = TOKEN_END;
        return t;
    }

    if (**input == '"') {
        ++(*input);
        int i = 0;
        while (**input != '"' && **input != '\0') t.text[i++] = *(*input)++;
        t.text[i] = '\0';
        if (**input == '"') (*input)++;
        t.type = TOKEN_STRING;
        return t;
    }

    if (**input >= '0' && **input <= '9') {
        int i = 0;
        while (**input >= '0' && **input <= '9') t.text[i++] = *(*input)++;
        t.text[i] = '\0';
        t.value = atoi(t.text);
        t.type = TOKEN_NUMBER;
        return t;
    }

    if (strchr("+-*/=", **input)) {
        t.type = TOKEN_OPERATOR;
        t.text[0] = *(*input)++;
        t.text[1] = '\0';
        return t;
    }

    if ((**input >= 'a' && **input <= 'z') ||
        (**input >= 'A' && **input <= 'Z')) {
        int i = 0;
        while ((**input >= 'a' && **input <= 'z') ||
               (**input >= 'A' && **input <= 'Z') ||
               (**input >= '0' && **input <= '9'))
            t.text[i++] = *(*input)++;
        t.text[i] = '\0';
        t.type = TOKEN_IDENTIFIER;
        return t;
    }

    t.type = TOKEN_ERROR;
    return t;
}

int RunLine(Line *line, Line **iter) {
    char lineCopy[256];
    memcpy(lineCopy, line->text, 256);

    Token t;

    char *linePtr = lineCopy;
    while ((t = GetNextToken(&linePtr)).type != TOKEN_END) {
        if (t.type == TOKEN_IDENTIFIER) {
            if (strcmp(t.text, "PRINT") == 0) {
                while (1) {
                    t = GetNextToken(&linePtr);
                    if (t.type == TOKEN_END || t.type == TOKEN_ERROR) break;
                    if (t.type == TOKEN_STRING)
                        printf("%s ", t.text);
                    else if (t.type == TOKEN_IDENTIFIER)
                        printf("%d ", variables[t.text[0] - 'A']);
                    else if (t.type == TOKEN_NUMBER)
                        printf("%d ", t.value);
                    else {
                        fprintf(FD_STDERR, "ERROR AT %d: CAN'T PRINT\n",
                                line->num);
                        return 1;
                    }
                }
                puts("\n");
            }

            else if (strcmp(t.text, "GOTO") == 0) {
                t = GetNextToken(&linePtr);
                if (t.type != TOKEN_IDENTIFIER && t.type != TOKEN_NUMBER) {
                    fprintf(FD_STDERR, "ERROR AT %d: GOTO REQUIRES NUMBER\n",
                            line->num);
                    return 1;
                }

                int gt = t.value;
                if (t.type == TOKEN_IDENTIFIER) gt = variables[t.text[0] - 'A'];

                Line *new = FindLine(gt);
                if (!new) {
                    fprintf(FD_STDERR, "ERROR AT %d: NO SUCH LINE %d\n",
                            line->num, gt);
                    return 1;
                }

                *iter = new;
            }

            else if (strcmp(t.text, "LET") == 0) {
                Token varTok = GetNextToken(&linePtr);
                if (GetNextToken(&linePtr).text[0] != '=') {
                    fprintf(FD_STDERR,
                            "ERROR AT %d: EXPECTED = AFTER VARNAME IN LET\n",
                            line->num);
                    return 1;
                }
                Token valTok = GetNextToken(&linePtr);

                int idx = varTok.text[0] - 'A';

                if (valTok.type == TOKEN_NUMBER) {
                    variables[idx] = valTok.value;
                } else if (valTok.type == TOKEN_IDENTIFIER) {
                    int src_idx = valTok.text[0] - 'A';
                    variables[idx] = variables[src_idx];
                } else {
                    fprintf(FD_STDERR,
                            "ERROR AT %d: VARIABLES ONLY ACCEPT INTEGERS\n",
                            line->num);
                    return 1;
                }
            } else {
                fprintf(FD_STDERR, "ERROR AT %d: INVALID KEYWORD: %s\n",
                        line->num, t.text);
                return 1;
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    printf("cyBASIC -- minimal BASIC variant\n\n");

    while (1) {
        char buffer[256];
        printf("] ");

        gets(buffer, sizeof(buffer));

        char *token = strtok(buffer, " ");

        if (buffer[0] == '\0') continue;

        if (strcmp(token, "LIST") == 0) {
            for (Line *r = programHead; r; r = r->next)
                printf("%4d %s\n", r->num, r->text);
        } else if (strcmp(token, "EXIT") == 0)
            break;
        else if (strcmp(token, "NEW") == 0)
            ClearProgram();
        else if (strcmp(token, "RUN") == 0) {
            for (int i = 0; i < 26; ++i) variables[i] = 0;
            for (Line *r = programHead; r; r = r->next)
                if (RunLine(r, &r)) break;
        } else {
            char *tokenc = token;
            int num = 1;
            while (*tokenc) {
                if (*tokenc >= '0' && *tokenc <= '9') {
                    ++tokenc;
                    continue;
                }
                num = 0;
                break;
            }

            if (!num) {
                fprintf(FD_STDERR, "UNKNOWN STATEMENT: %s\n", token);
                continue;
            }

            char *line = strtok(NULL, "");
            if (!line) line = "";
            ProcessLine(atoi(token), line);
        }
    }

    return 0;
}

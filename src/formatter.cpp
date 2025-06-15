#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LEN 256
#define MAX_LINE_LEN 1024

typedef enum {
    TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_OPERATOR,
    TOKEN_SEPARATOR, TOKEN_COMMENT, TOKEN_UNKNOWN, TOKEN_EOF
} TokenType;

const char* token_type_str[] = {
    "keyword", "identifier", "number", "operator",
    "separator", "comment", "unknown", "eof"
};

const char* keywords[] = {
    "const", "int", "void", "if", "else",
    "while", "break", "continue", "return"
};

int is_keyword(const char* str) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

TokenType scan_token(const char* src, int* offset, char* lexeme) {
    int i = *offset;
    while (isspace(src[i])) {
        if (src[i] == '\n') {
            lexeme[0] = '\n'; lexeme[1] = '\0';
            *offset = i + 1;
            return TOKEN_UNKNOWN;
        }
        i++;
    }

    if (src[i] == '\0') return TOKEN_EOF;

    int start = i;

    if (isalpha(src[i]) || src[i] == '_') {
        while (isalnum(src[i]) || src[i] == '_') i++;
        strncpy(lexeme, src + start, i - start);
        lexeme[i - start] = '\0';
        *offset = i;
        return is_keyword(lexeme) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    }

    if (isdigit(src[i])) {
        while (isalnum(src[i])) i++;
        strncpy(lexeme, src + start, i - start);
        lexeme[i - start] = '\0';
        *offset = i;
        return TOKEN_NUMBER;
    }

    if (src[i] == '/' && src[i + 1] == '/') {
        while (src[i] != '\0' && src[i] != '\n') i++;
        strncpy(lexeme, src + start, i - start);
        lexeme[i - start] = '\0';
        *offset = i;
        return TOKEN_COMMENT;
    }

    char ch = src[i];
    lexeme[0] = ch;
    lexeme[1] = '\0';
    *offset = i + 1;
    if (strchr("+-*/%=<>&|!", ch)) return TOKEN_OPERATOR;
    if (strchr(";,(){}[]", ch)) return TOKEN_SEPARATOR;

    return TOKEN_UNKNOWN;
}

void highlight_token(FILE* out, TokenType type, const char* lexeme) {
    if (strcmp(lexeme, "\n") == 0) {
        fprintf(out, "</span>\n<span class=\"line\">    ");
    } else {
        fprintf(out, "<span class=\"token-%s\">%s</span>", token_type_str[type], lexeme);
    }
}

void highlight_code(FILE* in, FILE* out) {
    char line[MAX_LINE_LEN];
    fprintf(out, "<span class=\"line\">    ");
    while (fgets(line, sizeof(line), in)) {
        int offset = 0;
        char lexeme[MAX_TOKEN_LEN];
        TokenType type;
        while ((type = scan_token(line, &offset, lexeme)) != TOKEN_EOF) {
            if (type == TOKEN_COMMENT) {
                fprintf(out, "<span class=\"token-comment\"><em>%s</em></span>", lexeme);
            } else {
                highlight_token(out, type, lexeme);
                fprintf(out, " ");
            }
        }
        fprintf(out, "</span>\n<span class=\"line\">    ");
    }
    fprintf(out, "</span>\n");
}

int main() {
    FILE* in = fopen("code/test2.sy", "r");
    FILE* out = fopen("output/test2-c.html", "w");

    if (!in || !out) {
        perror("文件打开失败");
        return 1;
    }

    fprintf(out, "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">");
    fprintf(out, "<style>\n");
    fprintf(out, "body { background: #f8f8f8; font-family: 'Courier New', monospace; padding: 1em; }\n");
    fprintf(out, ".line { display: block; line-height: 1.2; white-space: pre; }\n");
    fprintf(out, ".token-keyword { color: blue; font-weight: bold; }\n");
    fprintf(out, ".token-identifier { color: black; }\n");
    fprintf(out, ".token-number { color: darkorange; }\n");
    fprintf(out, ".token-operator { color: darkred; }\n");
    fprintf(out, ".token-separator { color: teal; }\n");
    fprintf(out, ".token-comment { color: green; font-style: italic; }\n");
    fprintf(out, "</style></head><body><pre><code>\n");

    highlight_code(in, out);

    fprintf(out, "</code></pre></body></html>\n");

    fclose(in);
    fclose(out);
    return 0;
}
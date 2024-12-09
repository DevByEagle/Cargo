#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

// Type Definitions
typedef long long integer_t;

typedef struct Error {
    enum ErrorType {
        ERROR_NONE = 0,
        ERROR_ARGUMENTS,
        ERROR_TYPE,
        ERROR_GENERIC,
        ERROR_SYNTAX,
        ERROR_TODO,
        ERROR_MAX,
    } type;
    char* msg;
} Error;

typedef struct Node {
    enum NodeType {
        NODE_TYPE_NONE,
        NODE_TYPE_INTEGER,
        NODE_TYPE_PROGRAM,
        NODE_TYPE_MAX,
    } type;
    union NodeValue {
        integer_t integer;
    } value;
    struct Node* children;
    struct Node* next_child;
} Node;

typedef struct Program {
    Node* root;
} Program;

typedef struct Binding {
    char* id;
    Node* value;
    struct Binding* next;
} Binding;

typedef struct Environment {
    struct Environment* parent;
    Binding* bind;
} Environment;

typedef struct Token {
    char* beginning;
    char* end;
    struct Token* next;
} Token;

// Constants and Macros
#define nonep(node)  ((node).type == NODE_TYPE_NONE)
#define integerp(node) ((node).type == NODE_TYPE_INTEGER)
#define ERROR_CREATE(n, t, msg) Error (n) = { (t), (msg) }
#define ERROR_PREP(n, t, message) \
    (n).type = (t);               \
    (n).msg = (message)

Error ok = { ERROR_NONE, NULL };
const char* whitespace = " \r\n";
const char* delimiters = " \r\n,():";

// File Operations
long file_size(FILE* file) {
    if (!file) return 0;
    fpos_t original = {0};
    if (fgetpos(file, &original) != 0) {
        printf("fgetpos() failed: %i\n", errno);
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if (fsetpos(file, &original) != 0) {
        printf("fsetpos() failed: %i\n", errno);
    }
    return size;
}

char* file_contents(char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Could not open file at %s\n", path);
        return NULL;
    }

    long size = file_size(file);
    char* contents = malloc(size + 1);
    char* write_it = contents;
    size_t bytes_read = 0;

    while (bytes_read < size) {
        size_t bytes_read_this_iteration = fread(write_it, 1, size - bytes_read, file);
        if (ferror(file)) {
            printf("Error while reading: %i\n", errno);
            free(contents);
            return NULL;
        }

        bytes_read += bytes_read_this_iteration;
        write_it += bytes_read_this_iteration;

        if (feof(file)) break;
    }
    contents[bytes_read] = '\0';
    fclose(file);
    return contents;
}

// Error Handling
void print_error(Error err) {
    if (err.type == ERROR_NONE) return;

    printf("ERROR: ");
    assert(ERROR_MAX == 6);

    switch (err.type) {
        case ERROR_TODO: printf("TODO (not implemented)"); break;
        case ERROR_SYNTAX: printf("Invalid syntax"); break;
        case ERROR_TYPE: printf("Mismatched types"); break;
        case ERROR_ARGUMENTS: printf("Invalid arguments"); break;
        case ERROR_GENERIC: break;
        default: printf("Unknown error type...");
    }

    putchar('\n');
    if (err.msg) {
        printf(": %s\n", err.msg);
    }
}

// Lexical Analysis
Error lex(char* source, Token* token) {
    Error err = ok;
    if (!source || !token) {
        ERROR_PREP(err, ERROR_ARGUMENTS, "Cannot lex empty source.");
        return err;
    }

    token->beginning = source;
    token->beginning += strspn(source, whitespace);
    token->end = token->beginning;

    if (*token->end == '\0') return err;

    token->end += strcspn(token->beginning, delimiters);
    if (token->end == token->beginning) {
        token->end += 1;
    }

    return err;
}

Token* token_create() {
    Token* token = malloc(sizeof(Token));
    assert(token && "Could not allocate memory for token");
    memset(token, 0, sizeof(Token));
    return token;
}

void tokens_free(Token* root) {
    while (root) {
        Token* token_to_free = root;
        root = root->next;
        free(token_to_free);
    }
}

void print_tokens(Token* root) {
    size_t count = 0;
    while (root) {
        if (count > 10000) { break; }
        printf("Token %ld: ", count);
        if (root->beginning && root->end) {
            printf("%.*s", (int)(root->end - root->beginning), root->beginning);
        }
        putchar('\n');
        root = root->next;
        count++;
    }
}

int token_string_equalp(char* string, Token* token) {
    if (!string || !token) return 0;
    char* beg = token->beginning;
    while (*string && beg < token->end) {
        if (*string != *beg) {
            return 0;
        }
        string++;
        beg++;
    }
    return 1;
}

// Node Management
void node_free(Node* root) {
    if (!root) return;
    Node* child = root->children;
    while (child) {
        Node* next_child = child->next_child;
        node_free(child);
        child = next_child;
    }
    free(root);
}

// Parsing
Error parse_expr(char* source, Node* result) {
    Token current_token = {0};
    Error err = ok;

    Node* root = calloc(1, sizeof(Node));
    assert(root && "Could not allocate memory for AST root.");

    Node working_node;
    working_node.children = NULL;
    working_node.next_child = NULL;
    working_node.type = NODE_TYPE_NONE;
    working_node.value.integer = 0;
    while ((err = lex(current_token.end, &current_token)).type == ERROR_NONE) {
        if (current_token.end - current_token.beginning == 0) break;
        if (token_string_equalp(":", &current_token)) {
            Token equals;
        }
    }
    
    return err;
}

// Utility
void print_usage(char** argv) {
    printf("USAGE: %s <path_to_file_to_compile>\n", argv[0]);
}

// Main Function
int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv);
        return 0;
    }

    char* path = argv[1];
    char* contents = file_contents(path);
    if (contents) {
        Node expression = {0};
        Error err = parse_expr(contents, &expression);
        print_error(err);
        free(contents);
    }

    return 0;
}
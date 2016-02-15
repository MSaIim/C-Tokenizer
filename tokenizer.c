#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <regex.h>

struct TokenizerT_
{
    // Original holds full string
    // token holds each token on a separate row
    // temp holds characters for testing
    char *original, **token, *temp;
    int prevPos, row;
};
typedef struct TokenizerT_ TokenizerT;

// Prototypes
TokenizerT *TKCreate(int, char **);
void TKDestroy(TokenizerT *);
char **TKGetNextToken(TokenizerT *);
int regeval(char *, char *);
void getPunct(char *);

int main(int sep, char **argv)
{
    // Create pointer to a token and assign it a string
    TokenizerT *mrToken = TKCreate(sep, argv);
    char **str;

    printf("\n");
    while (mrToken->original[mrToken->prevPos] != '\0')
    {
        str = TKGetNextToken(mrToken);

        // ERRORS
        if (regeval(str[mrToken->row], "^0[xX][g-zG-Z]$"))
            printf(" ERROR\t\t\t\"%s\"\n", str[mrToken->row]);
        else if (regeval(str[mrToken->row], "^[-]?[0-9]+[.][a-zA-Z]$"))
            printf(" ERROR\t\t\t\"%s\"\n", str[mrToken->row]);
        // ESCAPE (Print in Hex)
        else if(regeval(str[mrToken->row], "^[\\][bfnrt]?$"))
        {
            int i = 0;
            printf(" ESCAPE\t\t\t\"[0x");
            while(str[mrToken->row][i])
                printf("%x", str[mrToken->row][i++]);
            printf("]\"\n");
        }
        // PUNCTUATION/OPERATORS
        else if(regeval(str[mrToken->row], "^[[:punct:]]+$"))
        {
            getPunct(str[mrToken->row]);
            printf("\t\t\"%s\"\n", str[mrToken->row]);
        }
        // FLOAT
        else if(regeval(str[mrToken->row], "^[-]?[0-9]+[.][0-9]+([Ee][-]?[0-9]+([.][0-9]+)?)?$"))
            printf(" Float\t\t\t\"%s\"\n", str[mrToken->row]);
        // OCTAL
        else if(regeval(str[mrToken->row], "^0[0-7]+$"))
            printf(" Octal\t\t\t\"%s\"\n", str[mrToken->row]);
        // INTEGER
        else if(regeval(str[mrToken->row], "^[-]?[0-9]+$"))
            printf(" Integer\t\t\"%s\"\n", str[mrToken->row]);
        // WORD
        else if(regeval(str[mrToken->row], "^[a-zA-Z][a-zA-Z0-9]*$"))
            printf(" Word\t\t\t\"%s\"\n", str[mrToken->row]);
        // HEX
        else if(regeval(str[mrToken->row], "^(0[xX])[a-fA-F0-9]+$"))
            printf(" Hex\t\t\t\"%s\"\n", str[mrToken->row]);

        // Increase row
        mrToken->row = mrToken->row + 1;
    }
    printf("\n");

    // Free memory
    TKDestroy(mrToken);

    return 0;
}

// Create a TokenizerT object from the given string
TokenizerT *TKCreate(int sep, char **argv)
{
    int i, size = sep;
    TokenizerT *mrToken = malloc(sizeof(TokenizerT));

    // Get the inputted string length and allocate memory
    for (i = 1; i < sep; i++)
        size += strlen(argv[i]);
    mrToken->original = malloc(size);

    //Copy string into array
    for (i = 1; i < sep; i++)
    {
        strcat(mrToken->original, argv[i]);
        mrToken->original[strlen(mrToken->original)+1] = '\0';
        mrToken->original[strlen(mrToken->original)] = ' ';
    }

    // Assign number of rows equal to the number of white spaces
    mrToken->token = malloc(sizeof(char*)*(size + sep));
    mrToken->temp = malloc(size + sep);
    return mrToken;
}

void TKDestroy(TokenizerT *mrToken)
{
    int i;

    // Free memory
    free(mrToken->original);
    free(mrToken->temp);

    // Free row
    for(i = 0; i < mrToken->row; i++)
        free(mrToken->token[i]);
    free(mrToken->token);

    // Free struct
    free(mrToken);
}

char **TKGetNextToken(TokenizerT *mrToken)
{
    int i = 0, j = 0, flag = 0;
    int pos = mrToken->prevPos, row = mrToken->row;

    // Allocate memory for columns
    mrToken->token[mrToken->row] = malloc(100);

    // Skip white spaces
    while(isspace(mrToken->original[pos]))
        pos++;

    // Check tokens for matches, skip white space
    while (mrToken->original[pos] != ' ')
    {
        // Append character to temp array
        mrToken->temp[j++] = mrToken->original[pos];

        // SPECIAL (ADD SINGLE QUOTE!!)
        if(regeval(mrToken->temp, "^[\\][bfnrt]?$"))
            mrToken->token[row][i++] = mrToken->original[pos++];
        // PUNCTUATION & OPERATORS
        else if(regeval(mrToken->temp, "^[[:punct:]]+$") && flag == 0)
        {
            flag = -1;
            mrToken->token[row][i++] = mrToken->original[pos++];
        }
        // FLOAT
        else if(regeval(mrToken->temp, "^[-]?[0-9]+[.][0-9]+([Ee][-]?[0-9]+([.][0-9]+)?)?$"))
            mrToken->token[row][i++] = mrToken->original[pos++];
        else if(regeval(mrToken->temp, "^[-]?[0-9]+[.][0-9]+([Ee][-]?[0-9]+[.])?$"))
            mrToken->token[row][i++] = mrToken->original[pos++];
        else if(regeval(mrToken->temp, "^[-]?[0-9]+[.][0-9]+([Ee][-]?)$"))
            mrToken->token[row][i++] = mrToken->original[pos++];
        else if(regeval(mrToken->temp, "^[-]?[0-9]+[.][a-zA-Z]$"))
            mrToken->token[row][i++] = mrToken->original[pos++];
        else if(regeval(mrToken->temp, "^[-]?[0-9]+[.]$"))
            mrToken->token[row][i++] = mrToken->original[pos++];
        // HEXADECIMAL
        else if(regeval(mrToken->temp, "^(0[xX])[a-fA-F0-9]+$"))
            mrToken->token[row][i++] = mrToken->original[pos++];
        else if(regeval(mrToken->temp, "^0[xX][g-zG-Z]$"))
            mrToken->token[row][i++] = mrToken->original[pos++];
        else if(regeval(mrToken->temp, "^0[xX]$"))
            mrToken->token[row][i++] = mrToken->original[pos++];
        // OCTAL
        else if(regeval(mrToken->temp, "^0[0-7]+$"))
        {
            flag = 3;
            mrToken->token[row][i++] = mrToken->original[pos++];
        }
        // LETTER
        else if(regeval(mrToken->temp, "^[a-zA-Z][a-zA-Z0-9]*$"))
            mrToken->token[row][i++] = mrToken->original[pos++];
        // DIGIT
        else if(regeval(mrToken->temp, "^[-]?[0-9]+$") && flag != 3)
            mrToken->token[row][i++] = mrToken->original[pos++];
        // BASE CASE
        else
            break;
    }

    // Add nullbyte at end of row and save position
    mrToken->token[row][i] = '\0';
    mrToken->prevPos = pos;
    
    // Clear temp array
    memset(mrToken->temp, 0, sizeof(mrToken->temp));
    return mrToken->token;
}

int regeval(char *str, char *exp)
{
    // Make regex "object"
    regex_t regex;
    
    // Compile and execute expression
    int regi = regcomp(&regex, exp, REG_EXTENDED);
    regi = regexec(&regex, str, 0, NULL, 0);
    
    // Expressions matches token
    if (!regi)
       return 1;

    return 0;
}

// Get word for punctuation/operators
void getPunct(char *str)
{
    switch(*str)
    {
        case '+':
            printf(" Plus\t");
            break;
        case '-':
            printf(" Minus\t");
            break;
        case '>':
            printf(" Greater Than");
            break;
        case '<':
            printf(" Less Than");
            break;
        case '=':
            printf(" Equals\t");
            break;
        case '!':
            printf(" Exclamation");
            break;
        case '"':
            printf(" Quotation");
            break;
        case '#':
            printf(" Octothorp");
            break;
        case '$':
            printf(" Dollar\t");
            break;
        case '%':
            printf(" Percent");
            break;
        case '&':
            printf(" Ampersand");
            break;
        case '(':
            printf(" Left Paren");
            break;
        case ')':
            printf(" Right Paren");
            break;
        case '*':
            printf(" Star\t");
            break;
        case ',':
            printf(" Comma\t");
            break;
        case '.':
            printf(" Period\t");
            break;
        case ':':
            printf(" Colon\t");
            break;
        case ';':
            printf(" Semi-Colon");
            break;
        case '?':
            printf(" Question");
            break;
        case '@':
            printf(" At Sign");
            break;
        case '[':
            printf(" Left Bracket");
            break;
        case ']':
            printf(" Right Bracket");
            break;
        case '^':
            printf(" Caret\t");
            break;
        case '_':
            printf(" Underscore");
            break;
        case '{':
            printf(" Left Brace");
            break;
        case '}':
            printf(" Right Brace");
            break;
        case '|':
            printf(" Pipe\t");
            break;
        case '~':
            printf(" Tilde\t");
            break;
        case '/':
            printf(" Forward Slash");
            break;
    }
}
 

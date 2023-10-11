#include <stdio.h>
#include <string.h>

/* Rob Pike's Regular Expression Matcher */
int match(char *regexp, char *text);
int matchhere(char *regexp, char *text);
int matchstar(int c, char *regexp, char *text);
int matchquestion(char c, char *regexp, char *text);

// Function to read two lines from the file, removing the newline character if it exists
int readFromFile(const char *filename, char *line1, size_t size1, char *line2, size_t size2)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return 0;
    }

    // Read the first line from the file
    if (!fgets(line1, size1, fp))
    {
        printf("Error reading the first line from the file\n");
        fclose(fp);
        return 0;
    }
    // Remove the newline character, if it exists at the end of the first line
    size_t len1 = strlen(line1);
    if (len1 > 0 && line1[len1 - 1] == '\n')
        line1[len1 - 1] = '\0';

    // Read the second line from the file
    if (!fgets(line2, size2, fp))
    {
        printf("Error reading the second line from the file\n");
        fclose(fp);
        return 0;
    }
    // Remove the newline character, if it exists at the end of the second line
    size_t len2 = strlen(line2);
    if (len2 > 0 && line2[len2 - 1] == '\n')
        line2[len2 - 1] = '\0';

    fclose(fp);
    return 1; // Both lines were successfully read
}

int main(int argc, char *argv[])
{
    char regexp[100], text[100];

    // Check if the user has provided input file name
    if (argc != 2)
    {
        printf("Usage: ./a.out filename\n");
        return 1;
    }

    // Read the two lines from the file
    if (!readFromFile(argv[1], regexp, sizeof(regexp), text, sizeof(text)))
    {
        //Error already handled in the function
        return 1;
    }

    // // Print regexp and text
    // printf("regexp = %s\n", regexp);
    // printf("string = %s\n", text);

    // Perform regular expression matching
    if (match(regexp, text))
    {
        printf("\n");
    }
    else
    {
        printf("no match\n");
    }

    return 0;
}

/* Rob Pike's Regular Expression Matcher with multi-position capabilities*/

/* match: search for regexp anywhere in text and print the positions */
int match(char *regexp, char *text)
{
    int found = 0;  // Flag to indicate if a match is found
    int position = 0;  // Position counter

    if (regexp[0] == '^')
        return matchhere(regexp + 1, text);

    do {
        if (matchhere(regexp, text)) {
            if (found) {
                printf(" ");
            } else {
                found = 1;  // Set the found flag
                printf("match ");
            }
            printf("%d", position);
        }
        position++;
    } while (*text++ != '\0');

    return found;
}

/* matchhere: search for regexp at the beginning of text */
int matchhere(char *regexp, char *text)
{
    if (regexp[0] == '\0')
        return 1;
    if (regexp[1] == '?') 
        return matchquestion(regexp[0], regexp + 2, text);
    if (regexp[1] == '*')
        return matchstar(regexp[0], regexp + 2, text);
    if (regexp[0] == '$' && regexp[1] == '\0')
        return *text == '\0';
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
        return matchhere(regexp + 1, text + 1);
    return 0;
}

/* matchstar: search for c*regexp at the beginning of text */
int matchstar(int c, char *regexp, char *text)
{
    do {    /* a * matches zero or more instances */
        if (matchhere(regexp, text))
            return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}

/* matchquestion: search for c?regexp at the beginning of text */
int matchquestion(char c, char *regexp, char *text)
{
    // Try to match without the previous character first.
    if (matchhere(regexp, text))
        return 1;
    // If that fails, try to match with the previous character.
    if (*text != '\0' && (c == '.' || c == *text))
        return matchhere(regexp, text + 1);
    return 0;
}

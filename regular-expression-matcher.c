/*
 * This program implements Rob Pike's Regular Expression Matcher with multi-position capabilities and extra meta-characters.
 * The program reads two lines from a file specified by the user, removes the newline character if it exists,
 * and performs regular expression matching on the two lines. The program prints the positions of the matches
 * found in the second line. The regular expression syntax supported by the program includes:
 * - '.' to match any single character
 * - '*' to match zero or more occurrences of the previous character
 * - '?' to match zero or one occurrence of the previous character
 * - '+' to match one or more occurrences of the previous character
 * - '[]' to match any character in the set
 * - '^' to negate the set
 * - '\' to escape special characters
 * The program consists of the following functions:
 * - match: search for regexp anywhere in text and print the positions
 * - matchhere: search for regexp at the beginning of text
 * - matchstar: search for c*regexp at the beginning of text
 * - matchquestion: search for c?regexp at the beginning of text
 * - matchset: search for a character in the set
 * - matchplus: search for c+regexp at the beginning of text
 * - readFromFile: read two lines from a file and remove the newline character if it exists
 */

#include <stdio.h>
#include <string.h>

/* Rob Pike's Regular Expression Matcher */
int match(char *regexp, char *text);
int matchhere(char *regexp, char *text);
int matchstar(int c, char *regexp, char *text);
int matchquestion(char c, char *regexp, char *text);
int matchset(char c, char *regexp, char *text);
int matchplus(int c, char *regexp, char *text);

// Function to read two lines from the file, removing the newline character if it exists
int readFromFile(const char *filename, char *line1, size_t size1, char *line2, size_t size2) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return 0;
    }

    // Read the first line from the file
    if (!fgets(line1, size1, fp)) {
        printf("Error reading the first line from the file\n");
        fclose(fp);
        return 0;
    }
    // Remove the newline character, if it exists at the end of the first line
    size_t len1 = strlen(line1);
    if (len1 > 0 && line1[len1 - 1] == '\n')
        line1[len1 - 1] = '\0';

    // Read the second line from the file
    if (!fgets(line2, size2, fp)) {
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

int main(int argc, char *argv[]) {
    char regexp[100], text[100];

    // Check if the user has provided input file name
    if (argc != 2) {
        printf("Usage: ./a.out filename\n");
        return 1;
    }

    // Read the two lines from the file
    if (!readFromFile(argv[1], regexp, sizeof(regexp), text, sizeof(text))) {
        // Error already handled in the function
        return 1;
    }

    // Perform regular expression matching
    if (match(regexp, text)) {
        printf("\n");
    } else {
        printf("no match\n");
    }

    return 0;
}

/* Rob Pike's Regular Expression Matcher with multi-position capabilities and extra meta-characters */

/* match: search for regexp anywhere in text and print the positions */
int match(char *regexp, char *text) {
    int found = 0;  // Flag to indicate if a match is found
    int position = 0;  // Position counter
    int longest_match = 0; // Store the length of the longest match
    int is_combine = 1; // Flag to indicate printing case depending on combination of regex

    // Different printing mechanism depending on combination
    for (int i = 0; regexp[i] != '\0'; i++) {
        if (regexp[i] == '?' || regexp[i] == '+') {
            is_combine = 0;
            break;
        }
    }

    if (regexp[0] == '^')
        if (matchhere(regexp + 1, text)) {
            found = 1;
            printf("match 0");
        }

    do {
        if (matchhere(regexp, text)) {
            if (found) {
                printf(" ");
            } else {
                found = 1; // Set the found flag
                printf("match ");
            }
            printf("%d", position);
            longest_match = 1; // Set the longest_match flag

            if (!is_combine) {
                // Skip to the end of the current match to avoid overlaps
                while (text[longest_match] != '\0' && matchhere(regexp, text + longest_match)) {
                    longest_match++;
                }
            }
            else {
                // Skip to the end of the current match to avoid overlaps
                while (text[longest_match] != '\0' && matchhere(regexp, text + longest_match) && text[0] == text[longest_match]) {
                    longest_match++;
                }
            }
            text += longest_match;
            position += longest_match;
        } else {
            text++;
            position++;
        }
    } while (*text != '\0');
    return found;
}

/* matchhere: search for regexp at the beginning of text */
int matchhere(char *regexp, char *text) {
    if (regexp[0] == '\0')
        return 1;
    if (regexp[1] == '?')
        return matchquestion(regexp[0], regexp + 2, text);
    if (regexp[1] == '*')
        return matchstar(regexp[0], regexp + 2, text);
    if (regexp[1] == '+')
        return matchplus(regexp[0], regexp + 2, text);
    if (regexp[0] == '$' && regexp[1] == '\0')
        return *text == '\0';
    if (regexp[0] == '\\') {
        // Handle escape character
        if (regexp[1] == text[0]) {
            return matchhere(regexp + 2, text + 1);
        } else {
            return 0; // Mismatched escape sequence
        }
    }
    if (*text != '\0') {
        if (regexp[0] == '.') {
            return matchhere(regexp + 1, text + 1);
        } else if (regexp[0] == '[') {
            // Handle character sets
            return matchset(text[0], regexp, text);
        } else {
            if (regexp[0] == text[0]) {
                return matchhere(regexp + 1, text + 1);
            }
        }
    }
    return 0;
}

/* matchstar: search for c*regexp at the beginning of text */
int matchstar(int c, char *regexp, char *text) {
    while (*text != '\0' && (*text == c || c == '.')) {
        if (matchhere(regexp, text)) {
            return 1;
        }
        text++;
    }
    return matchhere(regexp, text);
}

/* matchquestion: search for c?regexp at the beginning of text */
int matchquestion(char c, char *regexp, char *text) {
    // Try to match without the previous character first.
    if (matchhere(regexp, text))
        return 1;
    // If that fails, try to match with the previous character.
    if (*text != '\0' && (c == '.' || c == *text))
        return matchhere(regexp, text + 1);
    return 0;
}

/* matchset: search for a character in the set */
/* matchset: search for a character in the set */
int matchset(char c, char *regexp, char *text) {
    int negate_set = 0;
    int literal_follows = 0; // Flag to indicate if a literal character follows the set
    int found = 0; // Flag to indicate if a match is found

    regexp++; // Skip '['

    if (regexp[0] == '^') {
        negate_set = 1;
        regexp++;
    }

    //check if literal or regex character follows the set
    int incremented = 0;
    while (regexp[0] != '\0' && regexp[0] != ']') {
        if (regexp[1] != '\0' && regexp[1] == ']') {
            if (regexp[2] == '\0' || regexp[2] == '[')
                literal_follows = 0;
            else
                literal_follows = 1;
            break;
        }
        regexp++;
        incremented++;
    }
    char *new_regexp = regexp+2;
    regexp -= incremented;

    while (regexp[0] != '\0' && regexp[0] != ']') {
        if (regexp[1] == '-' && regexp[2] >= regexp[0]) {
            if ((c >= regexp[0] && c <= regexp[2]) ||
                (negate_set && (c < regexp[0] || c > regexp[2]))) {
                found = 1; // Match found
            }
            regexp += 3; // Skip range and dash
        } else {
            if ((c == regexp[0]) ||
                (negate_set && c != regexp[0])) {
                found = 1; // Match found
            }
            regexp++;
        }
    }

    //check if new_regexp matches the next character in text
    if (found && literal_follows) {
        if (new_regexp[0] == '?' || new_regexp[0] == '*' || new_regexp[0] == '+')
            return matchhere(new_regexp-1, text);
        else
            return matchhere(new_regexp, text + 1);
    }

    if (literal_follows && new_regexp[0] == '*')
        return matchhere(new_regexp-1, text);

    return found; // No match in the character set
}

/* matchplus: search for c+regexp at the beginning of text */
int matchplus(int c, char *regexp, char *text) {
    if (*text == c) {
        return matchstar(c, regexp, text + 1);
    }
    return 0;
}
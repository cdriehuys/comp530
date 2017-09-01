/* COMP 530 - Assignment 1
 * Chathan Driehuys
 *
 * UNC Honor Pledge: I certify that no unauthorized assistance has been
 * received or given in the completion of this work.
 */

#define LINE_LENGTH 80

#include <stdio.h>


int main() {
    char buffer[LINE_LENGTH + 1];
    buffer[LINE_LENGTH] = '\0';

    int c;
    int index = 0;

    while (1) {
        /* Read character and exit if it's EOF */
        c = fgetc(stdin);

        if (c == EOF) {
            break;
        }

        /* Replace newline with a space */
        if (c == '\n') {
            c = ' ';
        }

        /* Replace double asterisk with a caret */
        if (c == '*') {
            if (index > 0 && buffer[index - 1] == '*') {
                index--;
                c = '^';
            }
        }

        /* Save character to buffer */
        buffer[index] = c;

        /* Output buffer if we've reached the line length */
        if (index == LINE_LENGTH - 1) {
            /* First look ahead to see if the next char is an asterisk */
            int next_char = fgetc(stdin);

            if (buffer[index] == '*' && next_char == '*') {
                /* We received two asterisks that should be collapsed. */
                buffer[index] = '^';
                printf("%s\n", buffer);
                index = 0;
            } else {
                /* The next character doesn't need to be collpsed, it should
                 * start the next buffer.
                 */
                printf("%s\n", buffer);

                if (next_char == EOF) {
                    break;
                } else if (next_char == '\n') {
                    next_char = ' ';
                }

                buffer[0] = next_char;
                index = 1;
            }
        } else {
            index++;
        }
    }

    return 0;
}

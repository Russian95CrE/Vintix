#pragma once

int isupper (int c) {
    return c >= 'A' && c <= 'Z';
}

int islower (int c) {
    return c >= 'a' && c <= 'z';
}

int isalpha (int c) {
    return isupper(c) || islower(c);
}

int isdigit (int c) {
    return c >= '0' && c <= '9';
}

int isxdigit (int c) {
    return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

int isalnum (int c) {
    return isalpha(c) || isdigit(c);
}

int isspace (int c) {
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
           c == '\v';
}

int isblank (int c) {
    return c == ' ' || c == '\t';
}

int iscntrl (int c) {
    return (c >= 0 && c < 0x20) || c == 0x7F;
}

int isgraph (int c) {
    return c > 0x20 && c < 0x7F;
}

int isprint (int c) {
    return c >= 0x20 && c < 0x7F;
}

int ispunct (int c) {
    return isprint(c) && !isalnum(c) && c != ' ';
}

int tolower (int c) {
    return isupper(c) ? (c - 'A' + 'a') : c;
}

int toupper (int c) {
    return islower(c) ? (c - 'a' + 'A') : c;
}

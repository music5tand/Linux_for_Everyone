/*
    grep3.c -- grep command without line length limitation

    Copyright (c) 2017 Minero Aoki

    This program is free software.
    Redistribution and use in source and binary forms,
    with or without modification, are permitted.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

static void do_grep(regex_t *pat, FILE *f);
static char* get_line(FILE *f);
static void die(const char *s);

int
main(int argc, char *argv[])
{
    regex_t pat;
    int err;
    int i;

    if (argc < 2) {
        fputs("no pattern\n", stderr);
        exit(1);
    }
    err = regcomp(&pat, argv[1], REG_EXTENDED | REG_NOSUB | REG_NEWLINE);
    if (err != 0) {
        char buf[1024];

        regerror(err, &pat, buf, sizeof buf);
        puts(buf);
        exit(1);
    }
    if (argc == 2) {
        do_grep(&pat, stdin);
    }
    else {
        for (i = 2; i < argc; i++) {
            FILE *f = fopen(argv[i], "r");
            if (!f) die(argv[i]);
            do_grep(&pat, f);
            fclose(f);
        }
    }
    regfree(&pat);
    exit(0);
}

static void
do_grep(regex_t *pat, FILE *src)
{
    char *line;

    while (line = get_line(src)) {
        if (regexec(pat, line, 0, NULL, 0) == 0) {
            puts(line);
        }
        free(line);
    }
}

static char*
get_line(FILE *src)
{
    unsigned char *line;   /* 버퍼 */
    size_t capa = 160;     /* 버퍼의 크기 */
    size_t idx = 0;        /* 현재 버퍼에 쓰는 위치 */
    int c;

    line = malloc(capa);
    if (!line) die("malloc");
    while ((c = getc(src)) != EOF) {
        if (c == '\n')
            break;
        if (idx == capa - 1) {   /* 버퍼의 길이 체크 */
            capa *= 2;
            line = realloc(line, capa);
            if (!line) die("realloc");
        }
        line[idx++] = (unsigned char)c;
    }
    if ((idx == 0) && (c == EOF)) {
        free(line);
        return NULL;
    }
    line[idx++] = '\0';

    return (char*)line;
}

static void
die(const char *s)
{
    perror(s);
    exit(1);
}

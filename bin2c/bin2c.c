#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pygetopt.h"

/*
 *  0.1.1       generate source(.c) with header(.h)
 *  0.1.2       fix toolkit help with example.bin.h
 *  2016.09.13  add parameter support, rewrite all.
 */
#define VERSION     "2016.09.13"

const char* info_size = "  extern const int %s_length;\n";
const char* info_data = "  extern const unsigned char %s_buffer[%d];\n";
const char* h_size = "extern const int %s_length;\n";
const char* h_data = "extern const unsigned char %s_buffer[%d];\n";
const char* c_size = "const int %s_length = %d;\n\n";
const char* c_data = "const unsigned char %s_buffer[%d] = {\n";
const char* c_tail = "};\n";

#define DEFAULT_PREFIX "bin2c_array"
#define DEFAULT_OUTFN  "bin2c_array.c"


int file_read_binary(const char* fn, char** pbuf)
{
    FILE* fp;
    char* buf;
    int size;

    if ((fp = fopen(fn, "rb")) == NULL) {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size == 0) {
        fclose(fp);
        return 0;
    }

    if (pbuf != NULL) {
        buf = (char*)malloc(size);
        fread(buf, 1, size, fp);
        *pbuf = buf;
    }

    fclose(fp);

    return size;
}

int bin2c(const char* infn, const char* prefix, const char* outfn, int verbose)
{
    int size;
    char* ptr;
    char* fn;
    char* p;
    FILE* fp;
    int n;
    int cols;
    int j;
    int i;
    const int N_COLS = 8;

    size = file_read_binary(infn, &ptr);
    if (size <= 0) {
        return -1;
    }

    n = strlen(outfn);
    fn = (char*)malloc(n + 3);
    memset(fn, 0, n + 3);
    memcpy(fn, outfn, n);

    p = strrchr(fn, '.');
    if ((p != NULL) && (p - fn < n - 4)) {
        p = NULL;
    }
    if (p == NULL) {
        p = fn + n;
    }

    if (verbose) {
        fprintf(stderr, "declare:\n");
        fprintf(stderr, info_size, prefix, size);
        fprintf(stderr, info_data, prefix, size);
        fprintf(stderr, "\n");
    }

    memcpy(p, ".h\0", 3);
    if ((fp = fopen(fn, "w")) == NULL) {
        goto L_ERROR;
    }
    if (verbose) {
        fprintf(stderr, "info: wrote to \"%s\"\n", fn);
    }
    fprintf(fp, h_size, prefix);
    fprintf(fp, h_data, prefix, size);
    fprintf(fp, "\n");
    fclose(fp);

    memcpy(p, ".c\0", 3);
    if ((fp = fopen(fn, "w")) == NULL) {
        goto L_ERROR;
    }
    if (verbose) {
        fprintf(stderr, "info: wrote to \"%s\"\n", fn);
    }
    fprintf(fp, c_size, prefix, size);
    fprintf(fp, c_data, prefix, size);

    for (i = 0;i < size;i+=N_COLS) {
        cols = i + N_COLS;
        if (cols > size) {
            cols = size;
        }
        fprintf(fp, "    ");
        for (j = i;j < cols;j++) {
            fprintf(fp, "0x%02X, ", ptr[j] & 0xFFu);
        }
        for (;j < i + N_COLS;j++) {
            fprintf(fp, "      ");
        }
        fprintf(fp, "  //%08x ", i);
        for (j = i;j < cols;j++) {
            if (ptr[j] == '\\') {
                fprintf(fp, ".");
            } else if (isprint(ptr[j]) && !iscntrl(ptr[j])) {
                fprintf(fp, "%c", ptr[j]);
            } else {
                fprintf(fp, ".");
            }
        }
        fprintf(fp, "  \n");
    }
    fprintf(fp, c_tail);
    fprintf(fp, "\n");

    free(fn);
    free(ptr);

    return 0;

L_ERROR:
    free(fn);
    free(ptr);

    return -1;
}

void print_usage(const char* prog)
{
    printf("Version: %s\n", VERSION);
    printf("Build  : %s %s\n", __DATE__, __TIME__);
    printf("Usage  : %s [option] file\n", prog);
    printf("  %-26s %s\n",  "-o, --output", "output file name, default " DEFAULT_OUTFN);
    printf("  %-26s %s\n",  "--prefix", "prefix of c array, default " DEFAULT_PREFIX);
    printf("  %-26s %s\n",  "--verbose", "show more information");
}

int main(int argc, char* argv[])
{
    pygetopt_t* cfg;
    const char* lfmt[] = {
        "help",
        "prefix=",
        "output=",
        "verbose",
        NULL,
    };
    const char* prefix;
    const char* outfn;
    int verbose = 0;
    int ret;

    cfg = pygetopt_parse(argc-1, argv+1, "ho:", lfmt);
    if (cfg == NULL) {
        goto L_ERROR;
    }

    if (pygetopt_key_exists(cfg, "h")) {
        goto L_ERROR1;
    }
    if (pygetopt_key_exists(cfg, "help")) {
        goto L_ERROR1;
    }

    if (pygetopt_key_exists(cfg, "verbose")) {
        verbose = 1;
    }

    if (cfg->args_n == 0) {
        printf("Error: missing input file\n");
        goto L_ERROR1;
    }

    outfn = pygetopt_get_value(cfg, "o");
    if (outfn == NULL) {
        outfn = pygetopt_get_value(cfg, "output");
    }
    if (outfn == NULL) {
        outfn = DEFAULT_OUTFN;
    }

    prefix = pygetopt_get_value(cfg, "prefix");
    if (prefix == NULL) {
        prefix = DEFAULT_PREFIX;
    }

    ret = bin2c(cfg->args[0], prefix, outfn, verbose);

    pygetopt_destroy(cfg);

    return ret;

L_ERROR1:
    pygetopt_destroy(cfg);

L_ERROR:
    print_usage(argv[0]);

    return -1;
}



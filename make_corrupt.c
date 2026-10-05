/* make_corrupt.c - generates corrupted flight_data.bin test files
 * for the Colossus Airlines reservation system.
 *
 * Build:  gcc -Wall -o make_corrupt make_corrupt.c
 * Run:    ./make_corrupt
 *
 * Output files (copy one over flight_data.bin, then run solution):
 *   good_flight.bin                - valid file, for comparison
 *   corrupt_truncated.bin          - ends after 10 outbound seats
 *   corrupt_oversized.bin          - valid file + 64 extra bytes
 *   corrupt_empty.bin              - 0 bytes
 *   corrupt_header.bin             - bad magic number
 *   corrupt_names_garbage.bin      - non-printable bytes inside a name
 *   corrupt_names_unterminated.bin - name fills all 32 bytes, no '\0'
 *   corrupt_seat_id.bin            - out-of-bounds seat number (999)
 *   corrupt_status.bin             - assigned flag set to 7
 *   corrupt_random.bin             - correct size, random bytes flipped
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/* Must match solution.c exactly */
#define SEATS    24
#define NAME_LEN 32

struct seat {
    int  id;
    int  assigned;
    char last[NAME_LEN];
    char first[NAME_LEN];
};

struct file_header {
    char     magic[4];
    uint32_t version;
    uint32_t seat_count;
};

struct flight_file {
    struct file_header h;
    struct seat out[SEATS];
    struct seat in[SEATS];
};

/* Build a valid file image with a few passengers in it. */
static void make_valid(struct flight_file *f)
{
    memset(f, 0, sizeof *f);
    memcpy(f->h.magic, "CLSS", 4);
    f->h.version = 1;
    f->h.seat_count = SEATS;

    for (int i = 0; i < SEATS; i++) {
        f->out[i].id = i + 1;
        f->in[i].id = i + 1;
    }

    f->out[0].assigned = 1; strcpy(f->out[0].first, "Anwar"); strcpy(f->out[0].last, "Abukar");
    f->out[4].assigned = 1; strcpy(f->out[4].first, "Maria"); strcpy(f->out[4].last, "Lopez");
    f->in[2].assigned  = 1; strcpy(f->in[2].first,  "John");  strcpy(f->in[2].last,  "Smith");
}

/* Write `len` bytes from `data` to `name`. */
static void write_file(const char *name, const void *data, size_t len)
{
    FILE *fp = fopen(name, "wb");
    if (fp == NULL) { perror(name); return; }
    if (len > 0 && fwrite(data, 1, len, fp) != len) perror(name);
    if (fclose(fp) != 0) perror(name);
    printf("  %-32s %zu bytes\n", name, len);
}

int main(void)
{
    struct flight_file f;
    unsigned char buf[sizeof f + 64];
    srand((unsigned)time(NULL));

    printf("Generating test files:\n");

    /* Valid baseline */
    make_valid(&f);
    write_file("good_flight.bin", &f, sizeof f);

    /* Truncated: header + only 10 outbound seats */
    write_file("corrupt_truncated.bin", &f, sizeof f.h + 10 * sizeof(struct seat));

    /* Oversized: valid data + 64 junk bytes */
    memcpy(buf, &f, sizeof f);
    for (int i = 0; i < 64; i++) buf[sizeof f + i] = (unsigned char)(rand() & 0xFF);
    write_file("corrupt_oversized.bin", buf, sizeof buf);

    /* Empty file */
    write_file("corrupt_empty.bin", "", 0);

    /* Bad header */
    make_valid(&f);
    memcpy(f.h.magic, "XXXX", 4);
    write_file("corrupt_header.bin", &f, sizeof f);

    /* Non-printable bytes inside a name (still null-terminated) */
    make_valid(&f);
    f.out[0].first[1] = '\x01';
    f.out[0].first[2] = '\x1B';   /* ESC - can mess with the terminal */
    f.out[0].first[3] = '\x7F';
    write_file("corrupt_names_garbage.bin", &f, sizeof f);

    /* Name with no '\0' anywhere in its 32 bytes -> printf would overrun */
    make_valid(&f);
    memset(f.out[4].last, 'A', NAME_LEN);
    write_file("corrupt_names_unterminated.bin", &f, sizeof f);

    /* Out-of-bounds seat number */
    make_valid(&f);
    f.in[2].id = 999;
    write_file("corrupt_seat_id.bin", &f, sizeof f);

    /* Invalid status flag */
    make_valid(&f);
    f.out[10].assigned = 7;
    write_file("corrupt_status.bin", &f, sizeof f);

    /* Correct size, but 20 random bytes overwritten after the header */
    make_valid(&f);
    unsigned char *p = (unsigned char *)&f;
    for (int i = 0; i < 20; i++) {
        size_t off = sizeof f.h + (size_t)rand() % (sizeof f - sizeof f.h);
        p[off] = (unsigned char)(rand() & 0xFF);
    }
    write_file("corrupt_random.bin", &f, sizeof f);

    printf("Done. To test: cp <file> flight_data.bin, then run your program.\n");
    return 0;
}
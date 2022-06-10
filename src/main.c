#include <stdio.h>
#include <args.h>
#include <bmp.h>

int main(int argc, char *argv[])
{
    /*
    args_t args = {0};
    parse_args(&args, argc, argv);
    printf("%s\n", args.in);
    printf("%s\n", args.bitmap_file);
    printf("%s\n", args.out);
    printf("%s\n", args.steganography_mode == LSB1 ? "LSB1" : "LSB4");
    */
    char *bmp_file_path = "./sample_640x426.bmp";
    bmp_header header = {0};
    FILE *bmp_stream = get_stream(bmp_file_path);
    load_header(bmp_stream, &header);
    printf("%d\n", header.type);
    printf("%d\n", header.size);
    printf("%d\n", header.reserved1);
    printf("%d\n", header.reserved2);
    printf("%d\n", header.offset);
    skip_offset(bmp_stream, header.offset);

    char pixel;
    bool reached_eof;
    int iterations = 10;
    while (iterations)
    {
        read_bmp_byte(bmp_stream, &pixel);
        printf("pixel: %d\n", pixel);
        iterations--;
    };
    close_stream(bmp_stream);
}

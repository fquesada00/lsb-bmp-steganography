import os
import argparse

encryption_modes = ["ecb", "cfb", "ofb", "cbc"]
block_cyphers = ["aes128", "aes192", "aes256", "des"]
steganography_modes = ["lsb1", "lsb4", "lsbi"]


def embed_without_encryption(carrierBmp, messageToHide, outputBmp):
    for steg_mode in steganography_modes:
        cmd = f"./stegobmp -embed -in {messageToHide} -p {carrierBmp} -out {steg_mode}_{outputBmp} -steg {steg_mode}"
        os.system(cmd)


def embed_with_encryption(carrierBmp, messageToHide, outputBmp, password):
    for block_cypher in block_cyphers:
        for encryption_mode in encryption_modes:
            for steg_mode in steganography_modes:
                cmd = f"./stegobmp -embed -in {messageToHide} -p {carrierBmp} -out {block_cypher}_{encryption_mode}_{steg_mode}_{outputBmp} -steg {steg_mode} -a {block_cypher} -m {encryption_mode} -pass {password}"
                os.system(cmd)


def embed(carrierBmp, messageToHide, outputBmp, password=None):
    embed_without_encryption(carrierBmp, messageToHide, outputBmp)

    if password is not None:
        embed_with_encryption(carrierBmp, messageToHide, outputBmp, password)


def extract_without_encryption(carrierBmp, outputFilename):
    for steg_mode in steganography_modes:
        cmd = f"./stegobmp -extract -p {carrierBmp} -out {steg_mode}_{outputFilename} -steg {steg_mode}"
        os.system(cmd)


def extract_with_encryption(carrierBmp, outputFilename, password):
    for block_cypher in block_cyphers:
        for encryption_mode in encryption_modes:
            for steg_mode in steganography_modes:
                cmd = f"./stegobmp -extract -p {carrierBmp} -out {block_cypher}_{encryption_mode}_{steg_mode}_{outputFilename} -steg {steg_mode} -a {block_cypher} -m {encryption_mode} -pass {password}"
                os.system(cmd)


def extract(carrierBmp, outputFilename, password=None):
    extract_without_encryption(carrierBmp, outputFilename)

    if password is not None:
        extract_with_encryption(carrierBmp, outputFilename, password)


if __name__ == "__name__":

    parser = argparse.ArgumentParser(
        "-em", "--embed", description="Embed a message into a bitmap", action="store_true")
    parser.add_argument("-c", "--carrier",
                        help="Carrier bitmap", required=False)
    parser.add_argument("-m", "--message",
                        help="Message to embed", required=False)
    parser.add_argument("-o", "--output", help="Output bitmap", required=False)
    parser.add_argument("-p", "--password",
                        help="Password for encryption", required=False)

    parser.add_argument(
        "-ex", "--extract", description="Extract a message from a bitmap", action="store_true")
    parser.add_argument("-out", "--output-filename",
                        description="Output filename", required=False)

    args = parser.parse_args()

    if args.embed:

        missing_arguments = False
        if args.carrier is None:
            print("Carrier bitmap is required. Use -c or --carrier to specify it.")
            missing_arguments = True

        if args.message is None:
            print("Message to embed is required. Use -m or --message to specify it.")
            missing_arguments = True

        if args.output is None:
            print("Output bitmap is required. Use -o or --output to specify it.")
            missing_arguments = True

        if missing_arguments:
            print("Missing arguments")
            exit(1)

        if args.password is None:
            print("No password given, embedding without encryption")

        embed(args.carrier, args.message, args.output, args.password)
    elif args.extract:
        if args.carrier is None or args.output_filename is None:
            print("Missing arguments")
            exit(1)

        if args.password is None:
            print("No password given, extracting without encryption")

        extract(args.carrier, args.output_filename, args.password)
    else:
        print("No action given")
        exit(1)

import os
import argparse

encryption_modes = ["ecb", "cfb", "ofb", "cbc"]
block_cyphers = ["aes128", "aes192", "aes256", "des"]
steganography_modes = ["LSB1", "LSB4", "LSBI"]


def embed_without_encryption(carrierBmp, messageToHide, outputBmp):
    for steg_mode in steganography_modes:
        cmd = f"./stegobmp -embed -in {messageToHide} -p {carrierBmp} -out {steg_mode}_{outputBmp} -steg {steg_mode}"
        print(f"Executing command: {cmd}")
        os.system(cmd)


def embed_with_encryption(carrierBmp, messageToHide, outputBmp, password):
    for block_cypher in block_cyphers:
        for encryption_mode in encryption_modes:
            for steg_mode in steganography_modes:
                cmd = f"./stegobmp -embed -in {messageToHide} -p {carrierBmp} -out {block_cypher}_{encryption_mode}_{steg_mode}_{outputBmp} -steg {steg_mode} -a {block_cypher} -m {encryption_mode} -pass {password}"
                print(f"Executing command: {cmd}")
                os.system(cmd)


def embed(carrierBmp, messageToHide, outputBmp, password=None):
    embed_without_encryption(carrierBmp, messageToHide, outputBmp)

    if password is not None:
        embed_with_encryption(carrierBmp, messageToHide, outputBmp, password)


def extract_without_encryption(carrierBmp, outputFilename):
    for steg_mode in steganography_modes:
        cmd = f"./stegobmp -extract -p {carrierBmp} -out {steg_mode}_{outputFilename} -steg {steg_mode}"
        print(f"Executing command: {cmd}")
        os.system(cmd)


def extract_with_encryption(carrierBmp, outputFilename, password):
    for block_cypher in block_cyphers:
        for encryption_mode in encryption_modes:
            for steg_mode in steganography_modes:
                cmd = f"./stegobmp -extract -p {carrierBmp} -out {block_cypher}_{encryption_mode}_{steg_mode}_{outputFilename} -steg {steg_mode} -a {block_cypher} -m {encryption_mode} -pass {password}"
                print(f"Executing command: {cmd}")
                os.system(cmd)


def extract(carrierBmp, outputFilename, password=None):
    extract_without_encryption(carrierBmp, outputFilename)

    if password is not None:
        extract_with_encryption(carrierBmp, outputFilename, password)


def embed_and_extract(carrierBmp, messageToHide, outputBmp, outputFilename, password=None):
    for steg_mode in steganography_modes:
        print(f"\n\n")
        print(f"Starting embedding & extraction with {steg_mode}")
        cmd = f"./stegobmp -embed -in {messageToHide} -p {carrierBmp} -out {steg_mode}_{outputBmp} -steg {steg_mode}"
        print(f"Executing command: {cmd}")
        os.system(cmd)

        cmd = f"./stegobmp -extract -p {steg_mode}_{outputBmp} -out {steg_mode}_{outputFilename} -steg {steg_mode}"
        print(f"Executing command: {cmd}")
        os.system(cmd)
    
    if password is not None:
        for block_cypher in block_cyphers:
            for encryption_mode in encryption_modes:
                for steg_mode in steganography_modes:
                    print(f"\n\n")
                    print(f"Starting embedding & extraction with {block_cypher}_{encryption_mode}_{steg_mode}")
                    cmd = f"./stegobmp -embed -in {messageToHide} -p {carrierBmp} -out {block_cypher}_{encryption_mode}_{steg_mode}_{outputBmp} -steg {steg_mode} -a {block_cypher} -m {encryption_mode} -pass {password}"
                    print(f"Executing command: {cmd}")
                    os.system(cmd)

                    cmd = f"./stegobmp -extract -p {block_cypher}_{encryption_mode}_{steg_mode}_{outputBmp} -out {block_cypher}_{encryption_mode}_{steg_mode}_{outputFilename} -steg {steg_mode} -a {block_cypher} -m {encryption_mode} -pass {password}"
                    print(f"Executing command: {cmd}")
                    os.system(cmd)



def previous_main(args):
    if args.em:

        missing_arguments = False
        if args.c is None:
            print("Carrier bitmap is required. Use -c or --carrier to specify it.")
            missing_arguments = True

        if args.m is None:
            print("Message to embed is required. Use -m or --message to specify it.")
            missing_arguments = True

        if args.o is None:
            print("Output bitmap is required. Use -o or --output to specify it.")
            missing_arguments = True

        if missing_arguments:
            print("Missing arguments")
            exit(1)

        if args.p is None:
            print("No password given, embedding without encryption")

        embed(args.c, args.m, args.o, args.p)
    elif args.ex:
        if args.c is None or args.out is None:
            print("Missing arguments")
            exit(1)

        if args.p is None:
            print("No password given, extracting without encryption")

        extract(args.c, args.out, args.p)
    else:
        print("No action given. Use -em or -ex to specify an action.")
        exit(1)


def main(args):
    missing_arguments = False
    if args.c is None:
        print("Carrier bitmap is required. Use -c or --carrier to specify it.")
        missing_arguments = True

    if args.m is None:
        print("Message to embed is required. Use -m or --message to specify it.")
        missing_arguments = True

    if args.o is None:
        print("Output bitmap is required. Use -o or --output to specify it.")
        missing_arguments = True


    if args.out is None:
        print("Output filename is required. Use --out or -output-filename to specify it.")
        exit(1)

    if missing_arguments:
        print("Missing arguments")
        exit(1)

    if args.p is None:
        print("No password given, extracting without encryption")

    embed_and_extract(args.c, args.m, args.o, args.out, args.p)


if __name__ == "__main__":

    parser = argparse.ArgumentParser()

    parser.add_argument("--c", "-carrier",
                        help="Carrier bitmap", required=False)
    parser.add_argument("--m", "-message",
                        help="Message to embed", required=False)
    parser.add_argument("--o", "-output", help="Output bitmap", required=False)
    parser.add_argument("--p", "-password",
                        help="Password for encryption", required=False)
    parser.add_argument("--out", "-output-filename",
                        help="Output filename", required=False)

    args = parser.parse_args()

    main(args)


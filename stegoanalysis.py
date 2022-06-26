import argparse


def different_bits(byte1, byte2):
    return bin(byte1 ^ byte2).count("1")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument("--f1", "-file1",
                        help="File number 1", required=True)
    parser.add_argument("--f2", "-file2",
                        help="File number 2", required=True)

    args = parser.parse_args()

    # read the two files byte by byte 
    with open(args.f1, "rb") as f1, open(args.f2, "rb") as f2:
        # sum all differences between the two files
        diff = 0
        for byte1, byte2 in zip(f1.read(), f2.read()):
            diff += different_bits(byte1, byte2)
        print(diff)



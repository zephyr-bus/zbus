import sys


def main():
    print(sys.argv)
    print(sys.argv[1])
    with open(sys.argv[1], "r") as source_file:
        complete = ""
        for line in source_file:
            if not line.startswith("#"):
                complete += line
        print(complete)


main()

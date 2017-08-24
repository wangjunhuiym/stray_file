# encoding:utf-8


def get_double_common(src_path):
    src = open(src_path)
    lines = src.readlines()
    for line in lines:
        id = str(int(line.strip('\n')) & 0xFFFFFFFF)
        if id[0:4] == '8014':
            print id
    src.close()

if __name__ == '__main__':
    #get_double_common('imageid20160918.log')
    get_double_common('imageid20160928.log')

# encoding:utf-8

import zlib
import gzip
import StringIO


def gzip_compress(buf):
    out = StringIO.StringIO()
    with gzip.GzipFile(fileobj=out, mode="w") as f:
        f.write(buf)
    return out.getvalue()


def gzip_decompress(buf):
    obj = StringIO.StringIO(buf)
    with gzip.GzipFile(fileobj=obj) as f:
        result = f.read()
    return result


def zlib_decompress(buf):
    return zlib.decompress(buf)



if __name__ == "__main__":

    request = 'Hello, World!'

    compressed = gzip_compress(request)

    decompressed = gzip_decompress(compressed)

    print decompressed

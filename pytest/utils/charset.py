# encoding:utf-8

import codecs


def utf16_le_to_utf_8(data):
    decode = codecs.getdecoder('utf_16_le')
    rv = decode(data)[0]
    return rv

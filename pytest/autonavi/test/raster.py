# encoding:utf-8

from autonavi.decoder import raster
from util import httpwrapper


def run():

    request = '<cross Type="1" Flag="0" Vers="1.0"><pictlist><ID>4655867298</ID><ID>4665867298</ID></pictlist></cross>'

    resp = httpwrapper.post(request, "10.218.252.22", 17196, 'cross')

    decoder = raster.CRasterDecoder()
    result = decoder.decode(resp)

    print decoder.get_last_error()
    print decoder.get_cross_num()

if __name__ == "__main__":
    run()

# encoding:utf-8

from autonavi.decoder import vector
from utils import httpwrapper


def run():

    request = '<cross Type="0" Flag="0" Vers="2.0" SdkVer="8.0.4" Uuid="123456789012345678901234567890123456789012345678901234567890abcd" Width="200" Height="200"><pict><in RF="23" linkID="5922798865127909737"><x1>120.003520</x1><y1>35.882522</y1><x2>120.003182</x2><y2>35.882496</y2></in><out RF="248" linkID="5922798865127316477"><x1>120.003182</x1><y1>35.882496</y1><x2>120.003173</x2><y2>35.882551</y2><exitLink RF="248" linkID="5922798867274794968"><x>120.003089</x><y>35.887304</y></exitLink></out></pict><pict><in RF="248" linkID="5923080138240562795"><x1>120.008142</x1><y1>35.895313</y1><x2>120.010578</x2><y2>35.896489</y2><enterLink RF="248" linkID="5923080138240563201"><x>120.007698</x><y>35.895100</y></enterLink></in><out RF="17" linkID="5923080138241158835"><x1>120.010578</x1><y1>35.896489</y1><x2>120.010907</x2><y2>35.895947</y2><exitLink RF="17" linkID="5923080138241158836"><x>120.011676</x><y>35.894642</y></exitLink></out></pict><pict><in RF="17" linkID="5923080138241157558"><x1>120.013013</x1><y1>35.892391</y1><x2>120.013076</x2><y2>35.892291</y2><enterLink RF="17" linkID="5923080138241157560"><x>120.012689</x><y>35.892936</y></enterLink></in><out RF="248" linkID="5923080138241157629"><x1>120.013076</x1><y1>35.892291</y1><x2>120.011947</x2><y2>35.891784</y2></out></pict></cross>'

    resp = httpwrapper.post(request, "10.218.252.22", 17196, 'cross')

    decoder = vector.CVectorDecoder()
    result = decoder.decode(resp)

    print decoder.get_last_error()
    print decoder.get_cross_num()

if __name__ == "__main__":
    run()

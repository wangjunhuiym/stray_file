#encoding:utf-8

import sys
sys.path.append('./extend')
sys.path.append('..')

import libextend as extend
from util.httpwrapper import post

def render(data):
    image = None
    try:
        image, length = extend.vector2render(data)
    except Exception, ex:
        print 'Exception: ', str(ex)
    finally:
        pass

    return image

if __name__ == '__main__':
    cross = '<cross Type="0" Flag="0" Vers="2.0" SdkVer="8.0.4" Uuid="123456789012345678901234567890123456789012345678901234567890abcd" Width="200" Height="200"><pict><in RF="80" linkID="5940566912902629480"><x1>112.241200</x1><y1>37.207136</y1><x2>112.241529</x2><y2>37.207536</y2><enterLink RF="80" linkID="5940566912902627742"><x>112.241156</x><y>37.207088</y></enterLink></in><out RF="80" linkID="5940566912902630168"><x1>112.241529</x1><y1>37.207536</y1><x2>112.241440</x2><y2>37.207596</y2><exitLink RF="80" linkID="5940566912902630167"><x>112.241396</x><y>37.207628</y></exitLink><exitLink RF="80" linkID="5940566912902629495"><x>112.241324</x><y>37.207682</y></exitLink><exitLink RF="80" linkID="5940566912902630173"><x>112.241240</x><y>37.207746</y></exitLink><exitLink RF="80" linkID="5940566912902630172"><x>112.241156</x><y>37.207808</y></exitLink><exitLink RF="80" linkID="5940566912902629493"><x>112.241058</x><y>37.207897</y></exitLink><exitLink RF="80" linkID="5940566912902627761"><x>112.240818</x><y>37.208140</y></exitLink><exitLink RF="80" linkID="5940566912902627762"><x>112.240671</x><y>37.208481</y></exitLink></out></pict><pict><in RF="16" linkID="5940564718174340761"><x1>112.286302</x1><y1>37.403667</y1><x2>112.286391</x2><y2>37.404129</y2><enterLink RF="16" linkID="5940564718174340758"><x>112.288604</x><y>37.384578</y></enterLink></in><out RF="48" linkID="5940564718174340759"><x1>112.286391</x1><y1>37.404129</y1><x2>112.286569</x2><y2>37.404493</y2><exitLink RF="16" linkID="5940564718174341304"><x>112.315618</x><y>37.414498</y></exitLink></out></pict><pict><in RF="16" linkID="5940560337307698113"><x1>112.817120</x1><y1>37.671698</y1><x2>112.817173</x2><y2>37.672173</y2><enterLink RF="16" linkID="5940561436819325367"><x>112.784080</x><y>37.621253</y></enterLink></in><out RF="98" linkID="5940560337307698054"><x1>112.817173</x1><y1>37.672173</y1><x2>112.817511</x2><y2>37.673573</y2><exitLink RF="98" linkID="5940560337307698053"><x>112.821218</x><y>37.678391</y></exitLink></out></pict></cross>'
    vector = post(cross, '10.218.252.22', 7196, 'cross')
    bmp = render(vector)
    f = open('image.bmp', 'w')
    f.write(bmp)
    f.flush()
    f.close()

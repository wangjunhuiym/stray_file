# encoding:utf-8

import sys, os

class MeshUtil(object):

    def __init__(self):
        pass

    @staticmethod
    def MeshSN2ID(sn):
        '''
        bits:   0000 | 000000 | 000000 | 00000000 | 00000000
        fields: sc   | ly     | lx     | sy       | sx
        '''
        sx = (ord(sn[7]) - ord('0')) * 100 + (ord(sn[8]) - ord('0')) * 10 + (ord(sn[9]) - ord('0'))
        sy = (ord(sn[4]) - ord('0')) * 100 + (ord(sn[5]) - ord('0')) * 10 + (ord(sn[6]) - ord('0'))
        lx = (ord(sn[1]) - ord('0')) * 10  + (ord(sn[2]) - ord('0'))
        ly = (ord(sn[0]) - ord('A'))
        sc = (ord(sn[3]) - ord('A'))
        id = sc<<28|ly<<22|lx<<16|sy<<8|sx
        return id

    @staticmethod
    def MeshID2SN(id):

        sc = (id&0xF0000000)>>28
        ly = (id&0x0FC00000)>>22
        lx = (id&0x003F0000)>>16
        sy = (id&0x0000FF00)>>8
        sx = (id&0x000000FF)

        snList = list()
        for i in  range(0, 10):
            snList.append('0')
  
        snList[0] = chr(ly + ord('A'))
        snList[1] = chr(lx/10 + ord('0'))
        snList[2] = chr(lx%10 + ord('0'))
        snList[3] = chr(sc + ord('A'))    

        snList[6] = chr(sy%10 + ord('0'))
        sy = sy/10
        snList[5] = chr(sy%10 + ord('0'))
        snList[4] = chr(sy/10 + ord('0'))    

        snList[9] = chr(sx%10 + ord('0'))
        sx = sx/10
        snList[8] = chr(sx%10 + ord('0'))
        snList[7] = chr(sx/10 + ord('0'))
        return ''.join(snList)


class LinkIdUtil(object):
  
    def __init__(self):
        pass

    @staticmethod
    def genLink64Id(meshId, direction, link32Id):
        return meshId<<32|direction<<31|link32Id

    @staticmethod
    def splitLink64Id(link64Id):
        return  MeshUtil.MeshID2SN(link64Id>>32), (link64Id&0x80000000)>>31, link64Id&0x7FFFFFFF


class CrossImageIDUtil(object):
    
    def __init__(self):
       pass

    @staticmethod
    def GetBackGroundImageIdWithoutDataVersion(id):
        rv = id & 0xFFFFFFFF
        rvStr = str(rv)
        if len(rvStr) == 7:
            rvStr = "0" + rvStr
        return rvStr


if __name__ == "__main__":
    '''
    if len(sys.argv) < 3:
        print '调用格式：python mapUtil.py 图幅ID     图幅内道路ID [道路方向] '
        print '举个例子：python mapUtil.py H51F009012 1121         [0|1]'
        exit()

    meshId = MeshUtil.MeshSN2ID(sys.argv[1])
    direction = 0

    link32Id = int(sys.argv[2])

    if len(sys.argv) == 4:
        direction = int(sys.argv[3])
        link64Id = LinkIdUtil.genLink64Id(meshId, direction, link32Id)
        print link64Id
    else:
        link64Id = LinkIdUtil.genLink64Id(meshId, 0, link32Id)
    if True:
        print "Link64Id:", link64Id, "\n"
        cmd = "curl -d '<roadattr><ID>"+str(link64Id)+"</ID></roadattr>' 10.218.252.22:7196/roadattr"
        os.system(cmd)
        print "\n"
        direction = 1
        link64Id = LinkIdUtil.genLink64Id(meshId, 1, link32Id)
        print "Link64Id:", link64Id, "\n"
        cmd = "curl -d '<roadattr><ID>"+str(link64Id)+"</ID></roadattr>' 10.218.252.22:7196/roadattr"
        os.system(cmd)
        print "\n"
    ''' 
    '''
    print CrossImageIDUtil.GetBackGroundImageIdWithoutDataVersion(sys.argv[1])
    '''
    '''
    print LinkIdUtil.splitLink64Id(5940812039571131872)
    '''
    pass

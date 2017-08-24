# encoding:utf-8


def fgid_to_imgid(version, fgid):

    '''
    [0,9]——[0,9]
    [10,35]——[A,Z]
    [36,61]——[a,z]
    '''

    startc = fgid[0]
    starts = ''

    if ord(startc) in range(ord('0'), ord('9')+1):
        starts = str(int(ord(startc)))
    elif ord(startc) in range(ord('A'), ord('Z')+1):
        starts = str(ord(startc)-ord('A')+10)
    elif ord(startc) in range(ord('a'), ord('z')+1):
        starts = str(ord(startc)-ord('a')+10)

    return (version<<32)|int(starts+fgid[1:-1])


def bgid_to_imgid(version, fgid):
    return (version<<32)|int(fgid)


def imgid_to_bgid(imgid):
    return str(imgid&0xFFFFFFFF)


def imgid_to_fgid(imgid):

    version = imgid>>32
    tmpid = imgid&0xFFFFFFFF
    tmpstr = str(tmpid)

    if len(tmpstr) == 7:
        tmpstr = '0'+tmpid
    elif len(tmpstr) == 8:
        pass
    elif len(tmpstr) == 9:
        if int(tmpstr[0:2]) in range(10, 35):
            tmpstr = str(int(tmpstr[0:2]) - 10 + ord('A')) + tmpstr[2:-1]
        elif int(tmpstr[0:2]) in range(36, 61):
            tmpstr = str(int(tmpstr[0:2]) - 10 + ord('a')) + tmpstr[2:-1]

    return tmpstr


# encoding:utf-8

import httplib


def post(data, ip, port, path):
    rv = None
    http_client = None
    status = 200
    try:
        headers = {'Content-type': 'text/xml', 'Accept': 'application/octet-stream'}
        http_client = httplib.HTTPConnection(ip, port, timeout=120) # 超时时间2分钟
        http_client.request(method="POST", url="http://"+ip+":"+str(port)+"/"+path, body=data, headers=headers)
        http_resp = http_client.getresponse()
        status = http_resp.status
        content = http_resp.read()
        rv = content

    except Exception, ex:
        print 'httpwrapper exception'
        rv = None
        raise Exception(str(ex))
    finally:
        if http_client:
            http_client.close()
        return status, rv

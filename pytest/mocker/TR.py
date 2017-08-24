# encoding:utf-8

import gzip
import StringIO

from flask import Flask
from flask import request

app = Flask(__name__)

@app.route("/RouteStatusService/Handle.do", methods=['GET', 'POST'])
def hello():
    frontcoors = request.form['frontcoords']
    fields = frontcoors.split(';')
    links = fields[1].split('|')
    print "%-30s %-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s"%('roadid', 'length', 'drivelen', 'enddist', 'status', 'speed', 'eta', 'startlon', 'startlat', 'endlon', 'endlat')
    for link in links:
        print "%-30s %-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s"%tuple(link.split(','))
        #print link.split(',')
    resp = '<?xml version="1.0" encoding="utf-8"?> <response type="trafficinfo" msgtype="TrafficInfo" detailType="8"> <status>0</status> <timestamp>20160927173120</timestamp> <updatetime>173107</updatetime> <front> <updatetime>173107</updatetime> <description><![CDATA[刘燕阳，假数据]]></description> <signature nearby="0" dist="-1" class="3"><status><len>2572</len><speed>19</speed><eta>495</eta><roadclass>41000</roadclass><formway>1</formway><startlon>-1</startlon><startlat>-1</startlat><endlon>116.5155029</endlon><endlat>39.98125839</endlat></status></signature> </front> </response>'
    def gzip_compress(buf):
        out = StringIO.StringIO()
        with gzip.GzipFile(fileobj=out, mode="w") as f:
            f.write(buf)
        return out.getvalue()
    rv = gzip_compress(resp)
    return rv 

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=8000)

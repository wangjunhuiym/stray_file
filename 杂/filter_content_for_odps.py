# encoding: UTF-8
from odps.udf import annotate
import re
@annotate("string->boolean")
class FilterContent_(object):
    def evaluate(self, content):
        pattern = re.compile(r'.*ROUTE.*')
        match = pattern.match(content)
        if match == None :
            return False
        pattern = re.compile(r'.*POST.*')
        match = pattern.match(content)
        if match == None :
            return False

        m = re.match(r'.*Vers="(.*?)"', content)
        if m != None:
            result =  m.group(1) == "2.5.3"
            if result == False:
                return False
        else:
            return False

        m = re.match(r'.*Reroute="(.*?)"', content)
        if m != None:
            result = m.group(1) == "0"
            if result == 0:
                return False

        m = re.match(r'.*Silence="(.*?)"', content)
        if m != None:
            result = m.group(1) == "0"
            if result == False:
                return False

        m = re.match(r'.*Source="(.*?)"', content)
        if m != None:
            result = m.group(1) == "amap"
            if result == False:
                return False
        else:
            return False


        m = re.match(r'.*Uuid="(.*?)"', content)
        if m != None:
            result = m.group(1) != "0" and  m.group(1) != " "
            if result == False:
                return False
        else:
            return False
   
        m = re.match(r'.*Invoker="(.*?)"', content)
        if m != None:
            result = m.group(1) == "plan" or  m.group(1) == "navi"
            if result == False:
                return False
        else:
            return False

        return True;

# encoding:utf-8

import sys

sys.path.append("../..")

from autonavi.decoder import route400
from utils import httpwrapper


def get_header(address='10.218.252.22', port=7196):
    # request = '<route SessionID="175769391" StepID="144" Type="13" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="861106032086848" MODEL="A8" Vers="4.0" cifa="3804F00629AD7F068FA70B02010D02CC010B000000000000000000723600003800000071E7000000000000160000000F004855415745492052494F2D414C30300A00687752494F2D414C303006004855415745490C00372E372E322E302E32303537000000000000000000002C00" Plate="" Diu2="142cd77dec7fffffffe8" UtdID="VyV04x+T3hEDACOumtR50A1F" AosID="" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65566" SdkVer="8.0.4" PlayStyle="0" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B0FFG2XGMY" Typecode="120203" PoiFlag="1"/> <StartAngle><Angle>267.26563</Angle><Credibility>0.5</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>109.03570443391801</x><y>34.31689737987967</y></startpoint>  <endpoint Type=""><x>108.94743159413338</x><y>34.22170314032344</y></endpoint><endpoint Type=""><x>108.94686698913576</x><y>34.2220003292521</y></endpoint> </route>'
    # request = '<route Source="amap" Flag="0x800000281" Type="0x00" ContentOptions="0x10" Vers="4.0" SdkVer="3.6.8.1.1.20160510.30306.1517"><startpoint><x>116.383240</x><y>39.857033</y></startpoint><endpoint><x>116.460384</x><y>39.867596</y></endpoint></route>'
    # request = '<route SessionID="175811414" StepID="2954" Type="12" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="861079032822431" MODEL="A8" Vers="4.0" cifa="380480075446F006108D6202000D01CC0100008A10000002972401000000000000000000000000000000001600000008004F50504F2052396D0200523904004F50504F0C00372E372E322E302E36303031000000000000000000000200" Plate="冀DXA913" Diu2="69f584cf0d37ffffffe8" UtdID="VoVRBfc772MDAPy3bwrq5Hu3" AosID="46166183" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="2" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B000A7HUUM" Typecode="140300" PoiFlag="1"/> <StartAngle><Angle>1</Angle><Credibility>0.0</Credibility></StartAngle> <StartLocate><Precision>2.7999</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>116.40994057059291</x><y>40.013073612280486</y></startpoint>  <endpoint Type=""><x>116.431293</x><y>40.019644</y></endpoint><endpoint Type=""><x>116.42758548259737</x><y>40.0219075484347</y></endpoint> </route>'
    # request = '<route Source="amap" Flag="0x800000281" Type="0x00" ContentOptions="0x10" Vers="4.0" SdkVer="3.6.8.1.1.20160510.30306.1517"><startpoint><x>116.318863</x><y>40.068188</y></startpoint><endpoint><x>117.004395</x><y>36.664013</y></endpoint></route>'
    # request = '<route Source="amap" Flag="0x80000028" Type="0x00" ContentOptions="0x10" Vers="4.0" SdkVer="3.6.8.1.1.20160510.30306.1517"><startpoint><x>116.461086</x><y>40.159066</y></startpoint><endpoint><x>113.260621</x><y>23.167123</y></endpoint></route>'
    # request = '<route Source="amap" Flag="0x80000028" Type="0x00" ContentOptions="0x10" Vers="4.0" SdkVer="3.6.8.1.1.20160510.30306.1517"><startpoint><x>126.672363</x><y>45.629406</y></startpoint><endpoint><x>102.744141</x><y>25.145285</y></endpoint></route>'
    # request = '<route SdkVer="8.0.4" ContentOptions="65590" Type="0" Flag="135352" Uuid="352621066304350" Vers="4.0" Source="amap" Invoker="plan" Reroute="0" RouteMode="1" Silence="0" ThreeD="1" PlayStyle="2" SoundType="0" Plate="京QZ3T38"><location Type="0"/><destination POIID="B000A857G9" PoiFlag="0" Typecode="190302"/><startpoint Type="0"><x>116.664322</x><y>39.924931</y></startpoint><endpoint Type="2"><x>116.372704</x><y>39.940620</y></endpoint></route>'
    request = '<route SessionID="175800295" StepID="77" Type="1" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="861874037864671" MODEL="A8" Vers="4.0" cifa="3804800767C341071DB1DB01010D01CC010000AD1800000C829101000000000000000000000000000000001600000009004F50504F205239746D0200523904004F50504F0C00372E372E322E302E36303031000000000000000000002300" Plate="沪C6UV72" Diu2="3d29640988e3ffffffe8" UtdID="VoVRBTzUXB0DAH2N22wCMrW1" AosID="51472293" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="2" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B00155LZU1" Typecode="110202" PoiFlag="1"/> <StartAngle><Angle>197</Angle><Credibility>0.0</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>121.75037562847142</x><y>31.174940181090733</y></startpoint>  <endpoint Type=""><x>121.51910752058029</x><y>30.817651468972155</y></endpoint><endpoint Type=""><x>121.51894256472589</x><y>30.817528232351656</y></endpoint> </route>'

    status, resp = httpwrapper.post(request, address, port, 'route')

    # print status, len(resp)

    if status != 200:
        print 'http status:', status
        return

    dec = route400.CCloudPathDecoder()
    dec.decode(resp)
    if dec.m_error_code != 0:
        print 'decode error:', dec.m_error_code
        return

    '''
    dist = 0
    for i in range(0, dec.m_pack_header.m_path_num):
        path = dec.m_path_lst[i]
        for j in range(0, path.m_seg_num):
            seg = path.m_seg_lst[j]
            for k in range(0, seg.m_link_num):
                link = seg.m_link_lst[k]
                #print link.m_topo_id_64, link.m_status, link.m_dist
                if link.m_status == 4:
                    dist += link.m_dist
    print dist
    '''
    #print dec.m_pack_header.m_data_version
    print dec


def compare(request_):

    # request = '<route SessionID="175772387" StepID="366" Type="13" Flag="8786040" Source="amap" Invoker="plan" Refresh="0" Uuid="867451023589606" MODEL="A8" Vers="4.0" cifa="38048007743E3E073F98DC01010001CC0100000B1800000CB5B30100000000000000000000000000000000170000000B004D49204E4F5445204C54450500766972676F06005869616F6D690C00372E372E322E302E32303535000000000000000000002300" Plate="" Diu2="fffffffffffdffffffe8" UtdID="VWQvtWHpQVIDAK65hGAFtqs8" AosID="" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65566" SdkVer="8.0.4" PlayStyle="0" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="" Typecode="" PoiFlag="1"/> <StartAngle><Angle>276.51563</Angle><Credibility>0.5</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>121.52408838272096</x><y>31.232036609860216</y></startpoint>  <endpoint Type="1"><x>121.42363965511322</x><y>31.237482341273118</y></endpoint> </route>'
    #request = '<route SessionID="175781561" StepID="31" Type="4" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="860482030862949" MODEL="A8" Vers="4.0" cifa="38041D07BA13C6060E981102010E02CC0103000000000000000000B9360000FFFF000001A5000000000000170000000F00485541574549204E58542D414C3130050048574E585406004855415745490000000000000000000000002300" Plate="" Diu2="fffffffffffdffffffe8" UtdID="VslPMEQJv/UDAKpJk1vD6k2x" AosID="" Div="ANDH051001" CIP="127.0.0.1" ContentOptions="30" SdkVer="8.0.4" PlayStyle="0" SoundType="0" ThreeD="0"> <vehicle type="0" height="2.5" load="0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="BV10092377" Typecode="150700" PoiFlag="1"/>  <startpoint Type="0"><x>113.65054428577423</x><y>34.7062750846157</y></startpoint>  <endpoint Type="2"><x>113.642952</x><y>34.699471</y></endpoint> <slope sigshelter="-1.0"/></route>'
    #request = '<route ThreeD="1" SdkVer="8.0.4" ContentOptions="22" Type="0" Flag="135352" Uuid="860858030689111" Vers="4.0" Source="amap" Invoker="plan" Reroute="0" Silence="0" Refresh="0" RerouteIndex="0"><location Type="0"/><destination POIID="B0FFFFA9NU" PoiFlag="0" Typecode="070501"/><StartAngle><Angle>179.222900</Angle><Credibility>0.800000</Credibility><Speed>9.0</Speed></StartAngle><startpoint Type="0"><x>125.364502</x><y>43.867485</y></startpoint><startpoint Type="0"><x>125.364502</x><y>43.867382</y></startpoint><endpoint Type="2"><x>125.434402</x><y>43.904266</y></endpoint></route>'
    #request = '<route SessionID="175771593" StepID="270" Type="13" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="865982025678767" MODEL="A8" Vers="4.0" cifa="38048007615AF006362F61020100010000000000000000FFFFFFFF00000000000000000000000000000000130000000B004D49204E4F5445204C54450500766972676F06005869616F6D690C00372E372E322E302E36303032000000000000000000001D00" Plate="" Diu2="7e100139795fffffffe8" UtdID="Vji66ed+vYADABaWA1i9RskB" AosID="" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65566" SdkVer="8.0.4" PlayStyle="2" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B0FFH4JY43" Typecode="100000" PoiFlag="1"/> <StartAngle><Angle>191</Angle><Credibility>0.0</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>116.4150743186474</x><y>39.923511232365904</y></startpoint>  <endpoint Type="2"><x>108.89915987849237</x><y>34.239357448320995</y></endpoint> </route>'
    #request = '<route SessionID="175771593" StepID="270" Type="13" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="865982025678767" MODEL="A8" Vers="4.0" cifa="38048007615AF006362F61020100010000000000000000FFFFFFFF00000000000000000000000000000000130000000B004D49204E4F5445204C54450500766972676F06005869616F6D690C00372E372E322E302E36303032000000000000000000001D00" Plate="" Diu2="7e100139795fffffffe8" UtdID="Vji66ed+vYADABaWA1i9RskB" AosID="" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65566" SdkVer="8.0.4" PlayStyle="2" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B0FFH4JY43" Typecode="100000" PoiFlag="1"/> <StartAngle><Angle>191</Angle><Credibility>0.0</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>116.4150743186474</x><y>39.923511232365904</y></startpoint>  <endpoint Type="2"><x>108.89915987849237</x><y>34.239357448320995</y></endpoint> </route>'
    #request = '<route SessionID="175771593" StepID="270" Type="13" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="865982025678767" MODEL="A8" Vers="4.0" cifa="38048007615AF006362F61020100010000000000000000FFFFFFFF00000000000000000000000000000000130000000B004D49204E4F5445204C54450500766972676F06005869616F6D690C00372E372E322E302E36303032000000000000000000001D00" Plate="" Diu2="7e100139795fffffffe8" UtdID="Vji66ed+vYADABaWA1i9RskB" AosID="" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65566" SdkVer="8.0.4" PlayStyle="2" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B0FFH4JY43" Typecode="100000" PoiFlag="1"/> <StartAngle><Angle>191</Angle><Credibility>0.0</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>116.4150743186474</x><y>39.923511232365904</y></startpoint>  <endpoint Type="2"><x>108.89915987849237</x><y>34.239357448320995</y></endpoint> </route>'

    # 坡度信息
    # request = '<route SdkVer="8.0.4" ContentOptions="65590" Type="0" Flag="135352" Uuid="352621066304350" Vers="4.0" Source="amap" Invoker="plan" Reroute="0" RouteMode="1" Silence="0" ThreeD="1" PlayStyle="2" SoundType="0" Plate="京QZ3T38"><location Type="0"/><destination POIID="B000A857G9" PoiFlag="0" Typecode="190302"/><startpoint Type="0"><x>116.664322</x><y>39.924931</y></startpoint><endpoint Type="2"><x>116.372704</x><y>39.940620</y></endpoint></route>'
    # 车道线
    # request = '<route SessionID="175767736" StepID="108" Type="4" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="A1000049C3143A" MODEL="A8" Vers="4.0" cifa="D002000524CF3306DD5DD401000D02CC010B0000000000000000004A3600000C0000000F0E0000000000001300000008007669766F2058355607005044313430315604007669766F0C00372E372E322E302E36303032000000000000000000000300" Plate="川A24D29" Diu2="f947ecb75cc3ffffffe8" UtdID="Vi223oyC3o0DAE1puvZoN97Z" AosID="33373617" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="2" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="" Typecode="" PoiFlag="1"/>  <StartLocate><Precision>3.0</Precision><Speed>0</Speed></StartLocate><startpoint Type=""><x>104.05872881412506</x><y>30.694785682113057</y></startpoint><startpoint Type=""><x>104.05872881412506</x><y>30.694785682113057</y></startpoint>  <endpoint Type="1"><x>104.05095040798189</x><y>30.683924101012362</y></endpoint> </route>'
    # 播报内容
    # request = '<route SessionID="175791168" StepID="37" Type="4" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="A0000059C8AC6F" MODEL="A8" Vers="4.0" cifa="3804F006CDF13206B28FD401000D02CC010B0000000000000000004A3600000C0000005C070000000000001600000008004B49572D434C30300700484E4B49572D5106004855415745490C00372E372E322E302E32303535000000000000000000000900" Plate="川A0Y29F" Diu2="f5f094f91123ffffffe8" UtdID="SVwHwROJQCgDAMcV1XrTeolc" AosID="51570295" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="0" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B001C8RQLM" Typecode="061000" PoiFlag="1"/> <StartAngle><Angle>30.03125</Angle><Credibility>0.5</Credibility></StartAngle> <StartLocate><Precision>9.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>104.00432020425797</x><y>30.70507410954143</y></startpoint>  <endpoint Type=""><x>104.07775774598123</x><y>30.655521140703797</y></endpoint><endpoint Type=""><x>104.07864958047868</x><y>30.657940383549793</y></endpoint> </route>'
    # 有门禁
    # request = '<route SessionID="175769070" StepID="27" Type="4" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="90EB1309-E06B-4F96-8EA3-F45B21BC4CD1" MODEL="A8" Vers="4.0" cifa="ee0236059799f006c1506002000000cc010000000000000000000000000000000000000000000000000000090000000500392e332e3309006950686f6e65372c3205004150504c450a00372e362e322e32303231000000000000000000004100" Plate="京p80k78" Diu2="C6682FBB-3709-4668-97EE-085928081715" UtdID="VFhHHdSaCSMDANj49Vv0GfY/" AosID="28906851" Div="IOSH070602" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="0" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.000000" load="0.000000"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B0FFFWT3S8" Typecode="991001" PoiFlag="1"/> <StartAngle><Angle>337.325897</Angle><Credibility>0.500000</Credibility></StartAngle> <StartLocate><Precision>65.000000</Precision><Speed>-1.000000</Speed></StartLocate><startpoint Type="0"><x>116.437468</x><y>39.867948</y></startpoint><startpoint Type="0"><x>116.437468</x><y>39.867948</y></startpoint><startpoint Type="0"><x>116.437468</x><y>39.867948</y></startpoint>  <endpoint Type="2"><x>116.225319</x><y>39.906878</y></endpoint> </route>'
    #request = '<route ThreeD="1" SdkVer="8.0.4" ContentOptions="54" Type="0" Flag="135352" Uuid="864232021999846" Vers="4.0" Source="amap" Invoker="plan" Reroute="0" Silence="0" Refresh="0" RerouteIndex="0" Plate="川A0V9B5"><location Type="0"/><startpoint Type="1"><x>104.043373</x><y>30.546560</y></startpoint><endpoint Type="1"><x>104.091431</x><y>30.711523</y></endpoint></route>'
    # 是否有限行信息
    # request = '<route SessionID="175768242" StepID="1805" Type="4" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="31E9D3A5-93B3-4CE0-BB79-9E8E440E0067" MODEL="A8" Vers="4.0" cifa="ee0236057b623807fd94d901000000cc010100000000000000000000000000000000000000000000000000090000000500392e332e3209006950686f6e65382c3105004150504c450a00372e372e322e32303338000000000000000000000500" Plate="沪C7HK69" Diu2="9CCE0C0B-AC2A-400B-A39C-EB0D60300CF0" UtdID="VsxL1qSj3rEDANGzoZ46zEwd" AosID="27411884" Div="IOSH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="0" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.000000" load="0.000000"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B00151936F" Typecode="110101" PoiFlag="1"/> <StartAngle><Angle>45.558491</Angle><Credibility>0.500000</Credibility></StartAngle> <StartLocate><Precision>5.000000</Precision><Speed>0.000000</Speed></StartLocate><startpoint Type="0"><x>121.140314</x><y>31.034699</y></startpoint><startpoint Type="0"><x>121.140314</x><y>31.034699</y></startpoint><startpoint Type="0"><x>121.140314</x><y>31.034699</y></startpoint>  <endpoint Type="2"><x>121.372769</x><y>31.102081</y></endpoint><endpoint Type="2"><x>121.373091</x><y>31.103196</y></endpoint> </route>'
    #request = '<route SessionID="175800295" StepID="77" Type="1" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="861874037864671" MODEL="A8" Vers="4.0" cifa="3804800767C341071DB1DB01010D01CC010000AD1800000C829101000000000000000000000000000000001600000009004F50504F205239746D0200523904004F50504F0C00372E372E322E302E36303031000000000000000000002300" Plate="沪C6UV72" Diu2="3d29640988e3ffffffe8" UtdID="VoVRBTzUXB0DAH2N22wCMrW1" AosID="51472293" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="2" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B00155LZU1" Typecode="110202" PoiFlag="1"/> <StartAngle><Angle>197</Angle><Credibility>0.0</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>121.75037562847142</x><y>31.174940181090733</y></startpoint>  <endpoint Type=""><x>121.51910752058029</x><y>30.817651468972155</y></endpoint><endpoint Type=""><x>121.51894256472589</x><y>30.817528232351656</y></endpoint> </route>'
    # 终点方向
    # request = '<route SdkVer="8.0.4" ContentOptions="54" Type="4" Flag="135352" Uuid="356001073061622" Vers="4.0" Source="amap" Invoker="plan" Reroute="0" Silence="1" RerouteIndex="0" Plate="冀RZ2478" NaviID="54b9638f80167f6066bc1612d9c3d861"><location Type="0"/><StartAngle><Angle>315.809479</Angle><Credibility>0.990000</Credibility></StartAngle><startpoint Type="0"><x>116.496658</x><y>39.825661</y></startpoint><startpoint Type="0"><x>116.495399</x><y>39.826656</y></startpoint><startpoint Type="0"><x>116.495392</x><y>39.826664</y></startpoint><endpoint Type="2"><x>116.488228</x><y>39.972870</y></endpoint><endpoint Type="2"><x>116.488419</x><y>39.973164</y></endpoint></route>'
    # 路径外事件数量
    # request = '<route ThreeD="1" SdkVer="8.0.4" ContentOptions="22" Type="2" Flag="135352" Uuid="868967028669555" Vers="4.0" Source="amap" Invoker="plan" Reroute="1" Silence="0" RerouteIndex="1" NaviID="8622f70356abf63038bc7229b333285f" Time="2147483648"><location Type="0"/><StartAngle><Angle>267.000000</Angle><Credibility>0.900000</Credibility><Speed>23.6</Speed></StartAngle><startpoint Type="0"><x>120.123901</x><y>30.239592</y></startpoint><startpoint Type="0"><x>120.123756</x><y>30.239567</y></startpoint><startpoint Type="0"><x>120.123642</x><y>30.239588</y></startpoint><endpoint Type="2"><x>119.099594</x><y>29.306730</y></endpoint><endpoint Type="2"><x>119.099731</x><y>29.306528</y></endpoint><routepoints><pt rc="8" fw="15">120.124146,30.239698,158</pt><pt rc="8" fw="15">120.126602,30.235577,190</pt><pt rc="8" fw="15">120.127846,30.233391,135</pt><pt rc="8" fw="15">120.129539,30.232166,120</pt><pt rc="8" fw="15">120.129997,30.231720,202</pt><pt rc="8" fw="15">120.131264,30.228926,126</pt><pt rc="8" fw="15">120.134949,30.225885,126</pt><pt rc="8" fw="15">120.138367,30.224436,118</pt><pt rc="8" fw="1">120.138130,30.203188,180</pt><pt rc="8" fw="1">120.137039,30.199572,176</pt><pt rc="8" fw="15">120.136887,30.199297,221</pt><pt rc="8" fw="15">120.134506,30.197210,236</pt><pt rc="7" fw="1">120.108437,30.177534,232</pt><pt rc="1" fw="1">120.111176,30.149094,176</pt><pt rc="0" fw="1">120.112000,30.141481,175</pt></routepoints><linkPoint><x>120.124008</x><y>30.239990</y><rc>8</rc><fw>15</fw><angle>157</angle></linkPoint></route>'
    # 躲避拥堵道路名称长度
    # request = '<route SessionID="175786490" StepID="18" Type="13" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="867790028750279" MODEL="A8" Vers="4.0" cifa="380408079A21EF06ABC05502000D01CC010000B230000002EC390E000000000000000000000000000000001300000009004F50504F2052377374030052377304004F50504F0C00372E372E302E302E32303336000000000000000000002800" Plate="冀GFZ573" Diu2="ab8d3d47a4d3ffffffe8" UtdID="V0gGsfcslMoDADDSO+OMJmoL" AosID="" Div="ANDH070700" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="0" SoundType="0" ThreeD="1"> <vehicle type="0" height="2.5" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="" Typecode="" PoiFlag="1"/> <StartAngle><Angle>356.52832</Angle><Credibility>0.5</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="1"><x>116.337387</x><y>39.178635</y></startpoint>  <endpoint Type="1"><x>116.467552</x><y>39.993553</y></endpoint> </route>'
    # 有易混淆路口
    # request = '<route SessionID="175791168" StepID="37" Type="4" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="A0000059C8AC6F" MODEL="A8" Vers="4.0" cifa="3804F006CDF13206B28FD401000D02CC010B0000000000000000004A3600000C0000005C070000000000001600000008004B49572D434C30300700484E4B49572D5106004855415745490C00372E372E322E302E32303535000000000000000000000900" Plate="川A0Y29F" Diu2="f5f094f91123ffffffe8" UtdID="SVwHwROJQCgDAMcV1XrTeolc" AosID="51570295" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="0" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B001C8RQLM" Typecode="061000" PoiFlag="1"/> <StartAngle><Angle>30.03125</Angle><Credibility>0.5</Credibility></StartAngle> <StartLocate><Precision>9.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>104.00432020425797</x><y>30.70507410954143</y></startpoint>  <endpoint Type=""><x>104.07775774598123</x><y>30.655521140703797</y></endpoint><endpoint Type=""><x>104.07864958047868</x><y>30.657940383549793</y></endpoint> </route>'
    # request = '<route SessionID="175810368" StepID="490" Type="4" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="868842025148467" MODEL="A8" Vers="4.0" cifa="3804800769D8EF0610476102010D01CC0100004310000083BF960700000000000000000000000000000000150000000C005265646D69204E6F7465203206006865726D657306005869616F6D690C00372E372E322E302E36303032000000000000000000000C00" Plate="京P98099" Diu2="46b29f459b03ffffffe8" UtdID="VgOWgY/o0GADAH4oGZdQoDH5" AosID="15167989" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="2" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="B000A87IVG" Typecode="120302" Mode="0"/> <destination POIID="B000A84OQ5" Typecode="991400" PoiFlag="1"/> <StartAngle><Angle>16</Angle><Credibility>0.0</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="2"><x>116.38004302978516</x><y>39.92952346801758</y></startpoint>  <endpoint Type=""><x>116.337794</x><y>39.902486</y></endpoint><endpoint Type=""><x>116.33779585361482</x><y>39.90259872516186</y></endpoint> </route>'
    # isByPassLimited
    # request = '<route SessionID="175773759" StepID="184" Type="4" Flag="397434" Source="amap" Invoker="plan" Refresh="0" Uuid="D0090135-7D34-429D-8A70-857C8414DE95" MODEL="A8" Vers="4.0" cifa="80027004bec73607291adb0101000000000000000000000000000000000000000000000000000000000000070000000500372e312e3109006950686f6e65362c3105004150504c450a00372e372e322e32303338000000000000000000000a00" Plate="沪CGZ028" Diu2="14A6A839-D5D5-41A8-BD72-7C0A020CFDA9" UtdID="VsxH/MV86rsDANZtQ45+YenL" AosID="" Div="IOSH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="0" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.000000" load="0.000000"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="" Typecode="" PoiFlag="1"/> <StartAngle><Angle>263.684418</Angle><Credibility>0.500000</Credibility></StartAngle> <StartLocate><Precision>10.000000</Precision><Speed>0.000000</Speed></StartLocate><startpoint Type="0"><x>121.035073</x><y>31.134319</y></startpoint><startpoint Type="0"><x>121.035073</x><y>31.134319</y></startpoint><startpoint Type="0"><x>121.035073</x><y>31.134319</y></startpoint>  <endpoint Type="1"><x>121.511654</x><y>31.076132</y></endpoint> </route>'
    # request = '<route SessionID="175800295" StepID="77" Type="1" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="861874037864671" MODEL="A8" Vers="4.0" cifa="3804800767C341071DB1DB01010D01CC010000AD1800000C829101000000000000000000000000000000001600000009004F50504F205239746D0200523904004F50504F0C00372E372E322E302E36303031000000000000000000002300" Plate="沪C6UV72" Diu2="3d29640988e3ffffffe8" UtdID="VoVRBTzUXB0DAH2N22wCMrW1" AosID="51472293" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="2" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="B00155LZU1" Typecode="110202" PoiFlag="1"/> <StartAngle><Angle>197</Angle><Credibility>0.0</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="0"><x>121.75037562847142</x><y>31.174940181090733</y></startpoint>  <endpoint Type=""><x>121.51910752058029</x><y>30.817651468972155</y></endpoint><endpoint Type=""><x>121.51894256472589</x><y>30.817528232351656</y></endpoint> </route>'
    # request = '<route SessionID="175786020" StepID="54" Type="0" Flag="397434" Source="amap" Invoker="plan" Refresh="0" Uuid="92B90571-2B9B-4C75-A666-6A0760266479" MODEL="A8" Vers="4.0" cifa="ee02360552fd3e0752d7da01000000cc010000000000000000000000000000000000000000000000000000090000000500392e332e3209006950686f6e65372c3205004150504c450a00372e372e322e3230333800000000000000000000d007" Plate="沪CXK125" Diu2="B28CD788-EFBA-4432-9B81-DE3ABFB44A04" UtdID="VzVlKSGe2JQDAMONwZ2e+Fjf" AosID="50266959" Div="IOSH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="0" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.000000" load="0.000000"/><location POIID="" Typecode="" Mode="0"/> <destination POIID="" Typecode="120302" PoiFlag="1"/> <StartAngle><Angle>63.589962</Angle><Credibility>0.500000</Credibility></StartAngle> <StartLocate><Precision>2000.000000</Precision><Speed>-1.000000</Speed></StartLocate><startpoint Type="0"><x>121.572831</x><y>31.116947</y></startpoint><startpoint Type="0"><x>121.572820</x><y>31.116947</y></startpoint><startpoint Type="0"><x>121.572813</x><y>31.116947</y></startpoint>  <endpoint Type="1"><x>121.751821</x><y>31.170636</y></endpoint> </route>'
    # request = '<route SessionID="175810368" StepID="490" Type="4" Flag="397432" Source="amap" Invoker="plan" Refresh="0" Uuid="868842025148467" MODEL="A8" Vers="4.0" cifa="3804800769D8EF0610476102010D01CC0100004310000083BF960700000000000000000000000000000000150000000C005265646D69204E6F7465203206006865726D657306005869616F6D690C00372E372E322E302E36303032000000000000000000000C00" Plate="京P98099" Diu2="46b29f459b03ffffffe8" UtdID="VgOWgY/o0GADAH4oGZdQoDH5" AosID="15167989" Div="ANDH070702" CIP="127.0.0.1" ContentOptions="65598" SdkVer="8.0.4" PlayStyle="2" SoundType="0" ThreeD="1"> <vehicle type="0" height="0.0" load="0.0"/><location POIID="B000A87IVG" Typecode="120302" Mode="0"/> <destination POIID="B000A84OQ5" Typecode="991400" PoiFlag="1"/> <StartAngle><Angle>16</Angle><Credibility>0.0</Credibility></StartAngle> <StartLocate><Precision>0.0</Precision><Speed>0</Speed></StartLocate><startpoint Type="2"><x>116.38004302978516</x><y>39.92952346801758</y></startpoint>  <endpoint Type=""><x>116.337794</x><y>39.902486</y></endpoint><endpoint Type=""><x>116.33779585361482</x><y>39.90259872516186</y></endpoint> </route>'

    request = request_

    status_a, resp_a = httpwrapper.post(request, "10.218.252.22", 7196, 'route')
    status_b, resp_b = httpwrapper.post(request, "10.218.252.24", 7196, 'route')

    if status_a != 200:
        print 'status a:', status_a
        exit()

    if status_b != 200:
        print 'status b:', status_b
        exit()

    if resp_a is None or len(resp_a) == 0:
        print 'response a:', resp_a
        exit()

    if resp_b is None or len(resp_b) == 0:
        print 'response a:', resp_b
        exit()

    dec_a = route400.CCloudPathDecoder()
    dec_a.decode(resp_a)
    dec_b = route400.CCloudPathDecoder()
    dec_b.decode(resp_b)

    if dec_a.m_error_code != 0:
        print 'decode error a:', dec_a.m_error_code
        exit()

    if dec_b.m_error_code != 0:
        print 'decode error b:', dec_b.m_error_code
        exit()

    if dec_a.m_pack_header.m_path_num != dec_b.m_pack_header.m_path_num:
        print 'path num not equal.'
        exit()

    for i in range(0, dec_a.m_pack_header.m_path_num):

        path_a = dec_a.m_path_lst[i]
        path_b = dec_b.m_path_lst[i]

        if path_a.m_seg_num != path_b.m_seg_num:
            print 'segment num not equal.'
            break

        if path_a.m_is_by_pass_limited != path_b.m_is_by_pass_limited:
            print 'r5: path:', i, ' m_is_by_pass_limited:', path_a.m_is_by_pass_limited
            print 'r9: path:', i, ' m_is_by_pass_limited:', path_b.m_is_by_pass_limited

        if path_a.m_avoid_jam_area_num != path_b.m_avoid_jam_area_num:
            print 'r5: path:', i, ' has_restriction_info:', path_a.m_avoid_jam_area_num
            print 'r9: path:', i, ' has_restriction_info:', path_b.m_avoid_jam_area_num
        else:
            for j in range(0, path_a.m_avoid_jam_area_num):
                print 'r5: path:', i, ', m_avoid_jam_area:', j, ", ", path_a.m_avoid_jam_area_lst[j].m_lon
                print 'r9: path:', i, ', m_avoid_jam_area:', j, ", ", path_b.m_avoid_jam_area_lst[j].m_lon

        if path_a.m_restriction_info != path_b.m_restriction_info:
            print 'r5: path:', i, ' has_restriction_info:', path_a.m_restriction_info.m_title
            print 'r9: path:', i, ' has_restriction_info:', path_b.m_restriction_info

        if path_a.m_dest_direction != path_b.m_dest_direction:
            print 'r5: path:', i, ' m_dest_direction:', path_a.m_dest_direction
            print 'r9: path:', i, ' m_dest_direction:', path_b.m_dest_direction

        if path_a.m_outer_incident_num != path_b.m_outer_incident_num:
            print 'r5: path:', i, ' m_outer_incident_num:', path_a.m_outer_incident_num
            print 'r9: path:', i, ' m_outer_incident_num:', path_b.m_outer_incident_num
        for j in range(0, path_a.m_seg_num):

            seg_a = path_a.m_seg_lst[j]
            seg_b = path_b.m_seg_lst[j]

            if seg_a.m_link_num != seg_b.m_link_num:
                print 'link num not equal.'
                break

            if seg_a.m_slope != seg_b.m_slope:
                print 'r5: path:', i,', segment:', j, ', slope:', seg_a.m_slope
                print 'r5: path:', i,', segment:', j, ', slope:', seg_b.m_slope

            if seg_a.m_play_pointer_num != seg_b.m_play_pointer_num:
                print 'r5: path:', i, ', segment:', j, ', playpointer number::', seg_a.m_play_pointer_num
                print 'r9: path:', i, ', segment:', j, ', playpointer number::', seg_b.m_play_pointer_num
            else:
                for k in range(0, seg_a.m_play_pointer_num):
                    if seg_a.m_play_pointer_lst[k].m_content.m_Len != seg_b.m_play_pointer_lst[k].m_content.m_Len:
                        print 'r5: path:', i, ', segment:', j, ', playpointer:', k, ', m_content.m_Content:', seg_a.m_play_pointer_lst[k].m_content.m_Content
                        print 'r9: path:', i, ', segment:', j, ', playpointer:', k, ', m_content.m_Content:', seg_b.m_play_pointer_lst[k].m_content.m_Content

            for k in range(0, seg_a.m_link_num):
                link_a = seg_a.m_link_lst[k]
                link_b = seg_b.m_link_lst[k]
                if link_a.get_topo_id_64() != link_b.get_topo_id_64():
                    print 'linkid not equal. ', link_a.get_topo_id_64(), link_b.get_topo_id_64()
                    break

                if link_a.has_mix_fork() != link_b.has_mix_fork():
                    print 'r5: path:', i, ', segment:', j, ', link:', k, ', has_mix_fork:', link_a.has_mix_fork()
                    print 'r9: path:', i, ', segment:', j, ', link:', k, ', has_mix_fork:', link_b.has_mix_fork()

                if link_a.has_entrance_guide() != link_b.has_entrance_guide():
                    print 'r5: path:', i, ', segment:', j, ', link:', k, ', has_entrance_guide:', link_a.has_entrance_guide()
                    print 'r9: path:', i, ', segment:', j, ', link:', k, ', has_entrance_guide:', link_b.has_entrance_guide()

                if link_a.m_sel_lane_info_id != link_b.m_sel_lane_info_id:
                    print 'r5: path:',i,', segment:', j, ', link:', k, ', m_sel_lane_info_id:', link_a.m_sel_lane_info_id
                    print 'r9: path:',i,', segment:', j, ', link:', k, ', m_sel_lane_info_id:', link_b.m_sel_lane_info_id
                    print 'r5: path:',i,', segment:', j, ', link:', k, ', m_back_lane_info_id:', link_a.m_back_lane_info_id
                    print 'r9: path:',i,', segment:', j, ', link:', k, ', m_back_lane_info_id:', link_b.m_back_lane_info_id

if __name__ == '__main__':
    compare(sys.argv[1])
    # get_header()
    # get_header(address="100.81.183.87", port=8765)
    # get_header(address="route.amap.test", port=8765)
    # get_header(address='10.218.252.22', port=7196)
    # get_header(address='10.218.252.24', port=7196)
    # get_header(address='100.81.153.116', port=9152)

import datetime

str='12click_list1|47|14:14:20|1&2|50|14:14:24|2&3|52|14:14:25|1&4|53|14:14:32|0&5|53|14:14:32|021num502'
#def getFinalId(str):

elapsed=0
strList=str.split('click_list')[-1]
strategys=strList.split('&')
time1=strategys[0].split('|')[2].split(':')
timel1=strategys[-1].split('|')[2].split(':')
d1=datetime.datetime(1999,1,1,int(time1[0]),int(time1[1]),int(time1[2]))
print d1.time
if int(time1[0])>int(timel1[0]):
	day=2
else:
	day=1
d2=datetime.datetime(1999,1,day,int(timel1[0],int(timel1[1]),int(timel1[2]))
	#elapsed=(d2-d1).seconds
	#if elapsed>10 and elapsed<600:
	#	return strategys[-1].split('|')[-1][0]
	#return -1
print strategys[-1].split('|')[-1][0]	


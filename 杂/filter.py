#/bin/env python
import json
import sys

for i in sys.stdin:
  fls=i.strip().split("\t")
  if len(fls) != 12:
    print >>sys.stderr, "skip for fields:%s" % fls[0]
    continue
  try:
    json.loads(fls[-1])
  except Exception,ex:
    print >>sys.stderr, "skip for parse:%s" % fls[0]
    continue
  sys.stdout.write(i)


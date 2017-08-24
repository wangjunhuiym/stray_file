# encoding:utf-8

import os
import sys
import logging
import logging.config


default_encoding = 'utf-8'
if sys.getdefaultencoding() != default_encoding:
    reload(sys)
    sys.setdefaultencoding(default_encoding)


def configure_logging(root='.', name='logging.conf'):

    rv = True

    if not os.path.exists(name):
        return False

    log = os.path.join(root, name)

    with open(log) as f:
        if f is not None:
            content = f.read()
            #todo: travel items to find out log directory
            logging.config.dictConfig(eval(content))
        else:
            rv = False

    return rv


def configure(root='.'):

    rv = True

    if not os.path.exists(root):
        return False

    if not os.path.isdir(root):
        return False

    rv = configure_logging(root)

    return rv

if __name__ == '__main__':
    configure()

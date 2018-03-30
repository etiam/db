#!/usr/bin/env python

import os, sys, re

data = """[{'message': u'thread-group-added',
 'payload': {u'id': u'i1'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': u'done',
 'payload': None,
 'stream': 'stdout',
 'token': None,
 'type': 'result'}]
writing:
[{'message': u'done',
 'payload': {u'files': [{u'file': u'hello.c',
                         u'fullname': u'/home/jasonr/src/pygdbmi/pygdbmi/tests/sample_c_app/hello.c'},
                        {u'file': u'/usr/lib/gcc/x86_64-linux-gnu/5/include/stddef.h',
                         u'fullname': u'/usr/lib/gcc/x86_64-linux-gnu/5/include/stddef.h'},
                        {u'file': u'bye.c',
                         u'fullname': u'/home/jasonr/src/pygdbmi/pygdbmi/tests/sample_c_app/bye.c'}]},
 'stream': 'stdout',
 'token': None,
 'type': 'result'}]
writing:
[{'message': u'done',
 'payload': {u'bkpt': {u'addr': u'0x000000000040056e',
                       u'disp': u'keep',
                       u'enabled': u'y',
                       u'file': u'hello.c',
                       u'fullname': u'/home/jasonr/src/pygdbmi/pygdbmi/tests/sample_c_app/hello.c',
                       u'func': u'main',
                       u'line': u'17',
                       u'number': u'1',
                       u'original-location': u'main',
                       u'thread-groups': [u'i1'],
                       u'times': u'0',
                       u'type': u'breakpoint'}},
 'stream': 'stdout',
 'token': None,
 'type': 'result'}]
writing:
[{'message': u'thread-group-started',
 'payload': {u'id': u'i1', u'pid': u'15531'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': u'thread-created',
 'payload': {u'group-id': u'i1', u'id': u'1'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': u'library-loaded',
 'payload': {u'host-name': u'/lib64/ld-linux-x86-64.so.2',
             u'id': u'/lib64/ld-linux-x86-64.so.2',
             u'symbols-loaded': u'0',
             u'target-name': u'/lib64/ld-linux-x86-64.so.2',
             u'thread-group': u'i1'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': u'running',
 'payload': None,
 'stream': 'stdout',
 'token': None,
 'type': 'result'},
{'message': u'running',
 'payload': {u'thread-id': u'all'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': u'library-loaded',
 'payload': {u'host-name': u'/lib/x86_64-linux-gnu/libc.so.6',
             u'id': u'/lib/x86_64-linux-gnu/libc.so.6',
             u'symbols-loaded': u'0',
             u'target-name': u'/lib/x86_64-linux-gnu/libc.so.6',
             u'thread-group': u'i1'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'}]
writing:
[{'message': u'breakpoint-modified',
 'payload': {u'bkpt': {u'addr': u'0x000000000040056e',
                       u'disp': u'keep',
                       u'enabled': u'y',
                       u'file': u'hello.c',
                       u'fullname': u'/home/jasonr/src/pygdbmi/pygdbmi/tests/sample_c_app/hello.c',
                       u'func': u'main',
                       u'line': u'17',
                       u'number': u'1',
                       u'original-location': u'main',
                       u'thread-groups': [u'i1'],
                       u'times': u'1',
                       u'type': u'breakpoint'}},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': None, 'payload': u'\\n', 'stream': 'stdout', 'type': 'console'},
{'message': None,
 'payload': u'Breakpoint 1, main () at hello.c:17\\n',
 'stream': 'stdout',
 'type': 'console'},
{'message': None,
 'payload': u'17\\t    printf(\\"Hello world\\\\n\\");\\n',
 'stream': 'stdout',
 'type': 'console'},
{'message': u'stopped',
 'payload': {u'bkptno': u'1',
             u'core': u'5',
             u'disp': u'keep',
             u'frame': {u'addr': u'0x000000000040056e',
                        u'args': [],
                        u'file': u'hello.c',
                        u'fullname': u'/home/jasonr/src/pygdbmi/pygdbmi/tests/sample_c_app/hello.c',
                        u'func': u'main',
                        u'line': u'17'},
             u'reason': u'breakpoint-hit',
             u'stopped-threads': u'all',
             u'thread-id': u'1'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': u'running',
 'payload': None,
 'stream': 'stdout',
 'token': None,
 'type': 'result'},
{'message': u'running',
 'payload': {u'thread-id': u'all'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': u'stopped',
 'payload': {u'core': u'5',
             u'frame': {u'addr': u'0x0000000000400578',
                        u'args': [],
                        u'file': u'hello.c',
                        u'fullname': u'/home/jasonr/src/pygdbmi/pygdbmi/tests/sample_c_app/hello.c',
                        u'func': u'main',
                        u'line': u'18'},
             u'reason': u'end-stepping-range',
             u'stopped-threads': u'all',
             u'thread-id': u'1'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'}]
writing:
[{'message': u'running',
 'payload': None,
 'stream': 'stdout',
 'token': None,
 'type': 'result'},
{'message': u'running',
 'payload': {u'thread-id': u'all'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': u'stopped',
 'payload': {u'core': u'5',
             u'frame': {u'addr': u'0x0000000000400582',
                        u'args': [],
                        u'file': u'hello.c',
                        u'fullname': u'/home/jasonr/src/pygdbmi/pygdbmi/tests/sample_c_app/hello.c',
                        u'func': u'main',
                        u'line': u'19'},
             u'reason': u'end-stepping-range',
             u'stopped-threads': u'all',
             u'thread-id': u'1'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'}]
writing:
[{'message': u'running',
 'payload': None,
 'stream': 'stdout',
 'token': None,
 'type': 'result'},
{'message': u'running',
 'payload': {u'thread-id': u'all'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': None,
 'payload': u'Hello world',
 'stream': 'stdout',
 'type': 'output'},
{'message': None,
 'payload': u'  leading spaces should be preserved. So should trailing spaces.  ',
 'stream': 'stdout',
 'type': 'output'},
{'message': None, 'payload': u'i = 0', 'stream': 'stdout', 'type': 'output'},
{'message': None, 'payload': u'i = 1', 'stream': 'stdout', 'type': 'output'},
{'message': None, 'payload': u'Bye', 'stream': 'stdout', 'type': 'output'},
{'message': None,
 'payload': u'[Inferior 1 (process 15531) exited normally]\\n',
 'stream': 'stdout',
 'type': 'console'},
{'message': u'thread-exited',
 'payload': {u'group-id': u'i1', u'id': u'1'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': u'thread-group-exited',
 'payload': {u'exit-code': u'0', u'id': u'i1'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'},
{'message': u'stopped',
 'payload': {u'reason': u'exited-normally'},
 'stream': 'stdout',
 'token': None,
 'type': 'notify'}]"""

entries = []
a = re.sub('\s+', ' ', data)
a = a.replace('\n','')
for n, entry in enumerate(a.split('writing:')):
    exec('entries.extend(%s)' % entry.strip())

def getn(num):
    return entries[num]

if __name__ == "__main__":
    print getn(31)
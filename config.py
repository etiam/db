#!/usr/bin/env python

import os, sys

workspacedir = os.path.dirname(os.getcwd())
projectdir = os.getcwd()
build = sys.argv[1]
arch = os.environ['ARCH']

builddir = 'build-%s-%s' % (build, arch)
if (not(os.path.exists(builddir))):
    os.makedirs(builddir)

prefix = os.path.join(projectdir, builddir, 'install')

options  = '--enable-maintainer-mode --enable-silent-rules '
options += ' '.join(sys.argv[2:])

cmd  = 'cd {builddir}; ../configure -C CXX="clang++" CC="clang" \
        --prefix={prefix} {options} '.format(**globals())

cmd += '; cd ..'

os.system(cmd)

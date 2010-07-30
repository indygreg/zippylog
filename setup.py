#  Copyright 2010 Gregory Szorc
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

from setuptools import setup, find_packages

setup(
    name             = 'pblog',
    version          = '0.0.0.dev',
    packages         = [ 'pblog' ],
    package_dir      = {'':'lib/py'},
    package_data     = { 'pblog': ['proto/*.proto'] },
    scripts          = ['bin/pblog_compile'],
    test_suite       = 'pblog.test.all_tests_suite',
    install_requires = [ 'protobuf>=2.3.0' ],
    author           = 'Gregory Szorc',
    author_email     = 'gregory.szorc@gmail.com',
    description      = 'structured message logging system',
    license          = 'Apache 2.0',
    url              = 'http://github.com/indygreg/pblog'
)

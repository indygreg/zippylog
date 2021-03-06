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


import unittest

def all_tests_suite():
    suite = unittest.TestLoader().loadTestsFromNames([
        'zippylog.test.store_test',
    ])

    return suite
        
if __name__ == '__main__':
    import sys
    import os.path

    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.dirname(__file__))))

    runner = unittest.TextTestRunner()
    suite = all_tests_suite()
    runner.run(suite) 

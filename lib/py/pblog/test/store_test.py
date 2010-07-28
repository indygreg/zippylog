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
import pblog.store

class WriterTest(unittest.TestCase):

    def test_file_for_time(self):
        f = pblog.store.file_for_time
        
        self.assertRaises(Exception, f, 1280210699, 86401)
        self.assertRaises(Exception, f, 1280210699, 34234234)
        self.assertRaises(Exception, f, 1280210699, 3)
        self.assertRaises(Exception, f, 1280210699, -1)
        self.assertRaises(Exception, f, 1280210699, 13)

        self.assertEqual(f(1280210699, 86400), ('2010-07-27', 1280275200))
        self.assertEqual(f(1280212529, 3600), ('2010-07-27-06', 1280214000))
        self.assertEqual(f(1280275200, 60), ('2010-07-28-00-001-060', 1280275260))
        self.assertEqual(f(1280275260, 60), ('2010-07-28-00-002-060', 1280275320))

        
if __name__ == '__main__':
    unittest.main()
            

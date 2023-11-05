import re
import subprocess
import unittest

class TestLab3(unittest.TestCase):

    def _make():
        result = subprocess.run(['make'], capture_output=True, text=True)
        return result

    def _make_clean():
        result = subprocess.run(['make', 'clean'],
                                capture_output=True, text=True)
        return result

    @classmethod
    def setUpClass(cls):
        cls.make = cls._make().returncode == 0

    @classmethod
    def tearDownClass(cls):
        cls._make_clean()

    def test_1(self):
        print(".Running tester code 1...")
        self.assertTrue(self.make, msg='make failed')

        hash_result = subprocess.check_output(('./hash-table-tester', '-t', '8', '-s', '50000')).decode()
        nums = re.sub(r'Generation: ([\d\,]+) usec\nHash table base: ([\d\,]+) usec\n  - ([\d\,]+) missing\nHash table v1: ([\d\,]+) usec\n  - ([\d\,]+) missing\nHash table v2: ([\d\,]+) usec\n  - ([\d\,]+) missing\n', 
                      r'\1|\2|\3|\4|\5|\6|\7',
                      hash_result)

        _, _, miss_0, _, miss_1, _, miss_2 = nums.split('|')

        miss_0 = int(miss_0.replace(",", ""))
        miss_1 = int(miss_1.replace(",", ""))
        miss_2 = int(miss_2.replace(",", ""))

        self.assertEqual(miss_0, 0, msg=f"The missing entries for Hash table base should be 0 but got {miss_0} instead.")
        self.assertEqual(miss_1, 0, msg=f"The missing entries for Hash table v1 should be 0 but got {miss_1} instead.")
        self.assertEqual(miss_2, 0, msg=f"The missing entries for Hash table v2 should be 0 but got {miss_2} instead.")

    def test_2(self):
        print("Running tester code 2...")
        self.assertTrue(self.make, msg='make failed')

        hash_result = subprocess.check_output(('./hash-table-tester', '-t', '8', '-s', '40000')).decode()
        nums = re.sub(r'Generation: ([\d\,]+) usec\nHash table base: ([\d\,]+) usec\n  - ([\d\,]+) missing\nHash table v1: ([\d\,]+) usec\n  - ([\d\,]+) missing\nHash table v2: ([\d\,]+) usec\n  - ([\d\,]+) missing\n', 
                      r'\1|\2|\3|\4|\5|\6|\7',
                      hash_result)

        _, _, miss_0, _, miss_1, _, miss_2 = nums.split('|')

        miss_0 = int(miss_0.replace(",", ""))
        miss_1 = int(miss_1.replace(",", ""))
        miss_2 = int(miss_2.replace(",", ""))

        self.assertEqual(miss_0, 0, msg=f"The missing entries for Hash table base should be 0 but got {miss_0} instead.")
        self.assertEqual(miss_1, 0, msg=f"The missing entries for Hash table v1 should be 0 but got {miss_1} instead.")
        self.assertEqual(miss_2, 0, msg=f"The missing entries for Hash table v2 should be 0 but got {miss_2} instead.")

    def test_3(self):
        print("Running tester code 3...")
        self.assertTrue(self.make, msg='make failed')

        hash_result = subprocess.check_output(('./hash-table-tester', '-t', '4', '-s', '50000')).decode()
        nums = re.sub(r'Generation: ([\d\,]+) usec\nHash table base: ([\d\,]+) usec\n  - ([\d\,]+) missing\nHash table v1: ([\d\,]+) usec\n  - ([\d\,]+) missing\nHash table v2: ([\d\,]+) usec\n  - ([\d\,]+) missing\n', 
                      r'\1|\2|\3|\4|\5|\6|\7',
                      hash_result)

        _, _, miss_0, _, miss_1, _, miss_2 = nums.split('|')

        miss_0 = int(miss_0.replace(",", ""))
        miss_1 = int(miss_1.replace(",", ""))
        miss_2 = int(miss_2.replace(",", ""))

        self.assertEqual(miss_0, 0, msg=f"The missing entries for Hash table base should be 0 but got {miss_0} instead.")
        self.assertEqual(miss_1, 0, msg=f"The missing entries for Hash table v1 should be 0 but got {miss_1} instead.")
        self.assertEqual(miss_2, 0, msg=f"The missing entries for Hash table v2 should be 0 but got {miss_2} instead.")
        

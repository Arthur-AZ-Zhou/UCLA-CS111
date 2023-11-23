import datetime
import os
import subprocess
import time
import unittest

class Lab4TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.year = 2021
        subprocess.run(['make'], capture_output=True)
        subprocess.run(['./ext2-create'], capture_output=True)
        p = subprocess.run(['dumpe2fs', 'cs111-base.img'], capture_output=True, text=True)
        cls.dump_lines = p.stdout.splitlines()
        p = subprocess.run(['fsck.ext2', '-f', '-n', 'cs111-base.img'], capture_output=True, text=True)
        cls.fsck_lines = p.stdout.splitlines()
        p = subprocess.run(['ls', '-f', '-n', 'cs111-base.img'], capture_output=True, text=True)
        cls.ls_lines = p.stdout.splitlines()
        subprocess.run(['mkdir', 'mnt'], capture_output=True)
        subprocess.run(['sudo', 'mount', '-o', 'loop', 'cs111-base.img', 'mnt'])
        cls.root_stat = os.lstat('mnt') if os.path.exists('mnt') else None
        cls.hello_world_stat = os.lstat('mnt/hello-world') if os.path.exists('mnt/hello-world') else None
        cls.hello_stat = os.lstat('mnt/hello') if os.path.exists('mnt/hello') else None

    @classmethod
    def tearDownClass(cls):
        subprocess.run(['sudo', 'umount', 'mnt'])
        subprocess.run(['rmdir', 'mnt'], capture_output=True)
        subprocess.run(['make', 'clean'], capture_output=True)

    def test_hello(self):
        self.assertEqual(os.readlink('mnt/hello'), 'hello-world')

    def test_hello_world(self):
        with open('mnt/hello-world') as f:
            self.assertEqual(f.read(), "Hello world\n")

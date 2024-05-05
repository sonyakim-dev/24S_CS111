import pathlib
import re
import subprocess
import unittest
import tempfile


class TestLab1(unittest.TestCase):

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

  def test_1_command(self):
    self.assertTrue(self.make, msg='make failed')
    cl_result = subprocess.run(('ls'),
                               capture_output=True, shell=True)
    pipe_result = subprocess.check_output(('./pipe', 'ls'))
    self.assertEqual(cl_result.stdout, pipe_result,
                     msg=f"The output from ./pipe should be {cl_result.stdout} but got {pipe_result} instead.")
    self.assertTrue(self._make_clean, msg='make clean failed')

  def test_3_commands(self):
    self.assertTrue(self.make, msg='make failed')
    cl_result = subprocess.run(('ls | cat | wc'),
                               capture_output=True, shell=True)
    pipe_result = subprocess.check_output(('./pipe', 'ls', 'cat', 'wc'))
    self.assertEqual(cl_result.stdout, pipe_result,
                     msg=f"The output from ./pipe should be {cl_result.stdout} but got {pipe_result} instead.")
    self.assertTrue(self._make_clean, msg='make clean failed')

  def test_no_orphans(self):
    self.assertTrue(self.make, msg='make failed')
    subprocess.call(('strace', '-o', 'trace.log', './pipe', 'ls', 'wc', 'cat', 'cat'))
    ps = subprocess.Popen(['grep', '-o', 'clone(', 'trace.log'], stdout=subprocess.PIPE)
    out1 = subprocess.check_output(('wc', '-l'), stdin=ps.stdout)
    ps.wait()
    ps.stdout.close()
    ps = subprocess.Popen(['grep', '-o', 'wait', 'trace.log'], stdout=subprocess.PIPE)
    out2 = subprocess.check_output(('wc', '-l'), stdin=ps.stdout)
    ps.wait()
    ps.stdout.close()
    out1 = int(out1.decode("utf-8")[0])
    out2 = int(out2.decode("utf-8")[0])
    if out1 == out2 or out1 < out2:
      orphan_check = True
    else:
      orphan_check = False
    self.assertTrue(orphan_check, msg="Found orphan processes")
    subprocess.call(['rm', 'trace.log'])
    self.assertTrue(self._make_clean, msg='make clean failed')

  def run_input_test(self, commands, input_text):
    with tempfile.NamedTemporaryFile(mode='w+', delete=True) as temp:
      temp.write(input_text)
      temp.seek(0)
      cl_result = subprocess.run((' | '.join(commands)),
                                 input=temp.read(), capture_output=True, shell=True, text=True)
      temp.seek(0)
      pipe_result = subprocess.check_output(['./pipe'] + commands, input=temp.read(), text=True)
      self.assertEqual(cl_result.stdout, pipe_result,
                       msg=f"The output from ./pipe should be {cl_result.stdout} but got {pipe_result} instead.")

  def test_input_file_1(self):
    self.run_input_test(['cat'], 'Hello World\n')

  def test_input_file_2(self):
    self.run_input_test(['cat', 'uniq', 'sort', 'tail', 'wc'],
                        'Hello World\nWorld\nWorld\n1\n2\n3\n1\na\nab\nsonya kim\nz\nz\nz\n')

  def test_input_file_3(self):
    self.run_input_test(['cat', 'sort', 'uniq', 'tail'],
                        'Hello World\nWorld\nWorld\n1\n2\n3\n1\na\nab\nsonya kim\nz\nz\nz\n')

  def test_empty_input(self):
    self.assertTrue(self.make, msg='make failed')
    with tempfile.NamedTemporaryFile(mode='w+', delete=True) as temp:
      cl_result = subprocess.run(('cat | wc'),
                                 input=temp.read(), capture_output=True, shell=True, text=True)
      temp.seek(0)
      pipe_result = subprocess.check_output(('./pipe', 'cat', 'wc'), input=temp.read(), text=True)
    self.assertEqual(cl_result.stdout, pipe_result,
                     msg=f"The output from ./pipe should be {cl_result.stdout} but got {pipe_result} instead.")
    self.assertTrue(self._make_clean, msg='make clean failed')

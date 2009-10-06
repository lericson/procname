"""
>>> import procname
>>> procname.setprocname("hello world")
>>> ps_self()
'hello world'
>>> procname.setprocname("foo bar")
>>> ps_self()
'foo bar'
"""

def ps_self():
    import os
    import subprocess
    p = subprocess.Popen(["ps", "-o", "command", "-p", str(os.getpid())],
                         stdout=subprocess.PIPE)
    p.stdout.readline()
    assert p.wait() == 0
    return p.stdout.readline().strip()

if __name__ == "__main__":
    import doctest
    doctest.testmod()

import subprocess
import os
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import shutil

class Build(build_ext):
    def run(self):
        # Clone lnsocket if not present
        if not os.path.exists("lnsocket"):
            subprocess.check_call([
                "git", "clone", "https://github.com/21M4TW/lnsocket.git", "lnsocket"
            ])


        # Detect GNU Make
        make_cmd = find_gnu_make()

        # Build static library using Makefile
        subprocess.check_call([make_cmd, "lnsocket.a", "libsodium.a", "libsecp256k1.a", "CFLAGS=-Wall -Os -Ideps/secp256k1/include -Ideps/libsodium/src/libsodium/include -Ideps -fPIC"], cwd="lnsocket")
        
        super().run()

def find_gnu_make():
    for cmd in ["gmake", "make"]:
        path = shutil.which(cmd)
        if path:
            try:
                output = subprocess.check_output([cmd, "--version"], text=True)
                if "GNU Make" in output:
                    return cmd
            except Exception:
                continue
    raise RuntimeError("GNU Make not found. Please install it.")

ext = Extension(
    name="pylnsocket.wrapper",
    sources=["src/wrapper.c"],
    include_dirs=["lnsocket/"],
    extra_objects=["lnsocket/lnsocket.a","lnsocket/libsodium.a","lnsocket/libsecp256k1.a"],
)

setup(
    name="pylnsocket",
    version="0.1.0",
    ext_modules=[ext],
    packages=["pylnsocket"],
    cmdclass={"build_ext": Build},
)

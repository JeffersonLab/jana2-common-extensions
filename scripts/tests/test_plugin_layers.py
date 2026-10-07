"""Run the production wrappers with a fake JANA; no JANA installation needed."""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

scripts = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix="jce config ") as directory:
    root = Path(directory)
    stack, first, second, alternate = [root / name for name in ("stack", "first", "second", "alternate")]
    for path in (stack / "config", stack / "bin", first, second, alternate):
        path.mkdir(parents=True)
    jana = stack / "bin/jana"
    jana.write_text('#!/bin/sh\nprintf "%s\\n" "$@"\n')
    jana.chmod(0o755)
    base = stack / "config/default_plugins.db"
    base.write_text("evio_common_modules,evio_parser,core\n")
    (first / "default_plugins.db").write_text(" # comment\nfirst, evio_common_modules\nfirst\n")
    (second / "default_plugins.db").write_text("second,first\n")
    custom = alternate / "default_plugins.db"
    custom.write_text("custom\n")
    env = {k: v for k, v in os.environ.items() if k not in
           ("JCE_CONFIG_DIR", "JANA_PLUGIN_PATH", "JANA_HOME")}
    env.update(JCE_HOME=str(stack), JCE_CONFIG_DIR=f":{first}:{second}:{first}:")
    wrappers = [("bash", "jce.sh")]
    if shutil.which("tcsh"):
        wrappers.append(("tcsh", "jce.csh"))
    else:
        print("SKIP: tcsh unavailable")
    for shell, wrapper in wrappers:
        def check(expected, args=(), updates=None, warning=None):
            result = subprocess.run([shell, str(scripts / wrapper), *args, "data file.evio"],
                                    env=env | (updates or {}), text=True, capture_output=True)
            assert result.returncode == 0, result
            assert f"-Pplugins={expected}" in result.stdout.splitlines(), result.stdout
            assert "data file.evio" in result.stdout.splitlines(), result.stdout
            assert "-PJCE:CORE_CONFIG_DIR" not in result.stdout, result.stdout
            assert "-PDEFAULT_PLUGINS:FILE" not in result.stdout, result.stdout
            assert "\033" not in result.stderr, result.stderr
            if warning:
                assert warning in result.stderr, result.stderr
            else:
                assert not result.stderr, result.stderr
        check("evio_parser,evio_common_modules,core,first,second,cli",
              ("-Pplugins=second,cli,evio_parser",))
        check("evio_parser,custom,first,evio_common_modules,second", (f"-PDEFAULT_PLUGINS:FILE={custom}",))
        check("evio_parser,custom,first,evio_common_modules,second", (f"-PJCE:CORE_CONFIG_DIR={alternate}",))
        check("evio_parser,custom,first,evio_common_modules,second",
              (f"-PJCE:CORE_CONFIG_DIR={root / 'missing'}", f"-PDEFAULT_PLUGINS:FILE={custom}"))
        check("evio_parser,evio_common_modules,core,second,first",
              updates={"JCE_CONFIG_DIR": f"{second}:{first}"})
        check("evio_parser,evio_common_modules,core", updates={"JCE_CONFIG_DIR": ""})
        check("evio_parser,evio_common_modules,core,first",
              updates={"JCE_CONFIG_DIR": f"{root / 'missing'}:{first}"}, warning="skipping")
        check("evio_parser,evio_common_modules,core", updates={"JCE_CONFIG_DIR": str(alternate / '..')})
        check("evio_parser,evio_common_modules,first,second",
              (f"-PDEFAULT_PLUGINS:FILE={root / 'missing'}",), warning="core fallback")
        original = base.read_text()
        base.write_text(" # empty\n , \n")
        check("evio_parser,evio_common_modules,first,second")
        base.unlink()
        check("evio_parser,evio_common_modules,first,second", warning="core fallback")
        base.write_text(original)
        print(f"PASS: {wrapper}")

from datetime import UTC as _UTC, datetime as _datetime
from pathlib import Path as _Path
from string import Template as _Template

###############################################################################
# apply_template()
###############################################################################

def apply_template(
    template_path: _Path,
    destination_path: _Path,
    /,
    **kw_args: str
):
    with open(template_path, "r") as fp:
        template_data = fp.read()
    output = _Template(template_data).substitute(**kw_args)
    with open(destination_path, "w") as fp:
        fp.write(output)

###############################################################################
# make_timestamp()
###############################################################################

def make_timestamp():
    return _datetime.now(_UTC)

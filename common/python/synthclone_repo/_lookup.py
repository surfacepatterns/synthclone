from collections.abc import Mapping as _Mapping, Sequence as _Sequence
from itertools import islice as _islice

from ._code import CodeComposer as _CodeComposer, indented as _indented

###############################################################################
# write_mapped_lookup_table()
###############################################################################

def write_mapped_lookup_table(
    composer: _CodeComposer,
    element_type: str,
    table_name: str,
    elements: _Mapping[str, str],
    /,
    *,
    default_value: str | None = None,
    element_count: str | None = None,
    export: bool = True,
    switch_expression: str = "n"
):
    if element_count is None:
        element_count = str(len(elements))
    if export:
        composer.write_line("export")
    composer.write_line("constexpr")
    composer.write_line(f"lookup_table<{element_type}, {element_count}>")
    composer.write_line(
        f"{table_name} = make_lookup_table<{element_count}>(")
    with _indented(composer):
        composer.write_line(
            f"[](std::size_t n) consteval -> {element_type} {{")
        with _indented(composer):
            composer.write_line(f"switch ({switch_expression}) {{")
            for k, v in elements.items():
                composer.write_line(f"case {k}:")
                with _indented(composer):
                    composer.write_line(f"return {v};")
            composer.write_line("default:")
            with _indented(composer):
                composer.write_line("break;")
            composer.write_line("}")
            if default_value is not None:
                composer.write_line(f"return {default_value};")
            else:
                composer.write_line(
                    "assume_unreachable(\"{0}: unexpected index\", n);")
        composer.write_line("}")
    composer.write_line(");")

###############################################################################
# write_simple_lookup_table()
###############################################################################

def write_simple_lookup_table(
    composer: _CodeComposer,
    element_type: str,
    table_name: str,
    elements: _Sequence[str],
    /,
    *,
    export: bool = True
):
    element_count = len(elements)
    assert element_count > 0
    if export:
        composer.write_line("export")
    composer.write_line("constexpr")
    composer.write_line(
        f"lookup_table<{element_type}, {element_count}> {table_name} {{")
    with _indented(composer):
        for element in _islice(elements, 0, element_count - 1):
            composer.write_line(f"{element},")
        composer.write_line(elements[-1])
    composer.write_line("};")

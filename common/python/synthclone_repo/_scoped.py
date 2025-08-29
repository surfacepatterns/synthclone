from collections.abc import Iterable as _Iterable, Iterator as _Iterator
from enum import Enum as _Enum
from itertools import islice as _islice, pairwise as _pairwise

from ._code import (
    CodeComposer as _CodeComposer,
    indented as _indented,
    make_string_view as _make_string_view,
    write_doc_comment as _write_doc_comment,
    write_pure_function_equality_test_case as
        _write_pure_function_equality_test_case
)
from ._lookup import write_simple_lookup_table as _write_simple_lookup_table

###############################################################################
# ScopedEnumElementTraits
###############################################################################

class ScopedEnumElementTraits:

    __slots__ = ("__description", "__identifier")

    __description: str
    __identifier: str | None

    def __init__(self, description: str, /, *, identifier: str | None = None):
        self.__description = description
        self.__identifier = identifier

    @property
    def description(self, /) -> str:
        return self.__description

    @property
    def identifier(self, /) -> str | None:
        return self.__identifier

###############################################################################
# get_scoped_enum_element_identifier()
###############################################################################

def _get_default_identifier(element: _Enum, /) -> str:
    return element.name.lower()

def get_scoped_enum_element_identifier[T: _Enum](element: T, /) -> str:
    identifier = element.value.identifier
    return _get_default_identifier(element) if identifier is None else \
        identifier

###############################################################################
# get_scoped_enum_identifier()
###############################################################################

def _get_scoped_enum_identifier_suffix(chars: _Iterable[str], /) -> str:
    return ''.join(f"_{c.lower()}" if c.isupper() else c for c in chars)

def get_scoped_enum_identifier(enum: type[_Enum], /) -> str:
    name = enum.__name__
    return f"{name[0].lower()}" \
        f"{_get_scoped_enum_identifier_suffix(_islice(name, 1, len(name)))}"

###############################################################################
# write_scoped_enum()
###############################################################################

def _generate_scoped_enum_identifier_string_view[T: _Enum](
    element: T,
    /
) -> str:
    return _make_string_view(get_scoped_enum_element_identifier(element))

def _write_scoped_enum_element[T: _Enum](
    composer: _CodeComposer,
    element: T,
    index: int,
    /,
    *,
    last: bool = False
):
    _write_doc_comment(composer, element.value.description)
    composer.write_blank_line()
    composer.write_line(
        f"{get_scoped_enum_element_identifier(element)} = "
        f"{index}{'' if last else ','}")

def _write_scoped_enum_elements[T: _Enum](
    composer: _CodeComposer,
    enum_type: type[T],
    /
):
    for index, (element, next_element) in enumerate(_pairwise(enum_type)):
        _write_scoped_enum_element(composer, element, index)
        composer.write_blank_line()
    _write_scoped_enum_element(composer, next_element, index + 1, last=True)

def write_scoped_enum[T: type[_Enum]](
    composer: _CodeComposer,
    enum_type: T,
    underlying_type: str,
    description: str,
    /,
    *,
    export: bool = True
):
    _write_doc_comment(composer, description)
    composer.write_blank_line()
    if export:
        composer.write_line("export")
    cls_name = get_scoped_enum_identifier(enum_type)
    composer.write_line(f"enum class {cls_name}: {underlying_type} {{")
    composer.write_blank_line()
    with _indented(composer):
        _write_scoped_enum_elements(composer, enum_type)
    composer.write_blank_line()
    composer.write_line("};")

    composer.write_blank_line()
    _write_doc_comment(composer, f"The number of `{cls_name}` elements")
    composer.write_blank_line()
    if export:
        composer.write_line("export")
    composer.write_line("constexpr")
    composer.write_line(f"std::size_t {cls_name}_count = {len(enum_type)};")

    composer.write_blank_line()
    _write_simple_lookup_table(
        composer, "std::string_view", f"{cls_name}_identifier_table",
        tuple(
            _generate_scoped_enum_identifier_string_view(element)
            for element in enum_type),
        export=False)

    composer.write_blank_line()
    _write_doc_comment(
        composer,
        f"Gets the identifier for the given `{cls_name}` element.\n\n"
        "@param element\n"
        f"  The `{cls_name}` element.\n\n"
        "@return\n"
        "  The identifier.")
    composer.write_blank_line()
    if export:
        composer.write_line("export")
    composer.write_line("constexpr")
    composer.write_line("std::string_view")
    composer.write_line(f"get_identifier({cls_name} element)")
    composer.write_line("{")
    with _indented(composer):
        composer.write_line(f"return {cls_name}_identifier_table.at(")
        with _indented(composer):
            composer.write_line("static_cast<std::size_t>(element));")
    composer.write_line("}")

###############################################################################
# write_scoped_enum_identifier_test_case()
###############################################################################

def write_scoped_enum_identifier_test_case[T: type[_Enum]](
    composer: _CodeComposer,
    enum_type: T,
    cls_name: str,
    /
):
    _write_pure_function_equality_test_case(
        composer, f"{cls_name}_identifiers", "synthclone::get_identifier",
        (
            (
                f"synthclone::{cls_name}::"
                f"{get_scoped_enum_element_identifier(element)}",

                _generate_scoped_enum_identifier_string_view(element)
            )
            for element in enum_type
        ))

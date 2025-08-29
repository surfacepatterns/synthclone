from collections.abc import Iterable as _Iterable
from contextlib import contextmanager as _contextmanager
from io import StringIO as _StringIO
from itertools import chain as _chain

###############################################################################
# CodeComposeError
###############################################################################

class CodeComposeError(Exception):

    __slots__ = ()

###############################################################################
# CodeComposer
###############################################################################

_INDENTATION = "    "
_SECTION_BAR = '/' * 79

class CodeComposer:

    __slots__ = ("__base_indentation_level", "__indentation_level", "__stream")

    __base_indentation_level: int
    __indentation_level: int
    __stream: _StringIO

    def __init__(self, /, *, base_indentation_level: int = 0):
        if base_indentation_level < 0:
            raise ValueError("base indentation level must be unsigned")
        self.__base_indentation_level = base_indentation_level
        self.__indentation_level = base_indentation_level
        self.__stream = _StringIO()

    def __str__(self, /):
        return self.__stream.getvalue()

    def dedent(self, /):
        base = self.__base_indentation_level
        level = self.__indentation_level
        if level == base:
            raise CodeComposeError(
                "cannot dedent when indentation level is {base}")
        self.__indentation_level = level - 1

    def indent(self, /):
        self.__indentation_level += 1

    def write_blank_line(self, /):
        self.__stream.write('\n')

    def write_line(self, s: str, /):
        self.__stream.write(
            f"{_INDENTATION * self.__indentation_level}{s}\n")

###############################################################################
# indented()
###############################################################################

@_contextmanager
def indented(composer: CodeComposer, /):
    composer.indent()
    try:
        yield
    finally:
        composer.dedent()

###############################################################################
# make_bool_literal()
###############################################################################

def make_bool_literal(b: bool, /) -> str:
    return 'true' if b else 'false'

###############################################################################
# make_char_literal()
###############################################################################

_ASCII_CHAR_ESCAPES = dict(
    _chain(

        # ASCII control characters that are represented as hexadecimal.
        (
            (chr(i), f"\\x{hex(i)[2:].rjust(2, '0')}")
            for i in _chain(range(0x09), range(0x0e, 0x20), (0xb, 0xc, 0x7f))
        ),

        # ASCII control characters that have friendlier (?) representations.
        (
            ("\'", "\\\'"),
            ("\"", "\\\""),
            ("\\", "\\\\"),
            ("\a", "\\a"),
            ("\b", "\\b"),
            ("\f", "\\f"),
            ("\n", "\\n"),
            ("\r", "\\r"),
            ("\t", "\\t"),
            ("\v", "\\v")
        ),

        # Trigraphs!
        (("?", "\\?"),)

    ))

def _escape_char(c: str) -> str:
    return _ASCII_CHAR_ESCAPES.get(c, c) if ord(c) <= 0x7f else \
        f"\\U{hex(ord(c))[2:].rjust(8, '0')}"

def make_char_literal(c: str) -> str:
    if len(c) != 1:
        raise ValueError(f"{0!r}: string length must be 1")
    return f"\'{_escape_char(c)}\'"

###############################################################################
# make_string_literal()
###############################################################################

def make_string_literal(s: str) -> str:
    return f"\"{''.join(_escape_char(c) for c in s)}\""

###############################################################################
# make_string_view()
###############################################################################

def make_string_view(s: str) -> str:
    return f"std::string_view({make_string_literal(s)})"

###############################################################################
# write_doc_comment()
###############################################################################

def write_doc_comment(composer: CodeComposer, text: str, /):
    composer.write_line("/**")
    for s in text.splitlines():
        composer.write_line(f" * {s}")
    composer.write_line(" */")

###############################################################################
# write_plain_comment()
###############################################################################

def write_plain_comment(composer: CodeComposer, text: str, /):
    for s in text.splitlines():
        composer.write_line(f"// {s}")

###############################################################################
# write_section_comment()
###############################################################################

def write_section_comment(composer: CodeComposer, text: str, /):
    composer.write_line(_SECTION_BAR)
    write_plain_comment(composer, text)
    composer.write_line(_SECTION_BAR)

###############################################################################
# write_compact_test_case()
###############################################################################

def write_compact_test_case(composer: CodeComposer, name: str, body: str, /):
    composer.write_line(f"BOOST_AUTO_TEST_CASE({name}){{{body}}}")

###############################################################################
# writing_test_case()
###############################################################################

@_contextmanager
def writing_test_case(composer: CodeComposer, name: str, /):
    composer.write_line(f"BOOST_AUTO_TEST_CASE({name})")
    composer.write_line("{")
    try:
        with indented(composer):
            yield
    finally:
        composer.write_line("}")

###############################################################################
# writing_test_suite()
###############################################################################

@_contextmanager
def writing_test_suite(composer: CodeComposer, name: str, /):
    composer.write_line(f"BOOST_AUTO_TEST_SUITE({name})")
    try:
        yield
    finally:
        composer.write_line("BOOST_AUTO_TEST_SUITE_END()")

###############################################################################
# write_pure_function_equality_test_case()
###############################################################################

def write_pure_function_equality_test_case(
    composer: CodeComposer,
    test_case_name: str,
    function_name: str,
    elements: _Iterable[tuple[str, str]],
    /
):
    with writing_test_case(composer, test_case_name):
        for params_str, expected_result in elements:
            composer.write_line(
                f"synthclone::verify_eq({function_name}({params_str}), "
                f"{expected_result});")

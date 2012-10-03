"""The main template parser."""

import os, re
from functools import partial

from .errors import TemplateCompileError
from .base import expression_evaluator, Template, TemplateFragment
from .macros import DEFAULT_MACROS
        
        
def string_node(value, context):
    """A node containing a static string value."""
    context.buffer.append(value)


def expression_node(evaluate, context):
    """A node that evaluates and prints the given expression."""
    value = str(evaluate(context))
    # Apply autoescaping.
    autoescape = context.meta.get("__autoescape__")
    if autoescape:
        value = autoescape(value)
    # Write the value.
    context.buffer.append(value)


RE_TOKEN = re.compile(r"{#.+?#}|{{\s*(.*?)\s*}}|{%\s*(.*?)\s*%}|\n[ \t]*%%[ \t]*([^\n]+)[ \t]*|\n[ \t]*##[ \t]*[^\n]+[ \t]*", re.DOTALL)


def tokenize(template):
    """Lexes the given template, returning an iterator or token."""
    lineno = 1
    index = 0
    for match in RE_TOKEN.finditer(template):
        # Process string tokens.
        if match.start() > index:
            string_token = template[index:match.start()]
            yield lineno, "STRING", string_token
            lineno += string_token.count("\n")
        # Process tag tokens.
        expression_token, macro_token, line_macro_token = match.groups()
        # Process expression tokens.
        if expression_token:
            yield lineno, "EXPRESSION", expression_token
            lineno += expression_token.count("\n")
        elif macro_token:
            yield lineno, "MACRO", macro_token
            lineno += macro_token.count("\n")
        elif line_macro_token:
            yield lineno, "MACRO", line_macro_token
            lineno += 1
        # Update the index.
        index = match.end()
    # Yield the final string token.
    yield lineno, "STRING", template[index:]


class ParserRun:
    
    """The state held by a parser during a run."""
    
    __slots__ = ("tokens", "name", "macros", "meta",)
    
    def __init__(self, template, name, macros):
        """Initializes the ParserRun."""
        self.tokens = tokenize(template)
        self.name = name
        self.macros = macros
        self.meta = {}
    
    def parse_template_chunk(self, end_chunk_handler):
        """
        Parses as many nodes as possible until an unknown block is reached.
        Returns the result of the end_chunk_handler.
        
        The chunk is ended when there is no more template to parse, or an unknown
        macro is encountered. Once this occurs, the end_chunk_handler is called
        with macro_name and nodes as positional arguments. The end_chunk handler
        must then process the nodes and return a result.
        """
        nodes = []
        for lineno, token_type, token_contents in self.tokens:
            try:
                if token_type == "STRING":
                    node = partial(string_node, token_contents)
                elif token_type == "EXPRESSION":
                    node = partial(expression_node, expression_evaluator(token_contents))
                elif token_type == "MACRO":
                    # Process macros.
                    node = None
                    for macro in self.macros:
                        node = macro(self, token_contents)
                        if node:
                            break
                    if not node:
                        return end_chunk_handler(token_contents, nodes)
                else:
                    assert False, "{!r} is not a valid token type.".format(token_type)
                # Set the node line number.
                nodes.append((lineno, node))
            except TemplateCompileError:
                raise
            except Exception as ex:
                raise TemplateCompileError(str(ex), self.name, lineno) from ex
        # No unknown macro.
        return end_chunk_handler(None, nodes)
        
    def parse_all_nodes(self):
        """Parses all remaining nodes."""
        def end_chunk_handler(token_contents, nodes):
            if token_contents:
                raise SyntaxError("{{% {} %}} is not a recognized tag.".format(token_contents))
            return nodes
        return self.parse_template_chunk(end_chunk_handler)
        
    def parse_block(self, start_tag, end_tag, regex):
        """
        Parses a block of template, looking for a macro token that matches the
        given regex.
        
        If a match is found, then a tuple of (match, template_fragment) is
        returned. A SyntaxError is raised.
        """
        def end_chunk_handler(token_contents, nodes):
            if not token_contents:
                raise SyntaxError("{{% {} %}} tag could not find a corresponding {{% {} %}}.".format(start_tag, end_tag))
            # Attempt to match.
            match = regex.match(token_contents)
            if not match:
                raise SyntaxError("{{% {} %}} is not a recognized tag.".format(token_contents))
            return match, TemplateFragment(nodes, self.name)
        return self.parse_template_chunk(end_chunk_handler)


# Ported from python 3.2 html module. In future versions, we'll just use the builtin.
_escape_map = {
    ord("&"): "&amp;",
    ord("<"): "&lt;",
    ord(">"): "&gt;",
    ord('"'): "&quot;",
    ord("'"): "&#x27;",
}

def escape_html(value):
    """Replaces all HTML special characters with the appropriate entities."""
    return value.translate(_escape_map)


# Default rules for autoescaping templates based on name.
DEFAULT_AUTOESCAPE_FUNCS = {
    ".xml": escape_html,
    ".xhtml": escape_html,
    ".html": escape_html,
    ".htm": escape_html,
}


class Parser:
    
    """A template parser."""
    
    __slots__ = ("_macros", "_autoescape_funcs",)
    
    def __init__(self, macros, autoescape_funcs=DEFAULT_AUTOESCAPE_FUNCS):
        """Initializes the Parser."""
        self._macros = macros
        self._autoescape_funcs = autoescape_funcs
        
    def compile(self, template, name="__string__", params=None, meta=None):
        """Compiles the template."""
        # Get the autoescape function.
        _, extension = os.path.splitext(name)
        autoescape = self._autoescape_funcs.get(extension)
        # Compile the meta params.
        default_meta = {
            "__name__": name,
            "__autoescape__": autoescape,
        }
        default_meta.update(meta or {})
        # Get the default params.
        params = params or {}
        # Render the main block.
        nodes = ParserRun(template, name, self._macros).parse_all_nodes()
        return Template(nodes, name, params, default_meta)
        
        
# The default parser, using the default set of macros.
default_parser = Parser(DEFAULT_MACROS)
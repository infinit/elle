"""The default built-in macros."""

import re
from functools import partial

from .base import expression_evaluator, name_setter, Template


def regex_macro(regex):
    """A decorator that defines a macro function."""
    regex = re.compile(regex, re.DOTALL)
    def decorator(func):
        def wrapper(parser, token):
            match = regex.match(token)
            if match:
                return func(parser, *match.groups(), **match.groupdict())
            return None
        return wrapper
    return decorator
        
        
def set_node(evaluate, set_name, context):
    """A node that sets a parameter in the context."""
    set_name(context, evaluate(context))
        

@regex_macro("^set\s+(.+?)\s+as\s+(.+?)$")
def set_macro(parser, expression, name):
    """Macro that allows setting of a value in the context."""
    return partial(set_node, expression_evaluator(expression), name_setter(name))


def print_node(evaluate, context):
    """A node that renders an expression without autoescaping."""
    context.buffer.append(str(evaluate(context)))
        
        
@regex_macro("^print\s+(.+?)$")
def print_macro(parser, expression):
    """Macro that allows an expression to be rendered without autoescaping."""
    return partial(print_node, expression_evaluator(expression))


def import_node(statement, context):
    """A node that executes the given import expression."""
    exec(statement, context.meta, context.params)
        
        
@regex_macro("(^from\s+.+?\s+import\s+.+?$|^import\s+.+?$)")
def import_macro(parser, statement):
    "Macro that implements an import statment."
    return partial(import_node, compile(statement, "<string>", "exec"))


def if_node(clauses, else_block, context):
    """A node that implements an 'if' expression."""
    for evaluate, block in clauses:
        if evaluate(context):
            block._render_to_context(context)
            return
    if else_block:
        else_block._render_to_context(context)


RE_IF_CLAUSE = re.compile("^(elif) (.+?)$|^(else)$|^(endif)$")

@regex_macro("^if\s+(.+?)$")
def if_macro(parser, expression):
    """A macro that implements an 'if' expression."""
    clauses = []
    else_tag = False
    else_block = None
    while True:
        match, block = parser.parse_block("if", "endif", RE_IF_CLAUSE)
        if else_tag:
            else_block = block
        else:
            clauses.append((expression_evaluator(expression), block))
        elif_flag, elif_expression, else_flag, endif_flag = match.groups()
        if elif_flag:
            if else_tag:
                raise SyntaxError("{{% elif %}} tag cannot come after {{% else %}}.")
            expression = elif_expression
        elif else_flag:
            if else_tag:
                raise SyntaxError("Only one {{% else %}} tag is allowed per {{% if %}} macro.")
            else_tag = True
        elif endif_flag:
            break
    return partial(if_node, clauses, else_block)
    
    
def for_node(set_name, evaluate, block, context):
    """A node that implements a 'for' loop."""
    items = evaluate(context)
    for item in items:
        set_name(context, item)
        block._render_to_context(context)


RE_ENDFOR = re.compile("^endfor$")

@regex_macro("^for\s+(.+?)\s+in\s+(.+?)$")
def for_macro(parser, name, expression):
    """A macro that implements a 'for' loop."""
    match, block = parser.parse_block("for", "endfor", RE_ENDFOR)
    return partial(for_node, name_setter(name), expression_evaluator(expression), block)


def py_node(code, context):
    """A node that allows arbitrary python to be executed."""
    exec(code, context.meta, context.params)


@regex_macro("^py\s(.+?)$")
def py_macro(parser, code):
    """Macro that allows arbitrary python to be executed."""
    # Fix indentation in the code.
    raw_lines = code.splitlines()
    code_lines = []
    while raw_lines:
        raw_line = raw_lines.pop(0)
        code_line = raw_line.lstrip()
        if code_line:
            indent = len(raw_line) - len(code_line)
            code_lines.append(code_line)
            break
    for raw_line in raw_lines:
        code_lines.append(raw_line[indent:])
    # Compile the node renderer.
    return partial(py_node, compile("\n".join(code_lines), "<string>", "exec"))
    
    
def get_template(context, template):
    """
    If template is a str, then looks up the loader and loads the template.

    Otherwise, if template is a template, returns the template.
    """
    if isinstance(template, Template):
        return template
    if isinstance(template, str):
        loader = context.meta.get("__loader__")
        if not loader:
            raise ValueError("Cannot load {!r} by name, as this template was not compiled using a template loader.".format(template))
        return loader.load(template)
    raise TypeError("Expected a Template or a str, found {!r}.".format(template))


def include_node(evaluate, context):
    """A node that implements an 'include' expression."""
    template = get_template(context, evaluate(context))
    template._render_to_sub_context(context, {})


@regex_macro("^include\s+(.+?)$")
def include_macro(parser, expression):
    """Macro that implements an 'include' expression."""
    return partial(include_node, expression_evaluator(expression))


def block_node(name, block, context):
    """A block of inheritable content."""
    # Get the block stack.
    block_stack = [(context, block)]
    child_context = context
    while "__child__" in child_context.meta:
        child_context = child_context.meta["__child__"]
        block = child_context.meta["__blocks__"].get(name)
        if block:
            block_context = child_context
            block_stack.append((block_context, block))
    # Render the topmost block.
    block_context, block = block_stack.pop()
    sub_context = block_context.sub_context(meta={"__parent_blocks__": block_stack})
    block._render_to_context(sub_context)


@regex_macro("^block\s+([a-zA-Z_][a-zA-Z_\-0-9]*)$")
def block_macro(parser, name):
    """Macro that implements an inheritable template block."""
    match, block = parser.parse_block("block", "endblock", re.compile("^endblock$|^endblock\s+{}$".format(name)))
    # Register with the parser.
    block_meta = parser.meta.get("__blocks__") or parser.meta.setdefault("__blocks__", {})
    if name in block_meta:
        raise SyntaxError("Multiple blocks named {!r} are not allowed in a child template.".format(name))
    block_meta[name] = block
    # Return the node.
    return partial(block_node, name, block)


def super_node(context):
    """A node that renders the parent block's content."""    
    if "__parent_blocks__" in context.meta:
        block_stack = context.meta["__parent_blocks__"][:]
        try:
            block_context, block = block_stack.pop()
        except Indexerror:
            pass
        else:
            sub_context = block_context.sub_context(meta={"__parent_blocks__": block_stack})
            block._render_to_context(sub_context)
    
    
@regex_macro("^super$")
def super_macro(parser):
    """Macro that renders the parent block's content."""
    return super_node


def extends_node(evaluate, block_nodes, context):
    """Implements a inherited child template."""
    # Create a summary of my blocks.
    context.meta["__blocks__"] =  block_nodes
    # Render the parent template with my blocks.
    template = get_template(context, evaluate(context))
    template._render_to_sub_context(context, {"__child__": context})


@regex_macro("^extends\s+(.+?)$")
def extends_macro(parser, expression):
    """Macro that implements an inherited child template."""
    # Parse the rest of the template.
    nodes = parser.parse_all_nodes()
    # Go through the nodes, looking for all block tags.
    block_nodes = parser.meta.get("__blocks__") or parser.meta.setdefault("__blocks__", {})
    return partial(extends_node, expression_evaluator(expression), block_nodes)


# The set of default macros.
DEFAULT_MACROS = (set_macro, print_macro, import_macro, if_macro, for_macro, py_macro, include_macro, block_macro, super_macro, extends_macro,)
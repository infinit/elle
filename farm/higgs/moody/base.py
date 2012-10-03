"""Base classes used by the template engine."""


import re

from .errors import TemplateRenderError
    
    
class Context:

    """The state of a template during render time."""

    __slots__ = ("params", "meta", "buffer")

    def __init__(self, params, meta, buffer):
        """Initializes the Context."""
        self.params = params
        self.meta = meta
        self.buffer = buffer
        
    def sub_context(self, params=None, meta=None):
        """
        Creates a new subcontext that is scoped to a block.

        Changes to the sub-context will not affect the parent context, although
        the buffer is shared.
        """
        sub_params = self.params.copy()
        sub_params.update(params or {})
        sub_meta = self.meta.copy()
        sub_meta.update(meta or {})
        return Context(sub_params, sub_meta, self.buffer)
        
    def read(self):
        """Reads the contents of the buffer as a string."""
        return "".join(self.buffer)


RE_NAME = re.compile("^[a-zA-Z_][a-zA-Z_0-9]*$")


def name_setter(name):
    """
    Returns a function that will assign a value to a name in a given context.
    
    The returned function has a signature of set_name(context, value).
    """
    # Parse the names.
    if "," in name:
        names = [name.strip() for name in name.split(",")]
        if not names[-1]:
            names.pop()
        def setter(context, value):
            # Handle variable expansion.
            value = iter(value)
            for name_part in names:
                try:
                    context.params[name_part] = next(value)
                except StopIteration:
                    raise ValueError("Not enough values to unpack.")
            # Make sure there are no more values.
            try:
                next(value)
            except StopIteration:
                pass
            else:
                raise ValueError("Need more than {} values to unpack.".format(len(names)))
    else:
        names = (name,)
        def setter(context, value):
            context.params[name] = value
    # Make sure that the names are valid.
    for name in names:
        if not RE_NAME.match(name):
            raise ValueError("{!r} is not a valid variable name. Only letters, numbers and undescores are allowed.".format(name))
    # Return the setter.
    return setter
        
        
def expression_evaluator(expression):
    expression = compile(expression, "<string>", "eval")
    def evaluator(context):
        return eval(expression, context.meta, context.params)
    return evaluator


class TemplateFragment:

    """A fragment of a template."""

    __slots__ = ("_nodes", "_name",)

    def __init__(self, nodes, name):
        """Initializes the TemplateFragment."""
        self._nodes = nodes
        self._name = name
    
    def _render_to_context(self, context):
        """Renders the template to the given context."""
        for lineno, node in self._nodes:
            try:
                node(context)
            except TemplateRenderError:
                raise
            except Exception as ex:
                raise TemplateRenderError(str(ex), self._name, lineno) from ex


class Template(TemplateFragment):

    """A compiled template."""

    __slots__ = ("_params", "_meta",)

    def __init__(self, nodes, name, params, meta):
        """Initializes the template."""
        super(Template, self).__init__(nodes, name)
        self._params = params
        self._meta = meta

    def _render_to_sub_context(self, context, meta):
        """Renders the template to the given context."""
        # Generate the params.
        sub_params = self._params.copy()
        sub_params.update(context.params)
        # Generate the meta.
        sub_meta = self._meta.copy()
        sub_meta.update(meta)
        # Generate the sub context.
        self._render_to_context(context.sub_context(sub_params, sub_meta))

    def render(self, **params):
        """Renders the template, returning the string result."""
        # Create the params.
        context_params = self._params.copy()
        context_params.update(params)
        # Create the context.
        context = Context(context_params, self._meta, [])
        # Render the template.
        self._render_to_context(context)
        return context.read()
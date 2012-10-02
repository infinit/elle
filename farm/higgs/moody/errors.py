"""Errors used by the template engine."""


class TemplateError(Exception):
    
    """An error occured in a template."""
    
    def __init__(self, message, template_name, template_lineno):
        """Initializes the TemplateError."""
        super(TemplateError, self).__init__(message)
        self.template_name = template_name
        self.template_lineno = template_lineno
        
    def __str__(self):
        """Returns a string representation."""
        message = super(TemplateError, self).__str__()
        return "{} [{!r} on line {}]".format(message, self.template_name, self.template_lineno)


class TemplateRenderError(TemplateError):

    """Something went wront while rendering a template."""

        
class TemplateCompileError(TemplateError):
    
    """Something went wrong while compiling a template."""
import unittest, sys

import moody
from moody.errors import TemplateRenderError, TemplateCompileError
from moody.loader import TemplateDoesNotExist, MemorySource


class TestRender(unittest.TestCase):
    
    def testCommentTag(self):
        self.assertEqual(moody.render("{# A comment. #}"), "")
        self.assertEqual(moody.render("{# Multi\nLine\nComment #}"), "")
        
    def testStringTag(self):
        self.assertEqual(moody.render("Hello world"), "Hello world")
        
    def testExpressionTag(self):
        self.assertEqual(moody.render("{{'Hello world'}}"), "Hello world")
        self.assertEqual(moody.render("{{('Hello '\n'world')}}"), "Hello world")
    
    def testSetMacro(self):
        self.assertEqual(moody.render("{% set 'foo' as test %}{{test}}"), "foo")
        self.assertEqual(moody.render("{% set 'foo', 'bar', as test1, test2 %}{{test1}}{{test2}}"), "foobar")
        self.assertEqual(moody.render("{% set 'foo', as test1, %}{{test1}}"), "foo")
        self.assertRaises(TemplateRenderError, lambda: moody.render("{% set 'foo', as foo, bar %}"))
        self.assertRaises(TemplateRenderError, lambda: moody.render("{% set 'foo', 'bar'  as bar, %}"))
    
    def testImportMacro(self):
        self.assertEqual(moody.render("{% from operator import add %}{{add(1,1)}}"), "2")
        self.assertEqual(moody.render("{% import operator %}{{operator.add(1,1)}}"), "2")
        
    def testIfMacro(self):
        # Test single if.
        template1 = moody.compile("{% if test == 'foo' %}foo{% endif %}")
        self.assertEqual(template1.render(test="foo"), "foo")
        self.assertEqual(template1.render(test="bar"), "")
        # Test if and else.
        template2 = moody.compile("{% if test == 'foo' %}foo{% else %}bar{% endif %}")
        self.assertEqual(template2.render(test="foo"), "foo")
        self.assertEqual(template2.render(test="bar"), "bar")
        # Test if, elif and else.
        template3 = moody.compile("{% if test == 'foo' %}foo{% elif test == 'bar' %}bar{% else %}snafu{% endif %}")
        self.assertEqual(template3.render(test="foo"), "foo")
        self.assertEqual(template3.render(test="bar"), "bar")
        self.assertEqual(template3.render(test="snafu"), "snafu")
        # Test if and elif
        template4 = moody.compile("{% if test == 'foo' %}foo{% elif test == 'bar' %}bar{% elif test == 'snafu' %}snafu{% endif %}")
        self.assertEqual(template4.render(test="foo"), "foo")
        self.assertEqual(template4.render(test="bar"), "bar")
        self.assertEqual(template4.render(test="snafu"), "snafu")
        self.assertEqual(template4.render(test="wibble"), "")
        # Test various syntax errors.
        self.assertRaises(TemplateCompileError, lambda: moody.compile("{% if True %}"))
        self.assertRaises(TemplateCompileError, lambda: moody.compile("{% if True %}{% else %}{% elif True %}{% endif %}"))
        self.assertRaises(TemplateCompileError, lambda: moody.compile("{% if True %}{% else %}{% else %}{% endif %}"))
    
    def testForMacro(self):
        # Test basic functionality.
        template1 = moody.compile("{% for n in range(0, 3) %}{{n}}{% endfor %}")
        self.assertEqual(template1.render(), "012")
        # Test various syntax errors.
        self.assertRaises(TemplateCompileError, lambda: moody.compile("{% for n in range(0, 3) %}"))
        # Test variable expansion.
        template2 = moody.compile("{% for n, m in value %}{{n}}{{m}}{% endfor %}")
        self.assertEqual(template2.render(value=[["foo", "bar"]]), "foobar")
        self.assertRaises(TemplateRenderError, lambda: template2.render(value=[["foo"]]))
        self.assertRaises(TemplateRenderError, lambda: template2.render(value=[["foo", "bar", "foobar"]]))
    
    def testPyMacro(self):
        template1 = moody.compile("""
            {% py items = range(10) %}
            {% for n in items %}{{n}}{% endfor %}
        """)
        self.assertEqual(template1.render().strip(), "0123456789")
        template2 = moody.compile("""
            {% py
            
                items = []
                for n in range(10):
                    items.append(n)
            %}
            {% for n in items %}{{n}}{% endfor %}
        """)
        self.assertEqual(template2.render().strip(), "0123456789")
    
    def testIncludeMacro(self):
        template1 = moody.compile("Foo")
        template2 = moody.compile("{% include inner %}")
        self.assertEqual(template1.render(), template2.render(inner=template1))
        
    def testInheritance(self):
        parent_template = moody.compile("Hello {% block name %}world{% endblock %}")
        child_template = moody.compile("{% extends parent %}{% block name %}Dave {% block surname %}Hall{% endblock %}{% endblock %}")
        grandchild_template = moody.compile("{% extends child %}{% block surname %}Foo{% endblock %}")
        self.assertEqual(parent_template.render(), "Hello world")
        self.assertEqual(child_template.render(parent=parent_template), "Hello Dave Hall")
        self.assertEqual(grandchild_template.render(parent=parent_template, child=child_template), "Hello Dave Foo")
        
    def testNestedTags(self):
        template1 = moody.compile("""
            {% if test.startswith("foo") %}
                {% if test == "foobar" %}
                    {{test}}
                {% else %}
                    {% for item in range(2) %}{% if True %}{{test}}{% endif %}{% endfor %}
                {% endif %}
            {% else %}
                snafu
            {% endif %}
        """)
        self.assertEqual(template1.render(test="foobar").strip(), "foobar")
        self.assertEqual(template1.render(test="foo").strip(), "foofoo")
        self.assertEqual(template1.render(test="").strip(), "snafu")
    
    def testLineMacros(self):
        template = moody.compile("""
            %% if test.startswith("foo")
                {% if test.endswith("bar") %}{{test}}{% endif %}
            %% endif
        """)
        self.assertEqual(template.render(test="foobar"), """
                foobar
        """)
        
    def testLineComments(self):
        template = moody.compile("""
            %% if test.startswith("foo")
                ## This comment should be ignored.
                {% if test.endswith("bar") %}{{test}}{% endif %}
            %% endif
        """)
        self.assertEqual(template.render(test="foobar"), """
                foobar
        """)
        
    def testAutoescape(self):
        template1 = moody.compile("{{value}}{% print value %}", name="test.html")
        self.assertEqual(template1.render(value="<foo bar='bar' baz=\"baz\">"), "&lt;foo bar=&#x27;bar&#x27; baz=&quot;baz&quot;&gt;<foo bar='bar' baz=\"baz\">")
        
    def testDefaultParams(self):
        template1 = moody.compile("{{test}}", params={"test": "foo"})
        self.assertEqual(template1.render(), "foo")
        self.assertEqual(template1.render(test="bar"), "bar")


test_loader = moody.make_loader(
    MemorySource({
        "simple.html": "{{test}}",
        "include.txt": "{% include 'simple.txt' %}",
        "parent.txt": "Hello {% block name %}world{% endblock %}",
        "child.txt": "{% extends 'parent.txt' %}{% block name %}Dave {% block surname %}Hall{% endblock %}{% endblock %}",
        "grandchild.txt": "{% extends 'child.txt' %}{% block surname %}Foo{% endblock surname %}",
        "override.txt": "Bar",
        "inherit.txt": "{% extends __super__ %}{% block surname %}Foo{% endblock %}",
        "scoped_meta_parent.txt": "{% block name %}{{__name__}}{% endblock %}",
        "scoped_meta_child.txt": "{% extends 'scoped_meta_parent.txt' %}{% block name %}{% super %} {{__name__}}{% endblock %}",
        "scoped_meta_grandchild.txt": "{% extends 'scoped_meta_child.txt' %}{% block name %}{% super %} {{__name__}}{% endblock %}",
        "super_block_parent.txt": "{% block name %}Dave{% endblock %}",
        "super_block_child.txt": "{% extends 'super_block_parent.txt' %}{% block name %}{% super %} Hall{% endblock %}",
        "super_block_grandchild.txt": "{% extends 'super_block_child.txt' %}{% block name %}{% super %} the great{% endblock %}",
    }), MemorySource({
        "simple.txt": "{{test}}",
        "overide.txt": "Foo",
        "inherit.txt": "{% extends __super__ %}{% block name %}Dave {% block surname %}Hall{% endblock %}{% endblock %}",
    }), MemorySource({
        "inherit.txt": "Hello {% block name %}world{% endblock %}",
    })
)


class TestLoader(unittest.TestCase):
    
    def setUp(self):
        test_loader.clear_cache()
    
    def testLoad(self):
        self.assertTrue(test_loader.load("simple.txt"))
        
    def testAutoescape(self):
        self.assertEqual(test_loader.render("simple.txt", test="<Hello world>"), "<Hello world>")
        self.assertEqual(test_loader.render("simple.html", test="<Hello world>"), "&lt;Hello world&gt;")
        
    def testNameStacking(self):
        self.assertEqual(test_loader.render("missing.txt", "simple.txt", test="foo"), "foo")
        
    def testTemplateDoesNotExist(self):
        self.assertRaises(TemplateDoesNotExist, lambda: test_loader.load("missing.txt"))
        
    def testIncludeTag(self):
        self.assertEqual(test_loader.render("include.txt", test="foo"), test_loader.render("simple.txt", test="foo"))
        
    def testInheritance(self):
        self.assertEqual(test_loader.render("parent.txt"), "Hello world")
        self.assertEqual(test_loader.render("child.txt"), "Hello Dave Hall")
        self.assertEqual(test_loader.render("grandchild.txt"), "Hello Dave Foo")
        
    def testCache(self):
        self.assertEqual(len(test_loader._cache), 0)
        test_loader.load("simple.txt")
        self.assertEqual(len(test_loader._cache), 1)
        test_loader.load("simple.txt")
        self.assertEqual(len(test_loader._cache), 1)
    
    def testOverride(self):
        self.assertEqual(test_loader.render("override.txt"), "Bar")
        
    def testSelfInherit(self):
        self.assertEqual(test_loader.render("inherit.txt"), "Hello Dave Foo")
        
    def testScopedMeta(self):
        self.assertEqual(test_loader.render("scoped_meta_parent.txt"), "scoped_meta_parent.txt")
        self.assertEqual(test_loader.render("scoped_meta_child.txt"), "scoped_meta_parent.txt scoped_meta_child.txt")
        self.assertEqual(test_loader.render("scoped_meta_grandchild.txt"), "scoped_meta_parent.txt scoped_meta_child.txt scoped_meta_grandchild.txt")
        
    def testSuperBlock(self):
        self.assertEqual(test_loader.render("super_block_parent.txt"), "Dave")
        self.assertEqual(test_loader.render("super_block_child.txt"), "Dave Hall")
        self.assertEqual(test_loader.render("super_block_grandchild.txt"), "Dave Hall the great")
        
    def testStandaloneCompile(self):
        self.assertEqual(test_loader.compile("{% include 'simple.txt' %}").render(test="foo"), "foo")

        
class TestDirectorySource(unittest.TestCase):
    
    def testLoad(self):
        loader = moody.make_loader(*sys.path)
        with open(moody.__file__, "r") as expected:
            self.assertEqual(loader.render("moody/__init__.py"), expected.read())


class TestErrorReporting(unittest.TestCase):
    
    def testCompileError(self):
        try:
            moody.compile("{% if foo %}\n{{foo}}\n{% else %}\n{% flobble %}\n{% endif %}", name="foo")
        except TemplateCompileError as ex:
            self.assertEqual(ex.template_lineno, 4)
            self.assertEqual(ex.template_name, "foo")
            self.assertTrue(isinstance(ex.__cause__, SyntaxError))
            
    def testRenderError(self):
        try:
            moody.compile("{% if foo %}\n{{bar}}\n{% else %}\nHello world\n{% endif %}", name="foo").render(foo="foo")
        except TemplateRenderError as ex:
            self.assertEqual(ex.template_lineno, 2)
            self.assertEqual(ex.template_name, "foo")
            self.assertTrue(isinstance(ex.__cause__, NameError))
        
        
if __name__ == "__main__":
    unittest.main()
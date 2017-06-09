import drake
import mistune
import re

non_unicode_word_character = re.compile('\W')

def id(text):
  return re.sub(non_unicode_word_character, '-', text.lower())

class HeaderIdsRenderer(mistune.Renderer):

  def header(self, text, level, raw = None):
    header = super().header(text, level, raw)
    position = 3 # 1 + len('h%d' % level)
    return header[0:position] + ' id="' + id(text) + '"' + header[position:]

class Renderer(drake.Builder):

  def __init__(
    self, source,
    target = None,
    replace_before = {
      '_blank': '&lowbar;blank',
    },
    replace_after = {
      '<code>--': '<code>&#8209;&#8209;',
      '&quot;': '\"',
    },
  ):
    self.__source = source
    self.__target = target or drake.Node(
      self.__source.name_relative.with_extension('html')
    )
    self.__replace_before = replace_before
    self.__replace_after = replace_after
    drake.Builder.__init__(self, [self.__source], [self.__target])

  def execute(self):
    self.output('Render %s' % self.__target)
    renderer = HeaderIdsRenderer()
    markdown = mistune.Markdown(
      renderer = renderer,
      parse_inline_html = True,
    )
    with open(str(self.__source.path()), encoding = 'utf-8') as input:
      with open(str(self.__target.path()), 'w', encoding = 'utf-8') as output:
        text = input.read()
        for k, v in self.__replace_before.items():
          text = text.replace(k, v)
        html = markdown(text)
        for k, v in self.__replace_after.items():
          html = html.replace(k, v)
        output.write(html)
    return True

class Source(drake.Node):

  def __init__(self, *args, **kwargs):
    super().__init__(*args, **kwargs)
    Renderer(self)

drake.Node.extensions['md'] = Source

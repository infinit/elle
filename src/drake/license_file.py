import drake
import drake.git
import os

class Packager(drake.Builder):

  """
  Only the name of the folder containing the licenses needs to be passed.
  The builder will automatically populate the list of source nodes by traversing
  the folder.
  """

  def __init__(self, license_folder, out_file):
    self.__license_folder = license_folder
    self.__target = out_file
    self.__context = drake.Drake.current.prefix
    licenses = list()
    def traverse(folder, in_dir):
      rel_dir = '%s/%s' % (in_dir, folder) if folder else in_dir
      git = drake.git.Git(rel_dir)
      for f in git.ls_files():
        path = str(drake.path_source() / self.__context / rel_dir / f)
        if os.path.isdir(path):
          traverse(f, rel_dir)
        else:
          licenses.append(drake.node('%s/%s' % (rel_dir, f)))
    traverse('', license_folder)
    super().__init__(licenses, [out_file])
    self.__sorted_sources = \
      list(map(lambda s: str(s), self.sources().values()))
    self.__sorted_sources.sort(key = lambda s: s.lower())

  def execute(self):
    print('Generating aggregated license file: %s' % self.__target)
    with open(str(self.__target), 'w', encoding = 'utf-8') as out:
      for license in self.__sorted_sources:
        l_name = license.replace(
          '%s/%s/' % (self.__context, self.__license_folder), '')
        out.write('# Begin: %s\n(*%s\n' % (l_name, 78 * '-'))
        l_file = str(drake.path_source() / license)
        with open(l_file, 'r', encoding = 'utf-8') as f:
          out.write(f.read())
        out.write('\n%s*)\n# End: %s\n\n' % (78 * '-', l_name))
    return True

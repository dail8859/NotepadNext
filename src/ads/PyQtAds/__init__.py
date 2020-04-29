from .rc import *
from ._version import get_versions
__version__ = get_versions()['version']
short_version = __version__
version = __version__
full_version = __version__
git_revision = get_versions()['full-revisionid']
release = not get_versions()['dirty']
del get_versions
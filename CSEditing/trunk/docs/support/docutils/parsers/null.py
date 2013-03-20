# Author: Martin Blais
# Contact: blais@furius.ca
# Revision: $Revision: 21817 $
# Date: $Date: 2005-07-21 22:39:57 +0200 (Thu, 21 Jul 2005) $
# Copyright: This module has been placed in the public domain.

"""A do-nothing parser."""

from docutils import parsers


class Parser(parsers.Parser):

    """A do-nothing parser."""

    supported = ('null',)

    config_section = 'null parser'
    config_section_dependencies = ('parsers',)

    def parse(self, inputstring, document):
        pass

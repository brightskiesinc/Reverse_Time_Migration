#!/usr/bin/env python
# coding: utf8

"""
loader.py:
Loading module.
"""

__author__ = "Brightskies inc."
__license__ = "LGPL-3.0 License"


class Loader:
    """Loader"""

    def __init__(self, file_path):
        """
        Constructor
        :param file_path: File path to load binary data from.
        """
        self.file_path = file_path

    def load_data(self):
        """Loading binary data"""
        file = open(self.file_path, "rb")
        return file.read()

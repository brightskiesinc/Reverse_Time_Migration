#!/usr/bin/env python
# coding: utf8

"""
parser.py:
Resolver module.
"""

__author__ = "Brightskies inc."
__license__ = "LGPL-3.0 License"

import struct

import numpy as np


class Channel:
    """
    Channel data capsule.
    """

    def __init__(self, channel_name, runtimes, bandwidths, stats, number_of_calls):
        """Constructor"""
        self.channel_name = channel_name
        self.runtimes = runtimes
        self.bandwidths = bandwidths
        self.stats = stats
        self.number_of_calls = number_of_calls


class Parser:
    """
    Resolver class to parse read bytes into channel objects
    """

    def __init__(self, data_bytes):
        """Constructor"""
        self.data_bytes = data_bytes

    def parse(self):
        """Method to resolve bytes"""
        channels = []
        number_of_channels = int.from_bytes(self.data_bytes[:4], "little")
        offset = 4
        dt = np.dtype('f8')
        for x in range(number_of_channels):
            stats = {}
            kernel_name_size = int.from_bytes(self.data_bytes[offset:offset + 4], "little")
            offset = offset + 4
            kernel_name = str(self.data_bytes[offset:offset + kernel_name_size], 'UTF-8')
            offset = offset + kernel_name_size
            number_of_calls = int.from_bytes(self.data_bytes[offset:offset + 4], "little")
            offset = offset + 4
            runtimes = np.frombuffer(self.data_bytes,
                                     dtype=dt,
                                     offset=offset,
                                     count=number_of_calls)
            offset = offset + 8 * number_of_calls
            stats['max_runtime'], stats['min_runtime'], stats['avg_runtime'] \
                = struct.unpack('<ddd', self.data_bytes[offset:offset + 24])
            offset = offset + 24
            bw_flag = int.from_bytes(self.data_bytes[offset:offset + 1], "little")
            offset = offset + 1
            if bw_flag:
                bandwidths = np.frombuffer(self.data_bytes,
                                           dtype=dt,
                                           offset=offset,
                                           count=number_of_calls)
                offset = offset + 8 * number_of_calls
                stats['max_bandwidth'], stats['min_bandwidth'], stats['avg_bandwidth'] \
                    = struct.unpack('<ddd', self.data_bytes[offset:offset + 24])
                offset = offset + 24
            else:
                bandwidths = None
            channels.append(Channel(kernel_name, runtimes, bandwidths, stats, number_of_calls))
        return channels

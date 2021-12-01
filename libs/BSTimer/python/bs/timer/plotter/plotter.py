#!/usr/bin/env python
# coding: utf8

"""
plotter.py:
Plotting module.
"""

__author__ = "Brightskies inc."
__license__ = "LGPL-3.0 License"

import math

import matplotlib.pyplot as plt
import numpy as np


def plot_data(channels, file_path):
    """
    Plotting loaded timer data.
    :param channels: Channels to plot
    :param file_path: Path to result figures
    """

    props = dict(facecolor='gainsboro', alpha=0.75)
    for channel in channels:
        if channel.bandwidths is not None:
            fig, (ax1, ax2) = plt.subplots(2)
            fig.suptitle(channel.channel_name)
            ax1.plot(channel.runtimes)
            ax1.set_xticks(np.arange(0,
                                     channel.number_of_calls,
                                     math.ceil(channel.number_of_calls / 5)))
            ax1.set(ylabel='runtime')
            ax1.hlines(y=channel.stats['avg_runtime'],
                       xmin=0,
                       xmax=channel.number_of_calls - 1,
                       colors='r',
                       label='Average')
            ax2.plot(channel.bandwidths)
            ax2.set_xticks(np.arange(0,
                                     channel.number_of_calls,
                                     math.ceil(channel.number_of_calls / 5)))
            ax2.set(ylabel='bandwidth')
            ax2.hlines(y=channel.stats['avg_bandwidth'],
                       xmin=0,
                       xmax=channel.number_of_calls - 1,
                       colors='r',
                       label='Average')
            runtime_str = 'Maximum runtime:\n   %.5f\nMinimum runtime:\n   %.5f' % (
                channel.stats['max_runtime'],
                channel.stats['min_runtime'])
            bandwidth_str = 'Maximum bandwidth:\n   %.3E\nMinimum bandwidth:\n   %.3E' % (
                channel.stats['max_bandwidth'],
                channel.stats['min_bandwidth'])
            ax1.text(1.03, 0.6, runtime_str, transform=ax1.transAxes, fontsize=9,
                     verticalalignment='top', bbox=props, ha='left')
            ax2.text(1.03, 0.6, bandwidth_str, transform=ax2.transAxes, fontsize=9,
                     verticalalignment='top', bbox=props, ha='left')
            plt.subplots_adjust(right=0.74)

            plt.savefig('./' + file_path + '/' + channel.channel_name + '.png')
        else:
            fig, ax = plt.subplots()
            ax.set_xticks(np.arange(0,
                                    channel.number_of_calls,
                                    math.ceil(channel.number_of_calls / 5)))
            if channel.number_of_calls == 1:
                ax.plot(channel.runtimes, marker='x', markersize=5, color="red")
            else:
                ax.plot(channel.runtimes)
            ax.set(ylabel='runtime')
            fig.suptitle(channel.channel_name)
            ax.hlines(y=channel.stats['avg_runtime'],
                      xmin=0,
                      xmax=channel.number_of_calls - 1,
                      colors='r',
                      label='Average')
            runtime_str = 'Maximum runtime:\n   %.5f\nMinimum runtime:\n   %.5f' % (
                channel.stats['max_runtime'],
                channel.stats['min_runtime'])
            ax.text(1.03, 0.6, runtime_str, transform=ax.transAxes, fontsize=9,
                    verticalalignment='top', bbox=props, ha='left')
            plt.subplots_adjust(right=0.77)
            plt.savefig('./' + file_path + '/' + channel.channel_name + '.png')

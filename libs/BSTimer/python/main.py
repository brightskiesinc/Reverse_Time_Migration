#!/usr/bin/env python
# coding: utf8

"""
main.py:
Driver code.
"""

__author__ = "Brightskies inc."
__license__ = "LGPL-3.0 License"

import argparse

from bs.timer.plotter import plotter, loader, parser


def main():
    """Main function."""
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("-i", "--input", required=True)
    arg_parser.add_argument("-o", "--output", required=True)
    args = arg_parser.parse_args()
    extension = args.input.split(".")[-1]
    if extension == "timer":
        l = loader.Loader("./" + args.input)
        p = parser.Parser(l.load_data())
        plotter.plot_data(p.parse(), args.output)
    else:
        raise Exception("Wrong file extension")
    pass


if __name__ == "__main__":
    main()

#!/usr/bin/env python3

#
# Copyright © 2024-2025 Arm Ltd and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#

import subprocess


class FileWriter:
    def __init__(self, file_dir, start_line, end_line, tab=4):

        self._file = open(file_dir, "+r")
        self.file_dir = file_dir
        self.tab = tab
        self.indentation = 0
        self._lines = self._file.readlines()

        # create a copy of the file we are writing too and delineate the generated section
        start_line_num = 0
        end_line_num = 0

        for num, line in enumerate(self._lines):
            if start_line in line:
                start_line_num = num + 1
            if end_line in line:
                end_line_num = num

        self._file.close()

        if start_line_num == 0:
            raise Exception(
                "Could not find generated code start of generated code section in: "
                + file_dir
                + "\n"
                "Start of generated code section is identified by string: \n"
                + start_line
            )
        if end_line_num == 0:
            raise Exception(
                "Could not find generated code end of generated code section in "
                + file_dir
                + "\n"
                "Start of generated code section is identified by string: \n" + end_line
            )

        self._end_lines = self._lines[end_line_num:]
        self._lines = self._lines[:start_line_num]

    # write to the file once the whole function has executed without error
    def __del__(self):
        self._file = open(self.file_dir, "+w")
        for line in self._lines:
            self._file.write(line)
        for line in self._end_lines:
            self._file.write(line)
        # -------- clang-format the result --------
        self._file.close()
        import subprocess
        import os
        from pathlib import Path

        try:
            result = subprocess.run(
                [
                    "clang-format",
                    "-i",
                    "--style=file",
                    self.file_dir,
                ],
                check=True,
                capture_output=True,
                text=True,
                cwd=Path.cwd(),
            )
            if result.stdout:
                print("[clang-format stdout]", result.stdout)
            if result.stderr:
                print("[clang-format stderr]", result.stderr)

        except subprocess.CalledProcessError as e:
            print("[clang-format failed]")
            print("stdout:", e.stdout)
            print("stderr:", e.stderr)

    def start_block(self):
        self.indentation += self.tab

    def end_block(self):
        self.indentation -= self.tab
        if self.indentation < 0:
            self.indentation = 0

    def get_indentation(self):
        return self.indentation

    def set_indentation(self, indentation):
        self.indentation = indentation

    def get_dir(self):
        return self.file_dir

    # write line with current indentation
    def write(self, line):
        self._lines.append(" " * self.indentation)
        self._lines.append(line)

    # write line without indentation (append content to the current line)
    def append(self, line):
        self._lines.append(line)

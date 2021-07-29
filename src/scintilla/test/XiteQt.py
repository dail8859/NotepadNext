#!/usr/bin/env python2
# -*- coding: utf-8 -*-
# Requires Python 2.7 or later

import ctypes, os, platform, sys, unittest

from PySide.QtCore import *
from PySide.QtGui import *

import ScintillaCallable

sys.path.append("..")
from bin import ScintillaEditPy

scintillaDirectory = ".."
scintillaIncludeDirectory = os.path.join(scintillaDirectory, "include")
scintillaScriptsDirectory = os.path.join(scintillaDirectory, "scripts")
sys.path.append(scintillaScriptsDirectory)
import Face

scintillaIncludesLexers = False
# Lexilla may optionally be tested it is built and can be loaded
lexillaAvailable = False

lexillaDirectory = os.path.join(scintillaDirectory, "..", "lexilla")
lexillaBinDirectory = os.path.join(lexillaDirectory, "bin")
lexillaIncludeDirectory = os.path.join(lexillaDirectory, "include")

lexName = "liblexilla.so"
try:
	lexillaSOPath = os.path.join(lexillaBinDirectory, lexName)
	lexillaLibrary = ctypes.cdll.LoadLibrary(lexillaSOPath)
	createLexer = lexillaLibrary.CreateLexer
	createLexer.restype = ctypes.c_void_p
	lexillaAvailable = True
	print("Found Lexilla")
except OSError:
	print("Can't find " + lexName)
	print("Python is built for " + " ".join(platform.architecture()))

class Form(QDialog):

	def __init__(self, parent=None):
		super(Form, self).__init__(parent)
		self.resize(460,300)
		# Create widget
		self.edit = ScintillaEditPy.ScintillaEdit(self)

class XiteWin():
	def __init__(self, test=""):
		self.face = Face.Face()
		self.face.ReadFromFile(os.path.join(scintillaIncludeDirectory, "Scintilla.iface"))
		try:
			faceLex = Face.Face()
			faceLex.ReadFromFile(os.path.join(lexillaIncludeDirectory, "LexicalStyles.iface"))
			self.face.features.update(faceLex.features)
		except FileNotFoundError:
			print("Can't find " + "LexicalStyles.iface")

		self.test = test

		self.form = Form()

		scifn = self.form.edit.send(int(self.face.features["GetDirectFunction"]["Value"]), 0, 0)
		sciptr = ctypes.c_char_p(self.form.edit.send(
			int(self.face.features["GetDirectPointer"]["Value"]), 0,0))

		self.ed = ScintillaCallable.ScintillaCallable(self.face, scifn, sciptr)
		self.form.show()

	def DoStuff(self):
		print(self.test)
		self.CmdTest()

	def DoEvents(self):
		QApplication.processEvents()

	def CmdTest(self):
		runner = unittest.TextTestRunner()
		tests = unittest.defaultTestLoader.loadTestsFromName(self.test)
		results = runner.run(tests)
		print(results)
		sys.exit(0)

	def ChooseLexer(self, lexer):
		if scintillaIncludesLexers:
			self.ed.LexerLanguage = lexer
		elif lexillaAvailable:
			pLexilla = createLexer(lexer)
			self.ed.SetILexer(0, pLexilla)
		else:	# No lexers available
			pass

xiteFrame = None

def main(test):
	global xiteFrame
	app = QApplication(sys.argv)
	xiteFrame = XiteWin(test)
	xiteFrame.DoStuff()
	sys.exit(app.exec_())

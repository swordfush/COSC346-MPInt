# Tests the MPInteger class on randomly generated test cases.
# Build a program using main as defined in test.m. Then copy the resulting
# executable into this directory and name it MPInteger-test
# 
# This script can then be run using python2.7 test.py <n>, where n is the
# number of randomly generated pairs (a, b) to test all operations between
# a and b for.

import subprocess
import sys
import random

testCount = int(sys.argv[1])
maxBits = 32 * 50

def debug(num):
	if num < 0:
		num = -num
	elif num == 0:
		return "0\n"
	max = 2**32 - 1
	desc = ""
	while num != 0:
		desc += str(num % max) + "\n"
		num = num / max
	return desc

def tryOperation(a, b, op, expect):
	output = subprocess.check_output(["./MPInteger-test", str(a), op, str(b)], stderr=subprocess.STDOUT)

	output = int(output)

	if output != expect:
		print "Failed", a, op, b
		print "\tExpected:", expect
		print "\tGot:     ", output
		print "\tExpected internal representation:"
		print debug(expect)
		print "\tReceived internal representation:"
		print debug(output)
		sys.exit(0)

for i in xrange(1, testCount + 1):
	aBits = random.randint(1, maxBits)
	bBits = random.randint(1, maxBits)

	a = int(random.getrandbits(aBits))
	if random.randint(0, 1) == 0:
		a = -a
	b = int(random.getrandbits(bBits))
	if random.randint(0, 1) == 0:
		b = -b


	tryOperation(a, b, "+", a + b)
	tryOperation(a, b, "-", a - b)
	tryOperation(a, b, "*", a * b)
	if b != 0:
		tryOperation(a, b, "/", a / b)
		tryOperation(a, b, "%", a % b)
	print "Passed", i

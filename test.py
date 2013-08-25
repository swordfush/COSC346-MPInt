import subprocess
import sys
import random

testCount = int(sys.argv[1])
maxBits = 32 * 5

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

def quote(string):
	return "\"{0}\"".format(string)

def tryOperation(a, b, op, expect):
	output = subprocess.check_output(["./test", str(a), op, str(b)], stderr=subprocess.STDOUT)

	internal = debug(expect)
	expect = "{0}\n".format(expect)

	if output != expect:
		print "Failed", a, op, b
		print "\tExpected:", quote(expect)
		print "\tGot:", quote(output)
		print "\tExpected internal representation:"
		print internal
		print "\tReceived internal representation:"
		print debug(int(output[:-1]))
		sys.exit(0)

for i in xrange(0, testCount):
	aBits = random.randint(1, maxBits)
	bBits = random.randint(1, maxBits)

	a = int(random.getrandbits(aBits))
	b = int(random.getrandbits(bBits))

	tryOperation(a, b, "+", a + b)
	tryOperation(a, b, "-", a - b)
	tryOperation(a, b, "*", a * b)
	if b != 0:
		tryOperation(a, b, "/", a / b)
		tryOperation(a, b, "%", a % b)
	print "Passed", i+1
	print "\t", a
	print "\t", b

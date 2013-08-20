import subprocess
import sys
import random

testCount = int(sys.argv[1])
maxBits = 32 * 5

def debug(num):
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

	expect = "{0}\n{1}".format(expect, debug(expect))

	if output != expect:
		print "Failed", a, op, b
		print "\tExpected:", quote(expect)
		print "\tGot:", quote(output)


for i in range(0, testCount):
	aBits = random.randint(1, maxBits)
	bBits = random.randint(1, maxBits)

	a = int(random.getrandbits(aBits))
	b = int(random.getrandbits(bBits))

	tryOperation(a, b, "+", a + b)
	tryOperation(a, b, "-", a - b)
	tryOperation(a, b, "*", a * b)
	if (b != 0):
		tryOperation(a, b, "/", a / b)
		tryOperation(a, b, "%", a % b)

import subprocess
import sys
import random

testCount = int(sys.argv[1])
maxBits = 32 * 5

def printnum(num):
	max = 2**32 - 1
	while num != 0:
		print num % max
		num = num / max

def quote(string):
	return "\"{0}\"".format(string)

def tryOperation(a, b, op, expect):
	expect = str(expect)

	output = subprocess.check_output(["./test", str(a), op, str(b)])

	if output != expect:
		print "Failed", a, op, b
		print "\tExpected:", quote(expect)
		print "\tGot:", quote(output)


for i in range(0, testCount):
	aBits = random.randint(0, maxBits)
	bBits = random.randint(0, maxBits)

	a = int(random.getrandbits(aBits))
	b = int(random.getrandbits(bBits))

	tryOperation(a, b, "+", a + b)
	tryOperation(a, b, "-", a - b)
	tryOperation(a, b, "*", a * b)
	tryOperation(a, b, "/", a / b)
	tryOperation(a, b, "%", a % b)

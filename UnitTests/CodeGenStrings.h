#pragma once
#include <string>

std::string RobotCodeString = R"(
class Robot(object):
	pass
)";

std::string RobotTwoCodeString = R"(
class RobotTwo(object):
	pass
)";

std::string RobotCodeWithZeroErrorString = R"(
class Robot(object):
	def __init__(self):
		self.age = 0
	def divide(self, dividend, divisor):
		return dividend / divisor
)";
std::string RobotCodeWithZeroErrorFixedString = R"(
class Robot(object):
	def __init__(self):
		self.age = 0
	def divide(self, dividend, divisor):
		if divisor == 0:
			return 0.0
		return dividend / divisor
)";
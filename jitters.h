#pragma once
#include <utility>
#include <vector>
#include <unordered_map>
// from https://www.glprogramming.com/red/chapter10.html Table 10-5
const std::vector<std::pair<double, double>> no_jitters = {
	{0.0, 0.0}
};

const std::vector<std::pair<double, double>> jitters2 = {
	{0.25, 0.75}, {0.75, 0.25}
};

const std::vector<std::pair<double, double>> jitters4 = {
	{0.375, 0.25}, {0.125, 0.75}, {0.875, 0.25}, {0.625, 0.75}
};

const std::vector<std::pair<double, double>> jitters8 = {
	{0.5625, 0.4375}, {0.0625, 0.9375}, {0.3125, 0.6875}, {0.6875, 0.8125},
	{0.8125, 0.1875}, {0.9375, 0.5625}, {0.4375, 0.0625}, {0.1875, 0.3125}
};

const std::vector<std::pair<double, double>> jitters16 = {
	{0.375, 0.4375}, {0.625, 0.0625}, {0.875, 0.1875}, {0.125, 0.0625},
	{0.375, 0.6875}, {0.875, 0.4375}, {0.625, 0.5625}, {0.375, 0.9375},
	{0.625, 0.3125}, {0.125, 0.5625}, {0.125, 0.8125}, {0.375, 0.1875},
	{0.875, 0.9375}, {0.875, 0.6875}, {0.125, 0.3125}, {0.625, 0.8125}
};

const std::unordered_map<unsigned int, std::vector<std::pair<double, double>>> jitters = {
	{1u, no_jitters}, {2u, jitters8}, {4u, jitters4}, {8u, jitters8}, {16u, jitters16}
};

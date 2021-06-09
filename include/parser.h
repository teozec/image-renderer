/* Copyright (C) 2021 Luca Nigro and Matteo Zeccoli Marazzini

This file is part of image-renderer.

image-renderer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

image-renderer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with image-renderer.  If not, see <https://www.gnu.org/licenses/>. */

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <sstream>

/**
 * @brief   Location of the token.
 * @details Filename is given as well as line number and column namber.
 */
struct SourceLocation {
    std::string filename;
    int line{};
    int col{};
};

/**
 * @brief An exception thrown when a grammar error in the input scenefiles
 */
struct GrammarError : std::runtime_error {
	SourceLocation location;
	std::string message;
};
/**
 * @brief   Wrapper used to parse the scene file.
 * @details It keeps updated the line number and the column number.
 *          It lets look-ahead tokens and un-read characters.
 */
struct InputStream {
    std::istream &stream;
    SourceLocation location;
    int tabulations;

    InputStream(std::istream &stream, SourceLocation location, int tabulations=8) : 
    stream{stream}, location{location}, tabulations{tabulations} {}

};
 
/**
 * @brief An enum to identify the available keywords
 */
enum class Keyword {
	NEW, MATERIAL, PLANE, SPHERE, DIFFUSE, SPECULAR, UNIFORM, CHECKERED,
	IMAGE, IDENTITY, TRANSLATION, ROTATION_X, ROTATION_Y, ROTATION_Z,
	SCALING, CAMERA, ORTHOGONAL, PERSPECTIVE, FLOAT
};

/**
 * @brief An enum to identify the available tokens
 */
enum class TokenType {
	KEYWORD, IDENTIFIER, STRING, FLOAT, SYMBOL, STOP
};

/**
 * @brief A union to store the value of a token
 */
union TokenUnion {
	TokenUnion() {}
	~TokenUnion() {}
	Keyword k;
	std::string s;
	float f;
	char c;
};

/**
 * @brief A struct to store tokens
 */
struct Token {
	SourceLocation location;
	TokenType type;
	TokenUnion value;

	Token(SourceLocation location) : location{location} {}

	void assignKeyword(Keyword k) {
		type = TokenType::KEYWORD;
		value.k = k;
	}

	void assignIdentifier(std::string s) {
		type = TokenType::IDENTIFIER;
		value.s = s;
	}

	void assignString(std::string s) {
		type = TokenType::STRING;
		value.s = s;
	}

	void assignFloat(float f) {
		type = TokenType::FLOAT;
		value.f = f;
	}

	void assignSymbol(char c) {
		type = TokenType::SYMBOL;
		value.c = c;
	}

	void assignStop() {
		type = TokenType::STOP;
	}
};

#endif // PARSER_H
